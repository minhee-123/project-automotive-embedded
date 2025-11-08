/**********************************************************************************************************************
 * file LED.h
 * copyright Copyright (C) Infineon Technologies AG 2019
 * Boost Software License - Version 1.0 - August 17th, 2003
 *********************************************************************************************************************/
#ifndef LED_H_
#define LED_H_

#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "Bsp.h"
#include "Gpt12.h"

// Function Prototypes
void delay_ms(unsigned int ms); //명명규칙 예외 (디버깅에서 주로 사용, 자주 사용되기 때문)

void Led_Init(void);
void Led_Set(unsigned char num_LED, unsigned char onOff);
void Led_Toggle(unsigned char num_LED);

//void Led_Isr_Gpt2_T6_Handler(void);

#endif /* LED_H_ */
