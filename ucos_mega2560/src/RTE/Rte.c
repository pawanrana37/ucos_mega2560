/*
*********************************************************************************************************
*                                           Atmel ATmega256
*                                               Rte
*
*                                (c) Copyright 2005, Micrium, Weston, FL
*                                          All Rights Reserved
*
*
* File : Rte.c
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

/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/
# define RTE_E_NOT_OK 1
# define RTE_E_OK 0
/*
*********************************************************************************************************
*                                              GLOBAL VARIABLES
*********************************************************************************************************
*/

static OS_STK TaskStartStk[APP_CFG_TASK_START_STK_SIZE];
static OS_STK TaskStk[APP_CFG_N_TASKS][APP_CFG_TASK_STK_SIZE];
OS_EVENT *msgbox; 
OS_EVENT *msgbox1; 
OS_EVENT *msgbox2;
/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static void TaskStart(void *p_arg);
static void TaskStartCreateTasks(void);
extern void OS_Resource_Init(void);
extern void OS_Task_Create_Ext(void);
extern void Fx_LED_Blink_MainFunction(void);
extern void Fx_DC_Motor_MainFunction(void);

/*
*********************************************************************************************************
*                                                 START TASK
*********************************************************************************************************
*/

static void TaskStart(void *p_arg)
{
    /*STATISTICS INITIALIZATION*/
    #if (OS_TASK_STAT_EN > 0)
    OSStatInit();
    #endif

    /*Create Tasks*/
    TaskStartCreateTasks();

    /* Delete start task*/
    OSTaskDel(OS_PRIO_SELF);    
} 

/*
*********************************************************************************************************
*                                                 EXT TASK
*********************************************************************************************************
*/

void OS_Task_Create_Ext()
{
    /*Create the start task*/
    OSTaskCreateExt((void (*)(void *)) TaskStart,
                    (void           *) 0,
                    (OS_STK         *)&TaskStartStk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_START_PRIO,
                    (INT16U          ) APP_CFG_TASK_START_PRIO,
                    (OS_STK         *)&TaskStartStk[0],
                    (INT32U          )(APP_CFG_TASK_START_STK_SIZE),
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));   
} 


/*
*********************************************************************************************************
*                                                   OS Resources
*********************************************************************************************************
*/
void OS_Resource_Init()
{

    /*Message Box Initialization*/
    msgbox = OSMboxCreate((void *)0);
    if(msgbox == ((void *)0))
    {
        /* Failed to create message box */
        /* Return error to caller? */
    } 

    msgbox1 = OSMboxCreate((void *)0);
    if(msgbox1 == ((void *)0))
    {
        /* Failed to create message box */
        /* Return error to caller? */
    } 

    msgbox2 = OSMboxCreate((void *)0);
    if(msgbox2 == ((void *)0))
    {
        /* Failed to create message box */
        /* Return error to caller? */
    } 
}

/*
*********************************************************************************************************
*                                                   TASKS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               TASK_RTE_LED
*
* Description : This Task will trigger the Runnables for LED Functionality
*               
* Arguments   : none
*********************************************************************************************************
*/
static void TASK_RTE_LED(void *p_arg)
{
    while (1)
    {
        Fx_LED_Blink_MainFunction();
        OSTimeDlyHMSM(0,0,0,100);

    }   
} 
/*
*********************************************************************************************************
*                                               TASK_RTE_LCD
*
* Description : This Task will Trigger the Runnables for LCD Functionality
*               
* Arguments   : none
*********************************************************************************************************
*/

// static void TASK_RTE_LCD(void *p_arg)
// {

//     unsigned char i,a[]={"Anuyukti Kumari"};
//     Lcd_CmdWrite(0x02);        // Initialize Lcd in 4-bit mode
//     Lcd_CmdWrite(0x28);        // enable 5x7 mode for chars 
//     Lcd_CmdWrite(0x01);        // Clear Display
//     Lcd_CmdWrite(0x0C);        // Display ON, Cursor OFF
//     Lcd_CmdWrite(0x06);          /*increment cursor (shift cursor to right)*/	
//     Lcd_CmdWrite(0x80);        // Move the cursor to beginning of first line    */  
//     // Task body (always written as an infinite loop) 
//     while(1)
//     {

//         Lcd_DataWrite('H');
//         Lcd_DataWrite('e');
//         Lcd_DataWrite('l');
//         Lcd_DataWrite('l');
//         Lcd_DataWrite('o');
//         Lcd_DataWrite(' ');
//         // Lcd_DataWrite('w');
//         // Lcd_DataWrite('o');
//         // Lcd_DataWrite('r');
//         // Lcd_DataWrite('l');
//         // Lcd_DataWrite('d');

