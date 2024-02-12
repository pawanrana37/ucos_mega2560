/*
 * https://github.com/pawanrana37/ucos_mega2560
 *
 * Copyright (c) [2024] [Pawan Singh Rana]
 * All rights reserved.
 *
 * This software is licensed under the [License Name].
 * See the LICENSE file in the project root for details.
 *
 * 
 * File timer.c
 */
 
/*
*********************************************************************************************************
*                                               INCLUDES
*********************************************************************************************************
*/
#include "Gpt.h"

/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/
/*8-Bit*/
# define TIMER0_OCR0B_VALUE (0x0000)
# define TIMER0_OCR0A_VALUE (0x00F9)

/*16-Bit*/
# define TIMER3_OCR3AH_VALUE (0x0000)
# define TIMER3_OCR3AL_VALUE (0x00F9)
/*
*********************************************************************************************************
*                                              GLOBAL VARIABLES
*********************************************************************************************************
*/
volatile unsigned int milliseconds ; 

/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static  void    Timer0_Init_1ms(void);
static  void    Timer3_Init_1ms(void);

/*
*********************************************************************************************************
*                                       TIMER INITIALIZATION
*
* Description : This function is called to initialize the timer module for 8 or 16 bit.
*
* Arguments   : none
*********************************************************************************************************
*/
void Gpt_Init(void)
{
     Timer0_Init_1ms();
     Timer3_Init_1ms();
}
/*
*********************************************************************************************************
*                                       TIMER0 INITIALIZATION
*
* Description : This function is called to initialize uC/OS-II's tick source (typically a timer generating
*               interrupts every 1 to 100 mS).
*
* Arguments   : none
*********************************************************************************************************
*/
static  void  Timer0_Init_1ms (void)
{
     /*Disable Interrupts*/
     cli();

     /*Update OCR H and L based on Formula Count = ((F_CLK / Prescaler / OS_TICKS_PER_SEC) -1 )*/
     /* to achieve 1ms timer interrupt : F_CLK = 16000000 , Prescaler = 64 , OS_TICKS_PER_SEC = 1000*/
     /* Count  = ((16000000 / 64 / 1000) - 1) = 250-1 = 249 = 0x000000F9*/
     OCR0B = TIMER0_OCR0B_VALUE; 
     OCR0A = TIMER0_OCR0A_VALUE; 

     /* Prescaler = 64 */
     TCCR0B        |= (1<<CS01) | (1<<CS00); 

     /* Timer mode = CTC */
     TCCR0A        |= (1<<WGM01); 

     /* Clear  TIMER0 compare Interrupt Flag        */
     TIFR0        |= (1<<OCF0A);   

     /*Enable TIMER0 compare match A interrupt */
     TIMSK0 |= (1<<OCIE0A);

     /*Set OC0A on compare match, clear OC0A at BOTTOM (non-inverting mode)*/
     TCCR0A |= (1 << COM0A1);

     /*Enable Global Interrupts*/
     sei();
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

static  void  Timer3_Init_1ms (void)
{
     /*Disable Interrupts*/
     cli();

     /*Update OCR H and L based on Formula Count = ((F_CLK / Prescaler / OS_TICKS_PER_SEC) -1 )*/
     /* to achieve 1ms timer interrupt : F_CLK = 16000000 , Prescaler = 64 , OS_TICKS_PER_SEC = 1000*/
     /* Count  = ((16000000 / 64 / 1000) - 1) = 250-1 = 249 = 0x000000F9*/
     OCR3AH = TIMER3_OCR3AH_VALUE; 
     OCR3AL = TIMER3_OCR3AL_VALUE; 

     /* Timer mode = CTC and  Prescaler = 64 */
     TCCR3B        |= (1<<CS31) | (1<<CS30) |(1<<WGM32); 

     /* Clear  TIMER3 compare Interrupt Flag        */
     TIFR3        |= (1<<OCF3A);   

     /*Enable TIMER3 compare match A interrupt */
     TIMSK3 |= (1<<OCIE3A);

     /*Set OC3A on compare match, clear OC3A at BOTTOM (non-inverting mode)*/
     TCCR3A |= (1 << COM3A1);

     /*Enable Global Interrupts*/
     sei();
  
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
	Bsw_TickISR_Handler();
}

/*
*********************************************************************************************************
*                                        TIMER ISR
*
* Description :  This ISR is triggered every time when timer compare overflow interrupt generates and shall 
                 be used for General Purpose.
*               
* Arguments   : none
*********************************************************************************************************
*/

ISR(TIMER3_COMPA_vect)         
{
     milliseconds++;
     if(milliseconds == 1000)
     {
          PORTK ^= (1<<PK4);
          milliseconds = 0;
     }
}

