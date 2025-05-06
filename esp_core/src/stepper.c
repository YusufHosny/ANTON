#include "stepper.h"
#include "config.h"

// min max freq in Hz
#define MIN_FREQ_H 400
#define MAX_FREQ_H 3000
#define MIN_FREQ_V 100
#define MAX_FREQ_V 500

// number of steps to go full extent of gantry
#define MAX_POSITION_STEP_H 8000
#define MAX_POSITION_STEP_V 2400

// stability epsilon coef
#define EPSILON 1e-6

// change in momentum per 10ms in % of easing function
#define MOMENTUM_DELTA 0.01

stepper_handle_t vhandle = {
    .pins.step = STEP_PIN_V,
    .pins.dir = DIR_PIN_V,
    .pins.enable = ENABLE_PIN_V,

    .limits.min_freq = MIN_FREQ_V,
    .limits.max_freq = MAX_FREQ_V,
    .limits.step_extent = MAX_POSITION_STEP_V,

    .status.period = freq_to_usec(MIN_FREQ_V),

    .config.ease = &ease,
    .config.default_dir = 1
};
stepper_handle_t hhandle = {
    .pins.step = STEP_PIN_H,
    .pins.dir = DIR_PIN_H,
    .pins.enable = ENABLE_PIN_H,

    .limits.min_freq = MIN_FREQ_H,
    .limits.max_freq = MAX_FREQ_H,
    .limits.step_extent = MAX_POSITION_STEP_H,

    .status.period = freq_to_usec(MIN_FREQ_H),

    .config.ease = &ease,
    .config.default_dir = 1
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
        // enable stepper
        gpio_set_level(handle->pins.enable, 0);

        // update step count and position tracking
        if(handle->status.current) {
            handle->status.steps -= 1;
            handle->status.position += handle->status.direction ? 1 : -1;
        }
        
        // update step and dir pins
        gpio_set_level(handle->pins.step, handle->status.current);
        gpio_set_level(handle->pins.dir, handle->config.default_dir ? handle->status.direction : !handle->status.direction);
        handle->status.current = !handle->status.current;

        // update stepper period
        handle->status.period = handle->config.ease(freq_to_usec(handle->limits.min_freq), freq_to_usec(handle->limits.max_freq), handle->status.momentum);
        stp_alarm_config.alarm_count = handle->status.period;
        gptimer_set_alarm_action(step_timer, &stp_alarm_config);
    }
    else {
        gpio_set_level(handle->pins.enable, 1); // disable stepper if inactive
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
    
    io_conf.pin_bit_mask = 1ULL<<handle->pins.step;
    gpio_config(&io_conf);

    io_conf.pin_bit_mask = 1ULL<<handle->pins.dir;
    gpio_config(&io_conf);

    io_conf.pin_bit_mask = 1ULL<<handle->pins.enable;
    gpio_config(&io_conf);

    gpio_set_level(handle->pins.enable, 0); // enable stepper
}

void update_stepper(StepMessage_t *msg, stepper_handle_t *handle) {
    switch(msg->type) {
        case NORMAL:
        {
            // calculate desired speed and steps required
            double current_position = handle->status.position * (1./handle->limits.step_extent);
            double dx = msg->position - current_position;

            handle->status.steps = dx * handle->limits.step_extent;
            
            uint8_t newdirection = dx>0;
            if(handle->status.direction != newdirection) handle->status.momentum = 0;
            handle->status.direction = newdirection;

            break;
        }
        case MANUAL:
        {
            handle->status.steps = UINT32_MAX;
            
            uint8_t newdirection = msg->position > .5;
            if(handle->status.direction != newdirection) handle->status.momentum = 0;
            handle->status.direction = newdirection;

            break;
        }
        case RESET: 
        {
            handle->status.steps = 0;
            handle->status.momentum = 0;
            break;
        }
    }
    

    // ESP_LOGI(pcTaskGetName(NULL), "stepper updated");
}


void stepper_controller_task(void *pvParameters) {

	initialize_stepper_timer();
	initialize_stepper_driver(&vhandle);
    initialize_stepper_driver(&hhandle);

    StepMessage_t msg;
	forever
	{   
		vTaskDelay(10 / portTICK_PERIOD_MS);
        if(hhandle.status.momentum < 1.) hhandle.status.momentum += MOMENTUM_DELTA;
        if(vhandle.status.momentum < 1.) vhandle.status.momentum += MOMENTUM_DELTA;

        // read the requested position from xqueue (from udp data)
        if(xQueueReceive(hstepQueue, &msg, 0)) { 
            update_stepper(&msg, &hhandle);                        
        }
        if(xQueueReceive(vstepQueue, &msg, 0)) { 
            update_stepper(&msg, &vhandle);                        
        }
		
	}

	vTaskDelete(NULL);
}

