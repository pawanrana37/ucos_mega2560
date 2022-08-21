/*
*********************************************************************************************************
*                                           Atmel ATmega256
*                                               UART
*
*                                (c) Copyright 2005, Micrium, Weston, FL
*                                          All Rights Reserved
*
*
* File : UART.c
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
#define BAUD                        9600UL       // baud rate
#define UBRR_VAL ((CPU_CLK_FREQ + BAUD * 8) / (BAUD * 16) -1)    // clever rounding 
/*
*********************************************************************************************************
*                                              GLOBAL VARIABLES
*********************************************************************************************************
*/

volatile char ir_raw_command_data_0_char[7];

/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void  UART_Init (void);
int uart_putc ( unsigned char c );
void uart_puts ( char * s );
/*
*********************************************************************************************************
*                                       UART_Init
*
* Description : This function is used to initialize the UART.
*               
* Arguments   : none
*********************************************************************************************************
*/

void  UART_Init (void)
{
     UBRR0 = UBRR_VAL ;  
     UCSR0B |= 1 << TXEN0;  
     // Frame format: Asynchronous 8N1 
     UCSR0C = ( 1 << UCSZ01 ) | ( 1 << UCSZ00 );       
}

/*
*********************************************************************************************************
*                                        uart_putc
*
* Description : This function shall send the character to the UART. 
*               
* Arguments   : none
*********************************************************************************************************
*/

int uart_putc ( unsigned char c )   
{
    // ir_raw_command_data_0_char[0] = c;
    // breakpoint();
    while(!(UCSR0A & (1<<UDRE0)));/* wait until sending possible */    
    UDR0 = c ; /* send characters */                        
    return 0 ; 
}
/*
*********************************************************************************************************
*                                        uart_puts
*
* Description : This function shall call the uart_putc function for sending the string. 
*               
* Arguments   : none
* Note: puts is independent of the controller type
*********************************************************************************************************
*/

void uart_puts ( char * s )   
{
    while ( * s ) 
    { /* as long as *s != '\0' so not equal to the "string end character (terminator)" */   
        uart_putc ( * s );
        s ++ ;
    }
}