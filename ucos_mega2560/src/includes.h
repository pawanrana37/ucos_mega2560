// Select USART to print to
#define C0 0
#define F0 1
#define PRINT_TO_USART C0

// CodeVision AVR definitions
//// USART.BAUDCTRLB
#define USART_BSCALE_gm 0xF0 // Baud Rate Scale group mask
#define USART_BSCALE_bp 4 // Baud Rate Scale group position

#define TRUE 1
#define FALSE 0

// uC/OS-II header files
#include "ucos_ii.h"
#include "app_cfg.h"
#include "bsp.h"
// AVR device-specific IO definitions
#include <avr/io.h>
// Other AVR header files
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
