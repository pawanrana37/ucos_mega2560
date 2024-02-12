/*
*********************************************************************************************************
*                                           Atmel ATmega256
*                                         Board Support Package
*
*                                (c) Copyright 2005, Micrium, Weston, FL
*                                          All Rights Reserved
*
*
* File : Bsw.C
* By   : Pawan Singh Rana
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*                                               INCLUDES
*********************************************************************************************************
*/

#include "Bsw.h"

/*
*********************************************************************************************************
*                                               PROTOTYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         Bsw INITIALIZATION
*
* Description : This function should be called by your application code for the Basic Software Initialization.
*               
* Arguments   : none
*********************************************************************************************************
*/

void  Bsw_Init (void)
{
     EcuM_Init();

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

void  Bsw_TickISR_Handler (void)
{
    OSTimeTick();                                           /* Call uC/OS-II's OSTimeTick()                          */
}


