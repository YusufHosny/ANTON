/* 
 * File:   comms.h
 * Author: Yusuf
 *
 * Created on March 25, 2025, 3:18 PM
 */

#ifndef COMMS_H
#define	COMMS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "config.h"

typedef void (*RacketMessageHandler_t)(RacketMessage_t*);
    
    
void initialize_comms();
void set_comms_handler(RacketMessageHandler_t handler);    
void step_comms();

#ifdef	__cplusplus
}
#endif

#endif	/* COMMS_H */

