/*
*********************************************************************************************************
*                                           Atmel ATmega256
*                                           IR_Sensor Decode
*
*                                (c) Copyright 2005, Micrium, Weston, FL
*                                          All Rights Reserved
*
*
* File : IR_Sensor.c
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
#define IR_FRAME_TIME_STARTUP_PLUS_ADDRESS 41u
#define IR_FRAME_TOTAL_TIME 68u

#define IR_TSOP_SESOR				PE5
#define IR_TSOP_SENSOR_PIN_STATUS Dio_ReadChannel(AVR_PORTE,IR_TSOP_SESOR)
/*
*********************************************************************************************************
*                                              GLOBAL VARIABLES
*********************************************************************************************************
*/
    /*Capture OS Tick*/
    volatile unsigned int previous_tick_count;
    volatile unsigned int current_tick_count;

    /*Capture Frame*/
    volatile unsigned int IR_Command_Sequence=0; 
    volatile unsigned int IR_Address_Sequence=0;
    volatile unsigned int IR_Startup_Sequence=0;  
    volatile unsigned int IR_Start_Frame_Init=0;
    volatile unsigned int IR_RepeatCode_Sequence=0;
    volatile unsigned int IR_CommandData_Time_Counter=0; 

    /*Capture Command Data*/
    volatile unsigned  int ir_raw_data_0;


// volatile ir_strucet_t ir_str_t=   {.previous_tick_count = FALSE,
//                                 .current_tick_count = FALSE,
//                                 .IR_Command_Sequence = FALSE,
//                                 .IR_Address_Sequence = FALSE,
//                                 .IR_Startup_Sequence = FALSE,
//                                 .IR_Start_Frame_Init = FALSE,
//                                 .IR_RepeatCode_Sequence = FALSE,
//                                 .IR_CommandData_Time_Counter = FALSE,
//                                 .ir_raw_data_0 = 0b0000000
//                                 }; 

extern OS_EVENT *msgbox; 

/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static void IR_Retrive_RepeatCode(void);
static void IR_Calculate_Frame(void);
static void IR_Retrive_CommandData(void);
static void IR_Retrive_AddressData(void);
static void IR_Retrive_StartFrame(void);
static void IR_ISR_Init(void);
void    IR_Sensor_ISR_Enable(void);

/*
*********************************************************************************************************
*                                                   IR_Retrive_RepeatCode 
* Description : This function shall capture the Repeate code of the IR Frame.
*               
* Arguments   : none
*********************************************************************************************************
*/
static void IR_Retrive_RepeatCode(void)
{
    if(IR_RepeatCode_Sequence == FALSE && IR_Command_Sequence == TRUE && IR_Address_Sequence == TRUE && IR_Startup_Sequence == TRUE && IR_Start_Frame_Init == TRUE)
    {
        current_tick_count = OSTimeGet();
        if(((current_tick_count - previous_tick_count) >= 121))
        {

            IR_Start_Frame_Init = FALSE;
            ir_raw_data_0 = FALSE;
        }

    }
}
/*
*********************************************************************************************************
*                                                   IR_Retrive_CommandData
* Description : This function capture the 27ms CommandFrame and Extract the Data out of it.
*               
* Arguments   : none
*********************************************************************************************************
*/
static void IR_Retrive_CommandData(void)
{
    if(IR_Command_Sequence == FALSE && IR_Address_Sequence == TRUE && IR_Startup_Sequence == TRUE && IR_Start_Frame_Init == TRUE)
    {
        current_tick_count = OSTimeGet();
        if(((current_tick_count - previous_tick_count) >= (IR_FRAME_TIME_STARTUP_PLUS_ADDRESS + IR_CommandData_Time_Counter))&& ((current_tick_count - previous_tick_count) <= IR_FRAME_TOTAL_TIME))
        {

            if ((IR_TSOP_SENSOR_PIN_STATUS) == FALSE) 
            {
                ir_raw_data_0 &= ~(1 << (8 - (IR_CommandData_Time_Counter/3))); //Clear bit (7-b)
            } 
            else 
            {
                ir_raw_data_0 |= (1 << (8 - (IR_CommandData_Time_Counter/3))); //Set bit (7-b)
            } 
            
            IR_CommandData_Time_Counter = IR_CommandData_Time_Counter +3;

        }
        else
        {
            /**Do Nothing*/

        }
        if((current_tick_count - previous_tick_count) >=68)
        {
            // breakpoint();
            OSMboxPost(msgbox, ir_raw_data_0);
            IR_Command_Sequence = TRUE;
            IR_CommandData_Time_Counter = 3;
            previous_tick_count = OSTimeGet();
            IR_Start_Frame_Init = FALSE;
        }

    }

}
/*
*********************************************************************************************************
*                                                   IR_Retrive_AddressData
* Description : This function capture the 27ms Address Frame.
*               
* Arguments   : none
*********************************************************************************************************
*/
static void IR_Retrive_AddressData(void)
{
    if(IR_Address_Sequence == FALSE && IR_Startup_Sequence == TRUE && IR_Start_Frame_Init == TRUE)
    {
    
        current_tick_count = OSTimeGet();
        if((current_tick_count - previous_tick_count) >=IR_FRAME_TIME_STARTUP_PLUS_ADDRESS)
        {
            IR_Address_Sequence = TRUE;

        }
    }
}
/*
*********************************************************************************************************
*                                                   IR_Retrive_StartFrame
* Description : This function capture the 9ms LOW and 4.5ms HIGH pulse.
*               
* Arguments   : none
*********************************************************************************************************
*/

