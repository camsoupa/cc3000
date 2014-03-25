/*****************************************************************************
*
*  cc3000_common.h  - CC3000 Host Driver Implementation.
*  Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/
#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdlib.h>
#include <errno.h>
#include <stdint.h>

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
//                  ERROR CODES
//*****************************************************************************
#define ESUCCESS        0
#define EFAIL          -1
#define EERROR          EFAIL

#define ERROR_SOCKET_INACTIVE   -57 

#define HCI_CC_PAYLOAD_LEN      5

#define WLAN_ENABLE      (1)   
#define WLAN_DISABLE     (0)


#define	MAC_ADDR_LEN	(6)

#define	SP_PORTION_SIZE	(20)


//TX and RX buffer sizes - allow to receive and transmit maximum data at lengh 8.
#ifndef CC3000_TINY_DRIVER
#define TINY_CC3000_MAXIMAL_RX_SIZE 44
#define TINY_CC3000_MAXIMAL_TX_SIZE 59
#endif
//Defines for minimal and maximal RX buffer size. This size includes the spi header and hci header.
//The maximal buffer size derives from:
// MTU + HCI header + SPI header + sendto() agrs size
//The minimum buffer size derives from:
// HCI header + SPI header + max args size
//
// This buffer is used for receiving events and data.
// Note that the maximal buffer size as defined here is a maximal at all 
// The packet can not be longer than MTU size and CC3000 does not support 
// fragmentation.Note that the same buffer is used for reception of the data and
// events from CC3000. That is why the minium is defined. 
// The calculation for the actual size of buffer for reception is:
// Given the maximal data size MAX_DATA that is expected to be received by application, the required buffer is:
// Using recv() or recvfrom():
// max(CC3000_MINIMAL_RX_SIZE, MAX_DATA + HEADERS_SIZE_DATA + fromlen + ucArgsize + 1)
// Using gethostbyname() with minimal buffer size will limit the host name returned to 99 bytes only.  
// The 1 is used for the overrun detection
#define CC3000_MINIMAL_RX_SIZE      (118 + 1)
#define CC3000_MAXIMAL_RX_SIZE      (1519 + 1)

//Defines for minimal and maximal TX buffer size.
// This buffer is used for sending events and data.
// Note that the maximal buffer size as defined here is a maximal at all 
// The packet can not be longer than MTU size and CC3000 does not support 
// fragmentation.Note that the same buffer is used for tranmission of the data and
// commnads. That is why the minium is defined. 
// The calculation for the actual size of buffer for transmission is:
// Given the maximal data size MAX_DATA, the required buffer is:
// Using Sendto:
// max(CC3000_MINIMAL_TX_SIZE, MAX_DATA + SPI_HEADER_SIZE + SOCKET_SENDTO_PARAMS_LEN + SIMPLE_LINK_HCI_DATA_HEADER_SIZE + 1)
// Using Send():
// max(CC3000_MINIMAL_TX_SIZE, MAX_DATA + SPI_HEADER_SIZE + HCI_CMND_SEND_ARG_LENGTH + SIMPLE_LINK_HCI_DATA_HEADER_SIZE + 1)
// The 1 is used for the overrun detection
#define  CC3000_MINIMAL_TX_SIZE      (118 + 1) 
#define CC3000_MAXIMAL_TX_SIZE      (1519 + 1)

//In order to determine your preferred buffer size, change CC3000_MAXIMAL_RX_SIZE and CC3000_MAXIMAL_TX_SIZE 
//to a value between the minimal and maximal specified above. 
// Note that the buyffers are allocated by SPI. In case you change the size of those buffers, you might need also
// to change the linker file, since for example on MSP430 FRAM devices the buffers are allocated in the FRAM section
// that is allocated manually and not by IDE. Thus its size changes...
  
#ifndef CC3000_TINY_DRIVER
  
	#define CC3000_RX_BUFFER_SIZE   (CC3000_MAXIMAL_RX_SIZE)
	#define CC3000_TX_BUFFER_SIZE   (CC3000_MAXIMAL_TX_SIZE)
  
//if defined TINY DRIVER we use smaller rx and tx buffer in order to minimize RAM consumption
#else
	#define CC3000_RX_BUFFER_SIZE   (TINY_CC3000_MAXIMAL_RX_SIZE)
	#define CC3000_TX_BUFFER_SIZE   (TINY_CC3000_MAXIMAL_TX_SIZE)

#endif  
//*****************************************************************************
//                  Compound Types
//*****************************************************************************
typedef long time_t;
typedef unsigned long clock_t;
typedef long suseconds_t;

typedef struct timeval timeval;

struct timeval 
{
    time_t         tv_sec;                  /* seconds */
    suseconds_t    tv_usec;                 /* microseconds */
};

