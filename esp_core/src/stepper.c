#include "stepper.h"

#include "utils.h"

// min max freq in Hz
#define MIN_FREQ 400
#define MAX_FREQ 6600

// min max period in terms of freq
// divided by 2 since this number is used to flip the step pin, 2 flips per period
#define MIN_PERIOD_USEC (1000000. / MAX_FREQ) / 2
#define MAX_PERIOD_USEC (1000000. / MIN_FREQ) / 2

// number of steps to go full extent of gantry
#define MAX_POSITION_STEP 8000

// stability epsilon coef
#define EPSILON 1e-6

// momentum transition delay in steps
#define MOMENTUM_DELAY 200

#define MOMENTUM_LOW lerp(MAX_PERIOD_USEC, MIN_PERIOD_USEC, 0.14)
#define MOMENTUM_MEDIUM lerp(MAX_PERIOD_USEC, MIN_PERIOD_USEC, 0.28)
#define MOMENTUM_HIGH lerp(MAX_PERIOD_USEC, MIN_PERIOD_USEC, 0.5)
#define MOMENTUM_FULL lerp(MAX_PERIOD_USEC, MIN_PERIOD_USEC, 0.77)


enum Momentum {
    LOW,
    MEDIUM,
    HIGH,
    FULL,
};

// stepper state struct
struct step_status_t {
	uint64_t period; // period of time in us to next step, used to control stepper speed
	uint8_t direction; // 0 or 1, to indicate direction
	uint32_t steps; // number of steps left
	uint64_t position; // current position of the stepper in steps from origin
	uint8_t current; // 0 or 1, whether the current step pin is high or low
    enum Momentum momentum; // momentum, decides how fast it should go
    uint32_t momentum_delay; // delay to transition to next momentum state
} step_status = {
	.period = MAX_PERIOD_USEC
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

// Never log inside an ISR, serial kills ISRs
static bool IRAM_ATTR stp_timer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_awoken = pdFALSE;

    if(step_status.steps) {
        if(step_status.current) {
            step_status.steps -= 1;
            step_status.momentum_delay -= 1;
            step_status.position += step_status.direction ? 1 : -1;
        }
    
        gpio_set_level(STEP_PIN, step_status.current);
        gpio_set_level(DIR_PIN, step_status.direction);
    
        step_status.current = !step_status.current;

        if(step_status.momentum_delay <= 0 && step_status.momentum < FULL) {
            step_status.momentum += 1;
            step_status.momentum_delay = MOMENTUM_DELAY;
        }

        // update stepper period
        step_status.period = (step_status.momentum == LOW) ? MOMENTUM_LOW : (step_status.momentum == MEDIUM) ? MOMENTUM_MEDIUM : (step_status.momentum == HIGH) ? MOMENTUM_HIGH : MOMENTUM_FULL;
        stp_alarm_config.alarm_count = step_status.period;
        gptimer_set_alarm_action(step_timer, &stp_alarm_config);
    }
    
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

void initialize_stepper_driver() {
	gpio_config_t io_conf; 

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    
    io_conf.pin_bit_mask = 1ULL<<STEP_PIN;
    gpio_config(&io_conf);

    io_conf.pin_bit_mask = 1ULL<<DIR_PIN;
    gpio_config(&io_conf);

    io_conf.pin_bit_mask = 1ULL<<ENABLE_PIN;
    gpio_config(&io_conf);

    gpio_set_level(ENABLE_PIN, 0); // enable stepper
}

void update_stepper(StepMessage_t *msg) {
    // calculate desired speed and steps required
    double current_position = step_status.position * (1./MAX_POSITION_STEP);
    double dx = msg->position - current_position;

    step_status.steps = (dx>0 ? dx : -dx) * MAX_POSITION_STEP;
    step_status.direction = dx>0;

    step_status.momentum_delay = MOMENTUM_DELAY;
    step_status.momentum = LOW;

    ESP_LOGI(pcTaskGetName(NULL), "stepper updated");
}


void stepper_controller_task(void *pvParameters) {

	initialize_stepper_timer();
	initialize_stepper_driver();

    StepMessage_t msg;
	forever
	{   
        // TODO ADJUST DELAY
		vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP_LOGI(pcTaskGetName(NULL), "momentum %d", step_status.momentum);

        // read the requested position from xqueue (from udp data)
        if(xQueueReceive(stepQueue, &msg, 0) && msg.update) { 
            update_stepper(&msg);                        
        }
		
	}
	
	
	vTaskDelete(NULL);
}

