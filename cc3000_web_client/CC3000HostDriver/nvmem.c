/*****************************************************************************
*
*  nvmem.c  - CC3000 Host Driver Implementation.
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

//*****************************************************************************
//
//! \addtogroup nvmem_api
//! @{
//
//*****************************************************************************

#include <stdio.h>
#include <string.h>
#include "nvmem.h"
#include "hci.h"
#include "socket.h"
#include "evnt_handler.h"

//*****************************************************************************
//
// Prototypes for the structures for APIs.
//
//*****************************************************************************

#define NVMEM_READ_PARAMS_LEN 	(12)
#define NVMEM_WRITE_PARAMS_LEN  (16)
/*****************************************************************************
 * \brief Read data from nvmem
 *
 * Reads data from the file referred by the ulFileId parameter. 
 * Reads data from file ulOffset till len. Err if the file can't
 * be used, is invalid, or if the read is out of bounds. 
 *
 *
 * \param[in] ulFileId   nvmem file id:\n
 * NVMEM_NVS_FILEID, NVMEM_NVS_SHADOW_FILEID,
 * NVMEM_WLAN_CONFIG_FILEID, NVMEM_WLAN_CONFIG_SHADOW_FILEID,
 * NVMEM_WLAN_DRIVER_SP_FILEID, NVMEM_WLAN_FW_SP_FILEID,
 * NVMEM_MAC_FILEID, NVMEM_FRONTEND_VARS_FILEID,
 * NVMEM_IP_CONFIG_FILEID, NVMEM_IP_CONFIG_SHADOW_FILEID,
 * NVMEM_BOOTLOADER_SP_FILEID or NVMEM_RM_FILEID.
 * \param[in] ulLength   number of bytes to read  
 * \param[in] ulOffset   ulOffset in file from where to read  
 * \param[out] buff    output buffer pointer
 *
 * \return	    number of bytes read.
 *
 * \sa
 * \note
 * \warning
 *
 *****************************************************************************/
signed long 
nvmem_read(unsigned long ulFileId, unsigned long ulLength, unsigned long ulOffset, unsigned char *buff)
{
    unsigned char ucStatus = 0xFF;
    unsigned char *ptr;
    unsigned char *args;
    
    ptr = tSLInformation.pucTxCommandBuffer;
    args = (ptr + HEADERS_SIZE_CMD);
    
    //
    // Fill in HCI packet structure
    //
	args = UINT32_TO_STREAM(args, ulFileId);
	args = UINT32_TO_STREAM(args, ulLength);
	args = UINT32_TO_STREAM(args, ulOffset);

	//
    // Initiate a HCI command
    //
	hci_command_send(HCI_CMND_NVMEM_READ, ptr, NVMEM_READ_PARAMS_LEN);

    SimpleLinkWaitEvent(HCI_CMND_NVMEM_READ, &ucStatus);

	//
	// In case there is a data - read it
	//
	if (ucStatus == 0)
	{
		//
		// Wait for the data in a synchronous way. Here we assume that the buffer is big enough
		// to store also parameters of nvmem
		//
		SimpleLinkWaitData(buff, 0, 0);
	}

    return(ucStatus);
}


/*****************************************************************************
 * \brief Write data to nvmem.
 *  
 * writes data to file referred by the ulFileId parameter. 
 * Writes data to file  ulOffset till ulLength. The file id will be 
 * marked invalid till the write is done. The file entry doesn't
 * need to be valid - only allocated.
 *  
 * \param[in] ulFileId   nvmem file id:\n
 * NVMEM_NVS_FILEID, NVMEM_NVS_SHADOW_FILEID,
 * NVMEM_WLAN_CONFIG_FILEID, NVMEM_WLAN_CONFIG_SHADOW_FILEID,
 * NVMEM_WLAN_DRIVER_SP_FILEID, NVMEM_WLAN_FW_SP_FILEID,
 * NVMEM_MAC_FILEID, NVMEM_FRONTEND_VARS_FILEID,
 * NVMEM_IP_CONFIG_FILEID, NVMEM_IP_CONFIG_SHADOW_FILEID,
 * NVMEM_BOOTLOADER_SP_FILEID or NVMEM_RM_FILEID.
 * \param[in] ulLength    number of bytes to write   
 * \param[in] ulEntryOffset  offset in file to start write operation from    
 * \param[in] buff      data to write 
 *
 * \return	  on succes 0, error otherwise.
 *
 * \sa
 * \note
 * \warning
 *
 *****************************************************************************/
