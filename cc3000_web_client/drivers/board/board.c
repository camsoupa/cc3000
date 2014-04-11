//*****************************************************************************
//
// board.c -  Board functions for EK-TM4C123GXL Tiva C Series LaunchPad.
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

#include <stdint.h>
#include <stdbool.h>
/*
#include "inc/hw_types.h"
#include "driverlib/timer.h"
#include "driverlib/rom_map.h"
#include "driverlib/systick.h"
#include "driverlib/fpu.h"
#include "driverlib/debug.h"
#include "utils/uartstdio.h"
#include "driverlib/uart.h"
#include "driverlib/ssi.h"
#include "wlan.h"
#include "evnt_handler.h"
#include "nvmem.h"
#include "socket.h"
#include "cc3000_common.h"
#include "netapp.h"
#include "spi.h"
#include "hci.h"
#include "dispatcher.h"
#include "spi_version.h"
#include "board.h"
*/

#include   "../mss_uart/mss_uart.h"
#include   "../mss_gpio/mss_gpio.h"
#include   "../mss_spi/mss_spi.h"
#include   "../mss_timer/mss_timer.h"
#include   "../board/board.h"
#include   "../mss_timer/mss_timer.h"
#include "../../CC3000HostDriver/wlan.h"
#include "../../CC3000HostDriver/evnt_handler.h"
#include "../../CC3000HostDriver/nvmem.h"
#include "../../CC3000HostDriver/socket.h"
#include "../../CC3000HostDriver/netapp.h"
#include "../../CC3000HostDriver/matt_spi.h"
#include "../../CC3000HostDriver/hci.h"


//*****************************************************************************
//
//! pio_init
//!
//! \param  none
//!
//! \return none
//!
//! \brief  Initialize the board's I/O
//
//*****************************************************************************
void pio_init()
{
    //
    // Initialize the system clock.
    //
    //initClk(); // I don't think we need to do this.

    //
    // Configure the system peripheral bus that IRQ & EN pin are mapped to.
    //
    //MAP_SysCtlPeripheralEnable( SYSCTL_PERIPH_IRQ_PORT); // I don't think we do this for our board
    //MAP_SysCtlPeripheralEnable( SYSCTL_PERIPH_SW_EN_PORT); // the actual interrupt config is below


    //
    // Disable all the interrupts before configuring the lines.
    //
    //MAP_GPIOIntDisable(SPI_GPIO_IRQ_BASE, 0xFF);
	MSS_GPIO_disable_irq(SPI_IRQ_PIN);


    //
    // Configure the WLAN_IRQ pin as an input.
    //
    //MAP_GPIOPinTypeGPIOInput(SPI_GPIO_IRQ_BASE, SPI_IRQ_PIN);
    //GPIOPadConfigSet(SPI_GPIO_IRQ_BASE, SPI_IRQ_PIN, GPIO_STRENGTH_2MA,
    //                    GPIO_PIN_TYPE_STD_WPU);
    //
    // Setup the GPIO interrupt for this pin
    //
    //MAP_GPIOIntTypeSet(SPI_GPIO_IRQ_BASE, SPI_IRQ_PIN, GPIO_FALLING_EDGE);


    // This single call takes care of the configure as input and as negative edge interrupt
    // handler for this interrupt is IntSpiGPIOHandler()
	NVIC_EnableIRQ(GPIO2_IRQn);
	MSS_GPIO_config( SPI_IRQ_PIN, MSS_GPIO_INPUT_MODE | MSS_GPIO_IRQ_EDGE_NEGATIVE );



    //
    // Configure the pins for the enable signal to the CC3000.
    //
    //MAP_GPIOPinTypeGPIOOutput(SPI_GPIO_SW_EN_BASE, SPI_EN_PIN);
    //MAP_GPIODirModeSet( SPI_GPIO_SW_EN_BASE, SPI_EN_PIN, GPIO_DIR_MODE_OUT );
    //MAP_GPIOPadConfigSet( SPI_GPIO_SW_EN_BASE, SPI_EN_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD );

	MSS_GPIO_config( SPI_EN_PIN, MSS_GPIO_OUTPUT_MODE);

    //MAP_GPIOPinWrite(SPI_GPIO_SW_EN_BASE, SPI_EN_PIN, PIN_LOW);
	MSS_GPIO_set_output(SPI_EN_PIN, 0);

    //see Ti's SysCtlDelay in tivaware driverlib/sysctl.c
	// it is an asm loop of 3 instructions doing what delay is doing

    //SysCtlDelay(600000);
    //SysCtlDelay(600000);
    //SysCtlDelay(600000);
    delay(600000);
    delay(600000);
    delay(600000);

    //
    // Disable WLAN CS with pull up Resistor
    //
    //MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SPI_PORT);
    //MAP_GPIOPinTypeGPIOOutput(SPI_CS_PORT, SPI_CS_PIN);
    //GPIOPadConfigSet(SPI_CS_PORT, SPI_CS_PIN, GPIO_STRENGTH_2MA,
    //                            GPIO_PIN_TYPE_STD_WPU);



    // Do this after spi is configured !!!
    // CS is low enabled!!!
    // CORE SPI is not doing this for us now... it is manual
	//MSS_GPIO_config( SPI_CS_PIN, MSS_GPIO_OUTPUT_MODE);

    //MAP_GPIOPinWrite(SPI_CS_PORT, SPI_CS_PIN, PIN_HIGH);
	// CS is low enabled so this is de-asserting it
	//MSS_GPIO_set_output(SPI_CS_PIN, 1);


    //
    // Enable interrupt for WLAN_IRQ pin
    //
    //MAP_GPIOIntEnable(SPI_GPIO_IRQ_BASE, SPI_IRQ_PIN);
    //MSS_GPIO_enable_irq( SPI_IRQ_PIN );

    //
    // Clear interrupt status
    //
    SpiCleanGPIOISR();  //TODO


    //MAP_IntEnable(INT_GPIO_SPI);  // same as enable_irq two lines above?

    //
    // Initialize LED Pins and state.
    //
    initLEDs();
}

