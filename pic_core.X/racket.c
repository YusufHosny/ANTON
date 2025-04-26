/* 
 * File:   racket.c
 * Author: Yusuf
 *
 * Created on March 25, 2025, 3:10 PM
 */

#include "racket.h"
#include "config.h"

//PWM clock(=FOSC/prescale) = 3.2MHz, PWMfreq = 50Hz (see mcc)
//duty = PWMclock/PWMfreq (nr of periods of PWMclock counted)
//minduty period = 0,5ms -> freq = 2kHz -> minduty = 3.200.000/2.000 = 1600
//maxduty period = 2,5ms -> freq = 400Hz -> maxduty = 3.2M/400 = 8000
#define MINDUTY 1600
#define MAXDUTY 8000
#define DEFAULTDUTY (MAXDUTY+MINDUTY)/2


uint16_t lerp_duty(float angle) {
    //TODO: fine-tune offset for specific servo
    int offset = 5;
    angle += offset;
    min_angle = offset;
    max_angle = 180+offset;
    
    if(angle>max_angle) angle = max_angle;
    if(angle<min_angle) angle = min_angle;
    
    //TODO: divide by max_angle instead of 180??
    return (uint16_t) ((float)MINDUTY + (float)(MAXDUTY-MINDUTY)*(angle/180));
}

void update_servo(float angle) {
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

