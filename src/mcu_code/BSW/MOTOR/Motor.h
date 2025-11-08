/**********************************************************************************************************************
 * file Motor.h
 * copyright Copyright (C) Infineon Technologies AG 2019
 * Boost Software License - Version 1.0 - August 17th, 2003
 *********************************************************************************************************************/
#ifndef MOTOR_H_
#define MOTOR_H_

#include "Gtm_Atom_Pwm.h"
#include "Stm.h"
#include "Isr_Priority.h"
#include "IfxCpu.h"
#include "Ifx_Types.h"
#include "Can.h"

// Function Prototypes
void Motor_Init(void);

void Motor_Set_Left(uint8 dir, uint8 duty);
void Motor_Stop_Left(void);

void Motor_Set_Right(uint8 dir, uint8 duty);
void Motor_Stop_Right(void);

#endif /* MOTOR_H_ */
