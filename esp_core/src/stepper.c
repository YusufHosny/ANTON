#include "stepper.h"

#include "utils.h"

// min max freq in Hz
#define MIN_FREQ 400
#define MAX_FREQ 3200

// min max period in terms of freq
// divided by 2 since this number is used to flip the step pin, 2 flips per period
#define MIN_PERIOD_USEC (1000000. / MAX_FREQ) / 2
#define MAX_PERIOD_USEC (1000000. / MIN_FREQ) / 2

// number of steps to go full extent of gantry
#define MAX_POSITION_STEP_H 8000
#define MAX_POSITION_STEP_V 2400

// stability epsilon coef
#define EPSILON 1e-6

// change in momentum per 10ms in % of easing function
#define MOMENTUM_DELTA 0.01


// stepper state struct
typedef struct step_status {
	uint64_t period; // period of time in us to next step, used to control stepper speed
	uint8_t direction; // 0 or 1, to indicate direction
	uint32_t steps; // number of steps left
	uint64_t position; // current position of the stepper in steps from origin
	uint8_t current; // 0 or 1, whether the current step pin is high or low
    double momentum; // momentum, decides how fast it should go (0 to 1)
} step_status_t; 

typedef struct stepper_handle {
    uint16_t step_pin;
    uint16_t dir_pin;
    uint16_t enable_pin;
    step_status_t status;
} stepper_handle_t;

stepper_handle_t vhandle = {
    .step_pin = STEP_PIN_V,
    .dir_pin = DIR_PIN_V,
    .enable_pin = ENABLE_PIN_V,
    .status.period = MAX_PERIOD_USEC
};
stepper_handle_t hhandle = {
    .step_pin = STEP_PIN_H,
    .dir_pin = DIR_PIN_H,
    .enable_pin = ENABLE_PIN_H,
    .status.period = MAX_PERIOD_USEC
};


gptimer_handle_t step_timer = NULL;
gptimer_config_t stp_timer_config = {
    .clk_src = GPTIMER_CLK_SRC_DEFAULT,
    .direction = GPTIMER_COUNT_UP,
    .resolution_hz = 1000000, // 1MHz, 1 tick=1us
};
gptimer_alarm_config_t stp_alarm_config = {
    .alarm_count = 1000000, // period = 1s
    .reload_count = 0,
    .flags.auto_reload_on_alarm = true
};


void isr_stepper(stepper_handle_t *handle) {
    if(handle->status.steps) {
        // update step count and position tracking
        if(handle->status.current) {
            handle->status.steps -= 1;
            handle->status.position += handle->status.direction ? 1 : -1;
        }
        
        // update step and dir pins
        gpio_set_level(handle->step_pin, handle->status.current);
        gpio_set_level(handle->dir_pin, handle->status.direction);
        handle->status.current = !handle->status.current;

        // update stepper period
        handle->status.period = ease(MAX_PERIOD_USEC, MIN_PERIOD_USEC, handle->status.momentum);
        stp_alarm_config.alarm_count = handle->status.period;
        gptimer_set_alarm_action(step_timer, &stp_alarm_config);
    }
}

// Never log inside an ISR, serial kills ISRs
static bool IRAM_ATTR stp_timer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_awoken = pdFALSE;

    isr_stepper(&hhandle);
    isr_stepper(&vhandle);
    
    // return whether we need to yield at the end of ISR
    return (high_task_awoken == pdTRUE);
}
gptimer_event_callbacks_t stp_callbacks = {
    .on_alarm = stp_timer_callback,
};

void initialize_stepper_timer() {
	
	ESP_LOGI(pcTaskGetName(NULL), "Create timer handle");
    ESP_ERROR_CHECK(gptimer_new_timer(&stp_timer_config, &step_timer));
	
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(step_timer, &stp_callbacks, NULL));

    ESP_LOGI(pcTaskGetName(NULL), "Enable timer");
    ESP_ERROR_CHECK(gptimer_enable(step_timer));

    ESP_LOGI(pcTaskGetName(NULL), "Start timer");
    ESP_ERROR_CHECK(gptimer_set_alarm_action(step_timer, &stp_alarm_config));
    ESP_ERROR_CHECK(gptimer_start(step_timer));
}

void initialize_stepper_driver(stepper_handle_t *handle) {
	gpio_config_t io_conf; 

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    
    io_conf.pin_bit_mask = 1ULL<<handle->step_pin;
    gpio_config(&io_conf);

    io_conf.pin_bit_mask = 1ULL<<handle->dir_pin;
    gpio_config(&io_conf);

    io_conf.pin_bit_mask = 1ULL<<handle->enable_pin;
    gpio_config(&io_conf);

    gpio_set_level(handle->enable_pin, 0); // enable stepper
}

void update_stepper(StepMessage_t *msg, stepper_handle_t *handle) {

    int max_pos;

    // calculate desired speed and steps required
    double current_position = handle->status.position * (1./max_pos);
    double dx = msg->position - current_position;

    handle->status.steps = (dx>0 ? dx : -dx) * max_pos;
    
    uint8_t newdirection = dx>0;
    if(handle->status.direction != newdirection) handle->status.momentum = 0;
    handle->status.direction = newdirection;

    ESP_LOGI(pcTaskGetName(NULL), "stepper updated");
}


void stepper_controller_task(void *pvParameters) {

	initialize_stepper_timer();
	initialize_stepper_driver(&vhandle);
    initialize_stepper_driver(&hhandle);

    StepMessage_t vmsg, hmsg;
	forever
	{   
		vTaskDelay(10 / portTICK_PERIOD_MS);
        if(hhandle.status.momentum < 1.) hhandle.status.momentum += MOMENTUM_DELTA;
        if(vhandle.status.momentum < 1.) vhandle.status.momentum += MOMENTUM_DELTA;

        // read the requested position from xqueue (from udp data)
        if(xQueueReceive(vstepQueue, &vmsg, 0) && hmsg.update) { 
            update_stepper(&hmsg, &vhandle);                        
        }

        // read the requested position from xqueue (from udp data)
        if(xQueueReceive(hstepQueue, &hmsg, 0) && vmsg.update) { 
            update_stepper(&vmsg, &hhandle);                        
        }
		
	}

	vTaskDelete(NULL);
}

