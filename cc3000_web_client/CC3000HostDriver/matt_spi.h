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

#define  FABRIC_BASE_ADDR  0x40050000

//int raw register masks
#define CORE_SPI_INT_RAW_TXRFM_MASK      = 0x0080;
#define CORE_SPI_INT_RAW_RXAVAIL_MASK    = 0x0040;
#define CORE_SPI_INT_RAW_SSEND_MASK      = 0x0020;
#define CORE_SPI_INT_RAW_CMDINT_MASK     = 0x0010;
#define CORE_SPI_INT_RAW_TXUNDERRUN_MASK = 0x0008;
#define CORE_SPI_INT_RAW_RXOVERFLOW_MASK = 0x0004;
#define CORE_SPI_INT_RAW_RESERVED_MASK   = 0x0002;
#define CORE_SPI_INT_RAW_TXDONE_MASK     = 0x0001;

// control 2 register masks
#define CORE_SPI_CTRL2_INTEN_TXRFM_MASK   = 0x0080;
#define CORE_SPI_CTRL2_INTEN_RXAVAIL_MASK = 0x0040;
#define CORE_SPI_CTRL2_INTEN_SSEND_MASK   = 0x0020;
#define CORE_SPI_CTRL2_INTEN_CMD_MASK     = 0x0010;
#define CORE_SPI_CTRL2_RESERVED_MASK      = 0x0008;
#define CORE_SPI_CTRL2_CMDSIZE_MASK       = 0x0007;

// control 1 register masks
#define CORE_SPI_CTRL1_OENOFF_MASK      = 0x0080;
#define CORE_SPI_CTRL1_FRAMEURUN_MASK   = 0x0040;
#define CORE_SPI_CTRL1_INTTXURUN_MASK   = 0x0020;
#define CORE_SPI_CTRL1_INTRXOVFLOW_MASK = 0x0010;
#define CORE_SPI_CTRL1_INTTXDONE_MASK   = 0x0008;
#define CORE_SPI_CTRL1_RESERVED_MASK    = 0x0004;
#define CORE_SPI_CTRL1_MASTER_MASK      = 0x0002;
#define CORE_SPI_CTRL1_ENABLE_MASK      = 0x0001;

// cmd register
#define CORE_SPI_CMD_TXFIFORST_MASK = 0x0002;
#define CORE_SPI_CMD_RXFIFORST_MASK = 0x0001;

// int clear register
#define CORE_SPI_INT_CLR_TXRFM_MASK   = 0x0080;
#define CORE_SPI_INT_CLR_RXAVAIL_MASK = 0x0040;

// status register masks
#define CORE_SPI_STATUS_ACTIVE_MASK     = 0x0080;
#define CORE_SPI_STATUS_SSEL_MASK       = 0x0040;
#define CORE_SPI_STATUS_TXUNDERRUN_MASK = 0x0020;
#define CORE_SPI_STATUS_RXOVFLOW_MASK   = 0x0010;
#define CORE_SPI_STATUS_TXFULL_MASK     = 0x0008;
#define CORE_SPI_STATUS_RXEMPTY_MASK    = 0x0004;
#define CORE_SPI_STATUS_DONE_MASK       = 0x0002;
#define CORE_SPI_STATUS_FIRSTFRAME_MASK = 0x0001;

