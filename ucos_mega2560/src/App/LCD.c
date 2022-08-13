/*
*********************************************************************************************************
*                                           Atmel ATmega256
*                                               LCD
*
*                                (c) Copyright 2005, Micrium, Weston, FL
*                                          All Rights Reserved
*
*
* File : LCD.c
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
static void Lcd_DataWrite(unsigned char dat);
static void Lcd_CmdWrite(unsigned char cmd);
/*
*********************************************************************************************************
*                                                   Lcd_CmdWrite 
* Description : This function with Write Command to LCD
*               
* Arguments   : none
*********************************************************************************************************
*/
static void Lcd_CmdWrite(unsigned char cmd)
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
*********************************************************************************************************
*                                                   Lcd_DataWrite 
* Description : This function will write Data to LCD
*               
* Arguments   : none
*********************************************************************************************************
*/
static void Lcd_DataWrite(unsigned char dat)
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
