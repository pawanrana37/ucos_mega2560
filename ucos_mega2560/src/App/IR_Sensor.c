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
#define IR_TSOP_SESOR				PE5
#define IR_TSOP_SENSOR_PIN_STATUS Dio_ReadChannel(AVR_PORTE,IR_TSOP_SESOR)

#define IR_TSOP_SENSOR_9ms_COUNT 9000u

#define IR_TSOP_SENSOR_4_5ms_COUNT 4500u

#define IR_TSOP_SENSOR_562us_COUNT 562u

#define IR_TSOP_SENSOR_2_25ms_COUNT 2250u

#define IR_TSOP_SENSOR_1_12ms_COUNT 1120u

#define IR_TSOP_SENSOR_3_37ms_COUNT 3370u
/*
*********************************************************************************************************
*                                              GLOBAL VARIABLES
*********************************************************************************************************
*/
    /*Capture OS Tick*/
    volatile unsigned int previous_tick_count;
    volatile unsigned int current_tick_count;

    /*Capture Frame*/
    volatile unsigned int IR_Command_Sequence_Data=0; 
    volatile unsigned int IR_Command_Sequence_Data_Inverse=0; 
    volatile unsigned int IR_Address_Sequence_Data=0;
    volatile unsigned int IR_Address_Sequence_Data_Inverse=0;
    volatile unsigned int IR_Startup_Sequence_4_5ms=0;  
    volatile unsigned int IR_Startup_Sequence_9ms=0; 
    volatile unsigned int IR_Start_Frame_Init=0;
    volatile unsigned int IR_RepeatCode_Sequence=0;
    volatile unsigned int IR_CommandData_Time_Counter=0; 
    volatile unsigned int IR_Address_Dummy_Sequence=0; 

    /*Capture Command Data*/
    volatile int ir_raw_command_data_0=0x00;
    volatile int ir_raw_command_data_1=0x00;

    /*Capture Address Data*/
    volatile int ir_raw_address_data_0=0x00;
    volatile int ir_raw_address_data_1=0x00;

    /*calculate number of interrupts*/
    volatile unsigned  int ir_no_interrupts=0;

    extern OS_EVENT *msgbox; 

/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static void IR_Retrive_RepeatCode(void);
static void IR_Calculate_Frame(void);
static void IR_Retrive_CommandData(void);
static void IR_Retrive_CommandData_Inverse(void);
static void IR_Retrive_AddressData(void);
static void IR_Retrive_AddressData_Inverse(void);

static void IR_Retrive_StartFrame(void);
static void IR_ISR_Init(void);
void IR_Sensor_ISR_Config(void);
void IR_Sensor_TIMER_ISR_Config(void);

void    IR_Sensor_ISR_Enable(void);
void    IR_Sensor_ISR_Disable(void);
void    IR_Sensor_TIMER_ISR_Disable(void);
void    IR_Sensor_TIMER_ISR_Enable(void);

