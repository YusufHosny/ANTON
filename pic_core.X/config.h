/* 
 * File:   config.h
 * Author: Yusuf
 *
 * Created on March 25, 2025, 3:08 PM
 */

#ifndef CONFIG_H
#define	CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
    
#include "mcc_generated_files/system/system.h"
    
// no padding: 5 bytes
typedef struct RacketMessage {
    float angle; // 4 bytes
    bool fire; // 1 byte
} RacketMessage_t;


#ifdef	__cplusplus
}
#endif

#endif	/* CONFIG_H */

