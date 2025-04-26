

#include "config.h"
#include "racket.h"
#include "comms.h"


void setup() {
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    
    initialize_comms();
    set_comms_handler(update_racket);
    initialize_racket();
}

void loop() {
    //step_comms();
    
    //test code for racket fine-tune
    float test_angle = 40;
    bool test_fire = false;
    RacketMessage_t msg;
    msg.angle = test_angle;
    msg.fire = test_fire;
    update_racket(&msg);
}

void main(void)
{
	setup();
    while(1) loop();
}