/* 
 * File:   fan.h
 * Author: Martin
 *
 * Created on den 27 mars 2018, 11:34
 */

#ifndef FANHEADER_H
#define	FANHEADER_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <xc.h>
#include "mcc_generated_files/mcc.h"
#include <math.h>
#include <stdlib.h>
#include "timers.h"
    
//#define BTIME 100
#define MEANSIZE 50             // Number of terms used to calculate moving mean temperature
#define TEMPHISTORYSIZE 40      // Number of samples to include in the temperature history, 4=1s   40=10s


#ifdef	__cplusplus
}
#endif

#endif	/* FANHEADER_H */

