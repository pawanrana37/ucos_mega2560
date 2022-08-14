/*
*********************************************************************************************************
*                                           Atmel ATmega256
*                                               MCAL:Dio
*
*                                (c) Copyright 2005, Micrium, Weston, FL
*                                          All Rights Reserved
*
*
* File : Dio.c
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
#include "Rte.h"

/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              GLOBAL VARIABLES
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void Dio_WriteChannel(uint8_t port,uint8_t channel,uint8_t level);
uint8_t Dio_ReadChannel(uint8_t port,uint8_t channel);
void Dio_FlipChannel(uint8_t port,uint8_t channel);

/*
*********************************************************************************************************
*                                                   Dio_WriteChannel 
* Description : This function will write the Logic level to the GPIO Port Pin
*               
* Arguments   : none
*********************************************************************************************************
*/
void Dio_WriteChannel(uint8_t port,uint8_t channel,uint8_t level)
{
    #if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
    #endif

    OS_ENTER_CRITICAL();
    switch( port )
	{
        case 1:
            if( level == 1 )
                { setBit( PORTA, channel ); }
            else if(level == 0 )
                { clearBit( PORTA, channel); }
            break;

        case 2:
            if( level == 1 )
                { setBit( PORTB, channel ); }
            else if(level == 0 )
                { clearBit( PORTB, channel); }
            break;

        case 3:
            if( level == 1 )
                { setBit( PORTC, channel ); }
            else if(level == 0 )
                { clearBit( PORTC, channel); }
            break;

        case 4:
            if( level == 1 )
                { setBit( PORTD, channel ); }
            else if(level == 0 )
                { clearBit( PORTD, channel); }
            break;

        case 5:
            if( level == 1 )
                { setBit( PORTE, channel ); }
            else if(level == 0 )
                { clearBit( PORTE, channel); }
            break;

        case 6:
            if( level == 1 )
                { setBit( PORTF, channel ); }
            else if(level == 0 )
                { clearBit( PORTF, channel); }
            break;

        case 7:
            if( level == 1 )
                { setBit( PORTG, channel ); }
            else if(level == 0 )
                { clearBit( PORTG, channel); }
            break;

        case 8:
            if( level == 1 )
                { setBit( PORTH, channel ); }
            else if(level == 0 )
                { clearBit( PORTH, channel); }
            break;

        case 9:
            if( level == 1 )
                { setBit( PORTJ, channel ); }
            else if(level == 0 )
                { clearBit( PORTJ, channel); }
            break;

        case 10:
            if( level == 1 )
                { setBit( PORTK, channel ); }
            else if(level == 0 )
                { clearBit( PORTK, channel); }
            break;

        case 11:
            if( level == 1 )
                { setBit( PORTL, channel ); }
            else if(level == 0 )
                { clearBit( PORTL, channel); }
            break;
	}
    OS_EXIT_CRITICAL();
}

/*
*********************************************************************************************************
*                                                   Dio_ReadChannel 
* Description : This function will read the Logic level to the GPIO Port Pin
*               
* Arguments   : none
*********************************************************************************************************
*/
uint8_t Dio_ReadChannel(uint8_t port,uint8_t channel)
{
    #if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
    #endif

    OS_ENTER_CRITICAL();
    switch( port )
	{
        case 1:
            return (PINA &(1<<channel));
            break;

        case 2:
            return (PINB &(1<<channel));
            break;

        case 3:
            return (PINC &(1<<channel));
            break;

        case 4:
            return (PIND &(1<<channel));
            break;

        case 5:
            return (PINE &(1<<channel));
            break;

        case 6:
            return (PINF &(1<<channel));
            break;

        case 7:
            return (PING &(1<<channel));
            break;

        case 8:
            return (PINH &(1<<channel));
            break;

        case 9:
            return (PINJ &(1<<channel));
            break;

        case 10:
            return (PINK &(1<<channel));
            break;

        case 11:
            return (PINL &(1<<channel));
            break;

        default:
            return 0;
            break;
	}
    OS_EXIT_CRITICAL();
}

/*
*********************************************************************************************************
*                                                   Dio_FlipChannel 
* Description : This function will flip the Logic level to the GPIO Port Pin
*               
* Arguments   : none
*********************************************************************************************************
*/
void Dio_FlipChannel(uint8_t port,uint8_t channel)
{
    #if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
    #endif

    OS_ENTER_CRITICAL();
    switch( port )
	{
        case 1:
            toggleBit( PORTA, channel);
            break;

        case 2:
            toggleBit( PORTB, channel);
            break;

        case 3:
            toggleBit( PORTC, channel);
            break;

        case 4:
            toggleBit( PORTD, channel);
            break;

        case 5:
            toggleBit( PORTE, channel);
            break;

        case 6:
            toggleBit( PORTF, channel);
            break;

        case 7:
            toggleBit( PORTG, channel);
            break;

        case 8:
            toggleBit( PORTH, channel);
            break;

        case 9:
            toggleBit( PORTJ, channel);
            break;

        case 10:
            toggleBit( PORTK, channel);
            break;

        case 11:
            toggleBit( PORTL, channel);
            break;
	}
    OS_EXIT_CRITICAL();
}