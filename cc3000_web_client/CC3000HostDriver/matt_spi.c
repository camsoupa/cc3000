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

extern int after_rec;

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


#define TX_DONE_MASK            0x00000001u
#define RX_DATA_READY_MASK      0x00000002u
#define RX_OVERFLOW_MASK        0x00000004u
#define RX_FIFO_EMPTY_MASK      0x00000040u
#define TX_FIFO_FULL_MASK       0x00000100u
#define TX_FIFO_EMPTY_MASK      0x00000400u


//*****************************************************************************
//
// SPI interface hardware parameters.
//
//*****************************************************************************
/*
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
*/
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
}tSpiInformation;

tSpiInformation sSpiInformation;




//*****************************************************************************
//
// A couple of bytes used as a dummy source when receiving SSI data from the
// CC3000 or as a dummy destination when transmitting to the CC3000.
//
//*****************************************************************************
//uint8_t g_pui8Dummy[2] = {0, 0x5A};

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
//uint8_t wlan_rx_buffer[CC3000_RX_BUFFER_SIZE];
//uint8_t wlan_tx_buffer[CC3000_TX_BUFFER_SIZE];
//uint8_t chBuffer[CC3000_RX_BUFFER_SIZE];
//static uint8_t ui8DMAChannelControlStructure[DMA_CHANNEL_CONTROL_STRUCTURE_SIZE] __attribute__ ((aligned(1024)));
//#endif

//*****************************************************************************
//
// An array containing the 10 bytes we need to send to the CC3000 when reading
// a response header from it.
//
//*****************************************************************************
//uint8_t tSpiReadHeader[] = {READ, READ, READ, READ, READ, READ, READ, READ, READ, READ};

//uint8_t tSpiReadHeader[100] = {READ};

//uint8_t tSpiReadHeader[100] = { [ 0 ... 99 ] = READ };
uint8_t tSpiReadHeader[100] = { [ 0 ] = READ, [1 ... 99] = 0 };

//
// Static buffer for 5 bytes of SPI HEADER
//
//unsigned char tSpiReadHeader[] = {READ, 0, 0, 0, 0}; //TODO is it this one or the one above?


//*****************************************************************************
//
// Internal function prototypes.
//
//*****************************************************************************
static void SpiWriteDataSynchronous(const uint8_t *data, uint16_t size);
//static void SpiReadData(uint8_t *data, uint16_t size);
void SpiPauseSpi(void);
void SpiResumeSpi(void);
static void SpiContReadOperation(void);

//static void SpiDisableInterrupts(void);

#define CC3000_BUFFER_MAGIC_NUMBER (0xDE)

uint8_t spi_buffer[CC3000_RX_BUFFER_SIZE];
uint8_t wlan_tx_buffer[CC3000_TX_BUFFER_SIZE];



//*****************************************************************************
//
// Read and clear the status of the interrupt from the CC3000 to smartfusion.
//
//*****************************************************************************
uint32_t
SpiCleanGPIOISR(void)
{
#ifdef VERBOSE
    printf("SpiCleanGPIOISR: clear SPI_IRQ int\r\n");
#endif
    MSS_GPIO_clear_irq(SPI_IRQ_PIN);  // TODO: get a status from this? Not used anywhere

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
#ifdef VERBOSE
 printf("EMPTY\r\n");
#endif


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
#ifdef VERBOSE
    printf("SpiClose: Closing spi\r\n");
#endif
    if (sSpiInformation.pRxPacket)
    {
        sSpiInformation.pRxPacket = 0;
    }

    //  Disable the interrupt from the IRQ GPIO input.
    tSLInformation.WlanInterruptDisable();

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
#ifdef VERBOSE
    printf("SpiOpen: setting handler... check this?\r\n");
#endif


    //
    // Set the initial state to indicate that we need to wait for the power-up
    // handshake from the CC3000.
    //
    sSpiInformation.ui32SpiState = eSPI_STATE_POWERUP;

    memset(spi_buffer, 0, sizeof(spi_buffer));
    memset(wlan_tx_buffer, 0, sizeof(spi_buffer));

    //
    // Remember our receive callback handler function pointer.
    //
    sSpiInformation.SPIRxHandler = pfnRxHandler;

    //
    // Initialize buffer-related fields.
    //
    sSpiInformation.ui16TxPacketLength = 0;
    sSpiInformation.pTxPacket = NULL;
	sSpiInformation.pRxPacket = (unsigned char *)spi_buffer;
    sSpiInformation.ui16RxPacketLength = 0;
	spi_buffer[CC3000_RX_BUFFER_SIZE - 1] = CC3000_BUFFER_MAGIC_NUMBER;
	wlan_tx_buffer[CC3000_TX_BUFFER_SIZE - 1] = CC3000_BUFFER_MAGIC_NUMBER;

    //
    // Enable the interrupt from the CC3000 IRQ.
    //
    tSLInformation.WlanInterruptEnable();

}

