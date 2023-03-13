/**********************************************************************************************************************
 * \file ADC_Single_Channel.c
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
#include "IfxVadc_Adc.h"
#include "ADC_Single_Channel.h"

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
#define LED1                        &MODULE_P00, 5              /* LED1                                             */
#define LED2                        &MODULE_P00, 6              /* LED2                                             */
#define LIMIT_HIGH                  0xAAA                       /* Higher limit to be compared with the measurement */
#define LIMIT_LOW                   0x555                       /* Lower limit to be compared with the measurement  */
#define VADC_GROUP                  IfxVadc_GroupId_0           /* Use the ADC group 0                              */
#define CHANNEL_ID                  0                           /* Use the Channel 0                                */
#define CHANNEL_RESULT_REGISTER     5                           /* Use the Result Register 5                        */

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
ApplicationVadcBackgroundScan g_vadcBackgroundScan;

/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/
/* The initialization of the port pins to control the LEDs is done by this function */

int adc_value = 0;

void initializeLEDs(void)
{
    /* Initialize the Port Pins */
    IfxPort_setPinMode(LED1, IfxPort_Mode_outputPushPullGeneral);
    IfxPort_setPinMode(LED2, IfxPort_Mode_outputPushPullGeneral);

    /* Turn off the LEDs */
    IfxPort_setPinHigh(LED1);
    IfxPort_setPinHigh(LED2);
}

/* This function retrieves the conversion value and indicates it by LEDs */
int indicateConversionValue(void)
{
    Ifx_VADC_RES conversionResult;

    /* Retrieve the conversion value until valid flag of the result register is true */
    do
    {
        conversionResult = IfxVadc_Adc_getResult(&g_vadcBackgroundScan.adcChannel);
    }
    while (!conversionResult.B.VF);

    adc_value = conversionResult.B.RESULT;

    if(conversionResult.B.RESULT < LIMIT_LOW)       /* LED1 lights up if the conversion value is smaller than 0x555 */
    {
        IfxPort_setPinLow(LED1);
        IfxPort_setPinHigh(LED2);
    }
    else if(conversionResult.B.RESULT > LIMIT_HIGH) /* LED2 lights up if the conversion value is greater than 0xAAA */
    {
        IfxPort_setPinHigh(LED1);
        IfxPort_setPinLow(LED2);
    }
    else                                            /* LED1 and LED2 light up if the conversion value is in between */
    {
        IfxPort_setPinLow(LED1);
        IfxPort_setPinLow(LED2);
    }

    return adc_value;
}

/* The VADC module and group are initialized */
void vadcBackgroundScanInit(void)
{
    /* VADC module configuration */
    /* Create VADC configuration */
    IfxVadc_Adc_Config adcConfig;

    /* Initialize the VADC configuration with default values */
    IfxVadc_Adc_initModuleConfig(&adcConfig, &MODULE_VADC);

    /* Initialize the VADC module using the VADC configuration */
    IfxVadc_Adc_initModule(&g_vadcBackgroundScan.vadc, &adcConfig);

    /* VADC group configuration */
    /* Create group configuration */
    IfxVadc_Adc_GroupConfig adcGroupConfig;

    /* Initialize the group configuration with default values */
    IfxVadc_Adc_initGroupConfig(&adcGroupConfig, &g_vadcBackgroundScan.vadc);

    /* Define which ADC group is going to be used */
    adcGroupConfig.groupId = VADC_GROUP;
    adcGroupConfig.master = VADC_GROUP;

    /* Enable background scan source */
    adcGroupConfig.arbiter.requestSlotBackgroundScanEnabled = TRUE;

    /* Enable background auto scan mode */
    adcGroupConfig.backgroundScanRequest.autoBackgroundScanEnabled = TRUE;

    /* Enable the gate in "always" mode (no edge detection) */
    adcGroupConfig.backgroundScanRequest.triggerConfig.gatingMode = IfxVadc_GatingMode_always;

    /* Initialize the group using the group configuration */
    IfxVadc_Adc_initGroup(&g_vadcBackgroundScan.adcGroup, &adcGroupConfig);
}

/* The input channels to be used are setup and the VADC is set into run mode */
void vadcBackgroundScanRun(void)
{
    /* Initialize the channel configuration of application handle g_vadcBackgroundScan with default values */
    IfxVadc_Adc_initChannelConfig(&g_vadcBackgroundScan.adcChannelConfig, &g_vadcBackgroundScan.adcGroup);

    g_vadcBackgroundScan.adcChannelConfig.channelId = (IfxVadc_ChannelId)CHANNEL_ID;
    g_vadcBackgroundScan.adcChannelConfig.resultRegister = (IfxVadc_ChannelResult)CHANNEL_RESULT_REGISTER;
    g_vadcBackgroundScan.adcChannelConfig.backgroundChannel = TRUE;

    /* Initialize the channel of application handle g_VadcBackgroundScan using the channel configuration */
    IfxVadc_Adc_initChannel(&g_vadcBackgroundScan.adcChannel, &g_vadcBackgroundScan.adcChannelConfig);

    /* Enable background scan for the channel */
    IfxVadc_Adc_setBackgroundScan(&g_vadcBackgroundScan.vadc,
                                  &g_vadcBackgroundScan.adcGroup,
                                  (1 << (IfxVadc_ChannelId)CHANNEL_ID),
                                  (1 << (IfxVadc_ChannelId)CHANNEL_ID));

    /* Start background scan conversion */
    IfxVadc_Adc_startBackgroundScan(&g_vadcBackgroundScan.vadc);
}