//*****************************************************************************
//
//! ReadWlanInterruptPin
//!
//! \param  none
//!
//! \return none
//!
//! \brief  return wlan interrup pin
//
//*****************************************************************************
long ReadWlanInterruptPin(void)
{
    //return MAP_GPIOPinRead(SPI_GPIO_IRQ_BASE, SPI_IRQ_PIN);

    uint32_t gpio_inputs;
    long spi_irq_status = 0;

    gpio_inputs = MSS_GPIO_get_inputs();
    spi_irq_status = gpio_inputs & MSS_GPIO_2_MASK; // this works according to
                                                    // where this mask is defined
	return spi_irq_status;
}

//*****************************************************************************
//
//! Enable waln IrQ pin
//!
//! \param  none
//!
//! \return none
//!
//! \brief  Nonr
//
//*****************************************************************************
void WlanInterruptEnable()
{
    //MAP_GPIOIntEnable(SPI_GPIO_IRQ_BASE, SPI_IRQ_PIN);
    MSS_GPIO_enable_irq( SPI_IRQ_PIN );
}

//*****************************************************************************
//
//! Disable waln IrQ pin
//!
//! \param  none
//!
//! \return none
//!
//! \brief  Nonr
//
//*****************************************************************************
void WlanInterruptDisable()
{
    // MAP_GPIOIntDisable(SPI_GPIO_IRQ_BASE, SPI_IRQ_PIN);
    MSS_GPIO_disable_irq( SPI_IRQ_PIN );
}

//*****************************************************************************
//
//! WriteWlanPin
//!
//! \param  new val
//!
//! \return none
//!
//! \brief  This functions enables and disables the CC3000 Radio
//
//*****************************************************************************
void WriteWlanPin( unsigned char val )
{

    if(val)
    {
        //MAP_GPIOPinWrite(SPI_GPIO_SW_EN_BASE, SPI_EN_PIN,PIN_HIGH);
    	MSS_GPIO_set_output(SPI_EN_PIN, 1);
    }
    else
    {
        //MAP_GPIOPinWrite(SPI_GPIO_SW_EN_BASE, SPI_EN_PIN, PIN_LOW);
    	MSS_GPIO_set_output(SPI_EN_PIN, 0);
    }

}

