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
#include <avr/io.h>
#include <util/delay.h>
#include "Dio_Cfg.h"
/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

#define LCD_Dir  DDRF			/* Define LCD data port direction */
#define LCD_Ctrl  DDRK			/* Define LCD data port direction */
#define LCD_Ctrl_Port  PORTK			/* Define LCD data port direction */
#define LCD_Port PORTF			/* Define LCD data port */
#define RS PK0				/* Define Register Select pin */
#define EN PK1 				/* Define Enable signal pin */
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
void LCD_Char(unsigned char data);
void LCD_Command(unsigned char cmd);
void LCD_Init(void);
void LCD_String(char *str);
void fx_LCD_temperature_Data( unsigned char);
void fx_LCD_humidity_Data( unsigned char);

/*
*********************************************************************************************************
*                                                   Lcd_CmdWrite 
* Description : This function with Write Command to LCD
*               
* Arguments   : none
*********************************************************************************************************
*/
void LCD_Command(unsigned char cmd)
{
    
    //LCD_Port = (LCD_Port & 0x0F) | (cmd & 0xF0);
    LCD_Ctrl_Port &= ~(1 << RS);
    LCD_Port = cmd;
    //LCD_Port &= ~(1 << RW);
    LCD_Ctrl_Port |= (1 << EN);
    _delay_ms(1);
    //OSTimeDlyHMSM(0,0,0,1);
    LCD_Ctrl_Port &= ~(1 << EN);
    //_delay_ms(1);
    //OSTimeDlyHMSM(0,0,0,1);
    //LCD_Port = (LCD_Port & 0x0F) | (cmd << 4);
    //LCD_Port &= ~(1 << RS);
    //LCD_Port |= (1 << EN);
    //_delay_us(1);
    //OSTimeDlyHMSM(0,0,0,1);
    //LCD_Port &= ~(1 << EN);
    //_delay_ms(50);
    //OSTimeDlyHMSM(0,0,0,50);
}
/*
*********************************************************************************************************
*                                                   Lcd_DataWrite 
* Description : This function will write Data to LCD
*               
* Arguments   : none
*********************************************************************************************************
*/
void LCD_Char(unsigned char data)
{
    //LCD_Port = (LCD_Port & 0x0F) | (data & 0xF0);
    LCD_Ctrl_Port |= (1 << RS);
    LCD_Port = data;
    //LCD_Port &= ~(1 << RW);
    LCD_Ctrl_Port |= (1 << EN);
    _delay_ms(1);
    //OSTimeDlyHMSM(0,0,0,1);
    LCD_Ctrl_Port &= ~(1 << EN);
    //_delay_ms(1);
    //OSTimeDlyHMSM(0,0,0,1);
    //LCD_Port = (LCD_Port & 0x0F) | (data << 4);
    //LCD_Port |= (1 << RS);
    //LCD_Port |= (1 << EN);
    //_delay_us(1);
    //OSTimeDlyHMSM(0,0,0,1);
    //LCD_Port &= ~(1 << EN);
    //_delay_ms(50);
    //Dio_WriteChannel(AVR_PORTE,LED_STATUS_RED,HIGH);
    ////OSTimeDlyHMSM(0,0,0,50);
}
/*
*********************************************************************************************************
*                                                   LCD_Init 
* Description : This function will  initialize the LCD
*               
* Arguments   : none
*********************************************************************************************************
*/
void LCD_Init(void)
{
    LCD_Dir = 0xFF;
    LCD_Ctrl = 0xFF;
    _delay_ms(20);
    LCD_Command(0x38);  //Use two lines and 5×7 matrix
    _delay_ms(5);
    LCD_Command(0x38);  //Use two lines and 5×7 matrix
    _delay_ms(5);
    LCD_Command(0x0C);  //Initialize LCD for 4-bit mode
    _delay_ms(1);
    LCD_Command(0x01);  //Initialize LCD for 4-bit mode
    _delay_ms(1);
    LCD_Command(0x06);  //Function set: 4-bit, 2-line, 5x8 dot


}
/*
*********************************************************************************************************
*                                                   LCD_String 
* Description : This function will write String to LCD
*               
* Arguments   : none
*********************************************************************************************************
*/
void LCD_String(char *str)
{
    int i;
    for(i = 0; str[i] != '\0'; i++)
    {
        LCD_Char(str[i]);
    }

}

/*
/*
*********************************************************************************************************
*                                                   fx_LCD_humidity_Data 
* Description : This function will print the current humidity value to LCD
*               
* Arguments   : none
*********************************************************************************************************
*/
void fx_LCD_humidity_Data( unsigned char humidity_value)
{
    int i;
    char buffer[10];
    char *str = "Humidity=";
    LCD_Command(0xC0);  //Start the Print from Second Row
    for(i = 0; str[i] != '\0'; i++)
    {
        LCD_Char(str[i]);
    }
    sprintf(buffer, "%d", humidity_value);
    for(i = 0; buffer[i] != '\0'; i++)
    {
        LCD_Char(buffer[i]);
    }
    LCD_Char('%');

}

/*
*********************************************************************************************************
*                                                   fx_LCD_temperature_Data 
* Description : This function will print the current temperature value to LCD
*               
* Arguments   : none
*********************************************************************************************************
*/
void fx_LCD_temperature_Data( unsigned char temp_value)
{
    int i;
    char buffer[10];
    char *str = "Temperature=";
    LCD_Command(0x80);  //Start the Print from Beginning
    for(i = 0; str[i] != '\0'; i++)
    {
        LCD_Char(str[i]);
    }
    sprintf(buffer, "%d", temp_value);
    for(i = 0; buffer[i] != '\0'; i++)
    {
        LCD_Char(buffer[i]);
    }
    LCD_Char('C');

}