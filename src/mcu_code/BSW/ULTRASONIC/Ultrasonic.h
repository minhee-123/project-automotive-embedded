/**********************************************************************************************************************
 * file Ultrasonic.h
 * copyright Copyright (C) Infineon Technologies AG 2019
 * Boost Software License - Version 1.0 - August 17th, 2003
 *********************************************************************************************************************/
#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#include "Stm.h"
#include "Isr_Priority.h"
#include "Ifx_Types.h"
#include "IfxCpu.h"

// Function Prototype
void Ultrasonic_Init(void);
float Ultrasonic_Get_Back_Dist(void);
float Ultrasonic_Get_Left_Dist(void);
float Ultrasonic_Get_Right_Dist(void);

//void Ultrasonic_Stm0_Isr_Handler(void);
//void Ultrasonic_Eru2_Isr_Handler(void)

#endif /* ULTRASONIC_H_ */