//*****************************************************************************
//
// Init SysTick timer.
//
//
//*****************************************************************************
void
InitSysTick(void)
{
	//THEIR METHOD
    //
    // Configure SysTick to occur 10 times per second and enable its interrupt.
    //
	// the Get() gets the clk frequency
	// see driverlib/systick.h in Tivaware
    //SysTickPeriodSet(SysCtlClockGet() / SYSTICK_PER_SECOND); // Tiva's clk returns 66,666,666 for the Get()
    //SysTickIntEnable(); // enable the interrupt from this timer
    //SysTickEnable();    // start the counter

	//MANUAL OUR METHOD
    // Set up handler to SysTickHandler in startup_a2fxxxm3.s
	/*
	MSS_TIM1_init( MSS_TIMER_PERIODIC_MODE );
	MSS_TIM1_load_immediate( 100000000 / SYSTICK_PER_SECOND );
	MSS_TIM1_enable_irq();
	MSS_TIM1_start();
    */

	//SMARTFUSION METHOD
	SysTick_Config(2); // 20ms between interrups i'm thinking is good? I think we have a 10ms tick



}

//*****************************************************************************
//
// The interrupt handler for the SysTick timer.  This handler is called every 1ms
//
//
//*****************************************************************************
__attribute__((__interrupt__)) void SysTick_Handler(void)
{

	// no call to SysTickIntRegister but this is set up in the TI code as the SysTick
	// interrupt handler for the timer in startup_ccs.c
    // very similar to our NVIC thing in CMSIS/startup_gcc/startup_A2FXXXM3.s

    static unsigned long ulTickCount = 0;

    //
    // Increment the tick counter.
    //
    ulTickCount++;

    //
    // Has half a second passed since we last called the event handler?
    //
    if(ulTickCount >= (SYSTICK_PER_SECOND / 2))
    {
    	// maybe flash an led on and off to make sure this is happening

        //
        // Yes = call the unsolicited event handler.  We need to do this a
        // few times each second.
        //
        hci_unsolicited_event_handler();
        ulTickCount = 0;
    }
}

//*****************************************************************************
//
//! init clk
//!
//!  \param  None
//!
//!  \return none
//!
//!  \Init the device with 16 MHz clock.
//
//*****************************************************************************
void initClk(void)
{

	// I don't think we need to init a clk on our board.
    //
    // 16 MHz Crystal on Board. SSI Freq - configure M4 Clock to be ~50 MHz
    //
    // MAP_SysCtlClockSet(SYSCTL_SYSDIV_3 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
    //                   SYSCTL_XTAL_16MHZ);
}

//*****************************************************************************
//
// Initialize all LEDs on the board.
//
//*****************************************************************************
void initLEDs()
{

	// Setup the 3 LEDs we want to use as GPIO outputs
    MSS_GPIO_config( MSS_GPIO_0, MSS_GPIO_OUTPUT_MODE);
    MSS_GPIO_config( MSS_GPIO_1, MSS_GPIO_OUTPUT_MODE);
    MSS_GPIO_config( MSS_GPIO_3, MSS_GPIO_OUTPUT_MODE);

    // Turn all of them off
    MSS_GPIO_set_outputs(15);

    // Turn one on so we know we're at this point in code
    // MSS_GPIO_set_output(0, 0);
    turnLedOn(LED_0);

}

//*****************************************************************************
//
// Turns a single board LED on.
//
//*****************************************************************************
void turnLedOn(tBoardLED eLED)
{
   MSS_GPIO_set_output(eLED, 0);
}

//*****************************************************************************
//
// Turns off a single LED on the board.
//
//*****************************************************************************
void turnLedOff(tBoardLED eLED)
{
	MSS_GPIO_set_output(eLED, 1);
}
