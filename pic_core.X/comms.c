/* 
 * File:   comms.c
 * Author: Yusuf
 *
 * Created on March 25, 2025, 3:18 PM
 */

#include "comms.h"
#include "mirf.h"
#include "config.h"
#include <string.h>

#define MSG_SIZE 8
#define CONFIG_RADIO_CHANNEL 89
#define CONFIG_RETRANSMIT_DELAY 100

RacketMessageHandler_t _handler = NULL;
    
NRF24_t dev;
void initialize_comms() {
    Nrf24_init(&dev);
	uint8_t payload = MSG_SIZE; // bytes since esp also sends padding
	uint8_t channel = CONFIG_RADIO_CHANNEL;
	Nrf24_config(&dev, channel, payload);

	int ret = Nrf24_setRADDR(&dev, (uint8_t *)"FGHIJ");
	if (ret != NRF_OK) while(1) DELAY_milliseconds(1); // failed inf loop

    Nrf24_SetSpeedDataRates(&dev, 0);
	Nrf24_setRetransmitDelay(&dev, CONFIG_RETRANSMIT_DELAY);

	// dump details
	Nrf24_printDetails(&dev);

    // clear rx fifo buffer
	uint8_t buf[MSG_SIZE];
	while(Nrf24_dataReady(&dev)) Nrf24_getData(&dev, buf);
}

void set_comms_handler(RacketMessageHandler_t handler) {
    _handler = handler;
} 

void step_comms() {
    RacketMessage_t msg;
        
    // wait for received data
    uint8_t buf[MSG_SIZE];
    if (Nrf24_dataReady(&dev)) {
        Nrf24_getData(&dev, buf);
        memcpy(&msg, buf, sizeof(RacketMessage_t));
        
        if(_handler) _handler(&msg); // run stored handler
    }
}