typedef struct {
 /*
  * reset value: 0x02
  * Bits Name      Type Description
  * 7    OENOFF    R/W  0: SPI output enable active as required
  *                     1: The core will not assert the SPI output enable.
  *                        This allows multiple slaves to be connected to a
  *                        single master sharing a single slave select and
  *                        software protocol implemented that can enable
  *                        the slaves transmit data when a certain broadcast
  *                        address SPI command is received.
  * 6    FRAMEURUN R/W  0: Underruns are generated whenever a read is attempted
  *                        from an empty transmit FIFO
  *                     1: Under run condition will be ignored for the
  *                        complete frame if the first data frame read resulted
  *                        in a potential overflow, i.e. the slave was not ready
  *                        to transmit any data. If the first data frame is read
  *                        from the FIFO and transmitted then an under run will
  *                        be generated if the FIFO becomes empty for any of the
  *                        remaining packet frames, that is, while SSEL is active.
  *                        Master operation will never create a transmit FIFO under run
  *                        condition.
  * 5    INTTXURUN R/W   Interrupt on transmit under run
  *                      0: Interrupt disabled
  *                      1: Interrupt enabled.
  * 4    INTRXOVFLOW R/W Interrupt on receive overflow
  *                      0: Interrupt disabled
  *                      1: Interrupt enabled.
  * 3    INTTXDONE  R/W  Interrupt on transmit data of data which has been placed
  *                      in TX FIFO through the TXDATA_LAST register.
  *                      0: Interrupt disabled
  *                      1: Interrupt enabled.
  * 2    Reserved
  * 1    MASTER     R/W  0: Run CoreSPI in Slave mode
  *                      1: Run CoreSPI in Master mode
  * 0    ENABLE     R/W  0: Core does not respond to external signals until this
  *                         bit is enabled. SPISCLKO driven to zero and SPIOE, S
  *                         PISS (slave select) driven inactive.
  *                      1: Core is active
  */
 volatile uint32_t controlReg1;
 /*
  * Bits Name    Type Description
  * 7    TXRFM   W    Writing 1 clears the TXRFM interrupt
  * 6    RXAVAIL W    Writing 1 clears the RXAVAIL interrupt
  * ...
  */
 volatile uint32_t intClear;
 /*
  * Receive Data Register
  * Reading from this register reads one frame from the RX FIFO.
  */
 volatile uint32_t RXdata;
 /*
  * Transmit Data Register
  * Writing to this register writes one frame to the TX FIFO.
  */
 volatile uint32_t TXdata;
 /* Masked interrupt status
  * These bits indicate the masked interrupt status
  * by ANDING the interrupt enables in the CONTROL and CONTROL2
  * registers with the raw interrupt register.
  * When any of these bits are set, the INTERRUPT output will be active.
  * The bits are cleared by writing to the Interrupt clear register.
  */
 volatile uint32_t intMask;
 /* Bits Name       Type Description
  * 7    TXRFM      R    Indicates that there is at least one frame free in the transmit FIFO for writing.
  * 6    RXAVAIL    R    Indicates that there is at least one frame available for reading from the receive FIFO.
  * 5    SSEND      R    Indicates that SSEL is Inactive.
  * 4    CMDINT     R    Indicates that the number of frames set by the CMDSIZE register have
  *                      been received as a single packet of frames (SSEL held active).
  * 3    TXUNDERRUN R    Indicates that in Slave mode that the data was not available when required
  *                      in the transmit FIFO.
  * 2    RXOVERFLOW R    Indicates that in Master and Slave mode the receive FIFO is overflowed.
  * 1    -          -    Reserved
  * 0    TXDONE     R    Indicates that all frames
  */
 volatile uint32_t intRaw;
 /*
  * Bits Name          Type Description
  * 7    INTEN_TXRFM   R/W  1: Enables the interrupt when there is room in the TX FIFO.
  * 6    INTEN_RXAVAIL R/W  1: Enables the interrupt when there are frames in the RX FIFO.
  * 5    INTEN_SSEND   R/W  1: Enables the interrupt as SSEL goes High. SPI master and slave modes.
  * 4    INTEN_CMD     R/W  1: Enables Interrupt after the number of frames set by CMDSIZE
  *                         (above) has been received as a single packet of frames (SSEL held active).
  * 3    Reserved
  * 2:0  CMDSIZE       R/W  Number of frames
  */
 volatile uint32_t controlReg2;
/*
 * Bits Name Type Description
 * 7:2            Reserved
 * 1    TXFIFORST W Writing 1 will reset the TX FIFO. This bit always reads as zero
 * 0    RXFIFORST W Writing 1 will reset the RX FIFO
 */
 volatile uint32_t cmd;        //write only
 /*
  * Bits Name       Type Description
  * 7    ACTIVE     R    Core is still transmitting data
  * 6    SSEL       R    Current state of SSEL
  * 5    TXUNDERRUN R    Transmit FIFO under flowed
  * 4    RXOVFLOW   R    Receive FIFO overflowed
  * 3    TXFULL     R    Transmit FIFO is full, that is, no space for more data
  * 2    RXEMPTY    R    Receive FIFO is empty, that is, no data available to read
  * 1    DONE       R    No of requested frames have been transmitted and received.
  * 0    FIRSTFRAME R    Next frame in Next frame in Receive FIFO was first
  *                      received after SSEL went active (Command frame)
  */
 volatile uint32_t status;
 /*
  * Slave Select Register
  * Specifies the slaves selected
  * Default 0 (nothing selected). Write 1 each bit to select one or more slaves.
  * Slave select output pin is active Low.
  */
 volatile uint32_t slaveSelect;
 /*
  * Transmit Data Register
  * Writing to this register writes one frame to the TX FIFO.
  * Also indicates to CoreSPI that this is the last frame
  * in this packet before SSEL is supposed to go inactive,
  * effectively allowing for the specification of the number
  * of transmitted frames.
  */
 volatile uint32_t TXEndDataRegister;

} CoreSpi_TypeDef;



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