extern OS_EVENT *msgbox_ir; 


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
    ir_no_interrupts = 0;
    IR_Command_Sequence_Data_Inverse = FALSE;
    IR_Command_Sequence_Data = FALSE;
    IR_Address_Sequence_Data_Inverse = FALSE;
    IR_Address_Sequence_Data = FALSE;
    IR_Startup_Sequence_4_5ms = FALSE;
    IR_Startup_Sequence_9ms = FALSE;
    IR_Start_Frame_Init = FALSE;
    OSMboxPost(msgbox_ir,(int *)&ir_raw_command_data_0);
    ir_raw_command_data_1 = 0;
    ir_raw_command_data_0 = 0;
    ir_raw_address_data_0 = 0;
    ir_raw_address_data_1 = 0;

}
/*
*********************************************************************************************************
*                                                   IR_Retrive_CommandData_Inverse
* Description : This function capture the IR Data Inverse Frame.
*               
* Arguments   : none
*********************************************************************************************************
*/
static void IR_Retrive_CommandData_Inverse(void)
{
    static unsigned int i=0;
    if(IR_Command_Sequence_Data_Inverse == FALSE && IR_Command_Sequence_Data == TRUE && IR_Address_Sequence_Data_Inverse == TRUE && IR_Address_Sequence_Data == TRUE && IR_Startup_Sequence_4_5ms == TRUE && IR_Startup_Sequence_9ms == TRUE && IR_Start_Frame_Init == TRUE)
    {
        while((PINE &(1<<IR_TSOP_SESOR)) != (1<<IR_TSOP_SESOR))
        {
            /*DO Nothing Dummy Transition*/
        }

        for(i=0;i<=7;i++)
        {
            IR_Command_Sequence_Data_Inverse = FALSE;

            while((PINE &(1<<IR_TSOP_SESOR)) == (1<<IR_TSOP_SESOR))
            {
                ir_no_interrupts++;

            }
            while((PINE &(1<<IR_TSOP_SESOR)) != (1<<IR_TSOP_SESOR))
            {
                ir_no_interrupts++;

            }
            if((ir_no_interrupts > IR_TSOP_SENSOR_2_25ms_COUNT))
            {
                ir_raw_command_data_1 |= (1 << (7 - i)); //Set bit (7-b)
                ir_no_interrupts = 0;
                
            } 
            else if ((ir_no_interrupts > IR_TSOP_SENSOR_1_12ms_COUNT))
            {
                ir_raw_command_data_1 &= ~(1 << (7 - i)); //Clear bit (7-b)
                ir_no_interrupts = 0;
            }

            IR_Command_Sequence_Data_Inverse = TRUE;
            ir_no_interrupts = 0;

        }

    }

}
/*
*********************************************************************************************************
*                                                   IR_Retrive_CommandData
* Description : This function capture the IR Data Frame.
*               
* Arguments   : none
*********************************************************************************************************
*/
static void IR_Retrive_CommandData(void)
{
    static unsigned int i=0;
    if(IR_Command_Sequence_Data == FALSE && IR_Address_Sequence_Data_Inverse == TRUE && IR_Address_Sequence_Data == TRUE && IR_Startup_Sequence_4_5ms == TRUE && IR_Startup_Sequence_9ms == TRUE && IR_Start_Frame_Init == TRUE)
    {
        while((PINE &(1<<IR_TSOP_SESOR)) != (1<<IR_TSOP_SESOR))
        {
            /*DO Nothing Dummy Transition*/
        }
        for(i=0;i<=7;i++)
        {
            IR_Command_Sequence_Data = FALSE;

            while((PINE &(1<<IR_TSOP_SESOR)) == (1<<IR_TSOP_SESOR))
            {
                ir_no_interrupts++;

            }
            while((PINE &(1<<IR_TSOP_SESOR)) != (1<<IR_TSOP_SESOR))
            {
                ir_no_interrupts++;

            }
            if((ir_no_interrupts > IR_TSOP_SENSOR_2_25ms_COUNT))
            {
                ir_raw_command_data_0 |= (1 << (7 - i)); //Set bit (7-b)
                ir_no_interrupts = 0;
                
            } 
            else if ((ir_no_interrupts > IR_TSOP_SENSOR_1_12ms_COUNT))
            {
                ir_raw_command_data_0 &= ~(1 << (7 - i)); //Clear bit (7-b)
                ir_no_interrupts = 0;
            }

            IR_Command_Sequence_Data = TRUE;
            ir_no_interrupts = 0;

        }

    }

}
/*
*********************************************************************************************************
*                                                   IR_Retrive_AddressData
* Description : This function capture the IR Address Inverse Frame.
*               
* Arguments   : none
*********************************************************************************************************
*/
static void IR_Retrive_AddressData_Inverse(void)
{
    static unsigned int i=0;
    if(IR_Address_Sequence_Data_Inverse == FALSE && IR_Address_Sequence_Data == TRUE && IR_Startup_Sequence_4_5ms == TRUE && IR_Startup_Sequence_9ms == TRUE && IR_Start_Frame_Init == TRUE)
    {
        while((PINE &(1<<IR_TSOP_SESOR)) != (1<<IR_TSOP_SESOR))
        {
            /*DO Nothing Dummy Transition*/
        }
        for(i=0;i<=7;i++)
        {
            IR_Address_Sequence_Data_Inverse = FALSE;

            while((PINE &(1<<IR_TSOP_SESOR)) == (1<<IR_TSOP_SESOR))
            {
                ir_no_interrupts++;

            }
            while((PINE &(1<<IR_TSOP_SESOR)) != (1<<IR_TSOP_SESOR))
            {
                ir_no_interrupts++;

            }
            if((ir_no_interrupts > IR_TSOP_SENSOR_2_25ms_COUNT))
            {
                ir_raw_address_data_1 |= (1 << (7 - i)); //Set bit (7-b)
                ir_no_interrupts = 0;
                
            } 
            else if ((ir_no_interrupts > IR_TSOP_SENSOR_1_12ms_COUNT) )
            {
                ir_raw_address_data_1 &= ~(1 << (7 - i)); //Clear bit (7-b)
                ir_no_interrupts = 0;
            }

            IR_Address_Sequence_Data_Inverse = TRUE;
            ir_no_interrupts = 0;

        }

    }
}
/*
*********************************************************************************************************
*                                                   IR_Retrive_AddressData
* Description : This function capture the IR Address Frame.
*               
* Arguments   : none
*********************************************************************************************************
*/
static void IR_Retrive_AddressData(void)
{
    static unsigned int i=0;
    if(IR_Address_Sequence_Data == FALSE && IR_Startup_Sequence_4_5ms == TRUE && IR_Startup_Sequence_9ms == TRUE && IR_Start_Frame_Init == TRUE)
    {
        while((PINE &(1<<IR_TSOP_SESOR)) != (1<<IR_TSOP_SESOR))
        {
            /*DO Nothing Dummy Transition*/
        }
        for(i=0;i<=7;i++)
        {
            IR_Address_Sequence_Data = FALSE;

            while((PINE &(1<<IR_TSOP_SESOR)) == (1<<IR_TSOP_SESOR))
            {
                ir_no_interrupts++;

            }
            while((PINE &(1<<IR_TSOP_SESOR)) != (1<<IR_TSOP_SESOR))
            {
                ir_no_interrupts++;

            }
            if ((ir_no_interrupts > IR_TSOP_SENSOR_1_12ms_COUNT))
            {
                ir_raw_address_data_0 &= ~(1 << (7 - i)); //Clear bit (7-b)
                ir_no_interrupts = 0;
            }
            else if((ir_no_interrupts > IR_TSOP_SENSOR_2_25ms_COUNT))
            {
                ir_raw_address_data_0 |= (1 << (7 - i)); //Set bit (7-b)
                ir_no_interrupts = 0;
                
            } 

            IR_Address_Sequence_Data = TRUE;
            ir_no_interrupts = 0;

        }

    }
}
/*
*********************************************************************************************************
*                                                   IR_Retrive_StartFrame
* Description : This function capture the 4.5ms HIGH pulse.
*               
* Arguments   : none
*********************************************************************************************************
*/

