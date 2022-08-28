/*
*********************************************************************************************************
*                                           Atmel ATmega256
*                                           Motion_Sensor Decode
*
*                                (c) Copyright 2005, Micrium, Weston, FL
*                                          All Rights Reserved
*
*
* File : Motion_Sensor.c
* By   : Pawan Singh Rana
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               INCLUDES
*********************************************************************************************************
*/
#include "Dio_Cfg.h"
#include "includes.h"
#include <avr_debugger.h>
#include "avr8-stub.h"

/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/
#define Motion_Sensor				PE4

#define MOTION_SENSOR_1_28sec_COUNT 1280000u
/*
*********************************************************************************************************
*                                              GLOBAL VARIABLES
*********************************************************************************************************
*/

/*Capture pulse*/
volatile unsigned int Motion_Sensor_Startup_Sequence_1_28sec=0; 
volatile unsigned int Motion_Sensor_Start_Frame_Init=0; 
volatile unsigned int ms_no_interrupts=0;


extern OS_EVENT *msgbox; 

/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static void Motion_Retrive_RepeatCode(void);
static void Motion_Sensor_Retrive_StartFrame(void);
static void Motion_ISR_Init(void);

void    Motion_Sensor_ISR_Disable(void);
void    Motion_Sensor_ISR_Enable(void);

/*
*********************************************************************************************************
*                                                   Motion_Retrive_RepeatCode 
* Description : This function shall capture the Repeate code of the Motion pulse.
*               
* Arguments   : none
*********************************************************************************************************
*/
static void Motion_Retrive_RepeatCode(void)
{

    ms_no_interrupts = 0;
    // OSMboxPost(msgbox,(int *)&Motion_Sensor_Startup_Sequence_1_28sec);
    Motion_Sensor_Start_Frame_Init = FALSE;
    Motion_Sensor_Startup_Sequence_1_28sec = FALSE;

}

/*
*********************************************************************************************************
*                                                   Motion_Sensor_Retrive_StartFrame
* Description : This function capture the 1.28sec HIGH Pulse.
*               
* Arguments   : none
*********************************************************************************************************
*/

static void Motion_Sensor_Retrive_StartFrame(void)
{
    if(Motion_Sensor_Startup_Sequence_1_28sec == FALSE && Motion_Sensor_Start_Frame_Init == TRUE)
    {
        while((PINE &(1<<Motion_Sensor)) == (1<<Motion_Sensor))
        {
            ms_no_interrupts++;

        }
        if((ms_no_interrupts > 0))
        {
            Motion_Sensor_Startup_Sequence_1_28sec  = TRUE;
            breakpoint();
            ms_no_interrupts = 0;
        }

    }

}
/*
*********************************************************************************************************
*                                                   Motion_ISR_Init
* Description : This function capture the Startup IR Frame.
*               
* Arguments   : none
*********************************************************************************************************
*/
static void Motion_ISR_Init(void)
{
    if(Motion_Sensor_Start_Frame_Init == FALSE)
    {
        Motion_Sensor_Start_Frame_Init = TRUE;
    }

}

/*
*********************************************************************************************************
*                                         Motion Sensor Interrupt Enable
*
* Description : This function should be called by your application code before you make use of any of the
*               functions found in this module.This function shall initialize the Motion Sensor PIN to ISR Mode (INT4).
*               
* Arguments   : none
*********************************************************************************************************
*/
void  Motion_Sensor_ISR_Config (void)
{ 
    PORTE = 0x00;
    DDRE = 0x00;
    EICRB |= (1<<ISC40);
    EIMSK |= (1<<INT4);                      
}

/*
*********************************************************************************************************
*                                                   ISR: INT4_vect
* Description : This ISR is configured to be triggered on the Rising and falling edge of the Motion Sensor Pulse.
*               
* Arguments   : none
*********************************************************************************************************
*/

ISR(INT4_vect)
{
    Motion_Sensor_ISR_Disable();
    Motion_ISR_Init();
    Motion_Sensor_Retrive_StartFrame();
    Motion_Retrive_RepeatCode();
    Motion_Sensor_ISR_Enable();

}

/*
*********************************************************************************************************
*                                                   Motion_Sensor_ISR_Disable
* Description : This function should be called by your application code before you make use of any of the
*               functions found in this module.This function shall disable the ISR.
*               
* Arguments   : none
*********************************************************************************************************
*/
void  Motion_Sensor_ISR_Disable(void)
{
    EIMSK&= ~(1<<INT4); 
}
/*
*********************************************************************************************************
*                                                   Motion_Sensor_ISR_Enable
* Description : This function should be called by your application code before you make use of any of the
*               functions found in this module.This function shall enable the ISR.
*               
* Arguments   : none
*********************************************************************************************************
*/
void  Motion_Sensor_ISR_Enable(void)
{
    EIMSK |= (1<<INT4); 
}


