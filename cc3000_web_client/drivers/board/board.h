//*****************************************************************************
//
// board.h - Board definitions for EK-TM4C123GXL Tiva C Series LaunchPad.
//
// Copyright (c) 2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
// This is part of revision 2.1.0.12573 of the Tiva Firmware Development Package.
//
//*****************************************************************************
#ifndef __BOARD_H__
#define __BOARD_H__



//*****************************************************************************
//
// The desired system tick frequency.Defines for setting up the system tick.
//
//*****************************************************************************
#define SYSTICK_PER_SECOND            10

//*****************************************************************************
//
// The desired system clock rate.
//
//*****************************************************************************
#define SYSCLOCK_RATE_HZ                100000000 // changed from 50,000,000 to 100,000,000

//*****************************************************************************
//
// The parameter to pass to SysCtlDelay() to cause a delay of approximately
// 50 microseconds.
//
//*****************************************************************************
#define DELAY_50_MICROSECONDS           ((SYSCLOCK_RATE_HZ / 20000))



//*****************************************************************************
//
// SPI-related definitions.
//
//*****************************************************************************

//
// SPI General Defines
//
#define SPI_WINDOW_SIZE                 DMA_WINDOW_SIZE
#define FWT_DELAY                       4000
#define DMA_WINDOW_SIZE                 1024

//
// CC3000 Board specific Macros
//

#define ASSERT_CS()            (MSS_GPIO_set_output(SPI_CS_PIN, 0))

#define DEASSERT_CS()          (MSS_GPIO_set_output(SPI_CS_PIN, 1))

#define ASSERT(a)               if(!a) exit(1);
//
// IRQ settings
//
#define SPI_IRQ_PIN                     MSS_GPIO_2      // was GPIO_PIN_2 but MSS_GPIO_2 is our board's method

//
// SW EN settings
//
#define SPI_EN_PIN                      MSS_GPIO_4  // We have the pass through pin config thing that comes directly
                                                      // from the spi core so I don't think we need to set this up
//
// CS settings  PE0
//
#define SPI_CS_PIN                      MSS_GPIO_9

//
// SPI Hardware Abstraction layer
//

#define SPI_CLK_PIN                     GPIO_PIN_4
#define SPI_RX_PIN                      GPIO_PIN_6
#define SPI_TX_PIN                      GPIO_PIN_7


//*****************************************************************************
//
// UART definitions.
//
//*****************************************************************************

// deleted because we use our own uart things

//*****************************************************************************
//
// LED connection definitions.
//
//*****************************************************************************

// deleted because we use our own uart things

typedef enum
{
    LED_0=0, //Red
    LED_1=1, //Green
    LED_2=3  //Blue
}
tBoardLED;

//*****************************************************************************
//
// Function Prototypes
//
//*****************************************************************************

extern int delay ( volatile uint32_t);
extern void pio_init(void);
extern void initLEDs(void);
extern long ReadWlanInterruptPin(void);
extern void WlanInterruptEnable(void);
extern void WlanInterruptDisable(void);
extern void WriteWlanPin( unsigned char val );
extern void InitSysTick(void);
extern void SysTickHandler(void);
extern void initClk(void);
extern void turnLedOn(tBoardLED eLED);
extern void turnLedOff(tBoardLED eLED);



#endif //__BOARD_H__
