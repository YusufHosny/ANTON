

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
    step_comms();
    DELAY_milliseconds(1);
}

void main(void)
{
	setup();
    while(1) loop();
}