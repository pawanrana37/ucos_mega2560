/*
*********************************************************************************************************
*                                     uC/OS-II on the Atmel ATxmega128A1
*********************************************************************************************************
*
* Project Version:  24th August 2012
* uC/OS-II Version: 2.92.01
*
* This project has been tested with the following:
*
* Target Board:     Atmel AVR XMEGA-A1 Xplained Rev 7 (ATxmega128A1)
* Programmer:       Atmel AVR JTAGICE mkII (via JTAG)
* Debugger:         Atmel AVR JTAGICE mkII (via JTAG)
* IDE:              Atmel Studio 6 (Version 6.0.1703 - beta)
* Compiler:         AVRGCC (3.3.2.31)
* Assembler:        AVRAssembler (2.1.51.24)
*
* Portions of code contained in this project were produced by the CodeWizardAVR V2.05.0 Standard
* Automatic Program Generator
* � Copyright 1998-2010 Pavel Haiduc, HP InfoTech s.r.l.
* http://www.hpinfotech.com
*
*********************************************************************************************************
*                                                  Notes
*********************************************************************************************************
*
* 1.    CPU clock frequency can be changed in Atmel Studio 6 via Project Properties (in the AVR/GNU C Compiler and AVR/GNU Assembler "Symbols" sections). Default is 32 MHz.
*       Valid frequencies = [32000000,2000000]
* 2.    Choose USART to print to in includes.h
*
*********************************************************************************************************
*                                       Atmel AVR Xplained Assignments
*********************************************************************************************************
***************************
* PORT E LEDs (active-low):
***************************
* LED0: Toggles when the Idle Task is run - see app_hooks.c for more details.
* LED1: Toggles when the Statistics Task is run - see app_hooks.c for more details.
* LED2: Toggles when a task switch is performed - see app_hooks.c for more details.
* LED3: Toggles each timer tick - see app_hooks.c and os_cfg.h for more details.
* LED4: Toggles when Task 2 is run (every 250ms) - see below for more details.
* LED5: Toggles when Task 3 is run (every 500ms) - see below for more details.
* LED6: Toggles when Task 4 is run (every 1000ms) - see below for more details.
* LED7: Toggles when pushbutton_timer_isr_handler() is invoked (every 20ms) - see below for more details.
*
************************************************************************
* PORT D(PD0:PD5) and PORT R(PR0:PR1) Push-Button Switches (active-low):
************************************************************************
* SW0:  Prints the OSStatTaskCPUUsage variable to USARTXX.
* SW1:  Prints the clock speed (F_CPU) to USARTXX.
* SW2:  Prints the uC/OS-III version (OS_VERSION) and the interrupt enable/disable method (CPU_CFG_CRITICAL_METHOD) to USARTXX.
* SW3:  Prints the tick rate (OS_CFG_TICK_RATE_HZ) and the USARTF0 RX ring buffer size (in bytes) (USARTXX_RX_BUFFER_SIZE_BYTES) to USARTXX.
* SW4:  Prints "Push-Button 4 press detected." to USARTXX.
* SW5:  Prints "Push-Button 5 press detected." to USARTXX.
* SW6:  Prints "Push-Button 6 press detected." to USARTXX.
* SW7:  Toggles ON/OFF the printing of the CPU USAGE variable to USARTXX every 1000ms.
*
* USARTC0 Pinouts:  PC2: RX
*                   PC3: TX
* USARTF0 Pinouts:  PF2: RX
*                   PF3: TX
*
* Note: Data sent to the USARTXX RX pin is echoed back to the USARTXX TX pin by Task 1.
*
*********************************************************************************************************
*        Please send all comments, suggestions, bug reports to: nickdademo(at)gmail.com
*********************************************************************************************************
*/

#include "includes.h"
#include <avr_debugger.h>
#include "avr8-stub.h"


/*
*********************************************************************************************************
*                                              GLOBAL VARIABLES
*********************************************************************************************************
*/
static OS_STK TaskStartStk[APP_CFG_TASK_START_STK_SIZE];
static OS_STK TaskStk[APP_CFG_N_TASKS][APP_CFG_TASK_STK_SIZE];

volatile INT8U buttonState;
bool show_cpu_usage=true;

// USARTC0 Receiver buffer
#define USARTC0_RX_BUFFER_SIZE_BYTES 512
char rx_buffer_usartc0[USARTC0_RX_BUFFER_SIZE_BYTES];
unsigned int rx_wr_index_usartc0=0,rx_rd_index_usartc0=0;
volatile unsigned int rx_counter_usartc0=0;
volatile bool rx_buffer_overflow_usartc0=false;

// USARTF0 Receiver buffer
#define USARTF0_RX_BUFFER_SIZE_BYTES 512
char rx_buffer_usartf0[USARTF0_RX_BUFFER_SIZE_BYTES];
unsigned int rx_wr_index_usartf0=0,rx_rd_index_usartf0=0;
volatile unsigned int rx_counter_usartf0=0;
volatile bool rx_buffer_overflow_usartf0=false;

volatile char out[32];
volatile unsigned int outp;
unsigned char ir_code=0;
volatile unsigned char tx_send;
static volatile INT32U ir_count;
static volatile INT32U ir_prev_count;
static float total_time_interval=0.00f;
static volatile INT32U current_timer_tick_count=0;
static volatile INT32U previous_timer_tick_count=0;
static volatile unsigned char current_interrupt_count=0;
static volatile unsigned char previous_interrupt_count=0;
// typedef struct ir_data_struct {
int first_start_pulse;
int seccound_verification_puls;
static volatile unsigned int previous_tick_count=0;
static volatile unsigned int current_tick_count=0;
static volatile unsigned int current_tick_count_4ms=0;
static volatile unsigned int alive_counter=0;
// static volatile unsigned int IR_Startup_Sequence_9ms=0;
// static volatile unsigned int previous_tick_count;
// static volatile unsigned int current_tick_count;
// static volatile INT32U IR_Startup_Sequence;
static volatile INT32U previous_tick_temp_count;
static volatile INT32U IR_Startup_Sequence_4_5ms;
static volatile INT32U IR_Data_Sequence;
static volatile INT32U IR_Data_Counter;
static volatile INT32U IR_Startup_Sequence_Flag;
static volatile unsigned int IR_Command_Sequence=0; 
static volatile unsigned int IR_Address_Sequence=0;
static volatile unsigned int IR_Startup_Sequence=0;  
static volatile unsigned int IR_Start_Frame_Init=0;
static volatile unsigned int IR_CommandData_Time_Counter=0; 
static volatile unsigned int IR_RepeatCode_Sequence=0;
unsigned volatile int ir_received_data=0b0000000;
unsigned volatile int ir_raw_data_0 = 0b0000000;
unsigned volatile int ir_raw_data_1 = 0b0000000;
unsigned volatile int ir_raw_data_2 = 0b0000000;
// } ir_strucet_t; // structure to hold IR data 
// ir_strucet_t ir_str_t; // structure variable
void protocol_check();
void IR_Retrive_RepeatCode(void);
void IR_Calculate_Frame(void);
void IR_Retrive_CommandData(void);
void IR_Retrive_AddressData(void);
void IR_Retrive_StartFrame(void);
void IR_ISR_Init(void);
int read_data();
// static unsigned int alive_counter;
int ir_result(char debug_info);
// static void UART_Send_IR(unsigned int uart_data );
static volatile INT32U count=0;
// Semaphore declarations
OS_EVENT *usartc0_rx;
OS_EVENT *usartf0_rx;
OS_EVENT *pushbutton_event;
OS_EVENT *IR_Sensor;
OS_Q IRS_Q;
OS_EVENT *msgbox; 
OS_EVENT *msgbox1; 
OS_EVENT *msgbox2;
extern unsigned int ISR_INT5_INTERRUPT_FLAG;


/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static void TaskStart(void *p_arg);
void TaskStartCreateTasks(void);
void AVRInit(void);
void PortsInit(void);
void SystemClocksInit(void);
void USARTC0Init(void);
void USARTF0Init(void);
void PushButtonTimerInit(void);
void Lcd_DataWrite(unsigned char dat);
void Lcd_CmdWrite(unsigned char cmd);
// static void  UART_Send_IR(unsigned char data);
// static unsigned char  IR_Read_Code(void);
//void tc0_disable(TC0_t *ptc);
//void tc1_disable(TC1_t *ptc);
void ClockTickStart(void);
#if PRINT_TO_USART == C0
char getchar_usartc0(void);
int usartc0_putchar(char c, FILE *stream);
#elif PRINT_TO_USART == F0
char getchar_usartf0(void);
int usartf0_putchar(char c, FILE *stream);
#endif

