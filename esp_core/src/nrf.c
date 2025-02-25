/*
References:
https://github.com/nopnop2002/esp-idf-mirf/tree/master/components/mirf
*/

#include "nrf.h"

#include <mirf.h>

#define CONFIG_RADIO_CHANNEL 115


void nrf_task(void *pvParameters)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	NRF24_t dev;
	Nrf24_init(&dev);
	uint8_t payload = sizeof(RacketMessage_t);
	uint8_t channel = CONFIG_RADIO_CHANNEL;
	Nrf24_config(&dev, channel, payload);

	// Set destination address using 5 characters
	esp_err_t ret = Nrf24_setTADDR(&dev, (uint8_t *)"FGHIJ");
	if (ret != ESP_OK) {
		ESP_LOGE(pcTaskGetName(NULL), "nrf24l01 not installed");
		vTaskDelete(NULL);
	}

	ESP_LOGW(pcTaskGetName(NULL), "Set RF Data Ratio to 1MBps");
	Nrf24_SetSpeedDataRates(&dev, 0);

	ESP_LOGW(pcTaskGetName(NULL), "Set retransmit delay register to %d", 1);
	Nrf24_setRetransmitDelay(&dev, 1);

	// Print settings
	Nrf24_printDetails(&dev);

	while(1) {
        RacketMessage_t msg;
        xQueueReceive(racketQueue, &msg, 0);

		Nrf24_send(&dev, (uint8_t*) &msg);

        vTaskDelay(1000/portTICK_PERIOD_MS); // TODO ADJUST DELAY
	}

	vTaskDelete(NULL);
}