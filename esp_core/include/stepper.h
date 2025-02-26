#ifndef STEPPER_H
#define STEPPER_H

#include "config.h"

#define STEP_PIN GPIO_NUM_13
#define DIR_PIN GPIO_NUM_14
#define ENABLE_PIN GPIO_NUM_12

void stepper_controller_task(void *pvParameters);

#endif // STEPPER_H