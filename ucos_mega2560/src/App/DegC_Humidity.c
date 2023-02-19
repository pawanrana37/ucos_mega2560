
/*
*********************************************************************************************************
*                                           Atmel ATmega256
*                                               Temperature and Humidity
*
*                                (c) Copyright 2005, Micrium, Weston, FL
*                                          All Rights Reserved
*
*
* File : DegC_Humidity.c
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
#include "Arduino.h"
/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

#define DHT11_DDR DDRE
#define DHT11_PORT PORTE
#define DHT11_PIN PINE
#define DHT11_INPUTPIN PE4

#define DHT11_LOW() DHT11_PORT &= ~(1 << DHT11_INPUTPIN)
#define DHT11_HIGH() DHT11_PORT |= (1 << DHT11_INPUTPIN)
#define DHT11_READ() (DHT11_PIN & (1 << DHT11_INPUTPIN))

#define DHT11_WAIT_LOW() while (DHT11_READ())
#define DHT11_WAIT_HIGH() while (!DHT11_READ())

#define MAX_TIMINGS 85
/*
*********************************************************************************************************
*                                              GLOBAL VARIABLES
*********************************************************************************************************
*/
volatile int temperature;

volatile int humidity;
uint8_t dht11_data[5] = {0, 0, 0, 0, 0};

extern OS_EVENT *msgbox_temperature; 
extern OS_EVENT *msgbox_humidity;
volatile int data[40];
/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void DHT11_Init(void);
int fx_DHT11_readData(void);
void fx_DHT11_MainFunction(void);

/*
*********************************************************************************************************
*                                                   DHT11_Init 
* Description : This function will  initialize the DHT11 Sensor
*               
* Arguments   : none
*********************************************************************************************************
*/
void DHT11_Init(void)
{
    DHT11_DDR |= (1 << DHT11_INPUTPIN);
    DHT11_PORT |= (1 << DHT11_INPUTPIN);
}
/*
*********************************************************************************************************
*                                                   fx_DHT11_readData 
* Description : This function with Write Command to LCD
*               
* Arguments   : none
*********************************************************************************************************
*/

int fx_DHT11_readData(void) 
{
  
  uint8_t laststate = 1;
  uint8_t counter = 0;
  uint8_t j = 0, i;
  dht11_data[0] = dht11_data[1] = dht11_data[2] = dht11_data[3] = dht11_data[4] = 0;
  DHT11_LOW();
  _delay_ms(18);
  DHT11_HIGH();
  _delay_us(40);
  DHT11_WAIT_LOW();
  DHT11_WAIT_HIGH();
  for (i = 0; i < MAX_TIMINGS; i++)
  {
    counter = 0;
    while (DHT11_READ() == laststate)
    {
      counter++;
      _delay_us(1);
      if (counter == 255)
      {
        break;
      }
    }
    laststate = DHT11_READ();
    if (counter == 255)
      break;
    if ((i >= 4) && (i % 2 == 0))
    {
      dht11_data[j / 8] <<= 1;
      if (counter > 16)
        dht11_data[j / 8] |= 1;
      j++;
    }
  }
  if ((j >= 40) && (dht11_data[4] == ((dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3]) & 0xFF)))
    return 1;
  else
    return 0;

}

/*
*********************************************************************************************************
*                                                   fx_DHT11_readData 
* Description : This function with Write Command to LCD
*               
* Arguments   : none
*********************************************************************************************************
*/

void fx_DHT11_MainFunction(void) 
{
    if (fx_DHT11_readData())
    {
     // printf("Humidity = %d.%d %% Temperature = %d.%d *C\n", dht11_data[0], dht11_data[1], dht11_data[2], dht11_data[3]);
     temperature = dht11_data[0];
     humidity = dht11_data[2];
    }
    else
    {
      //printf("Data not read\n");
    }
    _delay_ms(1000);

    OSMboxPost(msgbox_temperature,(int *)&temperature);
    OSMboxPost(msgbox_humidity,(int *)&humidity);
}

