#include "mock.h"
#include "config.h"

void mock_udp_task(void *pvParameters) {
    StepMessage_t sMsg;
    RacketMessage_t rMsg;
    bool fire = false;
    forever {
        // alternate position from 0.1 to 0.9 every 2s
        sMsg.type = NORMAL;
        sMsg.position = (sMsg.position == 0.1) ? 0.9 : 0.1;
        

        // alternate angle and firing
        rMsg.angle = (rMsg.angle == 40) ? 120 : 40;
        rMsg.fire = (fire = !fire);

        vTaskDelay(6000 / portTICK_PERIOD_MS);

        xQueueSendToFront(hstepQueue, &sMsg, 0);
        xQueueSendToFront(vstepQueue, &sMsg, 0);
        xQueueSendToFront(racketQueue, &rMsg, 0);

        ESP_LOGI(pcTaskGetName(NULL), "Sent Messages.");                  
    }
}


void mock_stepper_task(void *pvParameters) {
    StepMessage_t msg;
	forever
	{   
		vTaskDelay(10 / portTICK_PERIOD_MS);

        if(xQueueReceive(hstepQueue, &msg, 0)) { 
            ESP_LOGI(pcTaskGetName(NULL), "Horizontal StepMessage(%d %f)", msg.type, msg.position);                  
        }

        if(xQueueReceive(vstepQueue, &msg, 0)) { 
            ESP_LOGI(pcTaskGetName(NULL), "Vertical StepMessage(%d %f)", msg.type, msg.position);                  
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
