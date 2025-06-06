#include "config.h"

#include "udp.h"
#include "stepper.h"
#include "nrf.h"

#if MOCK
#include "mock.h"

void mock_tasks() {
	// xTaskCreate(&mock_udp_task, "mock_udp_task", 4096, NULL, 3, NULL);
	// xTaskCreate(&mock_stepper_task, "mock_stepper_task", 4096, NULL, 3, NULL);
	// xTaskCreate(&mock_nrf_task, "mock_nrf_task", 4096, NULL, 3, NULL); 
}
#endif

QueueHandle_t vstepQueue, hstepQueue, racketQueue;

void set_up() {
	#if UDP
	wifi_setup();
	#endif
	
	racketQueue = xQueueCreate(1, sizeof(RacketMessage_t));
	hstepQueue = xQueueCreate(1, sizeof(StepMessage_t));
	vstepQueue = xQueueCreate(1, sizeof(StepMessage_t));


	vTaskDelay(5000 / portTICK_PERIOD_MS);
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