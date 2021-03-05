/* 
 * File:   timers.h
 * Author: Martin
 *
 * Created on den 27 mars 2018, 11:36
 */

#ifndef TIMERS_H
#define	TIMERS_H

#ifdef	__cplusplus
extern "C" {
#endif
    
uint8_t timer1_250msBang;
uint8_t timer1_1sBang;
uint8_t timer1_5sBang;
uint8_t timer1_10sBang;

uint8_t timer2_250msBang;
uint8_t timer2_1sBang;
uint8_t timer2_5sBang;
uint8_t timer2_10sBang;

void timer1_250msBangSet();
void timer2_250msBangSet();


#ifdef	__cplusplus
}
#endif

#endif	/* TIMERS_H */

