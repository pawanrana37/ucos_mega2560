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

OS_EVENT *msgbox_ir; 
OS_EVENT *msgbox_temperature; 
OS_EVENT *msgbox_humidity;

volatile unsigned char *ir_received_data_ptr=NULL;
volatile unsigned char ir_received_data=0;

volatile unsigned char *temperature_received_data_ptr=NULL;
volatile unsigned char temperature_received_data=0;

volatile unsigned char *humidity_received_data_ptr=NULL;
volatile unsigned char humidity_received_data=0;

extern int ir_raw_command_data_0;
int *ir_raw_command_data_0_ptr = &ir_raw_command_data_0;

extern int temperature;
int *temperature_raw_command_data_0_ptr = &temperature;

extern int humidity;
int *humidity_raw_command_data_0_ptr = &humidity;
INT8U err;

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
extern void fx_LCD_temperature_Data(unsigned char);
extern void fx_LCD_humidity_Data(unsigned char);
extern void LCD_String(char *str);
extern void fx_DHT11_MainFunction(void);
extern void Fx_DC_Motor_MainFunction(void);
extern int uart_putc ( unsigned char c );
extern void uart_puts ( char * s );

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
    msgbox_ir = OSMboxCreate(ir_raw_command_data_0_ptr);
    if(msgbox_ir == ((void *)0))
    {
        /* Failed to create message box */
        /* Return error to caller? */
    } 

    msgbox_temperature = OSMboxCreate(temperature_raw_command_data_0_ptr);
    if(msgbox_temperature == ((void *)0))
    {
        /* Failed to create message box */
        /* Return error to caller? */
    } 

    msgbox_humidity = OSMboxCreate(humidity_raw_command_data_0_ptr);
    if(msgbox_humidity == ((void *)0))
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
        // ir_received_data_ptr = OSMboxPend(msgbox, 0, &err);
        // if(ir_received_data_ptr !=NULL && (*(unsigned char *)(ir_received_data_ptr) != 0))
        // {
        //     if((*(unsigned char *)(ir_received_data_ptr) == 24))
        //     {
                //Fx_LED_Blink_MainFunction();
                OSTimeDlyHMSM(0,0,1,0);
            // }

    }
    

    //}   
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

static void TASK_RTE_LCD(void *p_arg)
{
    while(1)
    {
        temperature_received_data_ptr = OSMboxPend(msgbox_temperature, 0, &err);

        if(temperature_received_data_ptr != NULL && (*(unsigned char *)(temperature_received_data_ptr) >= 0))
        {
            temperature_received_data = *(unsigned char *)(temperature_received_data_ptr);

            fx_LCD_temperature_Data(temperature_received_data);
            //fx_LCD_temperature_Data(19);
        }

        humidity_received_data_ptr = OSMboxPend(msgbox_humidity, 0, &err);
        if(humidity_received_data_ptr != NULL && (*(unsigned char *)(humidity_received_data_ptr)>= 0))
        {
            humidity_received_data = *(unsigned char *)(humidity_received_data_ptr);
            fx_LCD_humidity_Data(humidity_received_data);
            //fx_LCD_humidity_Data(63);
        }
        OSTimeDlyHMSM(0,0,3,0); 
    }  
    
}

/*
*********************************************************************************************************
*                                               TASK_RTE_DHT11
*
* Description : This Task will invoke the Runnables for Temperature and Humidity Sensor DHT11
*               
* Arguments   : none
*********************************************************************************************************
*/
static void TASK_RTE_DHT11(void *p_arg)
{

    while(1)
    {

        fx_DHT11_MainFunction();
        OSTimeDlyHMSM(0,0,3,0);

    }   
   
}
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
        OSTimeDlyHMSM(0,0,1,0);
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
    char buf[7];    
    while(1)
    {
 
        ir_received_data_ptr = OSMboxPend(msgbox_ir, 0, &err);
        if(ir_received_data_ptr !=NULL)
        {
            ir_received_data = (*(unsigned char *)(ir_received_data_ptr));
            sprintf(buf, "0x%x",ir_received_data);
            uart_puts (buf);  
            OSMboxPost(msgbox_ir,(int *)&ir_received_data);
        }
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
                    (INT8U           ) 5,
                    (INT16U          ) 5,
                    (OS_STK         *)&TaskStk[0][0],
                    (INT32U          ) APP_CFG_TASK_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));


    OSTaskCreateExt((void (*)(void *)) TASK_RTE_LCD,
                    (void           *) 0,
                    (OS_STK         *)&TaskStk[1][APP_CFG_TASK_STK_SIZE - 1],
                    (INT8U           ) 4,
                    (INT16U          ) 4,
                    (OS_STK         *)&TaskStk[1][0],
                    (INT32U          ) APP_CFG_TASK_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

    OSTaskCreateExt((void (*)(void *)) TASK_RTE_DHT11,
                    (void           *) 0,
                    (OS_STK         *)&TaskStk[2][APP_CFG_TASK_STK_SIZE - 1],
                    (INT8U           ) 3,
                    (INT16U          ) 3,
                    (OS_STK         *)&TaskStk[2][0],
                    (INT32U          ) APP_CFG_TASK_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

    // OSTaskCreateExt((void (*)(void *)) TASK_RTE_DC_MOTOR,
    //                 (void           *) 0,
    //                 (OS_STK         *)&TaskStk[2][APP_CFG_TASK_STK_SIZE - 1],
    //                 (INT8U           ) 5,
    //                 (INT16U          ) 5,
    //                 (OS_STK         *)&TaskStk[2][0],
    //                 (INT32U          ) APP_CFG_TASK_STK_SIZE,
    //                 (void           *) 0,
    //                 (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

    // OSTaskCreateExt((void (*)(void *)) TASK_RTE_IR,
    //                 (void           *) 0,
    //                 (OS_STK         *)&TaskStk[1][APP_CFG_TASK_STK_SIZE - 1],
    //                 (INT8U           ) 4,
    //                 (INT16U          ) 4,
    //                 (OS_STK         *)&TaskStk[1][0],
    //                 (INT32U          ) APP_CFG_TASK_STK_SIZE,
    //                 (void           *) 0,
    //                 (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

    // OSTaskCreateExt((void (*)(void *))TASK_RTE_UART,
    //             (void           *) 0,
    //             (OS_STK         *)&TaskStk[2][APP_CFG_TASK_STK_SIZE - 1],
    //             (INT8U           ) 4,
    //             (INT16U          ) 4,
    //             (OS_STK         *)&TaskStk[2][0],
    //             (INT32U          ) APP_CFG_TASK_STK_SIZE,
    //             (void           *) 0,
    //             (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
} 
