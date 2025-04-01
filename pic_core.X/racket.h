/* 
 * File:   racket.h
 * Author: Yusuf
 *
 * Created on March 25, 2025, 3:10 PM
 */

#ifndef RACKET_H
#define	RACKET_H

#ifdef	__cplusplus
extern "C" {
#endif

void initialize_racket();
void update_racket(RacketMessage_t *msg);

#ifdef	__cplusplus
}
#endif

#endif	/* RACKET_H */

