/**********************************************************************************************************************
 * file Cpu0_Main.c
 * copyright Copyright (C) Infineon Technologies AG 2019
 * Boost Software License - Version 1.0 - August 17th, 2003
 *********************************************************************************************************************/
#include "Cpu0_Main.h"

void core0_main(void)
{
    System_Init();

    volatile int parking_flag = 0;

    while(1)
    {
        parking_flag = Can_Get_Parking();

        if (parking_flag == 1)
        {
            Parking_On();

            parking_flag = 0;
            Can_Let_Parking(parking_flag);
        }
    }
}
