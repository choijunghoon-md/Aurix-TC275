#include "IfxPort.h"
#include "Bsp.h"

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
#define LED1         &MODULE_P00,2
#define LED2         &MODULE_P00,6
#define LED3         &MODULE_P00,8 /* LED: Port, Pin definition            */
#define WAIT_TIME   10

int count=0;/* Wait time constant in milliseconds   */

/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/
/* This function initializes the port pin which drives the LED */
void initLED(void)
{
    /* Initialization of the LED used in this example */
    IfxPort_setPinModeOutput(LED1, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(LED2, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(LED3, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    /* Switch OFF the LED (low-level active) */
    IfxPort_setPinHigh(LED1);
    IfxPort_setPinHigh(LED2);
    IfxPort_setPinHigh(LED3);
}

/* This function toggles the port pin and wait 500 milliseconds */
void blinkLED(void)
{
    count++;                                                     /* Toggle the state of the LED      */
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME));
    if(count%3==0){IfxPort_togglePin(LED1);} /* Wait 500 milliseconds            */
    if(count%2==0){IfxPort_togglePin(LED2);}
    if(count%1==0){IfxPort_togglePin(LED3);}
}
