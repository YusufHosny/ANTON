#ifndef STEPPER_H
#define STEPPER_H

#include "utils.h"
#include "inttypes.h"
#include "stdbool.h"

#define STEP_PIN_H GPIO_NUM_5
#define DIR_PIN_H GPIO_NUM_6
#define ENABLE_PIN_H GPIO_NUM_4

#define STEP_PIN_V GPIO_NUM_15
#define DIR_PIN_V GPIO_NUM_16
#define ENABLE_PIN_V GPIO_NUM_7


void stepper_controller_task(void *pvParameters);

// stepper state struct
typedef struct stepper_status {
	uint64_t period; // period of time in us to next step, used to control stepper speed
	uint8_t direction; // 0 or 1, to indicate direction
	uint32_t steps; // number of steps left
	uint64_t position; // current position of the stepper in steps from origin
	uint8_t current; // 0 or 1, whether the current step pin is high or low
    double momentum; // momentum, decides how fast it should go (0 to 1)
} stepper_status_t; 

typedef struct stepper_pin {
    // pin layout
    uint16_t step;
    uint16_t dir;
    uint16_t enable;
} stepper_pin_t;

typedef struct stepper_limits {
    double min_freq, max_freq; // min max speed frequencies
    uint32_t step_extent; // max position in steps
} stepper_limits_t;

typedef struct stepper_configuration {
    EasingFunction ease; // easing function used
    bool default_dir; // 0 or 1 to define the default direction of the motor
} stepper_configuration_t;

typedef struct stepper_handle {
    stepper_pin_t pins;
    stepper_limits_t limits;
    stepper_status_t status;
    stepper_configuration_t config;
} stepper_handle_t;


#endif // STEPPER_H