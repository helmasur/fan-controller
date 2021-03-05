/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs 

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs  - 1.45
        Device            :  PIC16F1619
        Driver Version    :  2.00
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.35
        MPLAB             :  MPLAB X 3.40
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

//#include <xc.h>
#include "mcc_generated_files/mcc.h"
#include <math.h>

#define BTIME 100

unsigned char ledstate = 0;
unsigned char button_pressed = 0;

void set_button_pressed(){
    button_pressed = 1;
}

void reset_button_pressed(){
    button_pressed = 0;
}

unsigned int calc_temp(unsigned int adc_val_int){
    double adc_val = (double)adc_val_int;
    adc_val -= 39; //kompensera för nollnivå som inte är noll
    if (adc_val < 1.0) adc_val = 1.0;
    double ntc_a = 0.003354016;
    double ntc_b = 0.0002744032;
    double resistor = 9850.0;
    double ntc_resistance = resistor * ((984.0/adc_val) - 1.0); //984 efterson nollpunkten har offset
    // resistans 24399 ok
    double ratio = ntc_resistance / (22000.0); //22000 is NTC reference resistance at 25 celsius;
    double log_ratio = log(ratio);
    double temp = 1.0 / (ntc_a + (ntc_b * log_ratio) );
    temp -= 273.15;
    temp *= 10.0;
    return (int)temp;
}

void leds_on(){
    LED_D4_SetHigh();
    LED_D5_SetHigh();
    LED_D6_SetHigh();
    LED_D7_SetHigh();
}

void leds_off(){
    LED_D4_SetLow();
    LED_D5_SetLow();
    LED_D6_SetLow();
    LED_D7_SetLow();
}

void blink_0(){
    LED_D7_SetHigh();
    __delay_ms(BTIME);
    LED_D7_SetLow();
    __delay_ms(BTIME);
}

void set_leds(int value){
    if (value & 0b1000000000000000) LED_D7_SetHigh();
    else LED_D7_SetLow();
    if (value & 0b0100000000000000) LED_D6_SetHigh();
    else LED_D6_SetLow();
    if (value & 0b0010000000000000) LED_D5_SetHigh();
    else LED_D5_SetLow();
    if (value & 0b0001000000000000) LED_D4_SetHigh();
    else LED_D4_SetLow();
}

void blink_1(){
    leds_on();
    __delay_ms(BTIME);
    leds_off();
    __delay_ms(BTIME);
}

void blink_int(unsigned int value){
    leds_off();
    blink_1();
    __delay_ms(500);
    for(int pos=0; pos<4; pos++){
        blink_1();
        set_leds(value);
        __delay_ms(1500);
        leds_off();
        __delay_ms(150);
        value = value << 4;
    }
    __delay_ms(2000);
}

void blink_temp(void){
    unsigned int current_value;
    current_value = ADC_GetConversion(NTC);
    current_value = current_value >> 6;
    unsigned int temp = (unsigned int)calc_temp(current_value);
    leds_off();
    for(int pos=0; pos<4; pos++){
        blink_1();
        set_leds(temp);
        __delay_ms(1500);
        leds_off();
        temp = temp << 4;
    }
    __delay_ms(2500);
}

void blink_current_int(void){
    unsigned int current_value;
    current_value = ADC_GetConversion(NTC);
    current_value = current_value >> 6;
//    current_value = 0b1111000011001010;
    leds_off();
    blink_1();
    __delay_ms(500);
    for(int pos=0; pos<4; pos++){
        blink_1();
        set_leds(current_value);
        __delay_ms(1500);
        leds_off();
        __delay_ms(150);
        current_value = current_value << 4;
    }
    __delay_ms(2000);
}

void seg_off(){
//    IO_RC1_SetHigh();
    IO_RC2_SetHigh();
    IO_RB4_SetHigh();
    IO_RB5_SetHigh();
    IO_RB6_SetHigh();
    IO_RC7_SetHigh();
    IO_RC6_SetHigh();
    IO_RC3_SetHigh();
}

void seg_nr(char num){
    seg_off();
    if (num==1){
        IO_RC7_SetLow();
        IO_RC6_SetLow();
    }
    else if (num==2){
        
    }
}

void switchleds(){
        if (ledstate == 1){
            leds_off();
            ledstate = 0;
        }
        else{
            leds_on();
            ledstate = 1;
        }
    }




/*
                         Main application
 */
void main(void)
{
    int adcResult;
    unsigned int temp = 0;
    
    // initialize the device
    SYSTEM_Initialize();

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    
//    TMR1_SetInterruptHandler(switchleds);
    //Enable the TMR0 Interrupts 
//    TMR1IE = 1;
    INT_SetInterruptHandler(set_button_pressed);
    EXT_INT_fallingEdgeSet();
    
    

    
    
    
//    662 689     659 704 706
    
    
    IO_RA0_SetHigh();       //tänder led
//    IO_RC1_SetLow();
    IO_RC2_SetLow();
    IO_RB4_SetLow();
    IO_RB5_SetLow();
    IO_RB6_SetLow();
    IO_RC7_SetLow();
    IO_RC6_SetLow();
    IO_RC3_SetLow();
    
    
    
    while (1)
    {
        if (button_pressed == 1){
            blink_temp();
            reset_button_pressed();
        }
        
        
//        __delay_ms(50);
        
//        __delay_ms(950);
        
        //Curiosity LEDs: 4,5,6,7   RA5     RA1     RA2     RC5
        //          PIN:            2       18      17      5
        
//        Test minsta ADC värde: 39. Max 1023.
        
        
        //Start ADC conversion
        adcResult = ADC_GetConversion(NTC);
        
        
        
        adcResult = adcResult >> 6;
//        adcResult -= 650;
//        adcResult *= 17;
        

        //Make the adcResult the PWM duty cycle
//        PWM3_LoadDutyValue(adcResult);
        temp = calc_temp(adcResult);
    }
}
/**
 End of File
*/