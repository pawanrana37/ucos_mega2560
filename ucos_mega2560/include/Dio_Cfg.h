/*
*********************************************************************************************************
*                                           Atmel ATmega256
*                                               MCAL:Dio
*
*                                (c) Copyright 2005, Micrium, Weston, FL
*                                          All Rights Reserved
*
*
* File : Dio_Cfg.h
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
# define HIGH 1
# define LOW 0
#define AVR_PORTA					    1
#define AVR_PORTB					    2
#define AVR_PORTC					    3
#define AVR_PORTD					    4
#define AVR_PORTE					    5
#define AVR_PORTF					    6
#define AVR_PORTG					    7
#define AVR_PORTH					    8
#define AVR_PORTJ					    9
#define AVR_PORTK					    10
#define AVR_PORTL					    11
#define setBit(data, bit) do { data |= (1 << bit); } while(0)
#define clearBit(data, bit) do{ data &= ~(1 << bit); } while(0)
#define toggleBit(data, bit) do { data ^= (1 << bit ); } while(0)
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

