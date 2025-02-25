#ifndef CONFIG_H
#define CONFIG_H

#define DEBUG 1

// enable and disable submodules
#define UDP 1
#define NRF 1
#define STEPPER 1

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

extern QueueHandle_t stepQueue, racketQueue;

typedef struct StepMessage {
    bool update;
    double position;
    uint8_t urgency;
} StepMessage_t;

typedef struct RacketMessage {
    double angle;
    bool fire;
} RacketMessage_t;


#endif // CONFIG_H