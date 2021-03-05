
//Curiosity LEDs: 4,5,6,7   RA5     RA1     RA2     RC5
//          PIN:            2       18      17      5

//        Test minsta ADC värde: 39. Max 1023.

//                          M FLÄKT     UTAN
//         Test MAX temp:   38          78C
//              MIN         21          21
//              DIFF        17          57
//              1/DIFF      0,0588      0,0175   
//              /250                    7,0175e-5
//
/* PWM  Temp u. fläkt
 * 0%   22C
 * 25%  44C
 * 50%  61C

*/




//#include <xc.h>
#include "mcc_generated_files/mcc.h"
#include <math.h>

#define BTIME 100

//-------------------------- VARIABLES ----------------------

unsigned char ledstate = 0;
unsigned char button_pressed = 0;
unsigned char timer1_bang = 0;
//int pot_value = 0;
//int ntc_value = 0;
double temp = 0;
double pot_pos = 0;
double target_temp = 30;
double step_size = 0.01;
double duty = 0;
//double exponent = 0;
double temp_old = 0;
double temp_delta = 0;
int delta_int = 0;
int pwm_was_maxed = 0;
int temp_was_right = 0;
double step_factor = 0.0001;

//-------------------------- FUNCTIONS ---------------------

void set_button_pressed(){
    button_pressed = 1;
}

void reset_button_pressed(){
    button_pressed = 0;
}



double getTemp(){
    double ntc = (double)ADC_GetConversion(NTC);
    ntc -= 39; //kompensera för nollnivå som inte är noll
    if (ntc < 1.0) ntc = 1.0;
    double ntc_a = 0.003354016;
    double ntc_b = 0.0002744032;
    double resistor = 9850.0;
    double ntc_resistance = resistor * ((984.0/ntc) - 1.0); //984 eftersom nollpunkten har offset
    // resistans 24399 ok
    double ratio = ntc_resistance / (22000.0); //22000 is NTC reference resistance at 25 celsius;
    double log_ratio = log(ratio);
    return (1.0 / (ntc_a + (ntc_b * log_ratio) )) - 273.15;
}

void timer1SetBang(){
    timer1_bang = 1;
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

void blink_1000(){
    leds_off();
    LED_D7_SetHigh();
    LED_D6_SetLow();
    LED_D5_SetLow();
    LED_D4_SetLow();
    __delay_ms(50);
    leds_off();
}

void blink_0001(){
    leds_off();
    LED_D7_SetLow();
    LED_D6_SetLow();
    LED_D5_SetLow();
    LED_D4_SetHigh();
    __delay_ms(50);
    leds_off();
}

//void blink_temp(void){
//    unsigned int current_value;
//    current_value = ADC_GetConversion(NTC);
//    current_value = current_value >> 6;
//    unsigned int temp = (unsigned int)calc_temp(current_value);
//    leds_off();
//    for(int pos=0; pos<4; pos++){
//        blink_1();
//        set_leds(temp);
//        __delay_ms(1500);
//        leds_off();
//        temp = temp << 4;
//    }
//    __delay_ms(2500);
//}

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



//------------------------------ MAIN --------------------
void main(void)
{
    // initialize stuff
    SYSTEM_Initialize();
    TMR1_Initialize();
    INTERRUPT_GlobalInterruptEnable();  // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    INTERRUPT_PeripheralInterruptEnable();  // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    TMR1_SetInterruptHandler(timer1SetBang);
    INT_SetInterruptHandler(set_button_pressed);
    EXT_INT_fallingEdgeSet();
    EUSART_Initialize();

    TMR1_StartTimer();
    
    double temp_diff = 0;
    //---------------------------- MAIN LOOP -----------------------------
    while (1)
    {
        if (button_pressed == 1){
            //blink_temp();
            reset_button_pressed();
        }
        
        //get potentiometer, temperature
        pot_pos = ADC_GetConversion(POT) / 1023.0;
        temp = getTemp();
        
        // 1-sec timer
        if (timer1_bang == 1){
            timer1_bang = 0;
            temp_delta = temp - temp_old;
            temp_old = temp;
        }
        
        target_temp = (pot_pos * 25)+25;
        temp_diff = target_temp - temp;
        step_size =  temp_diff * step_factor;
        
        if ((int)round(temp_diff == 0)) temp_was_right = 1;
        
        //limit step size
        if (step_size > 1) step_size = 1;
        if (step_size < -1) step_size = -1;
        
        
        //PWM
        duty += step_size; //duty 0...1
        if (duty >= 1){
        duty = 1;
        pwm_was_maxed = 1;
        }
        if (duty <= 0){
            duty = 0;
            pwm_was_maxed = 1;
        }
        
        if(pwm_was_maxed && temp_was_right){
            pwm_was_maxed = 0;
            temp_was_right = 0;
            //duty = 0.5;
            step_factor *= 0.5;
        }
        
        PWM3_LoadDutyValue((int)round(duty*1023));
        //PWM3_LoadDutyValue((int)round(0.25*1023));
        
        
        
        //EUSART debug output
        EUSART_Write((int)round(temp));
        //__delay_ms(1);
//        if (temp_delta < 0) temp_delta = -temp_delta;
//        EUSART_Write((int)round(temp_delta*10));
        
        //status LEDs
        if (step_size > 0){
            LED_D6_SetLow();            
            LED_D5_SetHigh();
        }
        if (step_size < 0){
            LED_D6_SetHigh();
            LED_D5_SetLow();
        }
        if (step_size == 0){
            LED_D6_SetLow();
            LED_D5_SetLow();
        }
        if (duty == 1) LED_D4_SetHigh();
        else LED_D4_SetLow();

        if (duty == 0) LED_D7_SetHigh();
        else LED_D7_SetLow();

        //__delay_ms(10);
    }
}
/**
 End of File
*/