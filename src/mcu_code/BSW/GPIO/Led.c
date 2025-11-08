/**********************************************************************************************************************
 * file Led.c
 * copyright Copyright (C) Infineon Technologies AG 2019
 * Boost Software License - Version 1.0 - August 17th, 2003
 *********************************************************************************************************************/
#include "Led.h"

void Led_Init(void){
    //LED1
    MODULE_P23.IOCR0.B.PC1 = 0x10; // P23.1 LED1 Output setting
    MODULE_P23.OUT.B.P1 = 0; // LED1 OFF is default;

    //LED2
    MODULE_P23.IOCR0.B.PC3 = 0x10; // P23.3 LED2 Output setting
    MODULE_P23.OUT.B.P3 = 0; // LED2 OFF is default;
}

void Led_Set(unsigned char num_LED, unsigned char onOff){
    if (num_LED == 1){
        MODULE_P23.OUT.B.P1 = onOff;
    }
    else if (num_LED == 2){
        MODULE_P23.OUT.B.P3 = onOff;
    }
}

void Led_Toggle(unsigned char num_LED){
    if (num_LED == 1){
        MODULE_P23.OUT.B.P1 ^= 1; // LED1 ONOFF
    }
    else if (num_LED == 2){
        MODULE_P23.OUT.B.P3 ^= 1; // LED1 OFF is default;
    }
}

void delay_ms(unsigned int ms)
{
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, ms));    /* Wait 500 milliseconds            */
}
