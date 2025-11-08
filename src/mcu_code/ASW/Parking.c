/**********************************************************************************************************************
 * file Parking.c
 * copyright Copyright (C) Infineon Technologies AG 2019
 * Boost Software License - Version 1.0 - August 17th, 2003
 *********************************************************************************************************************/
#include "Parking.h"

void Parking_On (void)
{
    int state = 0;

    uint64 start_time = 0;
    uint64 end_time = 0;
    uint64 space_duration_us = 0;
    int measuring_space = 0;
    unsigned int Buzzer_Flag = 1;

    float space_detect_threshold = 35.0;
    float rear_stop_threshold = 7.0;

    while (1)
    {
        float left_dist = Ultrasonic_Get_Left_Dist();
        float rear_dist = Ultrasonic_Get_Back_Dist();

        if (state == 0) // 공간 탐색 및 시간 측정
        {
            Motor_Set_Left(1, 80);
            Motor_Set_Right(1, 80);

            if (left_dist > space_detect_threshold)
            {
                if (measuring_space == 0)
                {
                    start_time = Stm_Get_Time_Us(); // 측정 시작
                    measuring_space = 1;
                }
            }
            else // 다시 15cm 이내가 된 경우 → 빈공간 종료
            {
                if (measuring_space == 1)
                {
                    end_time = Stm_Get_Time_Us();
                    space_duration_us = end_time - start_time;
                    measuring_space = 0;

                    if (space_duration_us >= 400000)  // 0.4초 이상 빈 공간이면
                    {
                        Motor_Stop_Left();
                        Motor_Stop_Right();
                        delay_ms(500);  // 안정화 대기

                        uint64 half_duration_ms = 1450;

                        Motor_Set_Left(0, 90);
                        Motor_Set_Right(0, 90);
                        delay_ms((int)half_duration_ms);
                        Motor_Stop_Left();
                        Motor_Stop_Right();

                        state = 1;
                    }
                    // else 공간이 너무 짧으면 continue scanning
                }
            }
        }

        else if (state == 1) // 정지 후 대기
        {
            delay_ms(500);
            state = 2;
        }

        else if (state == 2) // 전진 + 우회전
        {
            Motor_Set_Left(1, 255);
            Motor_Set_Right(1, 80);

            delay_ms(800);
            Motor_Stop_Left();
            Motor_Stop_Right();
            delay_ms(500);
            state = 3;
        }

        else if (state == 3)  // 후진 + 좌회전
        {
            Motor_Set_Left(0, 80);
            Motor_Set_Right(0, 255);

            delay_ms(555);

            Motor_Stop_Left();
            Motor_Stop_Right();
            delay_ms(500);

            Motor_Set_Left(0, 100);
            Motor_Set_Right(0, 100);
            delay_ms(50);
            state = 4;
        }

        else if (state == 4) // 후진 직선 주차
        {
            Motor_Set_Left(0, 60);
            Motor_Set_Right(0, 60);

            if(rear_dist >= 60){
                Buzzer_Off();
                Buzzer_Flag = 1;
            }
            else if(rear_dist > 45){
                Buzzer_Set_Cycle_Ms(300);
                if(Buzzer_Flag) {
                    Buzzer_On();
                    Buzzer_Flag = 0;
                }
            }
            else if(rear_dist > 30) {
                Buzzer_Set_Cycle_Ms(150);
            }
            else if (rear_dist >= 15) {
                Buzzer_Set_Cycle_Ms(50);
            }

            if (rear_dist <= rear_stop_threshold)
            {
                Motor_Stop_Left();
                Motor_Stop_Right();
                state = 5;
            }
        }

        else if (state == 5) // 주차 완료
        {
            // 정지 상태 유지
            Led_Set(1, 1);
            Led_Set(2, 1);
            Buzzer_Off();
            Buzzer_Always_On();

            delay_ms(2000);
            Led_Set(1, 0);
            Led_Set(2, 0);
            Buzzer_Off();

            break;
        }
    }
}
