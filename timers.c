#include "fan.h"

// Bang variables indicate that 
uint8_t timer1_250msBang = 0;
uint8_t timer1_1sBang = 0;
uint8_t timer1_5sBang = 0;
uint8_t timer1_10sBang = 0;

uint8_t timer2_250msBang = 0;
uint8_t timer2_1sBang = 0;
uint8_t timer2_5sBang = 0;
uint8_t timer2_10sBang = 0;

void timer1_250msBangSet(){
    
    timer1_250msBang = 1;
    
    static uint16_t timer1_1sCount = 0;
    static uint16_t timer1_5sCount = 0;
    static uint16_t timer1_10sCount = 0;
    
    timer1_1sCount++;
    timer1_5sCount++;
    timer1_10sCount++;
    
    if (timer1_1sCount == 4){
        timer1_1sBang = 1;
        timer1_1sCount = 0;
    }
    if (timer1_5sCount == 20){
        timer1_5sBang = 1;
        timer1_5sCount = 0;
    }
    if (timer1_10sCount == 40){
        timer1_10sBang = 1;
        timer1_10sCount = 0;
    }
}

void timer2_250msBangSet(){
    
    timer2_250msBang = 1;
    
    static uint16_t timer2_1sCount = 0;
    static uint16_t timer2_5sCount = 0;
    static uint16_t timer2_10sCount = 0;
    
    timer2_1sCount++;
    timer2_5sCount++;
    timer2_10sCount++;
    
    if (timer2_1sCount == 4){
        timer2_1sBang = 1;
        timer2_1sCount = 0;
    }
    if (timer2_5sCount == 20){
        timer2_5sBang = 1;
        timer2_5sCount = 0;
    }
    if (timer2_10sCount == 40){
        timer2_10sBang = 1;
        timer2_10sCount = 0;
    }
}