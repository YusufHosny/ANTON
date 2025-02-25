#include "config.h"

#include "udp.h"
#include "stepper.h"
#include "nrf.h"

QueueHandle_t stepQueue, racketQueue;

void set_up() {
	wifi_setup();
	vTaskDelay(5000 / portTICK_PERIOD_MS);

	racketQueue = xQueueCreate(1, sizeof(RacketMessage_t));
	stepQueue = xQueueCreate(1, sizeof(StepMessage_t));

}


void app_main(void)
{	
	set_up();

	#if NRF
	xTaskCreate(&nrf_task, "nrf_task", 1024*3, NULL, 4, NULL);
	#endif
	#if UDP
    xTaskCreate(&udp_client_task, "udp_client_task", 4096, NULL, 3, NULL);
	#endif
	#if STEPPER
	xTaskCreate(&stepper_controller_task, "stepper_controller", 4096, NULL, 3, NULL);
	#endif
}