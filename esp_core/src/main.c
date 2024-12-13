/*
References:
https://github.com/SIMS-IOT-Devices/FreeRTOS-ESP-IDF-5.0-Socket/blob/main/UDP%20Socket%20Client.c
https://github.com/nopnop2002/esp-idf-mirf/tree/master/components/mirf
*/

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_mac.h"
#include "esp_eth.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "esp_http_client.h"
#include "esp_event.h"
#include "esp_system.h"

#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/ip_addr.h"

#include "led_strip.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "ping/ping_sock.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"

#include "mirf.h"

/*
NRF24 Sender Task Definition
*/
#define CONFIG_RADIO_CHANNEL 115
void nrfsender(void *pvParameters)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	NRF24_t dev;
	Nrf24_init(&dev);
	uint8_t payload = 32;
	uint8_t channel = CONFIG_RADIO_CHANNEL;
	Nrf24_config(&dev, channel, payload);

	// Set destination address using 5 characters
	esp_err_t ret = Nrf24_setTADDR(&dev, (uint8_t *)"FGHIJ");
	if (ret != ESP_OK) {
		ESP_LOGE(pcTaskGetName(NULL), "nrf24l01 not installed");
		while(1) { vTaskDelay(1); }
	}

	ESP_LOGW(pcTaskGetName(NULL), "Set RF Data Ratio to 1MBps");
	Nrf24_SetSpeedDataRates(&dev, 0);

	ESP_LOGW(pcTaskGetName(NULL), "Set retransmit delay register to %d", 1);
	Nrf24_setRetransmitDelay(&dev, 1);

	// Print settings
	Nrf24_printDetails(&dev);

	uint8_t buf[32];
	while(1) {
		TickType_t nowTick = xTaskGetTickCount();
		snprintf((char *)buf, sizeof(buf), "Hello World %"PRIu32, nowTick);
		Nrf24_send(&dev, buf);
		//vTaskDelay(1);
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}

	vTaskDelete(NULL);
}


/*
WI-FI UDP Client Task Definition
*/
#define SSID "AAAAAAARGHHHHHHHA"
#define PASS "wordpass12"
#define HOST_IP_ADDR "192.168.137.1"
#define PORT 3201
void udpclient(void *pvParameters) {
	// initialize information
    char rx_buffer[128];
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
		ESP_LOGE("UDP", "Unable to create socket: errno %d", errno);
		vTaskDelete(NULL);
		return;
	}

	// Set timeout
	struct timeval timeout;
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;
	setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

	ESP_LOGI("UDP", "Socket created, sending to %s:%d", host_ip, PORT);

	// send start bytes
	char* startBytes = "strt\n";
	int err = sendto(sock, startBytes, strlen(startBytes), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
	if (err < 0) {
		ESP_LOGE("UDP", "Error occurred during sending start bytes: errno %d", errno);
	}
	ESP_LOGI("UDP", "start bytes sent");

	// recv data continuously until timeout (server shutdown)
	while (1) {
		struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
		socklen_t socklen = sizeof(source_addr);
		int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

		// Error occurred during receiving
		if (len < 0) {
			ESP_LOGE("UDP", "recvfrom failed: errno %d", errno);
			break;
		}
		// Data received
		else {
			rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
			ESP_LOGI("UDP", "Received %d bytes from %s:", len, host_ip);
			ESP_LOGI("UDP", "%s", rx_buffer);
			if (strncmp(rx_buffer, "OK: ", 4) == 0) {
				ESP_LOGI("UDP", "Received expected message, reconnecting");
				break;
			}
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}

	ESP_LOGE("UDP", "Shutting down socket and exiting...");
	shutdown(sock, 0);
	close(sock);
    vTaskDelete(NULL);
}

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
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

void wifi_connection()
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


/*
Stepper Control Task Definition 
*/
gptimer_handle_t step_timer = NULL;
gptimer_config_t stp_timer_config = {
	.clk_src = GPTIMER_CLK_SRC_DEFAULT,
	.direction = GPTIMER_COUNT_UP,
	.resolution_hz = 1000000, // 1MHz, 1 tick=1us
};
gptimer_alarm_config_t stp_alarm_config = {
	.alarm_count = 1000000, // period = 1s
};
struct step_status_t {
	uint64_t period; // period of time in us to next step, used to control stepper speed
	uint8_t direction; // 0 or 1, to indicate direction
	uint64_t steps; // number of steps left
	uint8_t current; // 0 or 1, whether the current step pin is high or low
} step_status = {
	.period = 1000000
};


static bool IRAM_ATTR stp_timer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_awoken = pdFALSE;

	ESP_LOGI("DEBUG", "STILL GOING %d", 0);

	// step_status.current = !step_status.current;
	// gpio_set_level(GPIO_NUM_48, step_status.current);
    
    // return whether we need to yield at the end of ISR
    return (high_task_awoken == pdTRUE);
}
gptimer_event_callbacks_t stp_callbacks = {
	.on_alarm = stp_timer_callback,
};
void initialize_stepper_timer() {
	ESP_LOGI("StepperCtrlr", "Create timer handle");
    ESP_ERROR_CHECK(gptimer_new_timer(&stp_timer_config, &step_timer));
	
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(step_timer, &stp_callbacks, NULL));

    ESP_LOGI("StepperCtrlr", "Enable timer");
    ESP_ERROR_CHECK(gptimer_enable(step_timer));

    ESP_LOGI("StepperCtrlr", "Start timer");
    //ESP_ERROR_CHECK(gptimer_set_alarm_action(step_timer, &stp_alarm_config));
    ESP_ERROR_CHECK(gptimer_start(step_timer));
}

void initialize_stepper_driver() {
	// TODO
}


void stepper_controller(void *pvParameters) {
	initialize_stepper_timer();

	// read the requested position from xqueue (from udp data)

	// calculate desired speed and steps required

	// update step status possibly in a mutex block

	// dummy led
	gpio_config_t io_conf = {
		io_conf.intr_type = GPIO_INTR_DISABLE,
		io_conf.mode = GPIO_MODE_OUTPUT,
		io_conf.pin_bit_mask = 1ULL << GPIO_NUM_48,
		io_conf.pull_down_en = 1,
    	io_conf.pull_up_en = 0
	};

	gpio_config(&io_conf);
	
	ESP_LOGI("DEBUG", "GOT HERE");
	int a = 0;
	while(1) {
		vTaskDelay( 1000 / portTICK_PERIOD_MS );
		ESP_LOGI("DEBUG", "LOOPING");
		a = !a;
		gpio_set_level(GPIO_NUM_48, a);
	}
 
	vTaskDelete(NULL);
}

void app_main(void)
{	
	vTaskDelay(4000 / portTICK_PERIOD_MS);
	// xTaskCreate(&nrfsender, "nrf_sender", 1024*3, NULL, 4, NULL);
	// wifi_connection();
    // vTaskDelay(5000 / portTICK_PERIOD_MS);
    // xTaskCreate(&udpclient, "udp_client", 4096, NULL, 3, NULL);
	xTaskCreate(&stepper_controller, "stepper_controller", 4096, NULL, 3, NULL);
}