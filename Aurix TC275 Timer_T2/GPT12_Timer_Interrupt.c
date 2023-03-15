/**********************************************************************************************************************
 * \file GPT12_Timer_Interrupt.c
 * \copyright Copyright (C) Infineon Technologies AG 2019
 *
 * Use of this file is subject to the terms of use agreed between (i) you or the company in which ordinary course of
 * business you are acting and (ii) Infineon Technologies AG or its licensees. If and as long as no such terms of use
 * are agreed, use of this file is subject to following:
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization obtaining a copy of the software and
 * accompanying documentation covered by this license (the "Software") to use, reproduce, display, distribute, execute,
 * and transmit the Software, and to prepare derivative works of the Software, and to permit third-parties to whom the
 * Software is furnished to do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including the above license grant, this restriction
 * and the following disclaimer, must be included in all copies of the Software, in whole or in part, and all
 * derivative works of the Software, unless such copies or derivative works are solely in the form of
 * machine-executable object code generated by a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *********************************************************************************************************************/

/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "GPT12_Timer_Interrupt.h"
#include "Ifx_Types.h"
#include "IfxGpt12.h"
#include "IfxPort.h"
#include "Bsp.h"

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
#define ISR_PRIORITY_GPT12_TIMER    11                       /* Define the GPT12 Timer interrupt priority            */
#define ISR_PRIORITY_GPT12_TIMER_T2 10
#define ISR_PROVIDER_GPT12_TIMER    IfxSrc_Tos_cpu0         /* Interrupt provider                                   */
#define RELOAD_VALUE                48828u                  /* Reload value to have an interrupt each 500ms    48828u     */
#define RELOAD_VALUE_50ms           4883u
#define RELOAD_VALUE_10ms           977u
#define LED                         &MODULE_P00, 5          /* LED which toggles in the Interrupt Service Routine   */
#define LED_2                       &MODULE_P00, 6
#define TEST_PIN1                   &MODULE_P33, 12
#define TEST_PIN2                   &MODULE_P33, 13
#define WAIT_TIME                   1
/*********************************************************************************************************************/
int t3_cnt=0;


/*--------------------------------------------Function Implementations-----------------------------------------------*/
/*********************************************************************************************************************/
/* Macro defining the Interrupt Service Routine */
IFX_INTERRUPT(interruptGpt12_T3, 0, ISR_PRIORITY_GPT12_TIMER);
IFX_INTERRUPT(interruptGpt12_T2, 0, ISR_PRIORITY_GPT12_TIMER_T2);
/* Function to initialize the GPT12 and start the timer */
void initGpt12Timer(void)
{
    /* Initialize the GPT12 module */
    IfxGpt12_enableModule(&MODULE_GPT120);                                          /* Enable the GPT12 module      */
    IfxGpt12_setGpt1BlockPrescaler(&MODULE_GPT120, IfxGpt12_Gpt1BlockPrescaler_16); /* Set GPT1 block prescaler     */

    /* Initialize the Timer T3 */
    IfxGpt12_T3_setMode(&MODULE_GPT120, IfxGpt12_Mode_timer);                       /* Set T3 to timer mode         */
    IfxGpt12_T3_setTimerDirection(&MODULE_GPT120, IfxGpt12_TimerDirection_down);    /* Set T3 count direction       */
    IfxGpt12_T3_setTimerPrescaler(&MODULE_GPT120, IfxGpt12_TimerInputPrescaler_64); /* Set T3 input prescaler       */
    IfxGpt12_T3_setTimerValue(&MODULE_GPT120, RELOAD_VALUE);                        /* Set T3 start value           */

    /* Initialize the Timer T2 */
    IfxGpt12_T2_setMode(&MODULE_GPT120, IfxGpt12_Mode_timer);                      /* Set T2 to reload mode        */
  //  IfxGpt12_T2_setReloadInputMode(&MODULE_GPT120, IfxGpt12_ReloadInputMode_risingEdgeTxIN); /* Set reload trigger  */
    IfxGpt12_T2_setTimerValue(&MODULE_GPT120, RELOAD_VALUE_10ms);                        /* Set T2 reload value          */

    /* Initialize the interrupt */
    volatile Ifx_SRC_SRCR *src = IfxGpt12_T3_getSrc(&MODULE_GPT120);                /* Get the interrupt address    */
    IfxSrc_init(src, ISR_PROVIDER_GPT12_TIMER, ISR_PRIORITY_GPT12_TIMER);           /* Initialize service request   */
    IfxSrc_enable(src);                                                             /* Enable GPT12 interrupt       */

    volatile Ifx_SRC_SRCR *src_T2 = IfxGpt12_T2_getSrc(&MODULE_GPT120);                /* Get the interrupt address    */
    IfxSrc_init(src_T2, ISR_PROVIDER_GPT12_TIMER, ISR_PRIORITY_GPT12_TIMER_T2);           /* Initialize service request   */
    IfxSrc_enable(src_T2);


    /* Initialize the LED */
    IfxPort_setPinModeOutput(LED, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(LED_2, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(TEST_PIN1, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(TEST_PIN2, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);

    IfxGpt12_T3_run(&MODULE_GPT120, IfxGpt12_TimerRun_start);                       /* Start the timer              */
    //IfxGpt12_T2_run(&MODULE_GPT120, IfxGpt12_TimerRun_start);
}

/* Interrupt Service Routine of the GPT12 */
void interruptGpt12_T3(void)
{
    IfxGpt12_T2_run(&MODULE_GPT120, IfxGpt12_TimerRun_start);
    IfxGpt12_T3_setTimerValue(&MODULE_GPT120, RELOAD_VALUE_50ms);
  //  IfxPort_setPinLow(LED);
    IfxPort_setPinHigh(TEST_PIN1);
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME));      // 1msec Áö¿¬ ·çÆ¾
    IfxPort_setPinLow(TEST_PIN1);
}

void interruptGpt12_T2(void)
{
    IfxGpt12_T2_setTimerValue(&MODULE_GPT120, RELOAD_VALUE_10ms);
  //  IfxPort_setPinLow(LED);
    IfxPort_setPinHigh(TEST_PIN2);
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, 9.7));      // 1msec Áö¿¬ ·çÆ¾
    IfxPort_setPinLow(TEST_PIN2);
}
