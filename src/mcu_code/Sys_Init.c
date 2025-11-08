/**********************************************************************************************************************
 * file Sys_Init.c
 * copyright Copyright (C) Infineon Technologies AG 2019
 * Boost Software License - Version 1.0 - August 17th, 2003
 *********************************************************************************************************************/
#include "Sys_Init.h"

void System_Init(void){
    IfxCpu_enableInterrupts();

    // !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());
    // Enable the watchdogs and service them periodically if it is required

    // Driver Init
    Asclin0_InitUart();
    Can_Init(BD_1M, CAN_NODE0); //CAN 사양에 따라 인자 수정
    Encoder_Init();
    Buzzer_Init();
    Led_Init();
    Gpt12_Gpt1_Init();
    Gpt12_Gpt2_Init();
    Motor_Init();
    Ultrasonic_Init();
}
