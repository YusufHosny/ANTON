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
#define NRF 1
#define STEPPER 1
#define MOCK 0

extern QueueHandle_t vstepQueue, hstepQueue, racketQueue;

// types {0: normal packet, 1: manual packet, position is 0./1. for direction, 2: reset packet, stops gantry in that axis}
typedef enum StepMessageType {
    NORMAL, MANUAL, RESET
} StepMessageType_t ;

// size 16 bytes
typedef struct StepMessage {
    StepMessageType_t type; // offset 0x0: 4 bytes then 4 padding
    double position; // offset 0x8: 8 byte value
} StepMessage_t;

// size 8 bytes
typedef struct RacketMessage {
    float angle; // offset 0x0: 4 bytes
    bool fire;  // offset 0x4: 1 byte then 3 padding
} RacketMessage_t;

// size 2 * stepmsg + rmsg = 40 bytes
typedef struct Packet {
    StepMessage_t hMsg, vMsg; // 0x0 then 0x10
    RacketMessage_t rMsg; // 0x20->0x28
} Packet_t;


#endif // CONFIG_H