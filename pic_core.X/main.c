

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

void comm_loop() {
    step_comms();   
}

void racket_test(){
    //test code for racket fine-tune
    float test_angle = 90;
    bool test_fire = true;
    RacketMessage_t msg;
    msg.angle = test_angle;
    msg.fire = test_fire;
    update_racket(&msg);
}

void main(void)
{
	setup();
    while(1){
        comm_loop();
        //racket_test();
    }
}