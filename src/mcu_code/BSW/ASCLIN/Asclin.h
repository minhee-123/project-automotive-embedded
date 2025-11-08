/**********************************************************************************************************************
 * file asclin.h
 * copyright Copyright (C) Infineon Technologies AG 2019
 * Boost Software License - Version 1.0 - August 17th, 2003
 *********************************************************************************************************************/
#ifndef ASCLIN_H_
#define ASCLIN_H_

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Ifx_reg.h"
#include "IfxAsclin_bf.h"
#include "IfxScuWdt.h"
#include "Isr_Priority.h"
#include "CompilerTasking.h"

// Function Prototypes
void Asclin0_InitUart(void);
void Asclin0RxIsrHandler(void);

int Asclin0_PollUart(unsigned char *chr);
unsigned char Asclin0_InUart(void);
unsigned char Asclin0_InUartNonBlock(void);
void Asclin0_OutUart(const unsigned char chr);

void remove_null(char *s);
void my_puts(const char *str);
void my_printf(const char *fmt, ...);
void my_scanf(const char *fmt, ...);

#endif /* ASCLIN_H_ */