/*
*********************************************************************************************************
*                                                   MAIN
*********************************************************************************************************
*/

// Set the standard output stream
#if PRINT_TO_USART == C0
//static FILE mystdout = FDEV_SETUP_STREAM(usartc0_putchar, NULL, _FDEV_SETUP_WRITE);
#elif PRINT_TO_USART == F0
static FILE mystdout = FDEV_SETUP_STREAM(usartf0_putchar, NULL, _FDEV_SETUP_WRITE);
#endif

int main(void)
{

    // breakpoint();
    // Redefine standard output stream
    //stdout=&mystdout;
    // Initialize AVR
    BSP_Init();
    debug_init();
    // Initialize uC/OS-II
    OSInit();
    /*Create Message Queue*/
    // IR_Sensor = OSQCreate((OS_Q *)&IRS_Q,10);
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
    // Create the start task
    OSTaskCreateExt((void (*)(void *)) TaskStart,
                    (void           *) 0,
                    (OS_STK         *)&TaskStartStk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_START_PRIO,
                    (INT16U          ) APP_CFG_TASK_START_PRIO,
                    (OS_STK         *)&TaskStartStk[0],
                    (INT32U          )(APP_CFG_TASK_START_STK_SIZE),
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
    // Create semaphores
    //usartc0_rx=OSSemCreate(0);
    //usartf0_rx=OSSemCreate(0);
    //pushbutton_event=OSSemCreate(0);
    // Start multi-tasking

    OSStart();

    return 0;
}

/*
*********************************************************************************************************
*                                                 START TASK
*********************************************************************************************************
*/

// StartTask: PRIORITY=2
static void TaskStart(void *p_arg)
{
    // Begin clock tick
    //ClockTickStart();

    #if (OS_TASK_STAT_EN > 0)
    OSStatInit();
    #endif
    // Create tasks
    TaskStartCreateTasks();
    // Delete start task
    OSTaskDel(OS_PRIO_SELF);    
} // TaskStart()

/*
*********************************************************************************************************
*                                                   TASKS
*********************************************************************************************************
*/
#if 0
// Task1: PRIORITY=3
static void Task1(void *p_arg)
{
    // Local variables
    INT8U err;
    // Task body (always written as an infinite loop) 
    while(1)
    {
        #if PRINT_TO_USART == C0
        // Wait on semaphore
        OSSemPend(usartc0_rx,0,&err);
        // Echo data received
        printf("%c", getchar_usartc0());
        #elif PRINT_TO_USART == F0
        // Wait on semaphore
        OSSemPend(usartf0_rx,0,&err);
        // Echo data received
        printf("%c", getchar_usartf0());
        #endif
    }
} // Task1()
#endif
// Task1: PRIORITY=3
static void Task1(void *p_arg)
{
    while (1)
    {
        // alive_counter++;
        // Task body (always written as an infinite loop) 
        
        // breakpoint();
        PORTJ^=(1<<LED_PIN0);

        OSTimeDlyHMSM(0,0,0,500);
    }
    
} // Task1()

// Task2: PRIORITY=4
static void Task2(void *p_arg)
{
    // Task body (always written as an infinite loop) 
    while (1)
    {
        PORTJ^=(1<<LED_PIN1);
        OSTimeDlyHMSM(0,0,0,500);
        // breakpoint();
    }   
} // Task2()

// Task3: PRIORITY=5
// static void Task3(void *p_arg)
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
// Task4: PRIORITY=6
static void Task_DC_Motor(void *p_arg)
{
    // sei(); // enable interrupts
    // Task body (always written as an infinite loop) 
    while(1)
    {
        PORTB|=(1<<DC_MOTOR_IN1);
        PORTB&=~(1<<DC_MOTOR_IN2);
        OSTimeDlyHMSM(0,0,5,0);

        PORTB|=(1<<DC_MOTOR_IN2);
        PORTB&=~(1<<DC_MOTOR_IN1);
        OSTimeDlyHMSM(0,0,5,0);
    }   
   
} // Task4()
// Task4: PRIORITY=6
// static void IR_Receiver(void *p_arg)
// {
//     unsigned int uart_data;
//     INT8U err;
//     // IR_Startup_Sequence_9ms = 0;
//     // previous_tick_count = 0;
//     // current_tick_count = 0;
//     // alive_counter = 0;
//     while(1)
//     {

//         //ir_result(1);
  
//         OSTimeDlyHMSM(0,0,2,0);
//     }

// } 
// /**Function to Get the IR Command */
// void IR_Retrive_RepeatCode(void)
// {
//     if(IR_RepeatCode_Sequence == 0 && IR_Command_Sequence == 1 && IR_Address_Sequence == 1 && IR_Startup_Sequence == 1 && IR_Start_Frame_Init == 1)
//     {
//         current_tick_count = OSTimeGet();
//         if(((current_tick_count - previous_tick_count) >= 121))
//         {

//             IR_Start_Frame_Init = 0;
//             breakpoint();
//             ir_raw_data_0 = 0;
//         }

//     }
// }

/**Function to Get the IR Command */
void IR_Retrive_CommandData(void)
{
    if(IR_Command_Sequence == 0 && IR_Address_Sequence == 1 && IR_Startup_Sequence == 1 && IR_Start_Frame_Init == 1)
    {
        current_tick_count = OSTimeGet();
        if(((current_tick_count - previous_tick_count) >= (42 + IR_CommandData_Time_Counter))&& ((current_tick_count - previous_tick_count) <= 68))
        {
            if (IR_TSOP_SENSOR_PIN_STATUS == 0) {
                ir_raw_data_0 &= ~(1 << (8 - (IR_CommandData_Time_Counter/3))); //Clear bit (7-b)
            } else {
                ir_raw_data_0 |= (1 << (8 - (IR_CommandData_Time_Counter/3))); //Set bit (7-b)
            } 
            
            IR_CommandData_Time_Counter = IR_CommandData_Time_Counter +3;

        }
        else
        {
                /**Do Nothing*/
        }
        if((current_tick_count - previous_tick_count) >=69)
        {
            OSMboxPost(msgbox, ir_raw_data_0);
            IR_Command_Sequence = 1;
            IR_CommandData_Time_Counter = 3;
            previous_tick_count = OSTimeGet();
            IR_Start_Frame_Init = 0;
        }

    }

}

/**Function to Get the IR Command */
void IR_Retrive_AddressData(void)
{
    if(IR_Address_Sequence == 0 && IR_Startup_Sequence == 1 && IR_Start_Frame_Init == 1)
    {
    
        current_tick_count = OSTimeGet();
        if((current_tick_count - previous_tick_count) > 40)
        {
            IR_Address_Sequence = 1;
            IR_Command_Sequence = 0;

        }
    }
}
    

/**Function to Get the IR Startup Frame */
void IR_Retrive_StartFrame(void)
{
    if(IR_Startup_Sequence == 0 && IR_Start_Frame_Init == 1)
    {
        current_tick_count = OSTimeGet();
        if((current_tick_count - previous_tick_count) > 14)
        {
            IR_Startup_Sequence = 1;
            IR_Address_Sequence = 0;

        }
    }

}
/**Function to Get the INIT */
void IR_ISR_Init(void)
{
    if(IR_Start_Frame_Init == 0)
    {
        previous_tick_count = OSTimeGet();
        IR_Start_Frame_Init = 1;
        IR_Startup_Sequence = 0;
    }

}

ISR(INT5_vect)
{
    IR_ISR_Init();
    IR_Retrive_StartFrame();
    IR_Retrive_AddressData();
    IR_Retrive_CommandData();
    // IR_Retrive_RepeatCode();
}


static void UART_Send_IR(int *p_arg)
{
    INT8U err;
    while(1)
    {
        // ir_received_data = (int)OSQPend(&IR_Sensor,10,&perr);
        // OSQPost
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
 
        UDR0 = ir_received_data;
        // ir_received_data = 0;
        breakpoint();
        OSTimeDlyHMSM(0,0,0,1000);
    }
} 

#if 0
// Task4: PRIORITY=6
static void Task4(void *p_arg)
{
    // Task body (always written as an infinite loop) 
    while(1)
    {
        // Toggle LED6
        //PORTE.OUTTGL=0x40;
        // Print CPU USAGE to USARTXX if enabled
        if(show_cpu_usage)
            printf("CPU USAGE=%d%%\n",OSCPUUsage);
        OSTimeDlyHMSM(0,0,1,0);
    }       
} // Task4()

// Task5: PRIORITY=7
static void Task5(void *p_arg)
{
    // Local variables
    INT8U err;
    // Task body (always written as an infinite loop) 
    while(1)
    {
        // Wait on semaphore
        OSSemPend(pushbutton_event,0,&err);
        // Print button pressed to USARTXX
        switch(buttonState)
        {
            case 0x01:  printf("Push-Button 0 press detected. CPU USAGE=%d%%\n",OSCPUUsage);
                        break;
            case 0x02:  printf("Push-Button 1 press detected. Clock Speed=%ld Hz\n",F_CPU);
                        break;
            case 0x04:  printf("Push-Button 2 press detected. OS_VERSION=%d, OS_CRITICAL_METHOD=%d\n",OS_VERSION, OS_CRITICAL_METHOD);
                        break;
            #if PRINT_TO_USART == C0
            case 0x08:  
                        printf("Push-Button 3 press detected. OS_TICKS_PER_SEC=%d, USARTC0_RX_BUFFER_SIZE_BYTES=%d bytes\n",OS_TICKS_PER_SEC, USARTC0_RX_BUFFER_SIZE_BYTES);
                        break;
            #elif PRINT_TO_USART == F0
            case 0x08:  
                        printf("Push-Button 3 press detected. OS_TICKS_PER_SEC=%d, USARTF0_RX_BUFFER_SIZE_BYTES=%d bytes\n",OS_TICKS_PER_SEC, USARTF0_RX_BUFFER_SIZE_BYTES);
                        break;          
            #endif  
            case 0x10:  printf("Push-Button 4 press detected. APP_CFG_N_TASKS=%d\n",APP_CFG_N_TASKS);
                        break;
            case 0x20:  printf("Push-Button 5 press detected. APP_CFG_TASK_START_STK_SIZE=%d bytes\n",APP_CFG_TASK_START_STK_SIZE);
                        break;
            case 0x40:  printf("Push-Button 6 press detected. APP_CFG_TASK_STK_SIZE=%d bytes\n",APP_CFG_TASK_STK_SIZE);
                        break;
            case 0x80:  show_cpu_usage=!show_cpu_usage;
                        if(show_cpu_usage)
                            printf("Push-Button 7 press detected. CPU Usage Display ON\n");
                        else if(!show_cpu_usage)
                            printf("Push-Button 7 press detected. CPU Usage Display OFF\n");
                        break;
        }
    }       
} // Task5()
#endif
/*
*********************************************************************************************************
*                                                FUNCTIONS
*********************************************************************************************************
*/

void TaskStartCreateTasks(void)
{
    // Task1: PRIORITY=3
    OSTaskCreateExt((void (*)(void *)) Task1,
                    (void           *) 0,
                    (OS_STK         *)&TaskStk[0][APP_CFG_TASK_STK_SIZE - 1],
                    (INT8U           ) 3,
                    (INT16U          ) 3,
                    (OS_STK         *)&TaskStk[0][0],
                    (INT32U          ) APP_CFG_TASK_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

    // Task2: PRIORITY=4
    OSTaskCreateExt((void (*)(void *)) Task2,
                    (void           *) 0,
                    (OS_STK         *)&TaskStk[1][APP_CFG_TASK_STK_SIZE - 1],
                    (INT8U           ) 4,
                    (INT16U          ) 4,
                    (OS_STK         *)&TaskStk[1][0],
                    (INT32U          ) APP_CFG_TASK_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

    // Task3: PRIORITY=5
    // OSTaskCreateExt((void (*)(void *)) Task3,
    //                 (void           *) 0,
    //                 (OS_STK         *)&TaskStk[2][APP_CFG_TASK_STK_SIZE - 1],
    //                 (INT8U           ) 5,
    //                 (INT16U          ) 5,
    //                 (OS_STK         *)&TaskStk[2][0],
    //                 (INT32U          ) APP_CFG_TASK_STK_SIZE,
    //                 (void           *) 0,
    //                 (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
    // Task3: PRIORITY=6
    OSTaskCreateExt((void (*)(void *)) Task_DC_Motor,
                    (void           *) 0,
                    (OS_STK         *)&TaskStk[2][APP_CFG_TASK_STK_SIZE - 1],
                    (INT8U           ) 5,
                    (INT16U          ) 5,
                    (OS_STK         *)&TaskStk[2][0],
                    (INT32U          ) APP_CFG_TASK_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
    // OSTaskCreateExt((void (*)(void *)) IR_Receiver,
    //                 (void           *) 0,
    //                 (OS_STK         *)&TaskStk[3][APP_CFG_TASK_STK_SIZE - 1],
    //                 (INT8U           ) 6,
    //                 (INT16U          ) 6,
    //                 (OS_STK         *)&TaskStk[3][0],
    //                 (INT32U          ) APP_CFG_TASK_STK_SIZE,
    //                 (void           *) 0,
    //                 (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
    OSTaskCreateExt((void (*)(void *))UART_Send_IR,
                (void           *) 0,
                (OS_STK         *)&TaskStk[4][APP_CFG_TASK_STK_SIZE - 1],
                (INT8U           ) 6,
                (INT16U          ) 6,
                (OS_STK         *)&TaskStk[4][0],
                (INT32U          ) APP_CFG_TASK_STK_SIZE,
                (void           *) 0,
                (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
    // OSTaskCreateExt((void (*)(void *)) UART_Send,
    //                 (void           *) 0,
    //                 (OS_STK         *)&TaskStk[2][APP_CFG_TASK_STK_SIZE - 1],
    //                 (INT8U           ) 3,
    //                 (INT16U          ) 3,
    //                 (OS_STK         *)&TaskStk[2][0],
    //                 (INT32U          ) APP_CFG_TASK_STK_SIZE,
    //                 (void           *) 0,
    //                 (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
#if 0
    // Task4: PRIORITY=6
    OSTaskCreateExt((void (*)(void *)) Task4,
                    (void           *) 0,
                    (OS_STK         *)&TaskStk[3][APP_CFG_TASK_STK_SIZE - 1],
                    (INT8U           ) 6,
                    (INT16U          ) 6,
                    (OS_STK         *)&TaskStk[3][0],
                    (INT32U          ) APP_CFG_TASK_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
    
    // Task5: PRIORITY=7
    OSTaskCreateExt((void (*)(void *)) Task5,
                    (void           *) 0,
                    (OS_STK         *)&TaskStk[4][APP_CFG_TASK_STK_SIZE - 1],
                    (INT8U           ) 7,
                    (INT16U          ) 7,
                    (OS_STK         *)&TaskStk[4][0],
                    (INT32U          ) APP_CFG_TASK_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
#endif
} // TaskStartCreateTasks()
/*
* Description : Function to send the command to LCD. As it is 4bit mode, a byte of data is sent in two 4-bit nibbles .
*               
* Arguments   : none
*********************************************************************************************************
*/
void Lcd_CmdWrite(unsigned char cmd)
{
    LCD_DATA = (LCD_DATA & 0x0f) |(0xF0 & cmd);      //Send higher nibble
    LCD_CONTROL &= ~(1<<LCD_RS);   // Send LOW pulse on RS pin for selecting Command register
//  LCD_RW = 0;   // Send LOW pulse on RW pin for Write operation
    LCD_CONTROL |= 1<<LCD_EN;   // Generate a High-to-low pulse on EN pin
    OSTimeDlyHMSM(0,0,0,2);
    LCD_CONTROL &= ~(1<<LCD_EN);
    LCD_DATA = (LCD_DATA & 0x0f) | (cmd<<4); //Send Lower nibble
    LCD_CONTROL &= ~(1<<LCD_RS);   // Send LOW pulse on RS pin for selecting Command register
//  LCD_RW = 0;   // Send LOW pulse on RW pin for Write operation
    LCD_CONTROL |= 1<<LCD_EN;   // Generate a High-to-low pulse on EN pin
    OSTimeDlyHMSM(0,0,0,2);
    LCD_CONTROL &= ~(1<<LCD_EN); 
    OSTimeDlyHMSM(0,0,0,00152);
}
/*
* Description : Function to send the command to LCD. As it is 4bit mode, a byte of data is sent in two 4-bit nibbles .
*               
* Arguments   : none
*********************************************************************************************************
*/
void Lcd_DataWrite(unsigned char dat)
{
    LCD_DATA = (LCD_DATA & 0x0f) | (0xF0 & dat);      //Send higher nibble
    LCD_CONTROL |= 1<<LCD_RS;   // Send HIGH pulse on RS pin for selecting data register
//  LCD_RW = 0;   // Send LOW pulse on RW pin for Write operation
    LCD_CONTROL |= 1<<LCD_EN;   // Generate a High-to-low pulse on EN pin
    OSTimeDlyHMSM(0,0,0,2);
    LCD_CONTROL &= ~(1<<LCD_EN);
    LCD_DATA = (LCD_DATA & 0x0f) | (dat<<4);  //Send Lower nibble
    LCD_CONTROL |= 1<<LCD_RS;    // Send HIGH pulse on RS pin for selecting data register
//  LCD_RW = 0;    // Send LOW pulse on RW pin for Write operation
    LCD_CONTROL |= 1<<LCD_EN;    // Generate a High-to-low pulse on EN pin
    OSTimeDlyHMSM(0,0,0,2);
    LCD_CONTROL &= ~(1<<LCD_EN); 
    OSTimeDlyHMSM(0,0,0,.00152);
}
// AVR initialization

#if 0
void AVRInit(void)
{
    // Local variables
    unsigned char n;
    // Interrupt system initialization
    // Make sure the interrupts are disabled
    asm("cli");
    // Low level interrupt: On
    // Round-robin scheduling for low level interrupt: Off
    // Medium level interrupt: On
    // High level interrupt: On
    // The interrupt vectors will be placed at the start of the Application FLASH section
    n=(PMIC.CTRL & (~(PMIC_RREN_bm | PMIC_IVSEL_bm | PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm))) |
        PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
    CCP=CCP_IOREG_gc;
    PMIC.CTRL=n;
    // Set the default priority for round-robin scheduling
    PMIC.INTPRI=0x00;
    // Setup XMEGA peripherals
    SystemClocksInit();
   // PortsInit();
    //#if PRINT_TO_USART == C0
    //USARTC0Init();
    //#elif PRINT_TO_USART == F0
    //USARTF0Init();
    //#endif
    //PushButtonTimerInit();
} // AVRInit()
#endif
#if 0
void PortsInit(void)
{
    // PORTA initialization
    // PORTB initialization
    // PORTC initialization
        // OUT register
        PORTC.OUT=0x08;
        // Bit0: Input
        // Bit1: Input
        // Bit2: Input
        // Bit3: Output
        // Bit4: Input
        // Bit5: Input
        // Bit6: Input
        // Bit7: Input
        PORTC.DIR=0x08;
        // Bit0 Output/Pull configuration: Totempole/No
        // Bit0 Input/Sense configuration: Sense both edges
        // Bit0 inverted: Off
        // Bit0 slew rate limitation: Off
        PORTC.PIN0CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Bit1 Output/Pull configuration: Totempole/No
        // Bit1 Input/Sense configuration: Sense both edges
        // Bit1 inverted: Off
        // Bit1 slew rate limitation: Off
        PORTC.PIN1CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Bit2 Output/Pull configuration: Totempole/No
        // Bit2 Input/Sense configuration: Sense both edges
        // Bit2 inverted: Off
        // Bit2 slew rate limitation: Off
        PORTC.PIN2CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Bit3 Output/Pull configuration: Totempole/No
        // Bit3 Input/Sense configuration: Sense both edges
        // Bit3 inverted: Off
        // Bit3 slew rate limitation: Off
        PORTC.PIN3CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Bit4 Output/Pull configuration: Totempole/No
        // Bit4 Input/Sense configuration: Sense both edges
        // Bit4 inverted: Off
        // Bit4 slew rate limitation: Off
        PORTC.PIN4CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Bit5 Output/Pull configuration: Totempole/No
        // Bit5 Input/Sense configuration: Sense both edges
        // Bit5 inverted: Off
        // Bit5 slew rate limitation: Off
        PORTC.PIN5CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Bit6 Output/Pull configuration: Totempole/No
        // Bit6 Input/Sense configuration: Sense both edges
        // Bit6 inverted: Off
        // Bit6 slew rate limitation: Off
        PORTC.PIN6CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Bit7 Output/Pull configuration: Totempole/No
        // Bit7 Input/Sense configuration: Sense both edges
        // Bit7 inverted: Off
        // Bit7 slew rate limitation: Off
        PORTC.PIN7CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Interrupt 0 level: Disabled
        // Interrupt 1 level: Disabled
        PORTC.INTCTRL=(PORTC.INTCTRL & (~(PORT_INT1LVL_gm | PORT_INT0LVL_gm))) |
            PORT_INT1LVL_OFF_gc | PORT_INT0LVL_OFF_gc;
        // Bit0 pin change interrupt 0: Off
        // Bit1 pin change interrupt 0: Off
        // Bit2 pin change interrupt 0: Off
        // Bit3 pin change interrupt 0: Off
        // Bit4 pin change interrupt 0: Off
        // Bit5 pin change interrupt 0: Off
        // Bit6 pin change interrupt 0: Off
        // Bit7 pin change interrupt 0: Off
        PORTC.INT0MASK=0x00;
        // Bit0 pin change interrupt 1: Off
        // Bit1 pin change interrupt 1: Off
        // Bit2 pin change interrupt 1: Off
        // Bit3 pin change interrupt 1: Off
        // Bit4 pin change interrupt 1: Off
        // Bit5 pin change interrupt 1: Off
        // Bit6 pin change interrupt 1: Off
        // Bit7 pin change interrupt 1: Off
        PORTC.INT1MASK=0x00;
    // PORTD initialization
        // OUT register
        PORTD.OUT=0x00;
        // Bit0: Input
        // Bit1: Input
        // Bit2: Input
        // Bit3: Input
        // Bit4: Input
        // Bit5: Input
        // Bit6: Input
        // Bit7: Input
        PORTD.DIR=0x00;
        // Bit0 Output/Pull configuration: Totempole/Pull-up (on input)
        // Bit0 Input/Sense configuration: Sense falling edge
        // Bit0 inverted: On
        // Bit0 slew rate limitation: Off
        PORTD.PIN0CTRL=PORT_INVEN_bm | PORT_OPC_PULLUP_gc | PORT_ISC_FALLING_gc;
        // Bit1 Output/Pull configuration: Totempole/Pull-up (on input)
        // Bit1 Input/Sense configuration: Sense falling edge
        // Bit1 inverted: On
        // Bit1 slew rate limitation: Off
        PORTD.PIN1CTRL=PORT_INVEN_bm | PORT_OPC_PULLUP_gc | PORT_ISC_FALLING_gc;
        // Bit2 Output/Pull configuration: Totempole/Pull-up (on input)
        // Bit2 Input/Sense configuration: Sense falling edge
        // Bit2 inverted: On
        // Bit2 slew rate limitation: Off
        PORTD.PIN2CTRL=PORT_INVEN_bm | PORT_OPC_PULLUP_gc | PORT_ISC_FALLING_gc;
        // Bit3 Output/Pull configuration: Totempole/Pull-up (on input)
        // Bit3 Input/Sense configuration: Sense falling edge
        // Bit3 inverted: On
        // Bit3 slew rate limitation: Off
        PORTD.PIN3CTRL=PORT_INVEN_bm | PORT_OPC_PULLUP_gc | PORT_ISC_FALLING_gc;
        // Bit4 Output/Pull configuration: Totempole/Pull-up (on input)
        // Bit4 Input/Sense configuration: Sense falling edge
        // Bit4 inverted: On
        // Bit4 slew rate limitation: Off
        PORTD.PIN4CTRL=PORT_INVEN_bm | PORT_OPC_PULLUP_gc | PORT_ISC_FALLING_gc;
        // Bit5 Output/Pull configuration: Totempole/Pull-up (on input)
        // Bit5 Input/Sense configuration: Sense falling edge
        // Bit5 inverted: On
        // Bit5 slew rate limitation: Off
        PORTD.PIN5CTRL=PORT_INVEN_bm | PORT_OPC_PULLUP_gc | PORT_ISC_FALLING_gc;
        // Bit6 Output/Pull configuration: Totempole/No
        // Bit6 Input/Sense configuration: Sense both edges
        // Bit6 inverted: Off
        // Bit6 slew rate limitation: Off
        PORTD.PIN6CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Bit7 Output/Pull configuration: Totempole/No
        // Bit7 Input/Sense configuration: Sense both edges
        // Bit7 inverted: Off
        // Bit7 slew rate limitation: Off
        PORTD.PIN7CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Interrupt 0 level: Disabled
        // Interrupt 1 level: Disabled
        PORTD.INTCTRL=(PORTD.INTCTRL & (~(PORT_INT1LVL_gm | PORT_INT0LVL_gm))) |
            PORT_INT1LVL_OFF_gc | PORT_INT0LVL_OFF_gc;
        // Bit0 pin change interrupt 0: Off
        // Bit1 pin change interrupt 0: Off
        // Bit2 pin change interrupt 0: Off
        // Bit3 pin change interrupt 0: Off
        // Bit4 pin change interrupt 0: Off
        // Bit5 pin change interrupt 0: Off
        // Bit6 pin change interrupt 0: Off
        // Bit7 pin change interrupt 0: Off
        PORTD.INT0MASK=0x00;
        // Bit0 pin change interrupt 1: Off
        // Bit1 pin change interrupt 1: Off
        // Bit2 pin change interrupt 1: Off
        // Bit3 pin change interrupt 1: Off
        // Bit4 pin change interrupt 1: Off
        // Bit5 pin change interrupt 1: Off
        // Bit6 pin change interrupt 1: Off
        // Bit7 pin change interrupt 1: Off
        PORTD.INT1MASK=0x00;    
    // PORTE initialization
        // OUT register
        PORTE.OUT=0x00;
        // Bit0: Output
        // Bit1: Output
        // Bit2: Output
        // Bit3: Output
        // Bit4: Output
        // Bit5: Output
        // Bit6: Output
        // Bit7: Output
        PORTE.DIR=0xFF;
        // Bit0 Output/Pull configuration: Totempole/No
        // Bit0 Input/Sense configuration: Sense both edges
        // Bit0 inverted: Off
        // Bit0 slew rate limitation: Off
        PORTE.PIN0CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Bit1 Output/Pull configuration: Totempole/No
        // Bit1 Input/Sense configuration: Sense both edges
        // Bit1 inverted: Off
        // Bit1 slew rate limitation: Off
        PORTE.PIN1CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Bit2 Output/Pull configuration: Totempole/No
        // Bit2 Input/Sense configuration: Sense both edges
        // Bit2 inverted: Off
        // Bit2 slew rate limitation: Off
        PORTE.PIN2CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Bit3 Output/Pull configuration: Totempole/No
        // Bit3 Input/Sense configuration: Sense both edges
        // Bit3 inverted: Off
        // Bit3 slew rate limitation: Off
        PORTE.PIN3CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Bit4 Output/Pull configuration: Totempole/No
        // Bit4 Input/Sense configuration: Sense both edges
        // Bit4 inverted: Off
        // Bit4 slew rate limitation: Off
        PORTE.PIN4CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Bit5 Output/Pull configuration: Totempole/No
        // Bit5 Input/Sense configuration: Sense both edges
        // Bit5 inverted: Off
        // Bit5 slew rate limitation: Off
        PORTE.PIN5CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Bit6 Output/Pull configuration: Totempole/No
        // Bit6 Input/Sense configuration: Sense both edges
        // Bit6 inverted: Off
        // Bit6 slew rate limitation: Off
        PORTE.PIN6CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Bit7 Output/Pull configuration: Totempole/No
        // Bit7 Input/Sense configuration: Sense both edges
        // Bit7 inverted: Off
        // Bit7 slew rate limitation: Off
        PORTE.PIN7CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Interrupt 0 level: Disabled
        // Interrupt 1 level: Disabled
        PORTE.INTCTRL=(PORTE.INTCTRL & (~(PORT_INT1LVL_gm | PORT_INT0LVL_gm))) |
            PORT_INT1LVL_OFF_gc | PORT_INT0LVL_OFF_gc;
        // Bit0 pin change interrupt 0: Off
        // Bit1 pin change interrupt 0: Off
        // Bit2 pin change interrupt 0: Off
        // Bit3 pin change interrupt 0: Off
        // Bit4 pin change interrupt 0: Off
        // Bit5 pin change interrupt 0: Off
        // Bit6 pin change interrupt 0: Off
        // Bit7 pin change interrupt 0: Off
        PORTE.INT0MASK=0x00;
        // Bit0 pin change interrupt 1: Off
        // Bit1 pin change interrupt 1: Off
        // Bit2 pin change interrupt 1: Off
        // Bit3 pin change interrupt 1: Off
        // Bit4 pin change interrupt 1: Off
        // Bit5 pin change interrupt 1: Off
        // Bit6 pin change interrupt 1: Off
        // Bit7 pin change interrupt 1: Off
        PORTE.INT1MASK=0x00;
    // PORTF initialization
        // OUT register
        PORTF.OUT=0x08;
        // Bit0: Input
        // Bit1: Input
        // Bit2: Input
        // Bit3: Output
        // Bit4: Input
        // Bit5: Input
        // Bit6: Input
        // Bit7: Input
        PORTF.DIR=0x08;
        // Bit0 Output/Pull configuration: Totempole/No
        // Bit0 Input/Sense configuration: Sense both edges
        // Bit0 inverted: Off
        // Bit0 slew rate limitation: Off
        PORTF.PIN0CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Bit1 Output/Pull configuration: Totempole/No
        // Bit1 Input/Sense configuration: Sense both edges
        // Bit1 inverted: Off
        // Bit1 slew rate limitation: Off
        PORTF.PIN1CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Bit2 Output/Pull configuration: Totempole/No
        // Bit2 Input/Sense configuration: Sense both edges
        // Bit2 inverted: Off
        // Bit2 slew rate limitation: Off
        PORTF.PIN2CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Bit3 Output/Pull configuration: Totempole/No
        // Bit3 Input/Sense configuration: Sense both edges
        // Bit3 inverted: Off
        // Bit3 slew rate limitation: Off
        PORTF.PIN3CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Bit4 Output/Pull configuration: Totempole/No
        // Bit4 Input/Sense configuration: Sense both edges
        // Bit4 inverted: Off
        // Bit4 slew rate limitation: Off
        PORTF.PIN4CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Bit5 Output/Pull configuration: Totempole/No
        // Bit5 Input/Sense configuration: Sense both edges
        // Bit5 inverted: Off
        // Bit5 slew rate limitation: Off
        PORTF.PIN5CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Bit6 Output/Pull configuration: Totempole/No
        // Bit6 Input/Sense configuration: Sense both edges
        // Bit6 inverted: Off
        // Bit6 slew rate limitation: Off
        PORTF.PIN6CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Bit7 Output/Pull configuration: Totempole/No
        // Bit7 Input/Sense configuration: Sense both edges
        // Bit7 inverted: Off
        // Bit7 slew rate limitation: Off
        PORTF.PIN7CTRL=PORT_OPC_TOTEM_gc | PORT_ISC_BOTHEDGES_gc;
        // Interrupt 0 level: Disabled
        // Interrupt 1 level: Disabled
        PORTF.INTCTRL=(PORTD.INTCTRL & (~(PORT_INT1LVL_gm | PORT_INT0LVL_gm))) |
            PORT_INT1LVL_OFF_gc | PORT_INT0LVL_OFF_gc;
        // Bit0 pin change interrupt 0: Off
        // Bit1 pin change interrupt 0: Off
        // Bit2 pin change interrupt 0: Off
        // Bit3 pin change interrupt 0: Off
        // Bit4 pin change interrupt 0: Off
        // Bit5 pin change interrupt 0: Off
        // Bit6 pin change interrupt 0: Off
        // Bit7 pin change interrupt 0: Off
        PORTF.INT0MASK=0x00;
        // Bit0 pin change interrupt 1: Off
        // Bit1 pin change interrupt 1: Off
        // Bit2 pin change interrupt 1: Off
        // Bit3 pin change interrupt 1: Off
        // Bit4 pin change interrupt 1: Off
        // Bit5 pin change interrupt 1: Off
        // Bit6 pin change interrupt 1: Off
        // Bit7 pin change interrupt 1: Off
        PORTF.INT1MASK=0x00;
    // PORTG initialization
    // PORTH initialization
    // PORTI initialization
    // PORTJ initialization
    // PORTK initialization
    // PORTL initialization
    // PORTM initialization
    // PORTN initialization
    // PORTO initialization
    // PORTP initialization
    // PORTQ initialization
    // PORTR initialization
        // OUT register
        PORTR.OUT=0x00;
        // Bit0: Input
        // Bit1: Input
        PORTR.DIR=0x00;
        // Bit0 Output/Pull configuration: Totempole/Pull-up (on input)
        // Bit0 Input/Sense configuration: Sense falling edge
        // Bit0 inverted: On
        // Bit0 slew rate limitation: Off
        PORTR.PIN0CTRL=PORT_INVEN_bm | PORT_OPC_PULLUP_gc | PORT_ISC_FALLING_gc;
        // Bit1 Output/Pull configuration: Totempole/Pull-up (on input)
        // Bit1 Input/Sense configuration: Sense falling edge
        // Bit1 inverted: On
        // Bit1 slew rate limitation: Off
        PORTR.PIN1CTRL=PORT_INVEN_bm | PORT_OPC_PULLUP_gc | PORT_ISC_FALLING_gc;
        // Interrupt 0 level: Disabled
        // Interrupt 1 level: Disabled
        PORTR.INTCTRL=(PORTR.INTCTRL & (~(PORT_INT1LVL_gm | PORT_INT0LVL_gm))) |
            PORT_INT1LVL_OFF_gc | PORT_INT0LVL_OFF_gc;
        // Bit0 pin change interrupt 0: Off
        // Bit1 pin change interrupt 0: Off
        PORTR.INT0MASK=0x00;
        // Bit0 pin change interrupt 1: Off
        // Bit1 pin change interrupt 1: Off
        PORTR.INT1MASK=0x00;    
} // PortsInit()
#endif
#if 0
// System clocks initialization
void SystemClocksInit(void)
{
    unsigned char n,s;
    // Save interrupts enabled/disabled state
    s=SREG;
    // Disable interrupts
    asm("cli");
    
   /* 
    * XMEGA-A1 XPLAINED DFLL errata workaround.  Uses the on-board 32.768KHz 
    * crystal and TOSC oscillator as the calibration reference for the Digital 
    * Frequency Locked Loops of the RC2M and RC32M oscillators. 
    * 
    * For more information on the workaround see the XMEGA-A1 data sheet 
    * errata (Atmel document 8067M-AVR-09/10, page 94, item 20).  It requires 
    * both the RC2M and RC32M oscillators and DFLLs to be enabled for either 
    * of them to function properly. 
    * 
    * Greg Muth (AVRFreaks: GTKNarwhal) 14-AUG-2012 
    */ 

    // Enable RC2M, RC32M and XOSC/TOSC oscillators and wait for ready 
    OSC.XOSCCTRL = OSC_XOSCSEL_32KHz_gc; 
    OSC.CTRL = OSC_RC2MEN_bm | OSC_RC32MEN_bm | OSC_XOSCEN_bm; 
    while (~OSC.STATUS & (OSC_RC2MRDY_bm | OSC_RC32MRDY_bm | OSC_XOSCRDY_bm));
    
    // Select TOSC as calibration reference for both DFLLs and enable each 
    OSC.DFLLCTRL = OSC_RC32MCREF_bm | OSC_RC2MCREF_bm; 
    DFLLRC32M.CTRL = DFLL_ENABLE_bm; 
    DFLLRC2M.CTRL = DFLL_ENABLE_bm; 
    
    #if F_CPU==2000000UL
        // Select the system clock source: 2 MHz Internal RC Osc. 
        CCP = CCP_IOREG_gc; 
        CLK.CTRL = CLK_SCLKSEL_RC2M_gc;
        
        // System Clock prescaler A division factor: 1
        // System Clock prescalers B & C division factors: B:1, C:1
        // ClkPer4: 2000.000 kHz
        // ClkPer2: 2000.000 kHz
        // ClkPer:  2000.000 kHz
        // ClkCPU:  2000.000 kHz
        n = (CLK.PSCTRL & (~(CLK_PSADIV_gm | CLK_PSBCDIV1_bm | CLK_PSBCDIV0_bm))) |
            CLK_PSADIV_1_gc | CLK_PSBCDIV_1_1_gc;
        CCP = CCP_IOREG_gc;
        CLK.PSCTRL = n;
    #elif F_CPU==32000000UL        
        // Select the system clock source: 32 MHz Internal RC Osc. 
        CCP = CCP_IOREG_gc; 
        CLK.CTRL = CLK_SCLKSEL_RC32M_gc;
        
        // System Clock prescaler A division factor: 1
        // System Clock prescalers B & C division factors: B:1, C:1
        // ClkPer4: 32000.000 kHz
        // ClkPer2: 32000.000 kHz
        // ClkPer:  32000.000 kHz
        // ClkCPU:  32000.000 kHz
        n = (CLK.PSCTRL & (~(CLK_PSADIV_gm | CLK_PSBCDIV1_bm | CLK_PSBCDIV0_bm))) |
            CLK_PSADIV_1_gc | CLK_PSBCDIV_1_1_gc;
        CCP = CCP_IOREG_gc;
        CLK.PSCTRL = n;
    #endif
    
    // Peripheral Clock output: Disabled
    PORTCFG.CLKEVOUT=(PORTCFG.CLKEVOUT & (~PORTCFG_CLKOUT_gm)) | PORTCFG_CLKOUT_OFF_gc;
    // Restore interrupts enabled/disabled state
    SREG=s;
} // SystemClocksInit()
#endif
#if 0
// USART_C0 initialization
void USARTC0Init(void)
{
    // Note: The correct PORTC direction for the RxD, TxD and XCK signals
    // is configured in the PortsInit function

    // Transmitter is enabled
    // Set TxD=1
    PORTC.OUTSET=0x08;

    // Communication mode: Asynchronous USART
    // Data bits: 8
    // Stop bits: 1
    // Parity: Disabled
    USARTC0.CTRLC=USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_CHSIZE_8BIT_gc;

    // Receive complete interrupt: High Level
    // Transmit complete interrupt: Disabled
    // Data register empty interrupt: Disabled
    USARTC0.CTRLA=(USARTC0.CTRLA & (~(USART_RXCINTLVL_gm | USART_TXCINTLVL_gm | USART_DREINTLVL_gm))) |
        USART_RXCINTLVL_HI_gc | USART_TXCINTLVL_OFF_gc | USART_DREINTLVL_OFF_gc;

    #if F_CPU==2000000UL
    // Required Baud rate: 9600
    // Real Baud Rate: 9601.0 (x2 Mode), Error: 0.0 %
    USARTC0.BAUDCTRLA=0x85;
    USARTC0.BAUDCTRLB=((0x09 << USART_BSCALE_bp) & USART_BSCALE_gm) | 0x0C;

    // Receiver: On
    // Transmitter: On
    // Double transmission speed mode: On
    // Multi-processor communication mode: Off
    USARTC0.CTRLB=(USARTC0.CTRLB & (~(USART_RXEN_bm | USART_TXEN_bm | USART_CLK2X_bm | USART_MPCM_bm | USART_TXB8_bm))) |
        USART_RXEN_bm | USART_TXEN_bm | USART_CLK2X_bm;

    #elif F_CPU==32000000UL
    // Required Baud rate: 9600
    // Real Baud Rate: 9601.0 (x1 Mode), Error: 0.0 %
    USARTC0.BAUDCTRLA=0xF5;
    USARTC0.BAUDCTRLB=((0x0C << USART_BSCALE_bp) & USART_BSCALE_gm) | 0x0C;

    // Receiver: On
    // Transmitter: On
    // Double transmission speed mode: Off
    // Multi-processor communication mode: Off
    USARTC0.CTRLB=(USARTC0.CTRLB & (~(USART_RXEN_bm | USART_TXEN_bm | USART_CLK2X_bm | USART_MPCM_bm | USART_TXB8_bm))) |
        USART_RXEN_bm | USART_TXEN_bm;
    #endif
} // USARTC0Init()

#endif
#if 0
// USART_F0 initialization
void USARTF0Init(void)
{
    // Note: the correct PORTD direction for the RxD, TxD and XCK signals
    // is configured in the PortsInit function

    // Transmitter is enabled
    // Set TxD=1
    PORTD.OUTSET=0x08;

    // Communication mode: Asynchronous USART
    // Data bits: 8
    // Stop bits: 1
    // Parity: Disabled
    USARTF0.CTRLC=USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_CHSIZE_8BIT_gc;

    // Receive complete interrupt: High Level
    // Transmit complete interrupt: Disabled
    // Data register empty interrupt: Disabled
    USARTF0.CTRLA=(USARTF0.CTRLA & (~(USART_RXCINTLVL_gm | USART_TXCINTLVL_gm | USART_DREINTLVL_gm))) |
        USART_RXCINTLVL_HI_gc | USART_TXCINTLVL_OFF_gc | USART_DREINTLVL_OFF_gc;

    #if F_CPU==2000000
    // Required Baud rate: 9600
    // Real Baud Rate: 9601.0 (x2 Mode), Error: 0.0 %
    USARTF0.BAUDCTRLA=0x85;
    USARTF0.BAUDCTRLB=((0x09 << USART_BSCALE_bp) & USART_BSCALE_gm) | 0x0C;

    // Receiver: On
    // Transmitter: On
    // Double transmission speed mode: On
    // Multi-processor communication mode: Off
    USARTF0.CTRLB=(USARTF0.CTRLB & (~(USART_RXEN_bm | USART_TXEN_bm | USART_CLK2X_bm | USART_MPCM_bm | USART_TXB8_bm))) |
        USART_RXEN_bm | USART_TXEN_bm | USART_CLK2X_bm;

    #elif F_CPU==32000000
    // Required Baud rate: 9600
    // Real Baud Rate: 9601.0 (x1 Mode), Error: 0.0 %
    USARTF0.BAUDCTRLA=0xF5;
    USARTF0.BAUDCTRLB=((0x0C << USART_BSCALE_bp) & USART_BSCALE_gm) | 0x0C;

    // Receiver: On
    // Transmitter: On
    // Double transmission speed mode: Off
    // Multi-processor communication mode: Off
    USARTF0.CTRLB=(USARTF0.CTRLB & (~(USART_RXEN_bm | USART_TXEN_bm | USART_CLK2X_bm | USART_MPCM_bm | USART_TXB8_bm))) |
        USART_RXEN_bm | USART_TXEN_bm;
    #endif
} // USARTF0Init()

#endif
#if 0
void PushButtonTimerInit(void)
{
    unsigned char s;
    unsigned char n;

    // Note: the correct PORTC direction for the Compare Channels outputs
    // is configured in the PortsInit function

    // Save interrupts enabled/disabled state
    s=SREG;
    // Disable interrupts
    asm("cli");

    // Disable and reset the timer/counter just to be sure
    tc0_disable(&TCC0);

    #if F_CPU==2000000
    // Clock source: Peripheral Clock/1
    TCC0.CTRLA=(TCC0.CTRLA & (~TC0_CLKSEL_gm)) | TC_CLKSEL_DIV1_gc;
    #elif F_CPU==32000000
    // Clock source: Peripheral Clock/64
    TCC0.CTRLA=(TCC0.CTRLA & (~TC0_CLKSEL_gm)) | TC_CLKSEL_DIV64_gc;    
    #endif

    // Mode: Normal Operation, Overflow Int./Event on TOP
    // Compare/Capture on channel A: Off
    // Compare/Capture on channel B: Off
    // Compare/Capture on channel C: Off
    // Compare/Capture on channel D: Off
    TCC0.CTRLB=(TCC0.CTRLB & (~(TC0_CCAEN_bm | TC0_CCBEN_bm | TC0_CCCEN_bm | TC0_CCDEN_bm | TC0_WGMODE_gm))) |
        TC_WGMODE_NORMAL_gc;

    // Capture event source: None
    // Capture event action: None
    TCC0.CTRLD=(TCC0.CTRLD & (~(TC0_EVACT_gm | TC0_EVSEL_gm))) |
        TC_EVACT_OFF_gc | TC_EVSEL_OFF_gc;

    // Overflow interrupt: Low Level
    // Error interrupt: Disabled
    TCC0.INTCTRLA=(TCC0.INTCTRLA & (~(TC0_ERRINTLVL_gm | TC0_OVFINTLVL_gm))) |
        TC_ERRINTLVL_OFF_gc | TC_OVFINTLVL_LO_gc;

    // Compare/Capture channel A interrupt: Disabled
    // Compare/Capture channel B interrupt: Disabled
    // Compare/Capture channel C interrupt: Disabled
    // Compare/Capture channel D interrupt: Disabled
    TCC0.INTCTRLB=(TCC0.INTCTRLB & (~(TC0_CCDINTLVL_gm | TC0_CCCINTLVL_gm | TC0_CCBINTLVL_gm | TC0_CCAINTLVL_gm))) |
        TC_CCDINTLVL_OFF_gc | TC_CCCINTLVL_OFF_gc | TC_CCBINTLVL_OFF_gc | TC_CCAINTLVL_OFF_gc;

    // High resolution extension: Off
    HIRESC.CTRLA&= ~HIRES_HREN0_bm;

    // Advanced Waveform Extension initialization
    // Disable locking the AWEX configuration registers just to be sure
    n=MCU.AWEXLOCK & (~MCU_AWEXCLOCK_bm);
    CCP=CCP_IOREG_gc;
    MCU.AWEXLOCK=n;

    // Pattern generation: Off
    // Dead time insertion: Off
    AWEXC.CTRL&= ~(AWEX_PGM_bm | AWEX_CWCM_bm | AWEX_DTICCDEN_bm | AWEX_DTICCCEN_bm | AWEX_DTICCBEN_bm | AWEX_DTICCAEN_bm);

    // Fault protection initialization
    // Fault detection on OCD Break detection: On
    // Fault detection restart mode: Latched Mode
    // Fault detection action: None (Fault protection disabled)
    AWEXC.FDCTRL=(AWEXC.FDCTRL & (~(AWEX_FDDBD_bm | AWEX_FDMODE_bm | AWEX_FDACT_gm))) |
        AWEX_FDACT_NONE_gc;
    // Fault detect events: 
    // Event channel 0: Off
    // Event channel 1: Off
    // Event channel 2: Off
    // Event channel 3: Off
    // Event channel 4: Off
    // Event channel 5: Off
    // Event channel 6: Off
    // Event channel 7: Off
    AWEXC.FDEMASK=0b00000000;
    // Make sure the fault detect flag is cleared
    AWEXC.STATUS|=AWEXC.STATUS & AWEX_FDF_bm;

    // Clear the interrupt flags
    TCC0.INTFLAGS=TCC0.INTFLAGS;

    #if F_CPU==2000000
    // Set counter register
    TCC0.CNT=0x0000;
    // Set period register
    TCC0.PER=0x9C3F;
    // Set channel A Compare/Capture register
    TCC0.CCA=0x0000;
    // Set channel B Compare/Capture register
    TCC0.CCB=0x0000;
    // Set channel C Compare/Capture register
    TCC0.CCC=0x0000;
    // Set channel D Compare/Capture register
    TCC0.CCD=0x0000;
    #elif F_CPU==32000000
    // Set counter register
    TCC0.CNT=0x0000;
    // Set period register
    TCC0.PER=0x270F;
    // Set channel A Compare/Capture register
    TCC0.CCA=0x0000;
    // Set channel B Compare/Capture register
    TCC0.CCB=0x0000;
    // Set channel C Compare/Capture register
    TCC0.CCC=0x0000;
    // Set channel D Compare/Capture register
    TCC0.CCD=0x0000;
    #endif

    // Restore interrupts enabled/disabled state
    SREG=s;
} // PushButtonTimerInit()
#endif
#if 0

// Disable a Timer/Counter type 0
void tc0_disable(TC0_t *ptc)
{
    // Timer/Counter off
    ptc->CTRLA=(ptc->CTRLA & (~TC0_CLKSEL_gm)) | TC_CLKSEL_OFF_gc;
    // Issue a reset command
    ptc->CTRLFSET=TC_CMD_RESET_gc;
} // tc0_disable()

// Disable a Timer/Counter type 1
void tc1_disable(TC1_t *ptc)
{
    // Timer/Counter off
    ptc->CTRLA=(ptc->CTRLA & (~TC1_CLKSEL_gm)) | TC_CLKSEL_OFF_gc;
    // Issue a reset command
    ptc->CTRLFSET=TC_CMD_RESET_gc;
} // tc1_disable()
#endif
#if 0
#if PRINT_TO_USART == C0
// Receive a character from USARTC0
char getchar_usartc0(void)
{
    // Local variables
    char data;
    #if OS_CRITICAL_METHOD==3
        OS_CPU_SR cpu_sr;
    #endif
    
    while (rx_counter_usartc0==0);
        data=rx_buffer_usartc0[rx_rd_index_usartc0++];
    
    if (rx_rd_index_usartc0 == USARTC0_RX_BUFFER_SIZE_BYTES)
            rx_rd_index_usartc0=0;

    OS_ENTER_CRITICAL();
    --rx_counter_usartc0;
    OS_EXIT_CRITICAL();
    return data;
} // getchar_usartc0()

int usartc0_putchar(char c, FILE *stream)
{
    if (c == '\n')
        usartc0_putchar('\r', stream);
    // Wait for the transmit buffer to be empty
    while (!( USARTC0.STATUS & USART_DREIF_bm));
    // Put our character into the transmit buffer
    USARTC0.DATA = c;
    return 0;
} // usartc0_putchar()

#elif PRINT_TO_USART == F0
// Receive a character from USARTF0
char getchar_usartf0(void)
{
    // Local variables
    char data;
    #if OS_CRITICAL_METHOD==3
        OS_CPU_SR cpu_sr;
    #endif
    
    while (rx_counter_usartf0==0);
        data=rx_buffer_usartf0[rx_rd_index_usartf0++];
    
    if (rx_rd_index_usartf0 == USARTF0_RX_BUFFER_SIZE_BYTES)
            rx_rd_index_usartf0=0;

    OS_ENTER_CRITICAL();
    --rx_counter_usartf0;
    OS_EXIT_CRITICAL();
    return data;
} // getchar_usartf0()

int usartf0_putchar(char c, FILE *stream)
{
    if (c == '\n')
        usartf0_putchar('\r', stream);
    // Wait for the transmit buffer to be empty
    while (!( USARTF0.STATUS & USART_DREIF_bm));
    // Put our character into the transmit buffer
    USARTF0.DATA = c;
    return 0;
} // usartf0_putchar()
#endif
#endif
/*
*********************************************************************************************************
*                                        SETUP THE TICK RATE
*
* Note(s): 1) PER = CPU_CLK_FREQ / (2 * Prescaler * OC_freq) - 1
*          2) The equation actually performs rounding by multiplying by 2, adding 1 and then dividing by 2
*             in integer math, this is equivalent to adding 0.5
*********************************************************************************************************
*/
#if 0
void ClockTickStart(void)
{
    // Local variables
    unsigned char s;
    uint32_t  clk_per_freq;
    uint32_t  period;
    // Save interrupts enabled/disabled state
    s=SREG;
    // Disable interrupts
    asm("cli");
    // Disable and reset the timer/counter just to be sure
    tc1_disable(&TCC1);
    // Clock source: Peripheral Clock/1
    TCC1.CTRLA=(TCC1.CTRLA & (~TC1_CLKSEL_gm)) | TC_CLKSEL_DIV1_gc;
    // Mode: Normal Operation, Overflow Int./Event on TOP
    // Compare/Capture on channel A: Off
    // Compare/Capture on channel B: Off
    TCC1.CTRLB=(TCC1.CTRLB & (~(TC1_CCAEN_bm | TC1_CCBEN_bm | TC1_WGMODE_gm))) |
        TC_WGMODE_NORMAL_gc;
    // Capture event source: None
    // Capture event action: None
    TCC1.CTRLD=(TCC1.CTRLD & (~(TC1_EVACT_gm | TC1_EVSEL_gm))) |
        TC_EVACT_OFF_gc | TC_EVSEL_OFF_gc;
    // Overflow interrupt: High Level
    // Error interrupt: Disabled
    TCC1.INTCTRLA=(TCC1.INTCTRLA & (~(TC1_ERRINTLVL_gm | TC1_OVFINTLVL_gm))) |
        TC_ERRINTLVL_OFF_gc | TC_OVFINTLVL_HI_gc;
    // Compare/Capture channel A interrupt: Disabled
    // Compare/Capture channel B interrupt: Disabled
    TCC1.INTCTRLB=(TCC1.INTCTRLB & (~(TC1_CCBINTLVL_gm | TC1_CCAINTLVL_gm))) |
        TC_CCBINTLVL_OFF_gc | TC_CCAINTLVL_OFF_gc;
    // High resolution extension: Off
    HIRESC.CTRLA&= ~HIRES_HREN1_bm;
    // Clear the interrupt flags
    TCC1.INTFLAGS=TCC1.INTFLAGS;
    // Set counter register
    TCC1.CNT=0x0000;
    // Calculate period
    // Note: The following equation to calculate the period assumes prescaler=CLK/1
    clk_per_freq   =  F_CPU;
    period         =  (uint32_t)(((2 * clk_per_freq) + (1 * 2 * (uint32_t)OS_TICKS_PER_SEC))
                   /             ((1 * 2 * (uint32_t)OS_TICKS_PER_SEC)));
    // Set timer period register (maximum value=(2^16)-1=65535)
    TCC1.PER=period;
    // Set channel A Compare/Capture register
    TCC1.CCA=0x0000;
    // Set channel B Compare/Capture register
    TCC1.CCB=0x0000;
    // Restore interrupts enabled/disabled state
    SREG=s;
} // ClockTickStart()
#endif
/*
*********************************************************************************************************
*                                              ISR HANDLERS
*
*
* Note(s): 1) Below are the ISR handlers written in C - these handlers are initially called by
*             handlers written in assembly located in isr.s. It is those handlers which perform the
*             register saving and restoring.
*          2) See below for instructions on how to create new interrupts.
*
* Creating a new interrupt (for example, named TestISR):
*          1) Add the new interrupt (TestISR) to the interrupt vector table in app_vect.s at the
*             appropriate interrupt source location (address).
*          2) Create an external reference to TestISR in app_vect.s: .extern   TestISR
*          3) Add the assembly ISR handler to isr.s - use an existing ISR as a template.
*             The following changes need to be made:
*               -   Within the section of the ISR handler which saves the registers, change the BRNE 
*                   instruction to branch to TestISR_1. Also change the label of this section to
*                   TestISR.
*               -   Within the section of the ISR handler which calls the C-based ISR handler and
*                   restores the registers, change the CALL instruction to call the appropriate C-based
*                   handler which will be added below in Step 4. Also change the label of this section to
*                   TestISR_1.
*               -   Create the following references near the beginning of the file (isr.s):
*                           .global  TestISR
*                           .extern  TestISRHandler
*          4) Add the ISR handler in C below (xmega_ucos-ii.c).
*          5) Ensure the appropriate XMEGA registers are configured for the interrupt source.
*        
*********************************************************************************************************
*/

// ISR(USART0_UDRE_vect) {
//   UCSR0B &= ~(1<<UDRIE0);
//     UDR0 = 'p';
//     UCSR0B |= (1<<UDRIE0);
//     asm("cli");
// }
#if 0
void usartc0_rx_isr_handler(void)
{
    unsigned char status;
    char data;
    status=USARTC0.STATUS;
    data=USARTC0.DATA;
    if ((status & (USART_FERR_bm | USART_PERR_bm | USART_BUFOVF_bm)) == 0)
    {
        rx_buffer_usartc0[rx_wr_index_usartc0++]=data;
        if (rx_wr_index_usartc0 == USARTC0_RX_BUFFER_SIZE_BYTES)
            rx_wr_index_usartc0=0;
        if (++rx_counter_usartc0 == USARTC0_RX_BUFFER_SIZE_BYTES)
        {
            rx_counter_usartc0=0;
            rx_buffer_overflow_usartc0=true;
        }
    }
    // Post semaphore
    OSSemPost(usartc0_rx);
} // usartc0_rx_isr_handler()

void usartf0_rx_isr_handler(void)
{
    unsigned char status;
    char data;
    status=USARTF0.STATUS;
    data=USARTF0.DATA;
    if ((status & (USART_FERR_bm | USART_PERR_bm | USART_BUFOVF_bm)) == 0)
    {
        rx_buffer_usartf0[rx_wr_index_usartf0++]=data;
        if (rx_wr_index_usartf0 == USARTF0_RX_BUFFER_SIZE_BYTES)
            rx_wr_index_usartf0=0;
        if (++rx_counter_usartf0 == USARTF0_RX_BUFFER_SIZE_BYTES)
        {
            rx_counter_usartf0=0;
            rx_buffer_overflow_usartf0=true;
        }
    }
    // Post semaphore
    OSSemPost(usartf0_rx);
} // usartf0_rx_isr_handler()

void pushbutton_timer_isr_handler(void)
{
    // Note: This ISR is called every 20ms

    // Local variables
    static INT8U oldState;
    static INT8U newState;
    static INT8U count;
    static bool edge=false;
    // Read state of push-buttons: SW0->{PORTD(PD0:PD5),PORTR(PR0:PR1)}<-SW7
    newState=(PORTD.IN&0x3F)|((PORTR.IN<<6)&0xC0);
    // Toggle LED7
    PORTE.OUTTGL=0x80;  
    // Increment counter if state remains the same
    if (newState==oldState)
        count++;
    // Else, reset count and record edge
    else
    {
        count=0;
        edge=true;      
    }                      
    // Save current state as old state
    oldState = newState;
    // If state of push-buttons stays constant for more than 80ms, button press event has occured.
    // Note: The edge flag ensures that holding down the button doesn't produce multiple events.
    if ((count>4)&&(edge))
    {
        // Save button state
        buttonState = newState;
        // Reset count variable and edge flag
        count=0;
        edge=false; 
        // Post semaphore
        OSSemPost(pushbutton_event);
    }
} // pushbutton_timer_isr_handler()
#endif
