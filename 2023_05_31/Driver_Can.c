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

IFX_INTERRUPT(canIsrTxHandler, 0, ISR_PRIORITY_CAN_TX);                                           // CAN TX interrupt
IFX_INTERRUPT(canIsrRxHandler, 0, ISR_PRIORITY_CAN_RX);                                           // CAN RX interrupt
/***********************************************************************/
/*Variable*/ 
/***********************************************************************/
App_MulticanBasic g_MulticanBasic; /**< \brief Demo information */

volatile CanRxMsg rec; //RXmsg 변수 설정
int a[8]={0,0x1,0x2,0x3,0x4,0x5,0x6,0x7}; //send data 설정(다른 부분)

/***********************************************************************/
/*Function*/ 
/***********************************************************************/

void Driver_Can_Init(void)
{
    /* create module config */

    /* Interrupt configuration*/
    IfxMultican_Can_initModuleConfig(&g_MulticanBasic.drivers.canConfig, &MODULE_CAN); /* create module config(drivers 구조체를 이용한)*/
    g_MulticanBasic.drivers.canConfig.nodePointer[TX_INTERRUPT_SRC_ID].priority = ISR_PRIORITY_CAN_TX; /* TX Interrupt configuration*/
    g_MulticanBasic.drivers.canConfig.nodePointer[RX_INTERRUPT_SRC_ID].priority = ISR_PRIORITY_CAN_RX; /* RX Interrupt configuration*/
    /* initialize module */
    IfxMultican_Can_initModule(&g_MulticanBasic.drivers.can, &g_MulticanBasic.drivers.canConfig); /* initialize module(drivers 구조체를 이용한) */



    /* create CAN node config */
    //IfxMultican_Can_NodeConfig canNodeConfig;
    IfxMultican_Can_Node_initConfig(&g_MulticanBasic.drivers.canNodeConfig, &g_MulticanBasic.drivers.can); /* create CAN node config(drivers 구조체를 이용한) */
    //근데 이거 왜 2개에요?
    g_MulticanBasic.drivers.canNodeConfig.baudrate = 500000UL;                                                              // CAN 속도 설정 500kbps
    g_MulticanBasic.drivers.canNodeConfig.nodeId    = (IfxMultican_NodeId)((int)IfxMultican_NodeId_0);                      // CAN의 TX Node ID 설정 0번으로 설정함
    g_MulticanBasic.drivers.canNodeConfig.rxPin     = &IfxMultican_RXD0B_P20_7_IN;                                          // RX핀 설정
    g_MulticanBasic.drivers.canNodeConfig.rxPinMode = IfxPort_InputMode_pullUp;
    g_MulticanBasic.drivers.canNodeConfig.txPin     = &IfxMultican_TXD0_P20_8_OUT;                                          // TX핀 설정
    g_MulticanBasic.drivers.canNodeConfig.txPinMode = IfxPort_OutputMode_pushPull;

    IfxMultican_Can_Node_init(&g_MulticanBasic.drivers.canSrcNode, &g_MulticanBasic.drivers.canNodeConfig);                // CAN node 초기화(drivers 구조체를 이용한)



    IfxMultican_Can_Node_initConfig(&g_MulticanBasic.drivers.canNodeConfig, &g_MulticanBasic.drivers.can);
    g_MulticanBasic.drivers.canNodeConfig.baudrate = 500000UL;                                                             // CAN 속도 설정 500kbps
    g_MulticanBasic.drivers.canNodeConfig.nodeId    = (IfxMultican_NodeId)((int)IfxMultican_NodeId_0);                     // CAN의 RX Node ID 설정 0번으로 설정함
    g_MulticanBasic.drivers.canNodeConfig.rxPin     = &IfxMultican_RXD0B_P20_7_IN;                                          // RX핀 설정
    g_MulticanBasic.drivers.canNodeConfig.rxPinMode = IfxPort_InputMode_pullUp;
    g_MulticanBasic.drivers.canNodeConfig.txPin     = &IfxMultican_TXD0_P20_8_OUT;                                          // TX핀 설정
    g_MulticanBasic.drivers.canNodeConfig.txPinMode = IfxPort_OutputMode_pushPull;


    IfxMultican_Can_Node_init(&g_MulticanBasic.drivers.canDstNode, &g_MulticanBasic.drivers.canNodeConfig);                // CAN RX node 초기화(drivers 구조체를 이용한)
    //근데 이거 왜 2개에요?

    /* Create message object config for TX (drivers 구조체를 이용한) */

    IfxMultican_Can_MsgObj_initConfig(&g_MulticanBasic.drivers.canMsgObjConfig, &g_MulticanBasic.drivers.canSrcNode);

    g_MulticanBasic.drivers.canMsgObjConfig.msgObjId              = (IfxMultican_MsgObjId)0;                               // 256개의 message object가 있음
    g_MulticanBasic.drivers.canMsgObjConfig.messageId             = 0x90;                                                  // TX ID 설정
    g_MulticanBasic.drivers.canMsgObjConfig.acceptanceMask        = 0x7FFFFFFFUL;                                          // CAN 통신 ID 마스크 설정
    g_MulticanBasic.drivers.canMsgObjConfig.frame                 = IfxMultican_Frame_transmit;                            // CAN TX로 설정
    g_MulticanBasic.drivers.canMsgObjConfig.control.messageLen    = IfxMultican_DataLengthCode_8;                          // Data 길이는 8
    g_MulticanBasic.drivers.canMsgObjConfig.control.extendedFrame = FALSE;                                                 // Extended ID 사용 안함
    g_MulticanBasic.drivers.canMsgObjConfig.control.matchingId    = TRUE;                                                  // TX IDE 설정

    g_MulticanBasic.drivers.canMsgObjConfig.txInterrupt.enabled   = TRUE;                                                  // CAN 인터럽트 enabled TX
    g_MulticanBasic.drivers.canMsgObjConfig.txInterrupt.srcId     = TX_INTERRUPT_SRC_ID;                                   // TX 인터럽트 서비스 요청 ID

    /* initialize message object TX (drivers 구조체를 이용한)*/
    IfxMultican_Can_MsgObj_init(&g_MulticanBasic.drivers.canSrcMsgObj, &g_MulticanBasic.drivers.canMsgObjConfig);



    /* Create message object config for RX (drivers 구조체를 이용한) */
    IfxMultican_Can_MsgObj_initConfig(&g_MulticanBasic.drivers.canMsgObjConfig, &g_MulticanBasic.drivers.canDstNode);

    g_MulticanBasic.drivers.canMsgObjConfig.msgObjId              = (IfxMultican_MsgObjId)1;                                        //256개의 message object가 있음
    g_MulticanBasic.drivers.canMsgObjConfig.messageId             = 0x91;                                                           // RX ID 설정
    g_MulticanBasic.drivers.canMsgObjConfig.acceptanceMask        = 0x7FFFFFFFUL;                                                   // CAN 통신 ID 마스크 설정
    g_MulticanBasic.drivers.canMsgObjConfig.frame                 = IfxMultican_Frame_receive;                                      // CAN RX로 설정
    g_MulticanBasic.drivers.canMsgObjConfig.control.messageLen    = IfxMultican_DataLengthCode_8;                                   // Data 길이는 8
    g_MulticanBasic.drivers.canMsgObjConfig.control.extendedFrame = FALSE;                                                          // Extended ID 사용 안함
    g_MulticanBasic.drivers.canMsgObjConfig.control.matchingId    = TRUE;                                                           // RX IDE 설정

    g_MulticanBasic.drivers.canMsgObjConfig.rxInterrupt.enabled   = TRUE;                                                           // CAN 인터럽트 enabled RX
    g_MulticanBasic.drivers.canMsgObjConfig.rxInterrupt.srcId     = RX_INTERRUPT_SRC_ID;                                            // RX 인터럽트 서비스 요청 ID

    /*
    SRC_CAN_CAN0_INT0.B.SRPN = 30u;                                                                        //Register의 역활은? 답:cpu가 필요한 데이터를 일시적으로 저장하는 역활이다.
    SRC_CAN_CAN0_INT0.B.TOS  = 0u;
    SRC_CAN_CAN0_INT0.B.SRE  = 1u;
    */

    /* initialize message object RX (drivers 구조체를 이용한)*/
    IfxMultican_Can_MsgObj_init(&g_MulticanBasic.drivers.canDstMsgObj, &g_MulticanBasic.drivers.canMsgObjConfig);


    //IfxCpu_enableInterrupts();
    /* IO Port */
    IfxPort_setPinModeOutput(STB, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    /* Set STB Pin of CAN chip (low-level active) */
    IfxPort_setPinLow(STB);
    //(drivers이전 코드에는 있었지만 직전 코드에는 없는 INT)
    IfxPort_setPinModeOutput(INT_RX_OUT, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general); //INT(00,7번 핀) 아웃풋
    IfxPort_setPinLow(INT_RX_OUT); //INT LOW상태로 유지
}


void Driver_Can_TxTest(void)
{
    const uint32 dataLow  = 0x12340000; //데이터 0
    const uint32 dataHigh = 0x9abc0000; //데이터 1

    /* Transmit Data */
    {
        IfxMultican_Message msg;  //TX데이터 msg저장 공간
        IfxMultican_Message_init(&msg, 0x100, dataLow, dataHigh, IfxMultican_DataLengthCode_8); //TX msg내용 넣기

        while (IfxMultican_Can_MsgObj_sendMessage(&g_MulticanBasic.drivers.canSrcMsgObj, &msg) == IfxMultican_Status_notSentBusy) //메세지 보내기
        {}
    }
}


void CAN_send(CanRxMsg *message)
{


    IfxMultican_Message msg;  //TX데이터 msg저장 공간
    //(이전 코드에는 8개의 데이터를 2개에 데이터로 넣지 않고 그냥 데이터 2개 생성함)
    const unsigned dataLow = message->Data[0]|(message->Data[1]<<8)|(message->Data[2]<<16)|(message->Data[3]<<24); //데이터 0 넣기
    const unsigned dataHigh = message->Data[4]|(message->Data[5]<<8)|(message->Data[6]<<16)|(message->Data[7]<<24); //데이터 1 넣기


    IfxMultican_Message_init(&msg,message->ID,dataLow,dataHigh,message->DLC); //TX msg내용 넣기

    while (IfxMultican_Can_MsgObj_sendMessage(&g_MulticanBasic.drivers.canSrcMsgObj, &msg) == IfxMultican_Status_notSentBusy) //메세지 보내기
    {

    }
}


void CAN_TEST(void)//(원래 디파인된 내용을 변경하며 설정함)
{
    CanRxMsg MES; //CAN RX 메세지 저장 공간
    int i=0;      //반복문용 변수

    MES.ID=0x90; //메세지 ID = 90으로 설정
    MES.IDE=0;   //통신 패킷이 0일시 Standard , 1일시 Extend
    MES.DLC=8;   //데이터 길이 8
    for(i=0; i<8; i++)
    {
        MES.Data[i]=a[i]; //보내는 데이터
    }
    CAN_send(&MES); //메세지 보내기
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
    IfxMultican_Status readStatus;
    static uint32 u32nuTemp1 = 0u;
    static uint32 u32nuTemp2 = 0u;

    //IfxCpu_enableInterrupts();
    /* Read the received CAN message and store the status of the operation */
    readStatus = IfxMultican_Can_MsgObj_readMessage(&g_MulticanBasic.drivers.canDstMsgObj, &g_MulticanBasic.drivers.rxMsg);

    if( !( readStatus & IfxMultican_Status_newData ) ) //새로운 데이터 없을시 received 에러
    {
           while(1)
           {
           }
    }

    /* If new data has been received but with one message lost, report an error */
    if( readStatus == IfxMultican_Status_newDataButOneLost ) //새로운 데이터 없을시 received 그러나 메세지 읽을시 에러
    {
         while(1)
         {
         }
    }



    if( readStatus == IfxMultican_Status_newData ) //새로운 데이터가 들어올 시
    {

        //g_MulticanBasic.drivers.rxMsg.data[0];(데이터를 따로 저장하는 방식이 읽은 메세지를 저장하는 것에서 데이터(내용)만 저장하는 방식으로 변경됨)
        u32nuTemp1 = g_MulticanBasic.drivers.rxMsg.data[0]; //데이터 0 저장
        u32nuTemp2 = g_MulticanBasic.drivers.rxMsg.data[1]; //데이터 1 저장
       // IfxPort_setPinHigh(INT_RX_OUT);
       // waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME_1ms));    /* Wait 1 milliseconds            */
       // IfxPort_setPinLow(INT_RX_OUT);
        blinkLED1();
    }


    /* Finally, check if the received data matches with the transmitted one */
    //IfxPort_setPinHigh(INT_RX_OUT);
    //waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME_10ms));    /* Wait 1 milliseconds            */
    //IfxPort_setPinLow(INT_RX_OUT);

    /* Turn on the LED2 to indicate correctness of the received message */

}

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
    //LED1 핀 설정
    g_led.led1.port      = &MODULE_P00;
    g_led.led1.pinIndex  = PIN5;
    g_led.led1.mode      = IfxPort_OutputIdx_general;
    g_led.led1.padDriver = IfxPort_PadDriver_cmosAutomotiveSpeed1;
    //LED2 핀 설정
    g_led.led2.port      = &MODULE_P00;
    g_led.led2.pinIndex  = PIN6;
    g_led.led2.mode      = IfxPort_OutputIdx_general;
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
   IfxPort_setPinHigh(LED1);                                                       //LED1 핀 HIGH
   waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME_10ms));    /* Wait 10 milliseconds            */
   IfxPort_setPinLow(LED1);                                                        //LED1 핀 LOW
}
void blinkLED2(void)
{
    IfxPort_togglePin(LED2);                                                     /* Toggle the state of the LED      */
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME_20ms));    /* Wait 20 milliseconds            */
}
