#ifndef STEPPER_H
#define STEPPER_H

#include "config.h"

#define STEP_PIN_H GPIO_NUM_13
#define DIR_PIN_H GPIO_NUM_14
#define ENABLE_PIN_H GPIO_NUM_12

#define STEP_PIN_V GPIO_NUM_13
#define DIR_PIN_V GPIO_NUM_14
#define ENABLE_PIN_V GPIO_NUM_12


void stepper_controller_task(void *pvParameters);

#endif // STEPPER_H