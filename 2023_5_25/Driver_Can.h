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

#define STB         &MODULE_P20,6

#define TX_INTERRUPT_SRC_ID         IfxMultican_SrcId_0         /* Transmit interrupt service request ID             */
#define RX_INTERRUPT_SRC_ID         IfxMultican_SrcId_1         /* Transmit interrupt service request ID             */
#define ISR_PRIORITY_CAN_TX         2                           /* Define the CAN TX interrupt priority              */
#define ISR_PRIORITY_CAN_RX         1                          /* Define the CAN TX interrupt priority              */
#define PIN5                        5                           /* LED1 used in TX ISR is connected to this pin      */
#define PIN6                        6                           /* LED2 used in RX ISR is connected to this pin      */


#define WAIT_TIME_1ms               1                           // 1ms�ð�
#define WAIT_TIME_5ms               5                           // 5ms�ð�
#define WAIT_TIME_10ms              10                          // 10ms�ð�
#define WAIT_TIME_20ms              20                          // 20ms�ð�
#define WAIT_TIME_50ms              50                          // 50ms�ð�
#define WAIT_TIME_100ms             100                         // 100ms�ð�

#define LED1         &MODULE_P00,5                              //infineon LED1�� ��
#define LED2         &MODULE_P00,6                              //infineon LED2�� ��
#define INT_RX_OUT   &MODULE_P00,7                              //RX������ ��ȣ
/***********************************************************************/
/*Typedef*/ 
/***********************************************************************/
typedef struct
{
    struct
    {
        IfxMultican_Can        can;          /**< \brief CAN driver handle */
        IfxMultican_Can_Node   canSrcNode;   /**< \brief CAN Source Node TX*/
        IfxMultican_Can_Node   canDstNode;   /**< \brief CAN Source Node RX*/
        IfxMultican_Can_MsgObj canSrcMsgObj; /**< \brief CAN Source Message object */
        IfxMultican_Can_MsgObj canDstMsgObj; /**< Can distinaion message object handle data structure */
        IfxMultican_Can_MsgObj canMsgObjConfig; /**< Can message object configuration structure */
        IfxMultican_Message    txMsg;       //tx �޼���
        IfxMultican_Message    rxMsg;       //rx �޼���


    }drivers;
} App_MulticanBasic;


typedef struct
{
    unsigned long ID;           //id ����
    unsigned char IDE;          //��� ��Ŷ ����
    unsigned char DLC;          //�������� ����
    unsigned char Data[8];      //������ 8byt ����
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
extern void Driver_Can_Init(void);                  //CAN��� ȯ�� ����
extern void Driver_Can_TxTest(void);
extern void CAN_send(CanRxMsg *message);            //CAN����� ������ �Լ�
extern void CAN_TEST(void);                         //CAN��� Msg�� �����Ͽ� ������ ������ �Լ�

void canIsrTxHandler(void);
void canIsrRxHandler(void);

void initLed(void);                                 //LED�� ����
void blinkLED1(void);                               //LED1�� 1ms���� �״� ���� �Լ�
void blinkLED2(void);                               //LED2�� HIGH,LOW���¸� �ٲٰ� 10ms�� ��ٸ��� �Լ�
#endif /* DRIVER_STM */
