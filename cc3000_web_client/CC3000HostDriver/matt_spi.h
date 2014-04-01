//*****************************************************************************
//
// spi.h  - CC3000 Host Driver Implementation.
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

#ifndef __SPI_H__
#define __SPI_H__

#include <stdint.h>
#include <stdbool.h>
#include "cc3000_common.h"

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef  __cplusplus
extern "C" {
#endif

//*****************************************************************************
//
// A function pointer type describing the packet receive callback.
//
//*****************************************************************************
typedef void (*tSpiHandleRx)(void *p);

//*****************************************************************************
//
// The global transmit buffer.  This is accessed from wlan.c.
//
//*****************************************************************************
extern uint8_t wlan_tx_buffer[];

//*****************************************************************************
//
// Prototypes for the public SPI module APIs.
//
//*****************************************************************************
extern void SpiOpen(tSpiHandleRx pfRxHandler);
extern void SpiClose(void);
extern long SpiWrite(uint8_t *pui8UserBuffer, uint16_t ui16Length);
extern void SpiResumeSpi(void);
extern void SpiConfigureHwMapping(void);
extern uint32_t SpiCleanGPIOISR(void);
extern int init_spi(uint32_t ui32SSIFreq, uint32_t ui32SysClck);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef  __cplusplus
}
#endif // __cplusplus

#endif