static void IR_Retrive_StartFrame_4_5ms(void)
{
    static unsigned int i=0;

    if(IR_Startup_Sequence_4_5ms == FALSE && IR_Startup_Sequence_9ms == TRUE && IR_Start_Frame_Init == TRUE)
    {
        while((PINE &(1<<IR_TSOP_SESOR)) == (1<<IR_TSOP_SESOR))
        {
            ir_no_interrupts++;
        }
        if((ir_no_interrupts > IR_TSOP_SENSOR_4_5ms_COUNT))
        {
            IR_Startup_Sequence_4_5ms  = TRUE;
            ir_no_interrupts = 0;
        }

    }
}
/*
*********************************************************************************************************
*                                                   IR_Retrive_StartFrame
* Description : This function capture the 9ms LOW pulse.
*               
* Arguments   : none
*********************************************************************************************************
*/

static void IR_Retrive_StartFrame_9ms(void)
{
    if(IR_Startup_Sequence_9ms == FALSE && IR_Start_Frame_Init == TRUE)
    {
        while((PINE &(1<<IR_TSOP_SESOR)) != (1<<IR_TSOP_SESOR))
        {
            ir_no_interrupts++;

        }
        if((ir_no_interrupts > IR_TSOP_SENSOR_9ms_COUNT))
        {
            IR_Startup_Sequence_9ms  = TRUE;
            ir_no_interrupts = 0;
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
    if(IR_Start_Frame_Init == FALSE)
    {
        IR_Start_Frame_Init = TRUE;
    }

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
void  IR_Sensor_ISR_Config (void)
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
    IR_Sensor_ISR_Disable();
    IR_ISR_Init();
    IR_Retrive_StartFrame_9ms();
    IR_Retrive_StartFrame_4_5ms();
    IR_Retrive_AddressData();
    IR_Retrive_AddressData_Inverse();
    IR_Retrive_CommandData();
    IR_Retrive_CommandData_Inverse();
    IR_Retrive_RepeatCode();
    IR_Sensor_ISR_Enable();

}


/*
*********************************************************************************************************
*                                                   IR_Sensor_ISR_Disable
* Description : This function should be called by your application code before you make use of any of the
*               functions found in this module.This function shall disable the ISR.
*               
* Arguments   : none
*********************************************************************************************************
*/
void  IR_Sensor_ISR_Disable(void)
{
    EIMSK&= ~(1<<INT5); 
}
/*
*********************************************************************************************************
*                                                   IR_Sensor_ISR_Enable
* Description : This function should be called by your application code before you make use of any of the
*               functions found in this module.This function shall enable the ISR.
*               
* Arguments   : none
*********************************************************************************************************
*/
void  IR_Sensor_ISR_Enable(void)
{
    EIMSK |= (1<<INT5); 
}
