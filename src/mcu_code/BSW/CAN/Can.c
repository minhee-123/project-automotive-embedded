/**********************************************************************************************************************
 * file Can.c
 * copyright Copyright (C) Infineon Technologies AG 2019
 * Boost Software License - Version 1.0 - August 17th, 2003
 *********************************************************************************************************************/
#include "Can.h"

McmcanType g_mcmcan;

static volatile int aeb = 0;
static volatile int parking = 0;
static volatile int turn_left = 0;
static volatile int turn_right = 0;

static volatile unsigned int dist_front = 0;


int Can_Get_Aeb(void) {
    return aeb;
}

int Can_Get_Parking(void) {
    return parking;
}

int Can_Get_Turn_Left(void) {
    return turn_left;
}

int Can_Get_Turn_Right(void) {
    return turn_right;
}

int Can_Get_Front_Dist(void) {
    return dist_front;
}

void Can_Let_Parking(int state){
    parking = state;
    if (state == 0) {
        uint8 data = 1;
        Can_Send_Msg(0x301, &data, 1);
    }
} //외부에서 parking mode 업데이트하는 함수


/* Function to initialize MCMCAN module and nodes related for this application use case */
void Can_Init(CAN_BAUDRATES ls_baudrate, CAN_NODE CAN_Node)
{
    /* wake up transceiver (node 0) */
    IfxPort_setPinModeOutput(&MODULE_P20,6, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    MODULE_P20.OUT.B.P6 = 0;

    IfxCan_Can_initModuleConfig(&g_mcmcan.canConfig, &MODULE_CAN0);
    IfxCan_Can_initModule(&g_mcmcan.canModule, &g_mcmcan.canConfig);
    IfxCan_Can_initNodeConfig(&g_mcmcan.canNodeConfig, &g_mcmcan.canModule);

    switch (ls_baudrate)
    {
        case BD_NOUSE:
            g_mcmcan.canNodeConfig.busLoopbackEnabled = TRUE;
            break;
        case BD_500K:
            g_mcmcan.canNodeConfig.baudRate.baudrate = 500000;
            break;
        case BD_1M:
            g_mcmcan.canNodeConfig.baudRate.baudrate = 1000000;
            break;
    }

    g_mcmcan.canNodeConfig.busLoopbackEnabled = FALSE;

    if (CAN_Node == CAN_NODE0) { /* CAN Node 0 for lite kit */
        g_mcmcan.canNodeConfig.nodeId = IfxCan_NodeId_0;
        const IfxCan_Can_Pins pins =
        {
                &IfxCan_TXD00_P20_8_OUT, IfxPort_OutputMode_pushPull, /* TX Pin for lite kit (can node 0) */
                &IfxCan_RXD00B_P20_7_IN, IfxPort_InputMode_pullUp, /* RX Pin for lite kit (can node 0) */
                IfxPort_PadDriver_cmosAutomotiveSpeed1
        };
        g_mcmcan.canNodeConfig.pins = &pins;
    } else if (CAN_Node == CAN_NODE2) { /* CAN Node 2 for mikrobus */
        g_mcmcan.canNodeConfig.nodeId = IfxCan_NodeId_2;
        const IfxCan_Can_Pins pins =
        {
                &IfxCan_TXD02_P15_0_OUT, IfxPort_OutputMode_pushPull, /* TX Pin for mikrobus (can node 2) */
                &IfxCan_RXD02A_P15_1_IN, IfxPort_InputMode_pullUp, /* RX Pin for mikrobus (can node 2) */
                IfxPort_PadDriver_cmosAutomotiveSpeed1
        };
        g_mcmcan.canNodeConfig.pins = &pins;
    }

    g_mcmcan.canNodeConfig.frame.type = IfxCan_FrameType_transmitAndReceive;
    g_mcmcan.canNodeConfig.interruptConfig.transmissionCompletedEnabled = TRUE;
    g_mcmcan.canNodeConfig.interruptConfig.traco.priority = ISR_PRIORITY_CAN_TX;
    g_mcmcan.canNodeConfig.interruptConfig.traco.interruptLine = IfxCan_InterruptLine_0;
    g_mcmcan.canNodeConfig.interruptConfig.traco.typeOfService = IfxSrc_Tos_cpu0;
    IfxCan_Can_initNode(&g_mcmcan.canSrcNode, &g_mcmcan.canNodeConfig);

    /* Reception handling configuration */
    g_mcmcan.canNodeConfig.rxConfig.rxMode = IfxCan_RxMode_sharedFifo0;
    g_mcmcan.canNodeConfig.rxConfig.rxBufferDataFieldSize = IfxCan_DataFieldSize_8;
    g_mcmcan.canNodeConfig.rxConfig.rxFifo0DataFieldSize = IfxCan_DataFieldSize_8;
    g_mcmcan.canNodeConfig.rxConfig.rxFifo0Size = 15;
    /* General filter configuration */
    g_mcmcan.canNodeConfig.filterConfig.messageIdLength = IfxCan_MessageIdLength_standard;
    g_mcmcan.canNodeConfig.filterConfig.standardListSize = 8;
    g_mcmcan.canNodeConfig.filterConfig.standardFilterForNonMatchingFrames = IfxCan_NonMatchingFrame_reject;
    g_mcmcan.canNodeConfig.filterConfig.rejectRemoteFramesWithStandardId = TRUE;
    /* Interrupt configuration */
    g_mcmcan.canNodeConfig.interruptConfig.rxFifo0NewMessageEnabled = TRUE;
    g_mcmcan.canNodeConfig.interruptConfig.rxf0n.priority = ISR_PRIORITY_CAN_RX;
    g_mcmcan.canNodeConfig.interruptConfig.rxf0n.interruptLine = IfxCan_InterruptLine_1;
    g_mcmcan.canNodeConfig.interruptConfig.rxf0n.typeOfService = IfxSrc_Tos_cpu0;
    IfxCan_Can_initNode(&g_mcmcan.canDstNode, &g_mcmcan.canNodeConfig);

    /* Rx filter configuration (default: all messages accepted) */
    Can_Set_Filter_Range(0x0, 0x7FF);
}

void Can_Set_Filter_Range(uint32 start, uint32 end)
{
    g_mcmcan.canFilter.number = 0;
    g_mcmcan.canFilter.type = IfxCan_FilterType_range;
    g_mcmcan.canFilter.elementConfiguration = IfxCan_FilterElementConfiguration_storeInRxFifo0;
    g_mcmcan.canFilter.id1 = start;
    g_mcmcan.canFilter.id2 = end;
    IfxCan_Can_setStandardFilter(&g_mcmcan.canDstNode, &g_mcmcan.canFilter);
}

void Can_Set_Filter_Mask(uint32 id, uint32 mask)
{
    g_mcmcan.canFilter.number = 0;
    g_mcmcan.canFilter.type = IfxCan_FilterType_classic;
    g_mcmcan.canFilter.elementConfiguration = IfxCan_FilterElementConfiguration_storeInRxFifo0;
    g_mcmcan.canFilter.id1 = id;
    g_mcmcan.canFilter.id2 = mask;
    IfxCan_Can_setStandardFilter(&g_mcmcan.canDstNode, &g_mcmcan.canFilter);
}

void Can_Send_Msg(unsigned int id, const uint8 *txData, int len)
{
    /* Initialization of the TX message with the default configuration */
    IfxCan_Can_initMessage(&g_mcmcan.txMsg);

    g_mcmcan.txMsg.messageId = id;
    g_mcmcan.txMsg.dataLengthCode = len;

    /* Define the content of the data to be transmitted */
    for (int i = 0; i < 8; i++) {
        g_mcmcan.txData[i] = txData[i];
    }

    /* Send the CAN message with the previously defined TX message content */
    while( IfxCan_Status_notSentBusy ==
           IfxCan_Can_sendMessage(&g_mcmcan.canSrcNode, &g_mcmcan.txMsg, (uint32 *)&g_mcmcan.txData[0]) )
    {
    }
}

int Can_Recv_Msg(unsigned int *id, uint8 *rxData, int *len)
{
    int err = 0;
    /* Clear the "RX FIFO 0 new message" interrupt flag */
    IfxCan_Node_clearInterruptFlag(g_mcmcan.canDstNode.node, IfxCan_Interrupt_rxFifo0NewMessage);

    /* Received message content should be updated with the data stored in the RX FIFO 0 */
    g_mcmcan.rxMsg.readFromRxFifo0 = TRUE;
    g_mcmcan.rxMsg.readFromRxFifo1 = FALSE;

    /* Read the received CAN message */
    IfxCan_Can_readMessage(&g_mcmcan.canDstNode,
                           &g_mcmcan.rxMsg,
                           (uint32*) &g_mcmcan.rxData);

    *id = g_mcmcan.rxMsg.messageId;
    for (int i = 0; i < 8; i++) {
        rxData[i] = g_mcmcan.rxData[i];
    }
    *len = g_mcmcan.rxMsg.dataLengthCode;

    return err;
}

IFX_INTERRUPT(Can_Rx_Isr_Handler, 0, ISR_PRIORITY_CAN_RX);
IFX_INTERRUPT(Can_Tx_Isr_Handler, 0, ISR_PRIORITY_CAN_TX);

void Can_Tx_Isr_Handler(void)
{
    /* Clear the "Transmission Completed" interrupt flag */
    IfxCan_Node_clearInterruptFlag(g_mcmcan.canSrcNode.node, IfxCan_Interrupt_transmissionCompleted);
}

void Can_Aeb_On(void)
{
    Motor_Stop_Left();
    Motor_Stop_Right();

    aeb = 1;
    Led_Set(1, 1);
    Led_Set(2, 1);
    uint8 data = 1;
    Can_Send_Msg(0x300, &data, 1);
}

void Can_Aeb_Off(void)
{
    aeb = 0;
    Led_Set(1, 0);
    Led_Set(2, 0);
    uint8 data = 0;
    Can_Send_Msg(0x300, &data, 1);
}

void Can_Rx_Isr_Handler (void)
{
    unsigned int rxID;
    uint8 rxData[8] = {0, };
    int rxLen;
    Can_Recv_Msg(&rxID, rxData, &rxLen);

    /* TOF Sensor + AEB */
    if (rxID == 522) {
        unsigned int tofVal = rxData[2] << 16 | rxData[1] << 8 | rxData[0];
        uint16 signal_strength =  rxData[5] << 8 | rxData[4];

        if (parking == 1) //주차 모드 중에는 작동하지 않음
            return;

        if (signal_strength == 0)
            dist_front = -1;
        else {
            dist_front = tofVal; // 단위 mm

            /* AEB 판단 로직 */
            int v_avg = (Encoder_Get_Rpm0_Left() + Encoder_Get_Rpm1_Right()) / 2;
            if (aeb == 0 && dist_front < 100 + v_avg + (v_avg/8) &&
                MODULE_P10.OUT.B.P1 == 1 && MODULE_P10.OUT.B.P2 == 1) //전진일 때만 AEB 동작 (모터 방향 확인)
                Can_Aeb_On();
            else if (aeb == 1 && dist_front > 400)
                Can_Aeb_Off();
        }
    }

    /* 원격 주행 (컨트롤러) */
    else if (rxID == 0x100) {
        if (parking == 1) //주차 모드 중에는 작동하지 않음
            return;
        Motor_Set_Left(rxData[0], rxData[1]);
        Motor_Set_Right(rxData[2], rxData[3]);
    }

    /* 모드 여부 (컨트롤러 버튼) */
    else if (rxID == 0x102) {
        if (rxData[2]) {
            if (aeb == 1)
                return;
            if (parking)
                parking = 0;
            else
                parking = 1;
        }
        if (rxData[4]) {
            if (turn_left)
            {
                turn_left = 0;
                Led_Set(1, 0);
            }
            else
            {
                turn_left = 1;
                Led_Set(1, 1);
            }
        }
        if (rxData[5]) {
            if (turn_right)
            {
                turn_right = 0;
                Led_Set(2, 0);
            }
            else
            {
                turn_right = 1;
                Led_Set(2, 1);
            }
        }
   }
}
