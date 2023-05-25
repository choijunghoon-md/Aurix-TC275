/***********************************************************************/
/*Include*/ 
/***********************************************************************/
#include "Driver_Can.h"


/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
AppLedType        g_led;                                    /* Global LED configuration and control structure        */


/***********************************************************************/
/*Define*/ 
/***********************************************************************/


/***********************************************************************/
/*Typedef*/ 
/***********************************************************************/


/***********************************************************************/
/*Static Function Prototype*/ 
/***********************************************************************/




/* Macro to define Interrupt Service Routine.*/

IFX_INTERRUPT(canIsrTxHandler, 0, ISR_PRIORITY_CAN_TX);  //CAN TX 인터럽트
IFX_INTERRUPT(canIsrRxHandler, 0, ISR_PRIORITY_CAN_RX);  //CAN RX 인터럽트

/***********************************************************************/
/*Variable*/ 
/***********************************************************************/
App_MulticanBasic g_MulticanBasic; /**< \brief Demo information */
volatile CanRxMsg rec;                    //can rx msg설정(현재 TX코드에 대해선 유기된 변수)
int a[8]={0,0x1,0x2,0x3,0x4,0x5,0x6,0x7}; //보낼 데이터 설정
 
/***********************************************************************/
/*Function*/ 
/***********************************************************************/


void initLed(void)
{
    /* ======================================================================
     * Configuration of the pins connected to the LEDs:
     * ======================================================================
     *  - define the GPIO port
     *  - define the GPIO pin that is the connected to the LED
     *  - define the general GPIO pin usage (no alternate function used)
     *  - define the pad driver strength
     * ======================================================================
     */
    g_led.led1.port      = &MODULE_P00;                                //led1핀 설정
    g_led.led1.pinIndex  = PIN5;
    g_led.led1.mode      = IfxPort_OutputIdx_general;                  //led1핀 모드 설정
    g_led.led1.padDriver = IfxPort_PadDriver_cmosAutomotiveSpeed1;

    g_led.led2.port      = &MODULE_P00;                                //led2핀 설정
    g_led.led2.pinIndex  = PIN6;
    g_led.led2.mode      = IfxPort_OutputIdx_general;                  //led1핀 모드 설정
    g_led.led2.padDriver = IfxPort_PadDriver_cmosAutomotiveSpeed1;

    /* Initialize the pins connected to LEDs to level "HIGH"; will keep the LEDs turned off as default state */
    IfxPort_setPinHigh(g_led.led1.port, g_led.led1.pinIndex);
    IfxPort_setPinHigh(g_led.led2.port, g_led.led2.pinIndex);

    /* Set the pin input/output mode for both pins connected to the LEDs */
    IfxPort_setPinModeOutput(g_led.led1.port, g_led.led1.pinIndex, IfxPort_OutputMode_pushPull, g_led.led1.mode);
    IfxPort_setPinModeOutput(g_led.led2.port, g_led.led2.pinIndex, IfxPort_OutputMode_pushPull, g_led.led2.mode);

    /* Set the pad driver mode for both pins connected to the LEDs */
    IfxPort_setPinPadDriver(g_led.led1.port, g_led.led1.pinIndex, g_led.led1.padDriver);
    IfxPort_setPinPadDriver(g_led.led2.port, g_led.led2.pinIndex, g_led.led2.padDriver);
}


void blinkLED1(void)
{
    //IfxPort_togglePin(LED1);                                                     /* Toggle the state of the LED      */

    IfxPort_setPinHigh(LED1); //LED1 켜기
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME_1ms));    /* Wait 1 milliseconds            */
    IfxPort_setPinLow(LED1); //LED1 끄기
}
void blinkLED2(void)
{
    IfxPort_togglePin(LED2);                                                     /* Toggle the state of the LED      */
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME_10ms));    /* Wait 10 milliseconds            */
}




