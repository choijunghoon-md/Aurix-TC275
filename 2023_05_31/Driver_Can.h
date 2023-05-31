#ifndef DRIVER_CAN
#define DRIVER_CAN

/***********************************************************************/
/*Include*/ 
/***********************************************************************/
#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxMultican_Can.h"
#include "IfxPort.h"
#include "Bsp.h"


/***********************************************************************/
/*Define*/ 
/***********************************************************************/

#define STB                         &MODULE_P20,6               //LOW상태로 유지시켜야 하는 핀

#define TX_INTERRUPT_SRC_ID         IfxMultican_SrcId_0         /* Transmit interrupt service request ID             */
#define ISR_PRIORITY_CAN_TX         2                           /* Define the CAN TX interrupt priority              */

#define RX_INTERRUPT_SRC_ID         IfxMultican_SrcId_1         /* Receive interrupt service request ID              */
#define ISR_PRIORITY_CAN_RX         1                           /* Define the CAN RX interrupt priority              */

#define PIN5                        5                           /* LED1 used in TX ISR is connected to this pin      */
#define PIN6                        6                           /* LED2 used in RX ISR is connected to this pin      */


#define WAIT_TIME_1ms               1                           // 1ms시간
#define WAIT_TIME_5ms               5                           // 5ms시간
#define WAIT_TIME_10ms              10                          // 10ms시간
#define WAIT_TIME_20ms              20                          // 20ms시간
#define WAIT_TIME_50ms              50                          // 50ms시간
#define WAIT_TIME_100ms             100                         // 100ms시간

#define LED1                        &MODULE_P00,5               //LED1 핀
#define LED2                        &MODULE_P00,6               //LED2 핀
#define INT_RX_OUT                  &MODULE_P00,7               //INT 핀(00,7),(이전 코드에선 사용하지 않음)

/***********************************************************************/
/*Typedef*/ 
/***********************************************************************/
typedef struct
{
    struct
    {

        IfxMultican_Can                 can;                   /* CAN module handle to HW module SFR set                 */
        IfxMultican_Can_Config          canConfig;             /* CAN module configuration structure                     */
        IfxMultican_Can_Node            canSrcNode;            /* CAN source node handle data structure                  */
        IfxMultican_Can_Node            canDstNode;            /* CAN destination node handle data structure             */
        IfxMultican_Can_NodeConfig      canNodeConfig;         /* CAN node configuration structure                       */
        IfxMultican_Can_MsgObj          canSrcMsgObj;          /* CAN source message object handle data structure        */
        IfxMultican_Can_MsgObj          canDstMsgObj;          /* CAN destination message object handle data structure   */
        IfxMultican_Can_MsgObjConfig    canMsgObjConfig;       /* CAN message object configuration structure             */
        IfxMultican_Message             txMsg;                 /* Transmitted CAN message structure                      */
        IfxMultican_Message             rxMsg;                 /* Received CAN message structure                         */

    }drivers;
} App_MulticanBasic;

typedef struct
{
     unsigned long ID;           //id 저장
     unsigned char IDE;          //통신 패킷 설정
     unsigned char DLC;          //데이터의 길이
     unsigned char Data[8];      //데이터 8byt 저장
} CanRxMsg;

typedef struct
{
    IfxPort_Pin_Config              led1;                  /* LED1 configuration structure                           */
    IfxPort_Pin_Config              led2;                  /* LED2 configuration structure                           */
} AppLedType;


/***********************************************************************/
/*External Variable*/ 
/***********************************************************************/
IFX_EXTERN App_MulticanBasic g_MulticanBasic;


/***********************************************************************/
/*Global Function Prototype*/ 
/***********************************************************************/
extern void Driver_Can_Init(void);                  //CAN통신 환경 설정
extern void Driver_Can_TxTest(void);                //테스트용 임시 TX함수?
extern void CAN_send(CanRxMsg *message);            //CAN통신을 보내는 함수
extern void CAN_TEST(void);                         //CAN통신 Msg를 설정하여 통인을 보내는 함수(이전 코드는 CAN_send와 통합되어 있다.)

void canIsrTxHandler(void);                         //TX인터럽트 함수
void canIsrRxHandler(void);                         //RX인터럽트 함수


void initLed(void);                                 //LED핀 설정
void blinkLED1(void);                               //LED1를 1ms동안 켰다 끄는 함수
void blinkLED2(void);                               //LED2의 HIGH,LOW상태를 바꾸고 10ms를 기다리는 함수
#endif /* DRIVER_STM */
