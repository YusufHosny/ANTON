#ifndef STEPPER_H
#define STEPPER_H

#include "config.h"

#define STEP_PIN GPIO_NUM_17
#define DIR_PIN GPIO_NUM_17
#define ENABLE_PIN GPIO_NUM_17

void stepper_controller_task(void *pvParameters);

#endif // STEPPER_H