void Driver_Can_Init(void)
{
    /* create module config */
    IfxMultican_Can_Config canConfig;
    IfxMultican_Can_initModuleConfig(&canConfig, &MODULE_CAN);

    /* Interrupt configuration*/
    canConfig.nodePointer[TX_INTERRUPT_SRC_ID].priority = ISR_PRIORITY_CAN_TX;
    canConfig.nodePointer[RX_INTERRUPT_SRC_ID].priority = ISR_PRIORITY_CAN_RX;
    /* initialize module */
    IfxMultican_Can_initModule(&g_MulticanBasic.drivers.can, &canConfig);

    /* create CAN node config */
    IfxMultican_Can_NodeConfig canNodeConfig;
    IfxMultican_Can_Node_initConfig(&canNodeConfig, &g_MulticanBasic.drivers.can);

    canNodeConfig.baudrate = 500000UL;                                                                      // CAN 속도 설정 500kbps
    {
        canNodeConfig.nodeId    = IfxMultican_NodeId_0;//(IfxMultican_NodeId)((int)IfxMultican_NodeId_0);   // CAN의 Node ID 설정 0번으로 설정함
        canNodeConfig.rxPin     = &IfxMultican_RXD0B_P20_7_IN;                                              // 입력핀 설정
        canNodeConfig.rxPinMode = IfxPort_InputMode_pullUp;
        canNodeConfig.txPin     = &IfxMultican_TXD0_P20_8_OUT;                                              // 출력핀 설정
        canNodeConfig.txPinMode = IfxPort_OutputMode_pushPull;

        IfxMultican_Can_Node_init(&g_MulticanBasic.drivers.canSrcNode, &canNodeConfig);                     // CAN node 초기화
    }

    /* Create message object config TX*/
    IfxMultican_Can_MsgObjConfig canMsgObjConfig;                                                           // CAN message object configuration
    IfxMultican_Can_MsgObj_initConfig(&canMsgObjConfig, &g_MulticanBasic.drivers.canSrcNode);

    canMsgObjConfig.msgObjId              = (IfxMultican_MsgObjId)0;                                                              //256개의 message object가 있음
    canMsgObjConfig.messageId             = 0x100;
    canMsgObjConfig.acceptanceMask        = 0x7FFFFFFFUL;
    canMsgObjConfig.frame                 = IfxMultican_Frame_transmit;                                     // CAN TX로 설정
    canMsgObjConfig.control.messageLen    = IfxMultican_DataLengthCode_8;                                   // Data 길이는 8
    canMsgObjConfig.control.extendedFrame = FALSE;                                                          // Extended ID 사용 안함
    canMsgObjConfig.control.matchingId    = TRUE;

    canMsgObjConfig.txInterrupt.enabled = TRUE;                                                             // CAN 인터럽트 enabled TX
    canMsgObjConfig.txInterrupt.srcId = TX_INTERRUPT_SRC_ID;                                                // 전송 인터럽트 서비스 요청 ID TX


    /* initialize message object TX */
    IfxMultican_Can_MsgObj_init(&g_MulticanBasic.drivers.canSrcMsgObj, &canMsgObjConfig);

    /*Create message object for RX*/

    canMsgObjConfig.msgObjId              = (IfxMultican_MsgObjId)0;                                                              //256개의 message object가 있음
        canMsgObjConfig.messageId             = 0x100;
        canMsgObjConfig.acceptanceMask        = 0x7FFFFFFFUL;
        canMsgObjConfig.frame                 = IfxMultican_Frame_transmit;                                     // CAN TX로 설정
        canMsgObjConfig.control.messageLen    = IfxMultican_DataLengthCode_8;                                   // Data 길이는 8
        canMsgObjConfig.control.extendedFrame = FALSE;                                                          // Extended ID 사용 안함

        canMsgObjConfig.rxInterrupt.enabled = TRUE;                                                             // CAN 인터럽트 enabled  RX
        canMsgObjConfig.rxInterrupt.srcId = RX_INTERRUPT_SRC_ID;                                                // 전송 인터럽트 서비스 요청 ID  RX

        /* initialize message object RX*/
           IfxMultican_Can_MsgObj_init(&g_MulticanBasic.drivers.canDstMsgObj, &canMsgObjConfig);



    /* IO Port */
    IfxPort_setPinModeOutput(STB, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    /* Set STB Pin of CAN chip (low-level active) */
    IfxPort_setPinLow(STB);
    IfxPort_setPinModeOutput(INT_RX_OUT, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinLow(INT_RX_OUT);
    //IfxPort_setPinHigh(STB);
}


void Driver_Can_TxTest(void)
{
    const uint32 dataLow  = 0x12340000;             //data[0]
    const uint32 dataHigh = 0x9abc0000;             //data[1]

    /* Transmit Data */
    {
        IfxMultican_Message msg;
        IfxMultican_Message_init(&msg, 0x100, dataLow, dataHigh, IfxMultican_DataLengthCode_8); // msg 설정

        while (IfxMultican_Can_MsgObj_sendMessage(&g_MulticanBasic.drivers.canSrcMsgObj, &msg) == IfxMultican_Status_notSentBusy) //CAN 메세지 보내기
        {}
    }
}


void CAN_send(CanRxMsg *message)
{
    IfxMultican_Message msg;
    const unsigned dataLow = message->Data[0]|(message->Data[1]<<8)|(message->Data[2]<<16)|(message->Data[3]<<24);  //데이터를 합쳐서 data[0]
    const unsigned dataHigh = message->Data[4]|(message->Data[5]<<8)|(message->Data[6]<<16)|(message->Data[7]<<24); //데이터를 합쳐서 data[1]


    IfxMultican_Message_init(&msg,message->ID,dataLow,dataHigh,message->DLC);// msg 설정

    while (IfxMultican_Can_MsgObj_sendMessage(&g_MulticanBasic.drivers.canSrcMsgObj, &msg) == IfxMultican_Status_notSentBusy) //CAN 메세지 보내기
    {
       // break;
    }
}


void CAN_TEST(void)
{
    CanRxMsg MES;
    int i=0;                                     //반복문용 변수 선언
    MES.ID=0x888;                                //두자리 숫자만 보내는게 가능한 CAN ID
    MES.IDE=0;                                   //통신 패킷이 0일시 Standard , 1일시 Extend
    MES.DLC=8;                                   //데이터 길이
    for(i=0; i<8; i++)
    {
        MES.Data[i]=a[i];                        //보내는 데이터
    }
    CAN_send(&MES);                              //CAN통신으로 보내는 함수
}


/* Interrupt Service Routine (ISR) called once the TX interrupt has been generated.
 * Turns on the LED1 to indicate successful CAN message transmission.
 */
void canIsrTxHandler(void)
{
    /* Just to indicate that the CAN message has been transmitted by turning on LED1 */
    blinkLED1();

    //IfxPort_setPinLow(g_led.led1.port, g_led.led1.pinIndex);
}

void canIsrRxHandler(void)
{
    IfxMultican_Status readStatus;//읽는 데이터 저장 변수
    readStatus = IfxMultican_Can_MsgObj_readMessage(&g_MulticanBasic.drivers.canDstMsgObj,&g_MulticanBasic.drivers.rxMsg);//can통신 읽기
    if(!(readStatus & IfxMultican_Status_newData)){ //새로운 데이터 없을시 received 에러
        while(1){}
    }
    if(!(readStatus & IfxMultican_Status_newDataButOneLost)){ //새로운 데이터 없을시 received 그러나 메세지 읽을시 에러
            while(1){}
        }
    IfxPort_setPinHigh(INT_RX_OUT); //신호 켜짐 알리기
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME_1ms));    /* Wait 1 milliseconds            */
    IfxPort_setPinLow(INT_RX_OUT); //종료
}