signed long 
nvmem_write(unsigned long ulFileId, unsigned long ulLength, unsigned long ulEntryOffset, 
            unsigned char *buff)
{
    long iRes;
    unsigned char *ptr;
    unsigned char *args;
    
    iRes = EFAIL;

    ptr = tSLInformation.pucTxCommandBuffer;
    args = (ptr + SPI_HEADER_SIZE + HCI_DATA_CMD_HEADER_SIZE);

    // Fill in HCI packet structure
	args = UINT32_TO_STREAM(args, ulFileId);
	args = UINT32_TO_STREAM(args, 12);
	args = UINT32_TO_STREAM(args, ulLength);
	args = UINT32_TO_STREAM(args, ulEntryOffset);
	
	memcpy((ptr + SPI_HEADER_SIZE + HCI_DATA_CMD_HEADER_SIZE + NVMEM_WRITE_PARAMS_LEN),
			buff,
			ulLength);

    // Initiate a HCI command but it will come on data channel
    hci_data_command_send(HCI_CMND_NVMEM_WRITE, ptr, NVMEM_WRITE_PARAMS_LEN, ulLength);
	
	SimpleLinkWaitEvent(HCI_EVNT_NVMEM_WRITE, &iRes);

    return(iRes);
}



/*****************************************************************************
 * \brief Write MAC address.
 *  
 * Write MAC address to EEPROM. 
 * mac address as appears over the air (OUI first)
 *  
 * \param[in] mac  mac address:\n
 *
 * \return	  on succes 0, error otherwise.
 *
 * \sa
 * \note
 * \warning
 *
 *****************************************************************************/
unsigned char nvmem_set_mac_address(unsigned char *mac)
{
	return  nvmem_write(NVMEM_MAC_FILEID, MAC_ADDR_LEN, 0, mac);
}


/*****************************************************************************
 * \brief Read MAC address.
 *  
 * Read MAC address from EEPROM. 
 * mac address as appears over the air (OUI first)
 *  
 * \param[out] mac  mac address:\n
 *
 * \return	  on succes 0, error otherwise.
 *
 * \sa
 * \note
 * \warning
 *
 *****************************************************************************/
unsigned char nvmem_get_mac_address(unsigned char *mac)
{
	return  nvmem_read(NVMEM_MAC_FILEID, MAC_ADDR_LEN, 0, mac);
}


/*****************************************************************************
 * \brief Write data to nvmem.
 *  
 * program a patch to a specific file ID. 
 * The SP data is assumed to be organized in 2-dimentional.
 * Each line is 150 bytes long.
 * Actual programming is applied in 150 bytes portions (SP_PORTION_SIZE).
 *  
 * \param[in] ulFileId   nvmem file id:\n
 * NVMEM_WLAN_DRIVER_SP_FILEID, 
 * NVMEM_WLAN_FW_SP_FILEID,
 * \param[in] spLength    number of bytes to write   
 * \param[in] spData      SP data to write 
 *
 * \return	  on succes 0, error otherwise.
 *
 * \sa
 * \note
 * \warning
 *
 *****************************************************************************/
unsigned char nvmem_write_patch(unsigned long ulFileId, unsigned long spLength, const unsigned char *spData)
{
	unsigned char 	status = 0;
	unsigned short	offset = 0;
	unsigned char*      spDataPtr = (unsigned char*)spData;

	while ((status == 0) && (spLength >= SP_PORTION_SIZE))
	{
		status = nvmem_write(ulFileId, SP_PORTION_SIZE, offset, spDataPtr);
		offset += SP_PORTION_SIZE;
		spLength -= SP_PORTION_SIZE;
		spDataPtr += SP_PORTION_SIZE;
	}

	if (status !=0)
	{
		// NVMEM error occured
		return status;
	}

	if (spLength != 0)
	{
		// if reached here, a reminder is left
		status = nvmem_write(ulFileId, spLength, offset, spDataPtr);
	}

	return status;
}



/*****************************************************************************
 * \brief Read patch version.
 *  
 * read package version (WiFi FW patch, driver-supplicant-NS patch, bootloader patch)
 *  
 * \param[out] patchVer    first number indicates package ID and the second number indicates package build number   
 *
 * \return	  on succes 0, error otherwise.
 *
 * \sa
 * \note
 * \warning
 *
 *****************************************************************************/
#ifndef CC3000_TINY_DRIVER
unsigned char nvmem_read_sp_version(unsigned char* patchVer)
{
    unsigned char *ptr;
    unsigned char	retBuf[5];	// 1st byte is the status and the rest is the SP version
    
    ptr = tSLInformation.pucTxCommandBuffer;
    
    //
    // Initiate a HCI command
    //
	hci_command_send(HCI_CMND_READ_SP_VERSION, ptr, 0);	// no args are required

    SimpleLinkWaitEvent(HCI_CMND_READ_SP_VERSION, retBuf);

	*patchVer = retBuf[3];			// package ID
	*(patchVer+1) = retBuf[4];		// package build number

    return(retBuf[0]);
}
#endif

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

