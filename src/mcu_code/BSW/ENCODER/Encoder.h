/**********************************************************************************************************************
 * file Encoder.h
 * copyright Copyright (C) Infineon Technologies AG 2019
 * Boost Software License - Version 1.0 - August 17th, 2003
 *********************************************************************************************************************/
#ifndef ENCODER_H_
#define ENCODER_H_

#include "IfxPort_reg.h"
#include "IfxScu_reg.h"
#include "IfxSrc_reg.h"
#include "IfxCpu.h"
#include "Isr_Priority.h"
#include "Platform_Types.h"
#include "Stm.h"
#include "Can.h"

// Function Prototypes
int Encoder_Get_Rpm0_Left(void);
int Encoder_Get_Rpm1_Right(void);

void Encoder_Init(void);
void Encoder_Init_Stm_100ms(void);

// void Encoder_Enc0_Isr_Handler_Enc0(void);
// void Encoder_Enc1_Isr_Handler_Enc0(void);
// void Encoder_Stm1_Isr_Handler(void);

#endif /* ENCODER_H_ */
