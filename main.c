
/*  PIC16F1619 PWM fan controller utilizing one NTC resistor as sensor.
    Planned future (todo):  LCD display driver.
                            One more NTC for ambient temperature.
                            Four individually controlled fans.

*/


#include "fan.h"


//-------------------------- GLOBALS ----------------------

uint8_t button_pressed = 0;             // Interrupt from button on RC4 sets this to 1


float target_temp;                      // Wanted temperature, set by user via potentiometer on RC0

float temp;                             // Temperature from NTC, updated ~100/s (main loop)
float meanList[MEANSIZE];               // Values of temp to calculate a moving average
float tempAvg = 0;                      // Moving average temperature
float tempHistory[TEMPHISTORYSIZE];     // Historical samples of tempAvg

float tempDelta = 0;                    // Temperature difference over the last second
float temp_diff = 0;                    // Distance to target; target_temp - tempAvg

float potPos_u = 0;                     // Potentiometer position 0...1

float duty = .5;                        // Output duty cycle

uint8_t mode = 1;                       // Current mode of operation

// Mode 1
float numerator = 1, denominator = 2;
// Mode 2
float step_size = 0.01;



//-------------------------- PROTOTYPES ---------------------
void btnPressedSet();
void btnPressedReset();
float tempAvgMake(float value);
void tempHistoryMake(float value);
float tempGet();
treeRight();
treeLeft();
ledStatus();
void modeSwitch(char newMode);

//------------------------------ MAIN ---------------------
void main(void)
{
    // Initialize
    SYSTEM_Initialize();
    TMR1_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    TMR1_SetInterruptHandler(timer1_250msBangSet);
    INT_SetInterruptHandler(btnPressedSet);
    EXT_INT_fallingEdgeSet();
    EUSART_Initialize();

    TMR1_StartTimer();

    // Initialize arrays to avoid garbage values
    for (uint8_t i = 0; i<MEANSIZE; i++){
        meanList[i] = 25;
    }
    for (uint8_t i = 0; i<TEMPHISTORYSIZE; i++){
        tempHistory[i] = 25;
    }

    //---------------------------- MAIN LOOP -----------------------------
    while (1)
    {
        // Potentiometer fetch
        potPos_u = ADC_GetConversion(POT) / 1023.0;
        target_temp = (potPos_u * 25)+25;

        // Temparature fetch
        temp = tempGet();                   // get temperature
        tempAvg = tempAvgMake(temp);        // make moving average
        temp_diff = target_temp - tempAvg;  // calc distance to target
        if(timer1_250msBang){               // store temp 4 times per second
            tempHistoryMake(tempAvg);
            timer1_250msBang = 0;
        }

        // Status output
        ledStatus();
        EUSART_Write((int16_t)round(tempAvg));

        /* MODE 1, the quick fix, seeks a rough estimate of the right duty cycle
           by successive halving. */
        if (mode == 1){
            duty = numerator / denominator;
            if (timer1_5sBang){
                timer1_5sBang = 0;
                if ((temp_diff < -2)&&(tempDelta > 0)) treeLeft();
                else if ((temp_diff > 2)&&(tempDelta < 0)) treeRight();
            }
            if (abs((int16_t)(round(temp_diff))) < 2) modeSwitch(2);

        }

        /* MODE 2, an act of balance, adjusts duty cycle in small values depending on
           the temperature movement. */
        else if (mode == 2){
            if (abs((int16_t)temp_diff) > 4) modeSwitch(1); // If target is too far away, switch mode.

            step_size = temp_diff*0.00001;

            if (step_size > 1) step_size = 1;
            else if (step_size < -1) step_size = -1;

            duty += step_size; //duty 0...1
            if (duty > 1){
                duty = 1;
            }
            else if (duty < 0){
                duty = 0;
            }
        }

        // PWM Output
        PWM3_LoadDutyValue((uint16_t)round(duty*1023));
    }
}

//--------------- FUNCTIONS -------------------
void btnPressedSet(){
    button_pressed = 1;
}

void btnPressedReset(){
    button_pressed = 0;
}



float tempAvgMake(float value){
    static uint16_t nextEntry = 0;
    static float sum = 0;
    sum -= meanList[nextEntry];
    meanList[nextEntry] = value;
    sum += value;
    nextEntry++;
    if (nextEntry == MEANSIZE) nextEntry = 0;
    return (sum/MEANSIZE);
}

void tempHistoryMake(float value){
    static uint8_t nextEntry = 4;
    static uint8_t old_1s_entry = 0;

    // calculate delta of last second
    tempDelta = value - tempHistory[old_1s_entry];

    // insert value into history
    tempHistory[nextEntry] = value;
    nextEntry++;
    if (nextEntry == TEMPHISTORYSIZE) nextEntry = 0;
}


float tempGet(){
    static const float ntc_a = 0.003354016;
    static const float ntc_b = 0.0002744032;
    static const float resistor = 9850.0;

    float ntc = (float)ADC_GetConversion(NTC);
    ntc -= 39; //kompensera f�r nollniv� som inte �r noll
    if (ntc < 1.0) ntc = 1.0;

    float ntc_resistance = resistor * ((984.0/ntc) - 1.0); //984 eftersom nollpunkten har offset
    // resistans 24399 ok
    float ratio = ntc_resistance / (22000.0); //22000 is NTC reference resistance at 25 celsius;
    float log_ratio = log(ratio);
    return (1.0 / (ntc_a + (ntc_b * log_ratio) )) - 273.15;
}

treeRight(){
    numerator *= 2;
    denominator *= 2;
    numerator++;
}

treeLeft(){
    numerator *= 2;
    denominator *= 2;
    numerator--;
}

ledStatus(){
    if (round(temp_diff) == 0){
        LED_D6_SetLow(); LED_D5_SetLow();
    }
    else if (temp_diff > 0){    // temp is below target
        LED_D6_SetLow(); LED_D5_SetHigh();
    }
    else if (temp_diff < 0){    // temp is above target
        LED_D6_SetHigh(); LED_D5_SetLow();
    }

    if (mode == 1) LED_D7_SetHigh();
    else LED_D7_SetLow();

    if (mode == 2) LED_D4_SetHigh();
    else LED_D4_SetLow();
}

void modeSwitch(char newMode){
    if (newMode == 1){
        numerator = 1; denominator = 2;
        mode = 1;
    }
    if (newMode == 2){
        mode = 2;
    }
}
/**
 End of File
*/
