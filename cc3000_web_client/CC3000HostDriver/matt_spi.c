//*****************************************************************************
//
// spi.c - CC3000 Host Driver Implementation for Tiva TM4C123 devices.
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
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_ssi.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/udma.h"
#include "driverlib/debug.h"
*/

#include "hci.h"
#include "matt_spi.h"
//#include "../utils/utils.c"
#include   "../drivers/mss_uart/mss_uart.h"
#include   "../drivers/mss_gpio/mss_gpio.h"
#include   "../drivers/mss_pdma/mss_pdma.h"
#include   "../drivers/mss_spi/mss_spi.h"
#include   "../drivers/mss_timer/mss_timer.h"
#include   "../drivers/board/board.h"
#include   "../drivers/mss_timer/mss_timer.h"
#include "../CC3000HostDriver/wlan.h"
#include "../CC3000HostDriver/evnt_handler.h"
#include "../CC3000HostDriver/nvmem.h"
#include "../CC3000HostDriver/socket.h"
#include "../CC3000HostDriver/netapp.h"
#include "../CC3000HostDriver/matt_spi.h"
#include "../CC3000HostDriver/hci.h"

//*****************************************************************************
//
// The size of the DMA channel control structure we need in bytes.
//
//*****************************************************************************
#define DMA_CHANNEL_CONTROL_STRUCTURE_SIZE  (512)
#define READ                    3
#define WRITE                   1
#define HI(value)               (((value) & 0xFF00) >> 8)
#define LO(value)               ((value) & 0x00FF)
#define HEADERS_SIZE_EVNT       (SPI_HEADER_SIZE + 5)
#define SPI_HEADER_SIZE         (5)

//*****************************************************************************
//
// SPI transaction states.
//
//*****************************************************************************
#define eSPI_STATE_POWERUP              (0)
#define eSPI_STATE_INITIALIZED          (1)
#define eSPI_STATE_IDLE                 (2)
#define eSPI_STATE_WRITE_IRQ            (3)
#define eSPI_STATE_WRITE_FIRST_PORTION  (4)
#define eSPI_STATE_WRITE_EOT            (5)
#define eSPI_STATE_READ_IRQ             (6)
#define eSPI_STATE_READ_FIRST_PORTION   (7)
#define eSPI_STATE_READ_EOT             (8)

//*****************************************************************************
//
// SPI interface hardware parameters.
//
//*****************************************************************************
typedef struct
{
    uint32_t ui32PioPortAddress;
    uint32_t ui32PioSpiPort;
    uint32_t ui32PioSpiCs;
    uint32_t ui32PortInt;
    uint32_t ui32PioSlEnable;

    uint32_t ui32DMAPort;
    uint32_t ui32DMARxChannel;
    uint32_t ui32DMATxChannel;

    uint32_t ui32SsiPort;
    uint32_t ui32SsiPortAddress;
    uint32_t ui32SsiTx;
    uint32_t ui32SsiRx;
    uint32_t ui32SsiClck;
    uint32_t ui32SsiPortInt;
}tSpiHwConfiguration;

//*****************************************************************************
//
// SPI connection and transaction state information.
//
//*****************************************************************************
typedef struct
{
    tSpiHandleRx SPIRxHandler;

    uint16_t ui16TxPacketLength;
    uint16_t ui16RxPacketLength;
    volatile uint32_t ui32SpiState;
    uint8_t *pTxPacket;
    uint8_t *pRxPacket;
    uint32_t ulRxBufferSize;
    tSpiHwConfiguration sHwSettings;
}tSpiInformation;

tSpiInformation sSpiInformation;

//*****************************************************************************
//
// A couple of bytes used as a dummy source when receiving SSI data from the
// CC3000 or as a dummy destination when transmitting to the CC3000.
//
//*****************************************************************************
uint8_t g_pui8Dummy[2] = {0, 0x5A};

//*****************************************************************************
//
// Define the transmit and receive buffers along with the control structure
// table for the DMA engine.  Unfortunately, C99 doesn't provide us with a
// standard method of dictating the alignment of variables or indicating that
// an array should not be initialized so we need some compiler-specific
// syntax here.
//
//*****************************************************************************

//
// Case for Code Composer Studio.
//
/*
#if defined(__CCS__) || defined(ccs)
uint8_t wlan_rx_buffer[CC3000_RX_BUFFER_SIZE];
uint8_t wlan_tx_buffer[CC3000_TX_BUFFER_SIZE];
uint8_t chBuffer[CC3000_RX_BUFFER_SIZE];
#pragma DATA_ALIGN(ui8DMAChannelControlStructure, 1024);
static uint8_t ui8DMAChannelControlStructure[DMA_CHANNEL_CONTROL_STRUCTURE_SIZE];
*/
//
// Case for IAR Embedded Workbench for ARM (ewarm).
//
/*
#elif defined(__IAR_SYSTEMS_ICC__) || defined(ewarm)
__no_init uint8_t wlan_rx_buffer[CC3000_RX_BUFFER_SIZE];
__no_init uint8_t wlan_tx_buffer[CC3000_TX_BUFFER_SIZE];
__no_init uint8_t chBuffer[CC3000_RX_BUFFER_SIZE];
#pragma data_alignment=1024
__no_init static uint8_t ui8DMAChannelControlStructure[DMA_CHANNEL_CONTROL_STRUCTURE_SIZE];
*/
//
// Case for Sourcery CodeBench, GCC, and Keil RVMDK.
//
//#else
uint8_t wlan_rx_buffer[CC3000_RX_BUFFER_SIZE];
uint8_t wlan_tx_buffer[CC3000_TX_BUFFER_SIZE];
uint8_t chBuffer[CC3000_RX_BUFFER_SIZE];
static uint8_t ui8DMAChannelControlStructure[DMA_CHANNEL_CONTROL_STRUCTURE_SIZE] __attribute__ ((aligned(1024)));
//#endif

