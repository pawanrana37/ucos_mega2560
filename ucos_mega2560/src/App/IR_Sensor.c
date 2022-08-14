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

#include "includes.h"
#include <avr_debugger.h>
#include "avr8-stub.h"
#include "Dio_Cfg.h"
/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/
#define IR_FRAME_TIME_STARTUP_PLUS_ADDRESS 41u
#define IR_FRAME_TOTAL_TIME 68u

#define IR_TSOP_SESOR				PE5
#define IR_TSOP_SENSOR_PIN_STATUS (Dio_ReadChannel(AVR_PORTE,IR_TSOP_SESOR))
/*
*********************************************************************************************************
*                                              GLOBAL VARIABLES
*********************************************************************************************************
*/

typedef struct ir_data_struct 
{
    /*Capture OS Tick*/
    volatile unsigned int previous_tick_count;
    volatile unsigned int current_tick_count;

    volatile unsigned int alive_counter;
    
    /*Capture Frame*/
    volatile unsigned int IR_Command_Sequence; 
    volatile unsigned int IR_Address_Sequence;
    volatile unsigned int IR_Startup_Sequence;  
    volatile unsigned int IR_Start_Frame_Init;
    volatile unsigned int IR_RepeatCode_Sequence;
    volatile unsigned int IR_CommandData_Time_Counter; 

    /*Capture Command Data*/
    volatile unsigned  int ir_raw_data_0;

} ir_strucet_t; 

static ir_strucet_t ir_str_t=   {.previous_tick_count = FALSE,
                                .current_tick_count = FALSE,
                                .IR_Command_Sequence = FALSE,
                                .IR_Address_Sequence = FALSE,
                                .IR_Startup_Sequence = FALSE,
                                .IR_Start_Frame_Init = FALSE,
                                .IR_RepeatCode_Sequence = FALSE,
                                .IR_CommandData_Time_Counter = FALSE,
                                .ir_raw_data_0 = 0b0000000
                                }; 

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
    if(ir_str_t.IR_RepeatCode_Sequence == FALSE && ir_str_t.IR_Command_Sequence == TRUE && ir_str_t.IR_Address_Sequence == TRUE && ir_str_t.IR_Startup_Sequence == TRUE && ir_str_t.IR_Start_Frame_Init == TRUE)
    {
        ir_str_t.current_tick_count = OSTimeGet();
        if(((ir_str_t.current_tick_count - ir_str_t.previous_tick_count) >= 121))
        {

            ir_str_t.IR_Start_Frame_Init = FALSE;
            breakpoint();
            ir_str_t.ir_raw_data_0 = FALSE;
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
    if(ir_str_t.IR_Command_Sequence == FALSE && ir_str_t.IR_Address_Sequence == TRUE && ir_str_t.IR_Startup_Sequence == TRUE && ir_str_t.IR_Start_Frame_Init == TRUE)
    {
        ir_str_t.current_tick_count = OSTimeGet();
        if(((ir_str_t.current_tick_count - ir_str_t.previous_tick_count) >= (IR_FRAME_TIME_STARTUP_PLUS_ADDRESS + ir_str_t.IR_CommandData_Time_Counter))&& ((ir_str_t.current_tick_count - ir_str_t.previous_tick_count) <= IR_FRAME_TOTAL_TIME))
        {
            if (IR_TSOP_SENSOR_PIN_STATUS == FALSE) {
                ir_str_t.ir_raw_data_0 &= ~(1 << (8 - (ir_str_t.IR_CommandData_Time_Counter/3))); //Clear bit (7-b)
            } else {
                ir_str_t.ir_raw_data_0 |= (1 << (8 - (ir_str_t.IR_CommandData_Time_Counter/3))); //Set bit (7-b)
            } 
            
            ir_str_t.IR_CommandData_Time_Counter = ir_str_t.IR_CommandData_Time_Counter +3;

        }
        else
        {
                /**Do Nothing*/
        }
        if((ir_str_t.current_tick_count - ir_str_t.previous_tick_count) >=69)
        {
            OSMboxPost(msgbox, ir_str_t.ir_raw_data_0);
            ir_str_t.IR_Command_Sequence = TRUE;
            ir_str_t.IR_CommandData_Time_Counter = 3;
            ir_str_t.previous_tick_count = OSTimeGet();
            ir_str_t.IR_Start_Frame_Init = FALSE;
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
    if(ir_str_t.IR_Address_Sequence == FALSE && ir_str_t.IR_Startup_Sequence == TRUE && ir_str_t.IR_Start_Frame_Init == TRUE)
    {
    
        ir_str_t.current_tick_count = OSTimeGet();
        if((ir_str_t.current_tick_count - ir_str_t.previous_tick_count) > IR_FRAME_TIME_STARTUP_PLUS_ADDRESS)
        {
            ir_str_t.IR_Address_Sequence = TRUE;
            ir_str_t.IR_Command_Sequence = FALSE;

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
    if(ir_str_t.IR_Startup_Sequence == FALSE && ir_str_t.IR_Start_Frame_Init == TRUE)
    {
        ir_str_t.current_tick_count = OSTimeGet();
        if((ir_str_t.current_tick_count - ir_str_t.previous_tick_count) > 14)
        {
            ir_str_t.IR_Startup_Sequence = TRUE;
            ir_str_t.IR_Address_Sequence = FALSE;

        }
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
    if(ir_str_t.IR_Start_Frame_Init == FALSE)
    {
        ir_str_t.previous_tick_count = OSTimeGet();
        ir_str_t.IR_Start_Frame_Init = TRUE;
        ir_str_t.IR_Startup_Sequence = FALSE;
    }

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
    IR_ISR_Init();
    IR_Retrive_StartFrame();
    IR_Retrive_AddressData();
    IR_Retrive_CommandData();
    // IR_Retrive_RepeatCode();
}