typedef char *(*tFWPatches)(unsigned long *usLength);

typedef char *(*tDriverPatches)(unsigned long *usLength);

typedef char *(*tBootLoaderPatches)(unsigned long *usLength);

typedef void (*tWlanCB)(long event_type, char * data, unsigned char length );

typedef long (*tWlanReadInteruptPin)(void);

typedef void (*tWlanInterruptEnable)(void);

typedef void (*tWlanInterruptDisable)(void);

typedef void (*tWriteWlanPin)(unsigned char val);

typedef struct
{
	unsigned short	 usRxEventOpcode;
	unsigned short	 usEventOrDataReceived;
	unsigned char 	*pucReceivedData;
	unsigned char 	*pucTxCommandBuffer;

	tFWPatches 			sFWPatches;
	tDriverPatches 		sDriverPatches;
	tBootLoaderPatches 	sBootLoaderPatches;
	tWlanCB	 			sWlanCB;
    tWlanReadInteruptPin  ReadWlanInterruptPin;
    tWlanInterruptEnable  WlanInterruptEnable;
    tWlanInterruptDisable WlanInterruptDisable;
    tWriteWlanPin         WriteWlanPin;

	signed long		 slTransmitDataError;
	unsigned short	 usNumberOfFreeBuffers;
	unsigned short	 usSlBufferLength;
	unsigned short	 usBufferSize;
	unsigned short	 usRxDataPending;

	unsigned long    NumberOfSentPackets;
	unsigned long    NumberOfReleasedPackets;

	unsigned char	 InformHostOnTxComplete;
}sSimplLinkInformation;



extern sSimplLinkInformation tSLInformation;


//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************



/**
 *  \brief Wait event
 *
 *
 * \param[in] usOpcode 
 * \param[out] pRetParams   
 * \return						
 *                              
 * \note
 * \warning
 */

extern void SimpleLinkWaitEvent(unsigned short usOpcode, void *pRetParams);

/**
 *  \brief Wait data
 *
 *
 * \param[out] pBuf 
 * \param[out] from   
 * \param[out] fromlen
 * \return						
 *                              
 * \note
 * \warning
 */

extern void SimpleLinkWaitData(unsigned char *pBuf, unsigned char *from, unsigned char *fromlen);


extern unsigned char* UINT32_TO_STREAM_f (unsigned char *p, unsigned long u32);

extern unsigned char* UINT16_TO_STREAM_f (unsigned char *p, unsigned short u16);

extern unsigned short STREAM_TO_UINT16_f(char* p, unsigned short offset);

extern unsigned long STREAM_TO_UINT32_f(char* p, unsigned short offset);

//This macro is used for copying 8 bit to stream while converting to little endian format.
#define UINT8_TO_STREAM(_p, _val)	{*(_p)++ = (_val);}
//This macro is used for copying 16 bit to stream while converting to little endian format.
#define UINT16_TO_STREAM(_p, _u16)	(UINT16_TO_STREAM_f(_p, _u16))
//This macro is used for copying 32 bit to stream while converting to little endian format.
#define UINT32_TO_STREAM(_p, _u32)	(UINT32_TO_STREAM_f(_p, _u32))
//This macro is used for copying a specified value length bits (l) to stream while converting to little endian format.
#define ARRAY_TO_STREAM(p, a, l) 	{register short _i; for (_i = 0; _i < l; _i++) *(p)++ = ((unsigned char *) a)[_i];}
//This macro is used for copying received stream to 8 bit in little endian format.
#define STREAM_TO_UINT8(_p, _offset, _u8)	{_u8 = (unsigned char)(*(_p + _offset));}
//This macro is used for copying received stream to 16 bit in little endian format.
#define STREAM_TO_UINT16(_p, _offset, _u16)	{_u16 = STREAM_TO_UINT16_f(_p, _offset);}
//This macro is used for copying received stream to 32 bit in little endian format.
#define STREAM_TO_UINT32(_p, _offset, _u32)	{_u32 = STREAM_TO_UINT32_f(_p, _offset);}
#define STREAM_TO_STREAM(p, a, l) 	{register short _i; for (_i = 0; _i < l; _i++) *(a)++ = (unsigned char) p[_i];}




//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // __COMMON_H__