static void IR_Retrive_StartFrame(void)
{
    if(IR_Startup_Sequence == FALSE && IR_Start_Frame_Init == TRUE)
    {
        current_tick_count = OSTimeGet();
        // if((current_tick_count - previous_tick_count) >=14)
        // {
        //     IR_Startup_Sequence = TRUE;
        // }
        // if((current_tick_count - previous_tick_count) <=8)
        // {

        //     if ((PINE &(1<<IR_TSOP_SESOR)) == FALSE) 
        //     {
        //         ir_raw_data_0 = 25;
        //     } 
        // }
        // if((current_tick_count - previous_tick_count) > 9)
        // {

        //     if ((PINE &(1<<IR_TSOP_SESOR)) == TRUE) 
        //     {
        //         ir_raw_data_0 = 26;
        //         breakpoint();
        //     } 
        // }

    }

}
/*
*********************************************************************************************************
*                                                   IR_ISR_Init
* Description : This function capture the Startup IR Frame.
*               
* Arguments   : none
*********************************************************************************************************
*/
static void IR_ISR_Init(void)
{
    if(IR_Start_Frame_Init == FALSE)
    {
        previous_tick_count = OSTimeGet();
        IR_Start_Frame_Init = TRUE;
    }
    // breakpoint();

}
/*
*********************************************************************************************************
*                                         IR Sensor Interrupt Enable
*
* Description : This function should be called by your application code before you make use of any of the
*               functions found in this module.This function shall initialize the IT Sensor PIN to ISR Mode (INT5).
*               
* Arguments   : none
*********************************************************************************************************
*/
void  IR_Sensor_ISR_Enable (void)
{ 
    PORTE = 0x00;
    DDRE = 0x00;
    EICRB = (1<<ISC50);
    EIMSK = (1<<INT5);                      
}

/*
*********************************************************************************************************
*                                                   ISR: INT5_vect
* Description : This ISR is configured to be triggered on the Rising and falling edge of the IR Frame.
*               
* Arguments   : none
*********************************************************************************************************
*/
ISR(INT5_vect)
{
    if ((PINE &(1<<IR_TSOP_SESOR)) == TRUE) 
    {
        ir_raw_data_0 = 26;
        breakpoint();
    }
    // IR_ISR_Init();
    // IR_Retrive_StartFrame();
    // IR_Retrive_AddressData();
    // IR_Retrive_CommandData();
    // IR_Retrive_RepeatCode();

}

