/*
*********************************************************************************************************
*                                           Atmel ATmega256
*                                         Board Support Package
*
*                                (c) Copyright 2005, Micrium, Weston, FL
*                                          All Rights Reserved
*
*
* File : BSP.C
* By   : Pawan Singh Rana
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*                                               INCLUDES
*********************************************************************************************************
*/

#include "includes.h"

/*
*********************************************************************************************************
*                                               PROTOTYPES
*********************************************************************************************************
*/

static  void    BSP_TickInit(void);
extern  void    Fx_LED_Init(void);
static  void    LCD_Init(void);
static  void    PB_Init(void);
extern void     UART_Init(void);
static  void    Fx_DC_Motor_Init(void);
extern  void    IR_Sensor_ISR_Config(void);


/*
*********************************************************************************************************
*                                         BSP INITIALIZATION
*
* Description : This function should be called by your application code before you make use of any of the
*               functions found in this module.
*               
* Arguments   : none
*********************************************************************************************************
*/

void  BSP_Init (void)
{
     Fx_LED_Init();
     Fx_DC_Motor_Init();
     LCD_Init();
     PB_Init();
     UART_Init();
     BSP_TickInit();
     IR_Sensor_ISR_Config();
}

/*
*********************************************************************************************************
*                                       TICKER INITIALIZATION
*
* Description : This function is called to initialize uC/OS-II's tick source (typically a timer generating
*               interrupts every 1 to 100 mS).
*
* Arguments   : none
*********************************************************************************************************
*/

static  void  BSP_TickInit (void)
{
    TCCR0A        = 0x02;                               /* Set TIMER0 prescaler to CTC Mode, CLK/1024  */
    TCCR0B        = 0x05;                               /* Set CLK/1024 Prescale                       */
    TCNT0         =    0;                               /* Start TCNT at 0 for a new cycle             */
    OCR0A         = CPU_CLK_FREQ / OS_TICKS_PER_SEC / 1024 - 1;
    TIFR0        |= 0x02;                               /* Clear  TIMER0 compare Interrupt Flag        */
    TIMSK0       |= 0x02;                               /* Enable TIMER0 compare Interrupt             */
}

/*
*********************************************************************************************************
*                                        TIMER ISR
*
* Description :  This ISR is triggered every time when timer compare overflow interrupt generates and shall 
                 be used for OS Tick.
*               
* Arguments   : none
*********************************************************************************************************
*/

ISR(TIMER0_COMPA_vect)         
{
	BSP_TickISR_Handler();
}

/*
*********************************************************************************************************
*                                         TIMER #0 IRQ HANDLER
*
* Description : This function handles the timer interrupt that is used to generate TICKs for uC/OS-II.
*
* Arguments   : none
*
* Note(s)     : 1) There is no need to clear the interrupt since this is done automatically in Output
*                  Compare mode when the ISR is executed.
*********************************************************************************************************
*/

void  BSP_TickISR_Handler (void)
{
    OSTimeTick();                                           /* Call uC/OS-II's OSTimeTick()                          */
}

/*
* Description : This function should be called by your application code before you make use of any of the
*               functions found in this module. This function shall initialize the LCD 1602 Module.
*               
* Arguments   : none
*********************************************************************************************************
*/
static  void  LCD_Init (void)
{
    DDRF  |= (1<<PF4)|(1<<PF5)|(1<<PF6)|(1<<PF7);                                 /* All PORTD pins are outputs   */
    DDRH  |= (1<<PH0)|(1<<PH1);                      
  

}

/*
*********************************************************************************************************
*                                             LED ON
*
* Description : This function is used to control any or all the LEDs on the board.
*               
* Arguments   : led    is the number of the LED to control
*                      0    indicates that you want ALL the LEDs to be ON
*                      1    turns ON LED1 on the board
*                      .
*                      .
*                      8    turns ON LED8 on the board
*********************************************************************************************************
*/

