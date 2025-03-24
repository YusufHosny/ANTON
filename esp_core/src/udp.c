/*
References:
https://github.com/SIMS-IOT-Devices/FreeRTOS-ESP-IDF-5.0-Socket/blob/main/UDP%20Socket%20Client.c
*/

#include "udp.h"

#include "config.h"

#define SSID "AAAAAAARGHHHHHHHA"
#define PASS "wordpass12"
#define HOST_IP_ADDR "192.168.137.1"
#define PORT 3201

#define MESSAGE_SIZE 24+24+16

void udp_client_task(void *pvParameters) {
	// initialize information
    char rx_buffer[MESSAGE_SIZE];
    char host_ip[] = HOST_IP_ADDR;
    int addr_family = 0;
    int ip_protocol = 0;

	// initialize socket
	struct sockaddr_in dest_addr;
	dest_addr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);
	addr_family = AF_INET;
	ip_protocol = IPPROTO_IP;

	int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
	if (sock < 0) {
		ESP_LOGE(pcTaskGetName(NULL), "Unable to create socket: errno %d", errno);
		vTaskDelete(NULL);
		return;
	}

	// Set timeout
	struct timeval timeout;
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

	ESP_LOGI(pcTaskGetName(NULL), "Socket created, sending to %s:%d", host_ip, PORT);

	// send start bytes
	char* startBytes = "strt\n";
	int err = sendto(sock, startBytes, strlen(startBytes), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
	if (err < 0) {
		ESP_LOGE(pcTaskGetName(NULL), "Error occurred during sending start bytes: errno %d", errno);
	}
	ESP_LOGI(pcTaskGetName(NULL), "start bytes sent");

	// recv data continuously until timeout (server shutdown)
	while (1) {
		struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
		socklen_t socklen = sizeof(source_addr);
		int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

		// Error occurred during receiving
		if (len < 0) {
			ESP_LOGE(pcTaskGetName(NULL), "recvfrom failed: errno %d", errno);
			break;
		}
		// Data received
		else {
			StepMessage_t hMsg, vMsg;
			RacketMessage_t rMsg;

			memcpy(&hMsg, rx_buffer, sizeof(hMsg));
			memcpy(&vMsg, rx_buffer+24, sizeof(vMsg));
			memcpy(&rMsg, rx_buffer+48, sizeof(rMsg));

			// send messages
			xQueueSendToFront(hstepQueue, &hMsg, 0);
			xQueueSendToFront(vstepQueue, &vMsg, 0);
			xQueueSendToFront(racketQueue, &rMsg, 0);
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}

	ESP_LOGE(pcTaskGetName(NULL), "Shutting down socket and exiting...");
	shutdown(sock, 0);
	close(sock);
    vTaskDelete(NULL);
}

void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        printf("WiFi connecting WIFI_EVENT_STA_START ... \n");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        printf("WiFi connected WIFI_EVENT_STA_CONNECTED ... \n");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("WiFi lost connection WIFI_EVENT_STA_DISCONNECTED ... \n");
        break;
    case IP_EVENT_STA_GOT_IP:
        printf("WiFi got IP ... \n\n");
        break;
    default:
        break;
    }
}

void wifi_setup()
{
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation);
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = SSID,
            .password = PASS}};
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    esp_wifi_connect();
}