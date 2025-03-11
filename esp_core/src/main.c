#include "config.h"

#include "udp.h"
#include "stepper.h"
#include "nrf.h"

#if MOCK
#include "mock.h"
#endif

QueueHandle_t stepQueue, racketQueue;

void set_up() {
	#if WIFI
	wifi_setup();
	#endif
	
	racketQueue = xQueueCreate(1, sizeof(RacketMessage_t));
	stepQueue = xQueueCreate(1, sizeof(StepMessage_t));

	vTaskDelay(5000 / portTICK_PERIOD_MS);
}

void mock_tasks() {
	// xTaskCreate(&mock_stepmessasges, "mock_stepmessasges", 4096, NULL, 3, NULL);
	xTaskCreate(&mock_udp_task, "mock_udp_task", 4096, NULL, 3, NULL);
	// xTaskCreate(&mock_stepper_task, "mock_stepper_task", 4096, NULL, 3, NULL);
	xTaskCreate(&mock_nrf_task, "mock_nrf_task", 4096, NULL, 3, NULL);
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
	#if MOCK 
	mock_tasks();
	#endif
}