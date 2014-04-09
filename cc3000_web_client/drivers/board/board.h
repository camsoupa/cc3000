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
#define DELAY_50_MICROSECONDS           ((SYSCLOCK_RATE_HZ / 20000) / 3)

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
#define ASSERT_CS()          (MAP_GPIOPinWrite(SPI_CS_PORT, SPI_CS_PIN, 0))     //Core spi is doing this for us...
#define DEASSERT_CS()        (MAP_GPIOPinWrite(SPI_CS_PORT, SPI_CS_PIN, 0xFF))  //Core spi is doing this for us...

//
// IRQ settings
//
//#define SYSCTL_PERIPH_IRQ_PORT          SYSCTL_PERIPH_GPIOB   // matt- I think this is a TI thing we don't need to do
//#define SPI_GPIO_IRQ_BASE               GPIO_PORTB_BASE       // matt- I think this is a TI thing we don't need to do
#define SPI_IRQ_PIN                     MSS_GPIO_2      // was GPIO_PIN_2 but MSS_GPIO_2 is our board's method
//#define INT_GPIO_SPI                    INT_GPIOB
//#define INT_SPI                         INT_SSI2

//
// SW EN settings
//
//#define SYSCTL_PERIPH_SW_EN_PORT        SYSCTL_PERIPH_GPIOB  // matt- I think this is a TI thing we don't need to do
//#define SPI_GPIO_SW_EN_BASE             GPIO_PORTB_BASE      // matt- I think this is a TI thing we don't need to do
#define SPI_EN_PIN                      MSS_GPIO_4  // We have the pass through pin config thing that comes directly
                                                      // from the spi core so I don't think we need to set this up
//
// CS settings  PE0
//
//#define SPI_CS_PORT                     GPIO_PORTE_BASE      // matt- TI thing
//#define SPI_CS_PIN                      GPIO_PIN_0           // matt- CORE SPI does this for us
//#define SYSCTL_PERIPH_SPI_PORT          SYSCTL_PERIPH_GPIOE  // Ti thing

//
// SPI Hardware Abstraction layer
//

// OURS
//#define SPI_INT                       this is on the FABINT
#define SPI_RX_AVAIL                    MSS_GPIO_31
#define SPI_TX_RFM                      MSS_GPIO_30


// THEIRS
#define SPI_BASE                        SSI2_BASE
#define SPI_CLK_PIN                     GPIO_PIN_4
#define SPI_RX_PIN                      GPIO_PIN_6
#define SPI_TX_PIN                      GPIO_PIN_7

#define SYSCTL_PERIPH_SPI               SYSCTL_PERIPH_SSI2
#define SYSCTL_PERIPH_SPI_BASE          SYSCTL_PERIPH_GPIOB

#define SPI_PORT                        GPIO_PORTB_BASE
#define SPI_CLK_MUX_SEL                 GPIO_PB4_SSI2CLK
#define SPI_RX_MUX_SEL                  GPIO_PB6_SSI2RX
#define SPI_TX_MUX_SEL                  GPIO_PB7_SSI2TX

#define SPI_UDMA_RX_CHANNEL             UDMA_CH12_SSI2RX
#define SPI_UDMA_TX_CHANNEL             UDMA_CH13_SSI2TX

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
