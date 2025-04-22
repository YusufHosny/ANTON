/* 
 * File:   racket.c
 * Author: Yusuf
 *
 * Created on March 25, 2025, 3:10 PM
 */

#include "racket.h"
#include "config.h"

#define MAXDUTY 38000
#define MINDUTY 4000
#define DEFAULTDUTY (MAXDUTY+MINDUTY)/2


uint16_t lerp_duty(float x) {
    return (uint16_t) ((float)MINDUTY + (float)(MAXDUTY-MINDUTY)*x);
}

void update_servo(float angle) {
    // TODO DERIVE FROM ANGLE VALUE PROPERLY
    uint16_t duty = lerp_duty(angle); 
    
    servoPWM_SetSlice1Output1DutyCycleRegister(duty);
    servoPWM_LoadBufferRegisters();
}

void set_racket(bool fire) {
    if(fire) racketOut_SetHigh();
    else racketOut_SetLow();
}


void initialize_racket() {
    uint16_t duty = DEFAULTDUTY;
    
    servoPWM_SetSlice1Output1DutyCycleRegister(duty);
    servoPWM_LoadBufferRegisters();
    
    racketOut_SetLow();
}

void update_racket(RacketMessage_t *msg){
    update_servo(msg->angle);
    set_racket(msg->fire);
}

