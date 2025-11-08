/**********************************************************************************************************************
 * file Sys_Init.h
 * copyright Copyright (C) Infineon Technologies AG 2019
 * Boost Software License - Version 1.0 - August 17th, 2003
 *********************************************************************************************************************/
#ifndef SYS_INIT_H_
#define SYS_INIT_H_

// default
#include "Ifx_Types.h"
#include "IfxScuWdt.h"
#include "IfxCpu.h"

// BSW
#include "Can.h"
#include "Encoder.h"
#include "Buzzer.h"
#include "Led.h"
#include "Gpt12.h"
#include "Gtm_Atom_Pwm.h"
#include "Motor.h"
#include "Stm.h"
#include "Ultrasonic.h"

// ASW
#include "Parking.h"

// DEBUG
#include "Asclin.h"

void System_Init(void);

#endif /* SYS_INIT_H_ */