//*****************************************************************************
//
// Configure the SPI peripheral to use the frequency specified in ui32SSIFreq
// and set up pin muxing as required for the SPI signals and CC3000 IRQ GPIO.
//
//*****************************************************************************
int init_spi(uint32_t ui32SSIFreq, uint32_t ui32SysClck)
{

#ifdef VERBOSE
    printf("init_spi: inti spi now\r\n");
#endif
	MSS_SPI_init( &g_mss_spi1 );

	// I think this is all right. I'm not sure if we want SPI_MODE2 or SPI_MODE1 though.
	// Actually looking at http://www.totalphase.com/support/articles/200349236-SPI-Background I think mode 1 is right

	MSS_SPI_configure_master_mode(&g_mss_spi1, MSS_SPI_SLAVE_0, MSS_SPI_MODE1, MSS_SPI_PCLK_DIV_32, MSS_SPI_BLOCK_TRANSFER_FRAME_SIZE);


    return(ESUCCESS);
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
    //printf("SpiFirstWrite\r\n");
    // Assert the chip select to CC3000.
    ASSERT_CS();

    // Wait 80 microseconds or so.
    delay(10000000);
    //delay((DELAY_50_MICROSECONDS * 8) / 5 + 100); // TODO is this enough time?

    // Write the first 4 bytes of the packet we have been provided.
    SpiWriteDataSynchronous(ui8Buf, 4);

    // Generate an 80 microsecond gap between the last byte sent and the
    // remainder of the packet.
    delay(10000000);
    //delay((DELAY_50_MICROSECONDS * 8) / 5 + 100);

    // Send the remaining bytes in the packet.
    SpiWriteDataSynchronous(ui8Buf + 4, ui16Length - 4);

    // We're done.  Switch the state to IDLE so that future transmissions
    // follow the usual convention of waiting for IRQ to become active before
    // starting to send data.
    sSpiInformation.ui32SpiState = eSPI_STATE_IDLE;

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
	//printf("SpiWrite()\r\n");

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

	if (wlan_tx_buffer[CC3000_TX_BUFFER_SIZE - 1] != CC3000_BUFFER_MAGIC_NUMBER)
	{
		while (1)
			;
	}

    if (sSpiInformation.ui32SpiState == eSPI_STATE_POWERUP)
    {

        while (sSpiInformation.ui32SpiState != eSPI_STATE_INITIALIZED);
        {

        }

    }


    if (sSpiInformation.ui32SpiState == eSPI_STATE_INITIALIZED)
    {

        // This is the first SPI transmission so we need to use special timing.
        SpiFirstWrite(pui8UserBuffer, ui16Length);
    }
    else
    {

		tSLInformation.WlanInterruptDisable();

        // We need wait until the device is idle to prevent us from trying to
        // transmit a packet when a packet receive may be ongoing.
        while (sSpiInformation.ui32SpiState != eSPI_STATE_IDLE); // spin

        sSpiInformation.ui32SpiState = eSPI_STATE_WRITE_IRQ;
        sSpiInformation.pTxPacket = pui8UserBuffer;
        sSpiInformation.ui16TxPacketLength = ui16Length;

        // Assert the CS line and wait till SSI IRQ line is active and then
        // initialize write operation.
        ASSERT_CS();


		tSLInformation.WlanInterruptEnable();

		if (tSLInformation.ReadWlanInterruptPin() == 0)
		{
                	SpiWriteDataSynchronous(sSpiInformation.pTxPacket, sSpiInformation.ui16TxPacketLength);

			sSpiInformation.ui32SpiState = eSPI_STATE_IDLE;

			DEASSERT_CS();
		}


    }


    // Wait for the transaction to complete before returning.
    while (eSPI_STATE_IDLE != sSpiInformation.ui32SpiState); // kill time


    return(0);
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
// SpiReadDataSynchronous() in MSP430 speak
static void
SpiReadData(uint8_t *data, uint16_t size)
{

#ifdef VERBOSE
	printf("SpiReadData: read %d bytes\r\n", size);
#endif
    unsigned char *data_to_send = tSpiReadHeader;


    MSS_SPI_set_slave_select( &g_mss_spi1, MSS_SPI_SLAVE_0 );



    int i = 0;
#ifdef VERBOSE
    printf("SpiReadData doing a write: {");
#endif
    for (i = 0; i < size; i++)
    {
#ifdef VERBOSE
        printf("0x%x ,", data_to_send[i]);
#endif
    }
#ifdef VERBOSE
    printf(" }\r\n");
#endif

#ifdef VERBOSE
    printf("SpiReadData: {");
#endif
    for (i = 0; i < size; i++)
    {
      data[i] = MSS_SPI_transfer_frame( &g_mss_spi1, data_to_send[i]);
#ifdef VERBOSE
      printf("0x%x ,", data[i]);
#endif
    }
#ifdef VERBOSE
    printf(" }\r\n");
#endif


 /*   MSS_SPI_transfer_block
      (
          &g_mss_spi1,
          data_to_send,
          size,
          data,
          size
      );

      */
/*
    MSS_SPI_transfer_block
        (
            &g_mss_spi1,
            data_to_send,
            size,
            0,
            0
        );
        */

/*
    MSS_SPI_transfer_block
        (
            &g_mss_spi1,
            0,
            0,
            data,
            size
        );
        */
    MSS_SPI_clear_slave_select( &g_mss_spi1, MSS_SPI_SLAVE_0 );

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
SpiWriteDataSynchronous(const uint8_t *data, uint16_t size)
{
	//printf("WriteSynchronous\r\n");

    MSS_SPI_set_slave_select( &g_mss_spi1, MSS_SPI_SLAVE_0 );

    int i = 0;
#ifdef VERBOSE
    printf("WriteSpiSynchro: {");
#endif
    for (i = 0; i < size; i++)
    {
#ifdef VERBOSE
    printf("0x%x ,", data[i]);
#endif
     MSS_SPI_transfer_frame( &g_mss_spi1, data[i]);

    }
#ifdef VERBOSE
    printf(" }\r\n");
#endif
/*
    uint32_t dummy;

    int rx_fifo_empty = (int)&g_mss_spi1.hw_reg->STATUS & (int) RX_FIFO_EMPTY_MASK;
    while(!rx_fifo_empty)
    {
        dummy = &g_mss_spi1.hw_reg->RX_DATA;
        dummy = dummy;
        rx_fifo_empty = (int)&g_mss_spi1.hw_reg->STATUS & (int)RX_FIFO_EMPTY_MASK;
    }


    int tx_done = (int)&g_mss_spi1.hw_reg->STATUS & (int)TX_FIFO_EMPTY_MASK;
    while (!tx_done)
    {
        dummy = (int)&g_mss_spi1.hw_reg->TX_DATA;
        dummy = dummy;
        tx_done = (int)&g_mss_spi1.hw_reg->STATUS & (int)TX_FIFO_EMPTY_MASK;

    }
*/
    /*
    MSS_SPI_transfer_block
      (
          &g_mss_spi1,
          data,
          size,
          0,
          0
      );
    */
    MSS_SPI_clear_slave_select( &g_mss_spi1, MSS_SPI_SLAVE_0 );


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
#ifdef VERBOSE
    printf("SpiReadHeader: read header\r\n");
#endif
    //sSpiInformation.ui32SpiState = eSPI_STATE_READ_IRQ; // TODO put this here?

	SpiReadData(sSpiInformation.pRxPacket, 10);

//	int i = 0;
//	printf("header rec'd: { ");
//	for (i; i < 10; i++)
//	{
//		printf("0x%x ", sSpiInformation.pRxPacket[i]);
//	}
 //   printf("}\r\n");

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
SpiReadDataCont()
{
#ifdef VERBOSE
	printf("SpiReadDataCont: keep reading\r\n");
#endif

	  long data_to_recv;
		unsigned char *evnt_buff, type;


	    //
	    //determine what type of packet we have
	    //
	    evnt_buff =  sSpiInformation.pRxPacket;
	    data_to_recv = 0;
		STREAM_TO_UINT8((char *)(evnt_buff + SPI_HEADER_SIZE), HCI_PACKET_TYPE_OFFSET, type);

	    switch(type)
	    {
	        case HCI_TYPE_DATA:
	        {
#ifdef VERBOSE
	        	printf("SpiReadDataCont: Case HCI_TYPE_DATA\r\n");
#endif
				//
				// We need to read the rest of data..
				//
				STREAM_TO_UINT16((char *)(evnt_buff + SPI_HEADER_SIZE), HCI_DATA_LENGTH_OFFSET, data_to_recv);
				if (!((HEADERS_SIZE_EVNT + data_to_recv) & 1))
				{
	    	        data_to_recv++;
				}

				if (data_to_recv)
				{
	            	SpiReadData(evnt_buff + 10, data_to_recv);
				}
#ifdef VERBOSE
		       	printf("SpiReadDataCont: data_to_recv: %d\r\n", data_to_recv);
#endif
	            break;
	        }
	        case HCI_TYPE_EVNT:
	        {
#ifdef VERBOSE
	        	printf("SpiReadDataCont: Case HCI_TYPE_EVNT\r\n");
#endif
				//
				// Calculate the rest length of the data
				//
	            STREAM_TO_UINT8((char *)(evnt_buff + SPI_HEADER_SIZE), HCI_EVENT_LENGTH_OFFSET, data_to_recv);
				data_to_recv -= 1;

				//
				// Add padding byte if needed
				//
				if ((HEADERS_SIZE_EVNT + data_to_recv) & 1)
				{

		            data_to_recv++;
				}

				if (data_to_recv)
				{
	            	SpiReadData(evnt_buff + 10, data_to_recv);
				}
#ifdef VERBOSE
	        	printf("SpiReadDataCont: data_to_recv: %d\r\n", data_to_recv);
#endif
				sSpiInformation.ui32SpiState = eSPI_STATE_READ_EOT;
	            break;
	        }
	    }

	    return (0);


}

//*****************************************************************************
//
//! This function enter point for write flow
//!
//!  \param  SpiPauseSpi
//!
//!  \return none
//!
//!  \brief  The function triggers a user provided callback for
//
//*****************************************************************************

void
SpiPauseSpi(void)
{
#ifdef VERBOSE
    printf("SpiPauseSpi: disable spi IRQ\r\n");
#endif
	MSS_GPIO_disable_irq(SPI_IRQ_PIN);
}


//*****************************************************************************
//
// Disable all interrupts associated with the SPI interface and the IRQ line.
//
//*****************************************************************************
static void
SpiDisableInterrupts(void)
{
    //printf("SpidisabelInt\r\n");
    MSS_GPIO_disable_irq(SPI_IRQ_PIN); // TODO might need to add one for SPI here?

}

//*****************************************************************************
//
// Re-enable the interrupts associated with the SPI interface and IRQ line.
//
//*****************************************************************************
void
SpiResumeSpi(void)
{
#ifdef VERBOSE
    printf("SpiResumeSpi\r\n");
#endif
	MSS_GPIO_enable_irq(SPI_IRQ_PIN);

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
SpiTriggerRxProcessing()
{
#ifdef VERBOSE
	printf("SpiTriggerRxProcessing: trigger rx\r\n");
#endif
	//
	// Trigger Rx processing
	//

	SpiPauseSpi();
	DEASSERT_CS();

        // The magic number that resides at the end of the TX/RX buffer (1 byte after the allocated size)
        // for the purpose of detection of the overrun. If the magic number is overriten - buffer overrun
        // occurred - and we will stuck here forever!
	if (sSpiInformation.pRxPacket[CC3000_RX_BUFFER_SIZE - 1] != CC3000_BUFFER_MAGIC_NUMBER)
	{
		while (1)
			;
	}

	sSpiInformation.ui32SpiState = eSPI_STATE_IDLE;
	sSpiInformation.SPIRxHandler(sSpiInformation.pRxPacket + SPI_HEADER_SIZE);
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
#ifdef VERBOSE
   printf("SpiContReadOperation: calling SpiReadDataCont \r\n");
#endif
	//
	// The header was read - continue with  the payload read
	//
	if (!SpiReadDataCont())
	{


		//
		// All the data was read - finalize handling by switching to teh task
		//	and calling from task Event Handler
		//
#ifdef VERBOSE
		  printf("SpiContReadOperation:calling SpiTriggerRxProcessing \r\n");
#endif
		SpiTriggerRxProcessing();
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
// WLAN (SPI_IRQ) handler
//
//*****************************************************************************
//void IntSpiGPIOHandler(void)
__attribute__((__interrupt__)) void IntSpiGPIOHandler(void)
{

   if (after_rec == 1)
   {
	   //check STATE!!!! must be IDLE
	   //printf("Got Interrupt after recv()\r\n");
   }

	// this is the interrupt handler for SPI_IRQ_PIN (MSS_GPIO_2)
#ifdef VERBOSE
	printf("IntSpiGPIOHandler: Got SPI_IRQ_PIN interrupt for");
#endif

	MSS_GPIO_clear_irq(SPI_IRQ_PIN);

	if (sSpiInformation.ui32SpiState == eSPI_STATE_POWERUP)
	{
#ifdef VERBOSE
		printf(" INIT!\r\n");
#endif
		// We received the first IRQ line edge after powering up the
		// part.  This is part of the startup sequence so change the state
		// to show that we received this.
		sSpiInformation.ui32SpiState = eSPI_STATE_INITIALIZED;
#ifdef VERBOSE
        printf("IntSpiGPIOHandler: Done with INIT\r\n");
#endif
	}
	else if (sSpiInformation.ui32SpiState == eSPI_STATE_IDLE)
	{
#ifdef VERBOSE
		printf(" READ!\r\n");
#endif

		// We're idle so an IRQ interrupt indicates that the CC3000 has
		// data to send us.
		sSpiInformation.ui32SpiState = eSPI_STATE_READ_IRQ;

		// IRQ line asserted so handshake with CS and set up to
		// receive a packet header from the CC3000.
		ASSERT_CS();

		// Start receiving the 10 byte header from the CC3000.

		// msp430 has a comment here about DMA interrupt????????
		SpiReadHeader();

		sSpiInformation.ui32SpiState = eSPI_STATE_READ_EOT;
       // printf("SpiContReadOperation!!            Must do this for a rec\r\n");
		SpiContReadOperation();
#ifdef VERBOSE
        printf("IntSpiGPIOHandler: Done with the read\r\n");
#endif

	}
	else if (sSpiInformation.ui32SpiState == eSPI_STATE_WRITE_IRQ)
	{
#ifdef VERBOSE
		printf(" WRITE!\r\n");
#endif
		SpiWriteDataSynchronous(sSpiInformation.pTxPacket, sSpiInformation.ui16TxPacketLength);

		sSpiInformation.ui32SpiState = eSPI_STATE_IDLE;

		DEASSERT_CS();
#ifdef VERBOSE
        printf("IntSpiGPIOHandler: Done with the write\r\n");
#endif
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
// PDMA interrupt
//
//*****************************************************************************
void
SpiIntHandler(bool bTxFinished, bool bRxFinished)
{
printf("should not have got here?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?");

}
