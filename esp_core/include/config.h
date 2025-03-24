#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/ip_addr.h"

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_mac.h"
#include "esp_eth.h"
#include "esp_netif.h"

#include "led_strip.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "ping/ping_sock.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"

#define forever for(;;)

#define DEBUG 1

// enable and disable submodules
#define UDP 1
#define NRF 0
#define STEPPER 0
#define MOCK 1

extern QueueHandle_t vstepQueue, hstepQueue, racketQueue;

// size 24 bytes
typedef struct StepMessage {
    bool update; // offset 0x0: 1 byte then 7 padding
    double position; // offset 0x8: 8 byte value
    uint8_t urgency; // offset 0x10: 1 byte then 7 padding
} StepMessage_t;

// size 16 bytes
typedef struct RacketMessage {
    double angle; // offset 0x0: 8 bytes
    bool fire; // offset 0x8: 1 byte then 7 padding
} RacketMessage_t;


#endif // CONFIG_H