//*****************************************************************************
//
// An array containing the 10 bytes we need to send to the CC3000 when reading
// a response header from it.
//
//*****************************************************************************
uint8_t tSpiReadHeader[] = {READ, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//*****************************************************************************
//
// Internal function prototypes.
//
//*****************************************************************************
static void SpiWriteDataSynchronous(const uint8_t *data, uint16_t size);
static void SpiWriteAsync(const uint8_t *data, uint16_t size);
static void SpiReadData(uint8_t *data, uint16_t size);
static void SpiDisableInterrupts(void);

bool SpiBusy()
{
   //how do we know if Spi is busy?
	//delay(200);
 return(!MSS_SPI_tx_done( &g_mss_spi1));
	//return(0);

}

//*****************************************************************************
//
// Initialize the hardware settings for the SPI and IRQ interface between
// the TM4C and CC3000.  The labels used here can be found in the relevant
// version of board.h for the intended target board.
//
//*****************************************************************************
void
SpiConfigureHwMapping(void)
{
}

//*****************************************************************************
//
// Read and clear the status of the interrupt from the CC3000 to smartfusion.
//
//*****************************************************************************
uint32_t
SpiCleanGPIOISR(void)
{
	// called from CC3000 GPIO interrupt handler
    // unlike TI, there is no status, so we just return 0;
    MSS_GPIO_clear_irq(SPI_IRQ_PIN);
    return 0;

}

//*****************************************************************************
//
// Configure the TM4C SPI interface connecting to the CC3000 and the DMA
// channels used to send and receive data.  ui32SSIFreq is the desired SSI
// clock rate in Hertz and ui32SysClck is the current system clock rate, also
// in Hertz.
//
//*****************************************************************************
void
SSIConfigure(uint32_t ui32SSIFreq, uint32_t ui32SysClck)
{
	MSS_SPI_init( &g_mss_spi1 );

	// I think this is all right. I'm not sure if we want SPI_MODE2 or SPI_MODE1 though.
	// Actually looking at http://www.totalphase.com/support/articles/200349236-SPI-Background I think mode 1 is right
	MSS_SPI_configure_master_mode(&g_mss_spi1, MSS_SPI_SLAVE_0, MSS_SPI_MODE1, MSS_SPI_PCLK_DIV_64, MSS_SPI_BLOCK_TRANSFER_FRAME_SIZE);

	MSS_SPI_enable( &g_mss_spi1 );

	// enable peripheral
	PDMA_init();

	// Transmit channel
	PDMA_configure
	(
	 SPI_UDMA_TX_CHANNEL,
	 PDMA_TO_SPI_1,
	 PDMA_LOW_PRIORITY | PDMA_BYTE_TRANSFER | PDMA_INC_SRC_ONE_BYTE,
	 PDMA_DEFAULT_WRITE_ADJ
	);

	// Receive channel
	PDMA_configure
	(
	 SPI_UDMA_RX_CHANNEL,
	 PDMA_FROM_SPI_1,
	 PDMA_LOW_PRIORITY | PDMA_BYTE_TRANSFER | PDMA_INC_DEST_ONE_BYTE,
	 PDMA_DEFAULT_WRITE_ADJ
	);

	PDMA_set_irq_handler( SPI_UDMA_RX_CHANNEL, dma_rx_done_irq_handler );
	PDMA_set_irq_handler( SPI_UDMA_TX_CHANNEL, dma_tx_done_irq_handler ); //needed?
	PDMA_enable_irq(SPI_UDMA_RX_CHANNEL);
	PDMA_enable_irq(SPI_UDMA_TX_CHANNEL); //needed?
}

void dma_tx_done_irq_handler()
{
	SpiIntHandler(true, false);
}
void dma_rx_done_irq_handler()
{
	SpiIntHandler(false, true);
}

//*****************************************************************************
//
// Close the SPI connection to the CC3000 by disabling all interrupts used in
// the communication.
//
//*****************************************************************************
void
SpiClose(void)
{
    if (sSpiInformation.pRxPacket)
    {
        sSpiInformation.pRxPacket = 0;
    }

    //  Disable the interrupt from the IRQ GPIO input.
    WlanInterruptDisable();

    NVIC_DisableIRQ(SPI1_IRQn);
}

//*****************************************************************************
//
// Open the SPI driver.  Before calling this function, it is assumed that the
// SPI peripheral has been configured via a call to init_spi().  On exit,
// interrupts from SPI and the CC3000 IRQ GPIO pin are enabled.
//
//*****************************************************************************
void
SpiOpen(tSpiHandleRx pfnRxHandler)
{
    //
    // Parameter sanity check.
    //
    ASSERT(pfnRxHandler);

    //
    // Set the initial state to indicate that we need to wait for the power-up
    // handshake from the CC3000.
    //
    sSpiInformation.ui32SpiState = eSPI_STATE_POWERUP;

    //
    // Remember our receive callback handler function pointer.
    //
    sSpiInformation.SPIRxHandler = pfnRxHandler;

    //
    // Initialize buffer-related fields.
    //
    sSpiInformation.ui16TxPacketLength = 0;
    sSpiInformation.pTxPacket = NULL;
    sSpiInformation.pRxPacket = wlan_rx_buffer;
    sSpiInformation.ui16RxPacketLength = 0;
    sSpiInformation.ulRxBufferSize = CC3000_RX_BUFFER_SIZE;

    //
    // Enable the interrupt from the CC3000 IRQ.
    //
    tSLInformation.WlanInterruptEnable();

    // Enable the IRQ and SPI interrupts in the NVIC.
    NVIC_EnableIRQ(SPI1_IRQn);
    MSS_GPIO_enable_irq(SPI_IRQ_PIN);
}

//*****************************************************************************
//
// Configure the SPI peripheral to use the frequency specified in ui32SSIFreq
// and set up pin muxing as required for the SPI signals and CC3000 IRQ GPIO.
//
//*****************************************************************************
int init_spi(uint32_t ui32SSIFreq, uint32_t ui32SysClck)
{

    // Configure the hardware to use the required SPI and GPIO peripherals.
    SpiConfigureHwMapping();

    // Set SPI interface configuration parameters.
    SSIConfigure(ui32SSIFreq, ui32SysClck);

    return(ESUCCESS);
}

//*****************************************************************************
//
// Returns the transfer mode of the specified DNA channel.  This will be
// one of UDMA_MODE_STOP if the channel is stopped or UDMA_MODE_BASIC if a
// transfer is ongoing on the channel.
//
//*****************************************************************************
static uint32_t
SpiCheckDMAStatus(uint32_t ui32Channel)
{
	// experimental, since I don't know what UDMA_MODE_STOP|DMA_MODE_BASIC
	// are defined as...
	return PDMA_status(ui32Channel);
}

//*****************************************************************************
//
// Determine whether or not a DMA transaction on a given channel is currently
// ongoing.  The function returns false if the DMA channel is enabled and,
// hence, the transaction is still ongoing, or true if the channel is disabled
// and no transaction is in progress.
//
//*****************************************************************************
static bool
SpiIsDMAStopped(uint32_t ui32Channel)
{
	// experimental
	uint32_t BOTH_CHANNELS_COMPLETE = 0x0002;
    return(BOTH_CHANNELS_COMPLETE & PDMA_status(ui32Channel));
}

//*****************************************************************************
//
// Flush any data which may exist in the SPI peripheral's receive FIFO.
//
//*****************************************************************************
static void
SpiFlushRxFifo(void)
{
	// TODO call MSS_SPI_transfer_frame() (which reads and writes) to get all the data
    uint32_t ui32Idx;

    //while(MAP_SSIDataGetNonBlocking(sSpiInformation.sHwSettings.ui32SsiPort,
    //                                &ui32Idx))
    //{
        // Keep reading until there's no data left to read.
    //}

    //Clear the RX overrun interrupt flag.
    //TODO: is there an overrun interrupt we have access to?
    //SSIIntClear(sSpiInformation.sHwSettings.ui32SsiPort, SSI_RIS_RORRIS);
}

//*****************************************************************************
//
// This function is called to write the first SPI packet to the CC3000 after
// it is enabled.  This packet uses a somewhat different protocol from all
// others, requiring delays be inserted after asserting CS and between the 4th
// and 5th bytes of the packet. Also, the IRQ line from the CC3000 is not used
// as a signal to start transmission in this case.
//
// pui8Buf points to the data to be sent to the CC3000 and ui16Length is the
// number of bytes to send.
//
// The function always returns 0.
//
//*****************************************************************************
uint32_t
SpiFirstWrite(uint8_t *ui8Buf, uint16_t ui16Length)
{
    // Assert the chip select to CC3000.
    ASSERT_CS();

    // Wait 80 microseconds or so.
    delay((DELAY_50_MICROSECONDS * 8) / 5);

    // Write the first 4 bytes of the packet we have been provided.
    SpiWriteDataSynchronous(ui8Buf, 4);

    // Wait for the transmission to complete.
    while(SpiBusy());

    // Generate an 80 microsecond gap between the last byte sent and the
    // remainder of the packet.
    delay((DELAY_50_MICROSECONDS * 8) / 5);

    // Send the remaining bytes in the packet.
    SpiWriteDataSynchronous(ui8Buf + 4, ui16Length - 4);

    // We're done.  Switch the state to IDLE so that future transmissions
    // follow the usual convention of waiting for IRQ to become active before
    // starting to send data.
    sSpiInformation.ui32SpiState = eSPI_STATE_IDLE;

    // Wait for the remainder of the packet transmission to complete.
    while(SpiBusy());

    // Pull CS high again to indicate the end of the packet.
    DEASSERT_CS();

    return(0);
}

//*****************************************************************************
//
// Write a packet to the CC3000 via SPI.
//
// This function writes the buffer identified by pui8UserBuffer and ui16Length
// to the CC3000 via the configured SPI connection.  The buffer passed is
// assumed to contain an uninitialized, 5 byte SPI header prior to the
// application- provided packet data.
//
// pui8UserBuffer must contain an even number of bytes. If the supplied packet
// length is odd, an additional byte of padding is added to the end of the
// transmitted packet. pui8UserBuffer must, therefore, allow the driver to read
// 1 byte past the length supplied in ui16Length if this value is odd.
//
// Packets are written synchronously - this function does not return until all
// packet data has been sent.  Remember that packet transmission involves a
// handshake between the CS signal and the IRQ from CC3000 so we don't actually
// start transmission inside this function except in the case of the very first
// write which is handled differently.  For all packets other than the first,
// transmission actually starts in the GPIO interrupt handler for the CC3000
// IRQ line, IntSpiGPIOHandler().
//
//*****************************************************************************
long
SpiWrite(uint8_t *pui8UserBuffer, uint16_t ui16Length)
{
	printf("SpiWrite()\r\n");

    uint8_t ui8Pad = 0;

    // Parameter sanity check.
    ASSERT(pui8UserBuffer);
    ASSERT(ui16Length);


    // Figure out the total length of the packet in order to determine if
    // there is padding or not.
    if(!(ui16Length & 0x0001))
    {
        ui8Pad++;
    }

    pui8UserBuffer[0] = WRITE;
    pui8UserBuffer[1] = HI(ui16Length + ui8Pad);
    pui8UserBuffer[2] = LO(ui16Length + ui8Pad);
    pui8UserBuffer[3] = 0;
    pui8UserBuffer[4] = 0;

    ui16Length += (SPI_HEADER_SIZE + ui8Pad);

    if (sSpiInformation.ui32SpiState == eSPI_STATE_POWERUP)
    {

        while (sSpiInformation.ui32SpiState != eSPI_STATE_INITIALIZED);
        {

        }

    }


    if (sSpiInformation.ui32SpiState == eSPI_STATE_INITIALIZED)
    {
    	printf("First spi write\n\r");
        // This is the first SPI transmission so we need to use special timing.
        SpiFirstWrite(pui8UserBuffer, ui16Length);
    }
    else
    {
        // We need wait until the device is idle to prevent us from trying to
        // transmit a packet when a packet receive may be ongoing.
        while (sSpiInformation.ui32SpiState != eSPI_STATE_IDLE); // spin

        sSpiInformation.ui32SpiState = eSPI_STATE_WRITE_IRQ;
        sSpiInformation.pTxPacket = pui8UserBuffer;
        sSpiInformation.ui16TxPacketLength = ui16Length;

        // Assert the CS line and wait till SSI IRQ line is active and then
        // initialize write operation.
        ASSERT_CS();
    }


    // Wait for the transaction to complete before returning.
    while (eSPI_STATE_IDLE != sSpiInformation.ui32SpiState); // kill time


    return(0);
}

//*****************************************************************************
//
// Disable the SSI receive and transmit DMA channels.
//
//*****************************************************************************
void
SpiDisableSSIDMAChannels()
{
	PDMA_disable_irq(SPI_UDMA_RX_CHANNEL);
	PDMA_disable_irq(SPI_UDMA_TX_CHANNEL);
	PDMA_pause(SPI_UDMA_RX_CHANNEL);
	PDMA_pause(SPI_UDMA_TX_CHANNEL);
}

//*****************************************************************************
//
// Enable the SSI receive and transmit DMA channels.
//
//*****************************************************************************
void
SpiEnableSSIDMAChannels()
{
	PDMA_enable_irq(SPI_UDMA_RX_CHANNEL);
	PDMA_enable_irq(SPI_UDMA_TX_CHANNEL);
	PDMA_resume(SPI_UDMA_RX_CHANNEL);
	PDMA_resume(SPI_UDMA_TX_CHANNEL);
}

//*****************************************************************************
//
// Schedule a single SPI receive DMA transaction (which requires a partner
// transmit transaction because, as the master, we drive the SPU clock).
// It is assumed that the size passed to this function has already been checked
// to ensure that it is smaller than the maximum size of a single transaction
// and also that the buffer passed via the data parameter has sufficient
// space to hold an additional "size" bytes of data.
//
// This function is asynchronous and returns immediately the DMA transactions
// have been enabled.  It does not block until the read has completed.
// Completion is indicated via the SSI uDMA interrupt.
//
//*****************************************************************************
static void
SpiReadData(uint8_t *pui8Data, uint16_t ui16Size)
{

	MSS_SPI_disable(&g_mss_spi1);
    // Disable both the transmit and receive DMA channels before we mess with
    // their setup.
    SpiDisableSSIDMAChannels();

    // Configure TX to generate the necessary clocks for read from the slave.
    // If we've been asked for 10 bytes or less, we set the transmit channel
    // to send a read header.  If more than 10 bytes, we send constant READ
    // values.
    if(ui16Size <= sizeof(tSpiReadHeader))
    {
    	PDMA_configure
    	(
    	 SPI_UDMA_TX_CHANNEL,
    	 PDMA_TO_SPI_1,
    	 PDMA_LOW_PRIORITY | PDMA_BYTE_TRANSFER | PDMA_INC_SRC_ONE_BYTE,
    	 PDMA_DEFAULT_WRITE_ADJ
    	);

    }
    else
    {
        // We've been asked for more bytes than a single read header so
        // just send the first byte repeatedly.
    	PDMA_configure
    	(
    	 SPI_UDMA_TX_CHANNEL,
    	 PDMA_TO_SPI_1,
    	 PDMA_LOW_PRIORITY | PDMA_BYTE_TRANSFER,
    	 PDMA_DEFAULT_WRITE_ADJ
    	);
    }

    PDMA_start(SPI_UDMA_TX_CHANNEL,PDMA_SPI1_TX_REGISTER,(uint32_t)tSpiReadHeader,ui16Size);

    // Configure RX to read the required number of bytes.
	PDMA_configure
	(
	 SPI_UDMA_RX_CHANNEL,
	 PDMA_FROM_SPI_1,
	 PDMA_LOW_PRIORITY | PDMA_BYTE_TRANSFER,
	 PDMA_DEFAULT_WRITE_ADJ
	);

	PDMA_start(SPI_UDMA_RX_CHANNEL,PDMA_SPI1_RX_REGISTER,(uint32_t)pui8Data,ui16Size);

    // Clean out any stale data that may be in the SSI receive FIFO.
    SpiFlushRxFifo();

    MSS_SPI_set_transfer_byte_count(&g_mss_spi1, ui16Size);

    // Enable the SSI transmit and receive DMA channels.
	MSS_SPI_enable(&g_mss_spi1);

    // Enable both the RX and TX channels here.  The transmit is also needed
    // here to ensure that the master (us) generates the clocks required to
    // read the required data from the slave.
    SpiEnableSSIDMAChannels();
}

//*****************************************************************************
//
// Schedule a single SPI transmit DMA transaction (which requires a partner
// receive transaction to keep the receive FIFO flushed).  If the size passed
// is larger than can be handled in a single DMA transaction, the transaction
// is split into pieces and the first piece is scheduled here.
//
// This function is asynchronous and returns immediately the DMA transactions
// have been enabled.  It does not block until the transmit has completed.
// Completion of a transaction is indicated via the SSI uDMA interrupt and
// the driver state machine handles scheduling further pieces of a multi-
// transfer write.
//
//*****************************************************************************
static void
SpiWriteAsync(const uint8_t *pui8Data, uint16_t ui16Size)
{

	MSS_SPI_disable(&g_mss_spi1);

	// The DMA channels must be disabled.
    //SpiDisableSSIDMAChannels();


    // The TI uDMA controller can only transfer 1024 bytes in a single transaction
    // when using basic mode.  If asked for more than 1024 bytes, we split the
    // transaction into two.
    if (ui16Size <= SPI_WINDOW_SIZE)
    {
        // We can handle this request in a single DMA transaction.
        sSpiInformation.ui32SpiState = eSPI_STATE_WRITE_EOT;
    }
    else
    {
        // Remember that we've split this into two DMA transactions and set
        // up to send the first part.
        sSpiInformation.ui32SpiState = eSPI_STATE_WRITE_FIRST_PORTION;

        // Truncate the packet to the maximum size for now.
        ui16Size = SPI_WINDOW_SIZE;
    }

    // Configure:

	MSS_SPI_set_transfer_byte_count(&g_mss_spi1, ui16Size);

	// Transmit channel
	PDMA_configure
	(
	 SPI_UDMA_TX_CHANNEL,
	 PDMA_TO_SPI_1,
	 PDMA_LOW_PRIORITY | PDMA_BYTE_TRANSFER | PDMA_INC_SRC_ONE_BYTE,
	 PDMA_DEFAULT_WRITE_ADJ
	);

	// Set up a dummy RX DMA to keep the receive FIFO flushed.
	// Receive channel
	PDMA_configure
	(
	 SPI_UDMA_RX_CHANNEL,
	 PDMA_FROM_SPI_1,
	 PDMA_LOW_PRIORITY | PDMA_BYTE_TRANSFER,
	 PDMA_DEFAULT_WRITE_ADJ
	);


	//printf("PDMA_start RX\r\n");
	PDMA_start(SPI_UDMA_RX_CHANNEL,PDMA_SPI1_RX_REGISTER,(uint32_t)g_pui8Dummy,16);
	//printf("PDMA_start TX\r\n");
	PDMA_start(SPI_UDMA_TX_CHANNEL,PDMA_SPI1_TX_REGISTER,(uint32_t)pui8Data,ui16Size);

    //printf ("MSS_SPI_ENABLE\r\n");
    // Enable the SSI transmit and receive DMA channels.

    SpiEnableSSIDMAChannels();
	MSS_SPI_enable(&g_mss_spi1);
    while ( !MSS_SPI_tx_done(&g_mss_spi1))
     {
         ;
     }
}

//*****************************************************************************
//
// This function, used during early initialization of the CC3000,  transmits a
// single packet via SPI and waits for the transmission to complete.  It is
// assumed that the packet to be transmitted is 1024 bytes or less in size and
// can be accommodated in a single DMA transaction.  On completion, the SSI
// EOT (used during normal operation to signal completion of a transmission)
// interrupt is left enabled.
//
//*****************************************************************************
static void
SpiWriteDataSynchronous(const uint8_t *pui8Data, uint16_t ui16Size)
{
    // This is a synchronous, polled write so we disable all SSI-related interrupts.
	NVIC_DisableIRQ(SPI1_IRQn);

    // Perform the write.
    SpiWriteAsync(pui8Data, ui16Size);

    // Wait for the DMA transmit transaction to complete.
	while (!MSS_SPI_tx_done( &g_mss_spi1));

    // Clear pending SSI DMA interrupts in the NVIC.
    // IntPendClear(INT_SPI); // No such thing, right?
    // Why?  Should it be only TX or only Rx?
    PDMA_clear_irq(SPI_UDMA_TX_CHANNEL);
    PDMA_clear_irq(SPI_UDMA_RX_CHANNEL);

    // Re-enable the SSI interrupt
    NVIC_EnableIRQ(SPI1_IRQn);
}

//*****************************************************************************
//
// This function is called to start reception of data from the CC3000.  It
// schedules reception of 10 bytes from the device.  This contains the packet
// header and 5 bytes of payload data which is sufficient to determine the
// type of packet being received and the length of the remaining payload data.
//
// The function schedules reception and enables the DMA transactions then
// returns immediately.  Reception of the remainder of the packet is handled
// in the SPI interrupt handler when the header reception completes.
//
//*****************************************************************************
void
SpiReadHeader(void)
{
    sSpiInformation.ui32SpiState = eSPI_STATE_READ_IRQ;

    SpiReadData(sSpiInformation.pRxPacket, 10);
}

//*****************************************************************************
//
// Parse a previously-received packet header and, based on the content,
// schedule a further DMA transaction to receive the remainder of the bytes
// that the CC3000 is sending.  This function is called when the DMA
// transaction initiated by SpiReadHeader() completes.
//
// This function returns the number of bytes that are scheduled to be received
// in the next DMA transaction.  Note that, if the packet payload is larger
// than can be accommodated in a single DMA transaction, this value may be
// less than the payload size.
//
// It is assumed that the receive buffer at sSpiInformation.pRxPacket contains
// the first 10 bytes of an incoming packet before this function is called.
// These 10 bytes contain the SPI packet header and the HCI header.
//
// In cases where the received header contains a payload length larger than the
// available receive buffer size, the function does not schedule another DMA
// transaction but returns 0 and sets *pbError to true to indicate a problem.
//
//*****************************************************************************
long
SpiReadDataCont(bool *pbError)
{
    uint32_t ui32Count;
    uint8_t *pui8Buff;
    uint8_t ui8Type;

    // Parameter sanity check.
    ASSERT(pbError);


    // Assume no error until we determine otherwise.
    *pbError = false;


    // Determine what type of packet we have
    pui8Buff =  sSpiInformation.pRxPacket;
    ui32Count = 0;
    STREAM_TO_UINT8((char *)(pui8Buff + SPI_HEADER_SIZE),
                    HCI_PACKET_TYPE_OFFSET, ui8Type);


    // What type of packet have we received?
    switch(ui8Type)
    {

        // The packet header indicates that this is a data packet.
        case HCI_TYPE_DATA:
        {

            // Determine the length of the data packet payload.  For a data
            // packet, the length is two bytes long allowing payloads of up
            // to 65535 bytes (although this implementation doesn't use
            // packets that long).
            STREAM_TO_UINT16((char *)(pui8Buff + SPI_HEADER_SIZE),
                             HCI_DATA_LENGTH_OFFSET, ui32Count);


            // Is this payload too long to be received into the buffer that
            // we have available to us?  If so, this indicates a software
            // error or a corruption in the received packet.  Either way, we
            // don't bother trying to receive the reset of the packet.  Note
            // that we need to round the packet size to the nearest higher
            // multiple of two because we've not considered padding yet.
            if(((ui32Count + HCI_DATA_HEADER_SIZE +
                 SPI_HEADER_SIZE + 1) & ~1) >
               sSpiInformation.ulRxBufferSize)
            {

                // This must be a corrupted packet because the length we parsed
                // out of the header is larger than our receive buffer.
                *pbError = true;
                return(0);
            }


            // Can we get the whole payload in a single DMA transaction?
            if (ui32Count >= SPI_WINDOW_SIZE)
            {

                // No - receive as much as we can in the next transaction.
                SpiReadData(pui8Buff + 10, SPI_WINDOW_SIZE);
                sSpiInformation.ui32SpiState = eSPI_STATE_READ_FIRST_PORTION;
            }
            else
            {
                // Yes - the amount we have to read is strictly less than the
                // maximum DMA transaction size so we can read the whole
                // payload in one transaction, even if we need an additional
                // padding byte.

                // Do we need that padding byte? Note that the payload length
                // is 2 bytes in a data packet but only one in an event packet
                // so the length we need to read is actually the length parsed
                // from the packet - we haven't already read the first payload
                // byte as we have in the (following) event case.  We've
                // already read 10 bytes so, to keep the whole packet an even
                // number of bytes, we need to read an even number of payload
                // bytes.
                if (!((HEADERS_SIZE_EVNT + ui32Count) & 1))
                {
                    ui32Count++;
                }

                if (ui32Count)
                {
                    SpiReadData(pui8Buff + 10, ui32Count);
                }

                sSpiInformation.ui32SpiState = eSPI_STATE_READ_EOT;
            }
            break;
        }


        // The packet header indicates that this is an event packet.
        case HCI_TYPE_EVNT:
        {

            // Determine the length of the event packet payload.  This is a
            // single byte in the event packet header.
            STREAM_TO_UINT8((char *)(pui8Buff + SPI_HEADER_SIZE),
                            HCI_EVENT_LENGTH_OFFSET, ui32Count);


            // Subtract one because we already read the first payload byte (or
            // a padding byte) when we read the initial 10 bytes.
            ui32Count -= 1;


            // Add a padding byte if needed.  All packets contain an even
            // number of bytes.
            if ((HEADERS_SIZE_EVNT + ui32Count) & 1)
            {
                ui32Count++;
            }


            // Is this payload too long to be received into the buffer that
            // we have available to us?  If so, this indicates a software
            // error or corruption in the received header.  Either way, we
            // don't bother trying to receive the reset of the packet.
            if((ui32Count + HEADERS_SIZE_EVNT) >
               sSpiInformation.ulRxBufferSize)
            {

                // This must be a corrupted packet because the length we parsed
                // out of the header is larger than our receive buffer.
                *pbError = true;
                return(0);
            }


            // If there is any more data to receive, schedule another DMA
            // transaction.

            if(ui32Count)
            {
                SpiReadData(pui8Buff + 10, ui32Count);
            }

            sSpiInformation.ui32SpiState = eSPI_STATE_READ_EOT;
            break;
        }
        // The packet type is unrecognized so don't receive anything more.
        default:
        {
            *pbError = true;
            return(0);
        }
    }

    // If we get here, we've scheduled a new DMA transaction to read the
    // remainder of the packet.  Tell the caller how many bytes they should
    // expect when that transaction completes.
    return (ui32Count);
}

//*****************************************************************************
//
// Disable all interrupts associated with the SPI interface and the IRQ line.
//
//*****************************************************************************
static void
SpiDisableInterrupts(void)
{
    MSS_GPIO_disable_irq(SPI_IRQ_PIN);
    NVIC_DisableIRQ(SPI1_IRQn);
}

//*****************************************************************************
//
// Re-enable the interrupts associated with the SPI interface and IRQ line.
//
//*****************************************************************************
void
SpiResumeSpi(void)
{
	MSS_GPIO_enable_irq(SPI_IRQ_PIN);
	NVIC_EnableIRQ(SPI1_IRQn);
}

//*****************************************************************************
//
// A packet has been received.  Clean up, deasserting CS after the last bit has
// cleared the transmitter (remember, we need to transmit dummy data during
// receipt of a packet because we're the master and, hence, drive the SPI
// clock), then, if the packet isn't marked as bad, pass it up the stack for
// further processing.
//
//*****************************************************************************
void
SpiTriggerRxProcessing(bool bBadPacket)
{
    // Tidy up at the end of receiving a packet.
    SpiDisableInterrupts();

    //TODO: wait while spi is busy
    while(SpiBusy());

    DEASSERT_CS();

    MSS_SPI_disable(&g_mss_spi1);
    //
    // Move back to idle state so that we are ready to send or receive another
    // packet.
    //
    sSpiInformation.ui32SpiState = eSPI_STATE_IDLE;

    //
    // If this packet isn't marked as bad, pass it up the stack for processing.
    //
    if(!bBadPacket)
    {
        sSpiInformation.SPIRxHandler(sSpiInformation.pRxPacket +
                                     SPI_HEADER_SIZE);
    }
}

//*****************************************************************************
//
// This function is called whenever a SPI DMA transaction completes during
// reception of a packet from the CC3000.  It schedules the next DMA
// transaction if one is required to receive additional packet data then, once
// no more data is to be read, passes the completed packet for processing via
// SpiTriggerRxProcessing().
//
//*****************************************************************************
static void
SpiContReadOperation(void)
{
    bool bError;

    //
    // Parse the header we already received then receive the remainder of the
    // packet.  This function returns 0 when the packet is complete or when
    // reception is aborted due to a packet error.  In cases of bad packets,
    // bError will be "true", otherwise it will be "false".
    //
    if (!SpiReadDataCont(&bError))
    {
        //
        // The packet was received successfully so pass it up the stack
        // for processing.
        //
        SpiTriggerRxProcessing(bError);
    }
}

//*****************************************************************************
//
// The interrupt handler for the GPIO input which is connected to the CC3000
// IRQ signal.  This line is used to signal asynchronous reception of data
// from the CC3000 or as a handshake for the CS line when sending commands to
// the CC3000.
//
// When the IRQ is asserted (pulled low) with CS high, this indicates that the
// CC3000 has information that we must read.  We assert CS and start the packet
// read here.
//
// When a command is being sent to the CC3000, we assert CS (with IRQ still
// high) then the CC3000 handshakes this by pulling IRQ low.  When we detect
// this transition, we start transmission of the command packet.
//
//*****************************************************************************
//void IntSpiGPIOHandler(void)
__attribute__((__interrupt__)) void IntSpiGPIOHandler(void)
{
	// this is the interrupt handler for SPI_IRQ_PIN (MSS_GPIO_2)

	printf("IntSpiGPIOHandler: got SPI_IRQ_PIN interrupt for");


	MSS_GPIO_clear_irq(SPI_IRQ_PIN);

	if (sSpiInformation.ui32SpiState == eSPI_STATE_POWERUP)
	{
		printf(" INIT!\r\n");
		// We received the first IRQ line edge after powering up the
		// part.  This is part of the startup sequence so change the state
		// to show that we received this.
		sSpiInformation.ui32SpiState = eSPI_STATE_INITIALIZED;
	}
	else if (sSpiInformation.ui32SpiState == eSPI_STATE_IDLE)
	{
		printf(" READ!\r\n");
		// We're idle so an IRQ interrupt indicates that the CC3000 has
		// data to send us.
		sSpiInformation.ui32SpiState = eSPI_STATE_READ_IRQ;

		// IRQ line asserted so handshake with CS and set up to
		// receive a packet header from the CC3000.
		ASSERT_CS();

		// Start receiving the 10 byte header from the CC3000.
		SpiReadHeader();
	}
	else if (sSpiInformation.ui32SpiState == eSPI_STATE_WRITE_IRQ)
	{
		printf(" WRITE!\r\n");
		// The CC3000 has responded to us lowering CS by handshaking with
		// its IRQ.  This indicates that we can now start transmitting the
		// command packet that we're waiting to send.
		SpiWriteAsync(sSpiInformation.pTxPacket,
					  sSpiInformation.ui16TxPacketLength);
	}
}

//*****************************************************************************
//
// The SPI peripheral interrupt handler.  This is called whenever a transmit
// or receive DMA transaction involving the SPI peripheral completes.  Note
// that, on TM4C123 parts, DMA completion is signalled using the peripheral
// interrupt but the peripheral interrupt status register is not involved and
// does not contain status bits indicating DMA interrupts.
//
//*****************************************************************************
void
SpiIntHandler(bool bTxFinished, bool bRxFinished)
{
    uint16_t ui16Count;
    uint8_t *pui8Buff;

    pui8Buff =  sSpiInformation.pRxPacket;
    ui16Count = 0;

    if (sSpiInformation.ui32SpiState == eSPI_STATE_READ_IRQ)
    {
        // If both DMA transactions have completed...
        if (bTxFinished && bRxFinished)
        {
            // Clear any pending SSI interrupt which can be caused by the other
            // DMA transaction completing.
            NVIC_ClearPendingIRQ(SPI1_IRQn);


            // Parse the packet header data received and receive the remainder
            // of the response.
            SpiContReadOperation();
        }
    }
    // Have we just read the first portion of a packet that is too long for
    // a single DMA transaction and which has to be split into one or more
    // sections?
    else if (sSpiInformation.ui32SpiState == eSPI_STATE_READ_FIRST_PORTION)
    {
        // Did the receive DMA transaction complete?
        if (bRxFinished)
        {
            // Clean up any additional interrupts pending at the NVIC.
            NVIC_ClearPendingIRQ(SPI1_IRQn);
            // Get the size of the packet payload from the header.  We assume
            // that this must be a data packet because an event packet cannot
            // have a payload longer than 255 bytes and, therefore, can always
            // be accommodated in a single DMA transaction.
            STREAM_TO_UINT16((char *)(pui8Buff + SPI_HEADER_SIZE),
                              HCI_DATA_LENGTH_OFFSET, ui16Count);

            // We have already read one maximum-DMA-sized chunk of the data
            // so subtract this from the payload length to get the number of
            // bytes we still need to read.
            ui16Count -=SPI_WINDOW_SIZE;

            // Pad the count of bytes to receive so that the incoming packet
            // always contains an odd number of bytes.
            if (!((HEADERS_SIZE_EVNT + ui16Count) & 1))
            {
                ui16Count++;
            }

            // Schedule a new DMA transaction to receive the remaining bytes
            // in the packet.  The assumption here is that no packet can be
            // longer than (10 + (2* SPI_WINDOW_SIZE)) bytes.
            SpiReadData(sSpiInformation.pRxPacket + 10 + SPI_WINDOW_SIZE,
                        ui16Count);

            // Remember that we're waiting for the last portion of the packet
            // to be read.
            sSpiInformation.ui32SpiState = eSPI_STATE_READ_EOT;
        }
    }
    // Were we waiting for the last part of a received packet to arrive?
    else if (sSpiInformation.ui32SpiState == eSPI_STATE_READ_EOT)
    {
        // Did the receive DMA complete?
        if (bRxFinished)
        {
            // Yes - clean up any SSI interrupt that may be pending at the NVIC.
            NVIC_ClearPendingIRQ(SPI1_IRQn);

            // Process the received packet.
            SpiTriggerRxProcessing(false);
        }
    }
    // Were we waiting for the last part of a transmitted packet to be sent?
    else if (sSpiInformation.ui32SpiState == eSPI_STATE_WRITE_EOT)
    {
        // Did the transmit DMA complete?
        if (bTxFinished)
        {
            // Wait for the last bit of the transmission to make it onto the
            // wire.
            while(SpiBusy());

            // Raise CS to indicate that the packet has ended.
            DEASSERT_CS();

            // Clean out any stray data that may exit in the receive FIFO.
            SpiFlushRxFifo();

            // We're finished with this transaction so go back to idle.
            sSpiInformation.ui32SpiState = eSPI_STATE_IDLE;
        }
    }
    //
    // Have we just sent the first part of a large transmit packet?
    //
    else if (sSpiInformation.ui32SpiState == eSPI_STATE_WRITE_FIRST_PORTION)
    {
        // Are we being notified that the transmit DMA completed?
        if (bTxFinished)
        {
            // Yes - send the remainder of the packet.  The assumption, again,
            // is that no packet can be more than (10 + (2 * SPI_WINDOW_SIZE))
            // bytes long.
            SpiFlushRxFifo();

            sSpiInformation.ui32SpiState = eSPI_STATE_WRITE_EOT;

            SpiWriteAsync(sSpiInformation.pTxPacket + SPI_WINDOW_SIZE,
                          sSpiInformation.ui16TxPacketLength - SPI_WINDOW_SIZE);
        }
    }
}
