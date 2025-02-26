#include "mock.h"

#include "config.h"


void mock_stepmessasges(void *pvParameters) {

    StepMessage_t sMsg;
    forever {
        // alternate position from 0.1 to 0.9 every 2s
        sMsg.position = (sMsg.position == 0.1) ? 0.9 : 0.1;
        sMsg.update = true;
        sMsg.urgency = 10;

        vTaskDelay(2000 / portTICK_PERIOD_MS);

        xQueueSendToFront(stepQueue, &sMsg, 0);
    }
}

void mock_udp_task(void *pvParameters) {
    StepMessage_t sMsg;
    RacketMessage_t rMsg;
    bool fire = false;
    forever {
        // alternate position from 0.1 to 0.9 every 2s
        sMsg.position = (sMsg.position == 0.1) ? 0.9 : 0.1;
        sMsg.update = true;
        sMsg.urgency = 10;

        // alternate angle and firing
        rMsg.angle = (rMsg.angle == 40) ? 120 : 40;
        rMsg.fire = (fire = !fire);

        vTaskDelay(2000 / portTICK_PERIOD_MS);

        xQueueSendToFront(stepQueue, &sMsg, 0);
        xQueueSendToFront(racketQueue, &rMsg, 0);

        ESP_LOGI(pcTaskGetName(NULL), "Sent Messages.");                  
    }
}


void mock_stepper_task(void *pvParameters) {
    StepMessage_t msg;
	forever
	{   
		vTaskDelay(10 / portTICK_PERIOD_MS);

        if(xQueueReceive(stepQueue, &msg, 0)) { 
            ESP_LOGI(pcTaskGetName(NULL), "StepMessage(%d %f %d)", msg.update, msg.position, msg.urgency);                  
        }
		
	}
}

void mock_nrf_task(void *pvParameters) {
    RacketMessage_t msg;
	forever
	{   
		vTaskDelay(10 / portTICK_PERIOD_MS);

        if(xQueueReceive(racketQueue, &msg, 0)) { 
            ESP_LOGI(pcTaskGetName(NULL), "RacketMessage(%f %d)", msg.angle, msg.fire);                  
        }
		
	}
}