//         Lcd_CmdWrite(0xc0);        //Go to Next line and display Good Morning
//         for(i=0;a[i]!=0;i++)
//         {
//             Lcd_DataWrite(a[i]);
//         }
//     }  
        
// } // Task3()
/*
*********************************************************************************************************
*                                               TASK_RTE_DC_MOTOR
*
* Description : This Task will invoke the Runnables for DC Motor
*               
* Arguments   : none
*********************************************************************************************************
*/
static void TASK_RTE_DC_MOTOR(void *p_arg)
{

    while(1)
    {
        Fx_DC_Motor_MainFunction();

    }   
   
}
/*
*********************************************************************************************************
*                                               TASK_RTE_IR
*
* Description : This Task will invoke the IR Runnables
*               
* Arguments   : none
*********************************************************************************************************
*/

static void TASK_RTE_IR(void *p_arg)
{
    while(1)
    {
        OSTimeDlyHMSM(0,0,2,0);
    }

} 

/*
*********************************************************************************************************
*                                               TASK_RTE_UART
*
* Description : This Task will invoke the UART Runnables
*               
* Arguments   : none
*********************************************************************************************************
*/

static void TASK_RTE_UART(int *p_arg)
{
    static unsigned int *ir_received_data=0;
    INT8U err;
    while(1)
    {
        ir_received_data = OSMboxPend(msgbox, 0, &err);

        switch (err) 
        { 
            case OS_ERR_NONE:
            case OS_ERR_TIMEOUT:
            case OS_ERR_PEND_ABORT:
            break;
            default:
            break;
        }

        if (err != OS_ERR_NONE) 
        { 
        /* Try again? */
        } 

        while(!(UCSR0A & (1<<UDRE0)));
 
        UDR0 = (*(unsigned int *)(ir_received_data));
        breakpoint();
        OSTimeDlyHMSM(0,0,0,1000);
    }
} 

/*
*********************************************************************************************************
*                                               TaskStartCreateTasks
*
* Description : This function create OS Tasks
*               
* Arguments   : none
*********************************************************************************************************
*/

void TaskStartCreateTasks(void)
{
    OSTaskCreateExt((void (*)(void *)) TASK_RTE_LED,
                    (void           *) 0,
                    (OS_STK         *)&TaskStk[0][APP_CFG_TASK_STK_SIZE - 1],
                    (INT8U           ) 3,
                    (INT16U          ) 3,
                    (OS_STK         *)&TaskStk[0][0],
                    (INT32U          ) APP_CFG_TASK_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));


    // OSTaskCreateExt((void (*)(void *)) TASK_RTE_LCD,
    //                 (void           *) 0,
    //                 (OS_STK         *)&TaskStk[1][APP_CFG_TASK_STK_SIZE - 1],
    //                 (INT8U           ) 4,
    //                 (INT16U          ) 4,
    //                 (OS_STK         *)&TaskStk[1][0],
    //                 (INT32U          ) APP_CFG_TASK_STK_SIZE,
    //                 (void           *) 0,
    //                 (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

    // OSTaskCreateExt((void (*)(void *)) TASK_RTE_DC_MOTOR,
    //                 (void           *) 0,
    //                 (OS_STK         *)&TaskStk[2][APP_CFG_TASK_STK_SIZE - 1],
    //                 (INT8U           ) 5,
    //                 (INT16U          ) 5,
    //                 (OS_STK         *)&TaskStk[2][0],
    //                 (INT32U          ) APP_CFG_TASK_STK_SIZE,
    //                 (void           *) 0,
    //                 (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

    OSTaskCreateExt((void (*)(void *)) TASK_RTE_IR,
                    (void           *) 0,
                    (OS_STK         *)&TaskStk[1][APP_CFG_TASK_STK_SIZE - 1],
                    (INT8U           ) 4,
                    (INT16U          ) 4,
                    (OS_STK         *)&TaskStk[1][0],
                    (INT32U          ) APP_CFG_TASK_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

    // OSTaskCreateExt((void (*)(void *))TASK_RTE_UART,
    //             (void           *) 0,
    //             (OS_STK         *)&TaskStk[2][APP_CFG_TASK_STK_SIZE - 1],
    //             (INT8U           ) 5,
    //             (INT16U          ) 5,
    //             (OS_STK         *)&TaskStk[2][0],
    //             (INT32U          ) APP_CFG_TASK_STK_SIZE,
    //             (void           *) 0,
    //             (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
} 