void  LED_On (INT8U led)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif


    OS_ENTER_CRITICAL();
    switch (led) {
        case 0:
             PORTD  =  0x00;
             break;

        case 1:
             PORTD &= ~0x01;
             break;

        case 2:
             PORTD &= ~0x02;
             break;

        case 3:
             PORTD &= ~0x04;
             break;

        case 4:
             PORTD &= ~0x08;
             break;

        case 5:
             PORTD &= ~0x10;
             break;

        case 6:
             PORTD &= ~0x20;
             break;

        case 7:
             PORTD &= ~0x40;
             break;

        case 8:
             PORTD &= ~0x80;
             break;
    }
    OS_EXIT_CRITICAL();
}

/*
*********************************************************************************************************
*                                             LED OFF
*
* Description : This function is used to control any or all the LEDs on the board.
*               
* Arguments   : led    is the number of the LED to turn OFF
*                      0    indicates that you want ALL the LEDs to be OFF
*                      1    turns OFF LED1 on the board
*                      .
*                      .
*                      8    turns OFF LED8 on the board
*********************************************************************************************************
*/

void  LED_Off (INT8U led)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif


    OS_ENTER_CRITICAL();
    switch (led) {
        case 0:
             PORTD  =  0xFF;
             break;

        case 1:
             PORTD |=  0x01;
             break;

        case 2:
             PORTD |=  0x02;
             break;

        case 3:
             PORTD |=  0x04;
             break;

        case 4:
             PORTD |=  0x08;
             break;

        case 5:
             PORTD |=  0x10;
             break;

        case 6:
             PORTD |=  0x20;
             break;

        case 7:
             PORTD |=  0x40;
             break;

        case 8:
             PORTD |=  0x80;
             break;
    }
    OS_EXIT_CRITICAL();
}

/*
*********************************************************************************************************
*                                        LED TOGGLE
*
* Description : This function is used to toggle the state of any or all the LEDs on the board.
*               
* Arguments   : led    is the number of the LED to toggle
*                      0    indicates that you want ALL the LEDs to toggle
*                      1    Toggle LED1 on the board
*                      .
*                      .
*                      8    Toggle LED8 on the board
*********************************************************************************************************
*/

void  LED_Toggle (INT8U led)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif


    OS_ENTER_CRITICAL();
    switch (led) {
        case 0:
             PORTD ^=  0xFF;
             break;

        case 1:
             PORTD ^=  0x01;
             break;

        case 2:
             PORTD ^=  0x02;
             break;

        case 3:
             PORTD ^=  0x04;
             break;

        case 4:
             PORTD ^=  0x08;
             break;

        case 5:
             PORTD ^=  0x10;
             break;

        case 6:
             PORTD ^=  0x20;
             break;

        case 7:
             PORTD ^=  0x40;
             break;

        case 8:
             PORTD ^=  0x80;
             break;
    }
    OS_EXIT_CRITICAL();
}

/*
*********************************************************************************************************
*                                        PUSH-BUTTON INITIALIZATION
*
* Description : This function should be called by your application code before you make use of any of the
*               functions found in this module.
*               
* Arguments   : none
*********************************************************************************************************
*/

static  void  PB_Init (void)
{
    DDRA  = 0x00;                                 /* All PORTA pins are inputs                         */
}

/*
*********************************************************************************************************
*                                         GET 'PUSH BUTTON' STATUS
*
* Description : This function is used to get the status of any push button on the board.
*
* Arguments   : push_button    is the number of the push button to probe
*                              1    probe the push button Sw0
*                              2    probe the push button Sw1
*                              3    probe the push button Sw2
*                              4    probe the push button Sw3
*                              5    probe the push button Sw4
*                              6    probe the push button Sw5
*                              7    probe the push button Sw6
*                              8    probe the push button Sw7
*
* Returns     : TRUE           if the button is pressed
*               FALSE          if the button is not pressed
*********************************************************************************************************
*/

BOOLEAN  PB_GetStatus (INT8U pb_id)
{
    INT8U  sw;
    INT8U  mask;


    sw   = ~PINA;
    mask = 1 << (pb_id - 1);
    if (sw & mask) {
        return (TRUE);
    } else {
        return (FALSE);
    }
}

