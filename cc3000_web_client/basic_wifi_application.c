//*****************************************************************************
//
// basic_wifi_application.c - A command-line driven CC3000 WiFi example.
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
// This is part of revision 2.1.0.12573 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>

#include "drivers/mss_timer/mss_timer.h"
#include "drivers/mss_uart/mss_uart.h"
#include "drivers/mss_gpio/mss_gpio.h"
#include "drivers/mss_spi/mss_spi.h"
#include "drivers/mss_timer/mss_timer.h"
#include "drivers/board/board.h"
#include "CC3000HostDriver/wlan.h"
#include "CC3000HostDriver/evnt_handler.h"
#include "CC3000HostDriver/nvmem.h"
#include "CC3000HostDriver/socket.h"
#include "CC3000HostDriver/netapp.h"
#include "CC3000HostDriver/matt_spi.h"
#include "CC3000HostDriver/hci.h"
#include "application_commands.h"
#include "utils/utils.h"
#include "drivers/air_weather_display/air_weather_display.h"

int after_rec = 0;
extern int myTick;


#ifdef DEBUG
//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    //
    // Tell the user about the error reported.
    //
    //printf("Runtime error at line %d of file %s!\n", ui32Line, pcFilename);

    while(1)
    {
        //
        // Hang here to allow debug.
        //
    }
}
#endif // DEBUG

//*****************************************************************************
//
//  Global variables used by this program for event tracking.
//
//*****************************************************************************
volatile uint32_t g_ui32SmartConfigFinished, g_ui32CC3000Connected,
                  g_ui32CC3000DHCP,g_ui32OkToDoShutDown,
                  g_ui32CC3000DHCP_configured;

//*****************************************************************************
//
// Global to see if socket has been connected to (valid for TCP only)
//
//*****************************************************************************
bool g_bSocketConnected = false;

//*****************************************************************************
//
//  Smart Config flag variable. Used to stop Smart Config.
//
//*****************************************************************************
volatile uint8_t g_ui8StopSmartConfig;

//*****************************************************************************
//
// Global pointer to socket struct.
//
//*****************************************************************************
volatile uint32_t g_ui32Socket = SENTINEL_EMPTY;

//*****************************************************************************
//
// Global to hold socket type. Rewritten each time 'socketopen' is called.
//
//*****************************************************************************
sockaddr g_tSocketAddr;

//*****************************************************************************
//
// Flag used to signify type of socket, TCP or UDP.
//
//*****************************************************************************
uint32_t g_ui32SocketType = 0;

//*****************************************************************************
//
// Flag used to denote of bind has been called on socket yet
//
//*****************************************************************************
uint32_t g_ui32BindFlag = SENTINEL_EMPTY;

//*****************************************************************************
//
// Simple Config Prefix
//
//*****************************************************************************
char g_pcCC3000_prefix[] = {'T', 'T', 'T'};

//*****************************************************************************
//
// Input buffer for the command line interpreter.
//
//*****************************************************************************
//static char g_cInput[MAX_COMMAND_SIZE]; //matt-not used anymore

//*****************************************************************************
//
// Device name used by default for smart config response & mDNS advertising.
//
//*****************************************************************************
char g_pcdevice_name[] = "home_assistant";


//*****************************************************************************
//__no_init is used to prevent the buffer initialization in order to
// prevent hardware WDT expiration  before entering 'main()'.
//for every IDE, different syntax exists :
// __CCS__ for CCS v5
// __IAR_SYSTEMS_ICC__ for IAR Embedded Workbench
//
// Reception from the air, buffer - the max data length  + headers
//
//*****************************************************************************

//
// Code Composer Studio pragmas.
//

uint8_t g_pui8CC3000_Rx_Buffer[CC3000_APP_BUFFER_SIZE +
                                            CC3000_RX_BUFFER_OVERHEAD_SIZE];


//*****************************************************************************
//
// This function returns a pointer to the driver patch.  Since there is
// no patch (patches are taken from the EEPROM and not from the host) it
// returns NULL.
//
//*****************************************************************************
char *
sendDriverPatch(unsigned long *Length)
{
    *Length = 0;
    return(NULL);
}

//*****************************************************************************
//
// This function returns a pointer to the bootloader patch.  Since there
// is no patch (patches are taken from the EEPROM and not from the host)
// it returns NULL.
//
//*****************************************************************************
char *
sendBootLoaderPatch(unsigned long *Length)
{
    *Length = 0;
    return(NULL);
}

//*****************************************************************************
//
// This function returns a pointer to the driver patch.  Since there is
// no patch (patches are taken from the EEPROM and not from the host)
// it returns NULL.
//
//*****************************************************************************
char *
sendWLFWPatch(unsigned long *Length)
{
    *Length = 0;
    return(NULL);
}

//*****************************************************************************
//
// This function handles asynchronous events that come from CC3000.
//
//*****************************************************************************
void
CC3000_UsynchCallback(long lEventType, char *pcData, unsigned char ucLength)
{
    netapp_pingreport_args_t *psPingData;

    // Handle completion of simple config callback
    if(lEventType == HCI_EVNT_WLAN_ASYNC_SIMPLE_CONFIG_DONE)
    {
        g_ui32SmartConfigFinished = 1;
        g_ui8StopSmartConfig = 1;
    }

    // Handle unsolicited connect callback.
    if(lEventType == HCI_EVNT_WLAN_UNSOL_CONNECT)
    {
        // Set global variable to indicate connection.
        g_ui32CC3000Connected = 1;
    }

    // Handle unsolicited disconnect callback. Turn LED Green -> Red.
    if(lEventType == HCI_EVNT_WLAN_UNSOL_DISCONNECT)
    {
        //printf("\r    Received Unsot Disconnect from CC3000\n>");
        g_ui32CC3000Connected = 0;
        g_ui32CC3000DHCP = 0;
        g_ui32CC3000DHCP_configured = 0;

        // Turn off the LED3 (Green)
        turnLedOff(LED_1);

        // Turn back on the LED 1 (RED)
        turnLedOn(LED_0);
    }

    // Handle DHCP connection callback.
    if(lEventType == HCI_EVNT_WLAN_UNSOL_DHCP)
    {
        // Notes:
        // 1) IP config parameters are received swapped
        // 2) IP config parameters are valid only if status is OK,
        //      i.e. g_ui32CC3000DHCP becomes 1

        // Only if status is OK, the flag is set to 1 and the addresses are
        // valid

    	// the this is *pcData+ 20
    	if( *(pcData + 4) == 0)
        {
            // DHCP success, set global accordingly.
            g_ui32CC3000DHCP = 1;

            // Turn on the LED3 (Green).
            turnLedOn(LED_1);
        }
        else
        {
            // DHCP failed, set global accordingly.
            g_ui32CC3000DHCP = 0;
        }
    }

    // Ping event handler
    if(lEventType == HCI_EVNT_WLAN_ASYNC_PING_REPORT)
    {
        // Ping data received, print to screen
        psPingData = (netapp_pingreport_args_t *)pcData;
#ifdef DEBUG
        printf("Data Rec'd='\r\n");
        for(lEventType = 0; lEventType < ucLength; lEventType++)
        {
            printf("%d,",pcData[lEventType]);
        }
        printf("'\n");
#endif
    }

    // Handle shutdown callback.
    if(lEventType == HCI_EVENT_CC3000_CAN_SHUT_DOWN)
    {
        // Set global variable to indicate the device can be shutdown.
        g_ui32OkToDoShutDown = 1;
    }
}

//*****************************************************************************
//
// This function initializes a CC3000 device and triggers it to start
// operation
//
//*****************************************************************************
int
initDriver(void)
{
    // Initialize device pin configuration and the system clock.
    pio_init();

    // Initialize and configure the SPI.
    init_spi(1000000, 100000000);

    MSS_GPIO_enable_irq(SPI_IRQ_PIN); // Do this for each?
#ifdef VERBOSE
	printf("Starting Wlan_init!\r\n");
#endif
    // Initialize and enable WiFi on the CC3000.
    wlan_init(CC3000_UsynchCallback, sendWLFWPatch, sendDriverPatch,
              sendBootLoaderPatch, ReadWlanInterruptPin,
              WlanInterruptEnable, WlanInterruptDisable, WriteWlanPin);

	printf("Wlan_init done!\n\r");

    // Start up the CC3000 stack.
    wlan_start(0);
#ifdef VERBOSE
	printf("Done starting wlan!\r\n");
#endif

    // Turn on the the red LED to indicate that we are active and initiated
    // WLAN successfully.
    turnLedOn(LED_1);


    // Mask out all non-required events from CC3000.
    wlan_set_event_mask(HCI_EVNT_WLAN_KEEPALIVE | HCI_EVNT_WLAN_UNSOL_INIT);

    // Set flag to stop smart config if running.
    g_ui8StopSmartConfig = 0;

    // Configure and enable the system tick.
    InitSysTick();

    return(0);
}

//*****************************************************************************
//
// Takes in a string of the form YYY.YYY.YYY.YYY (where YYY is a string
// representation of a decimal between0 and 255), converts each pair to a
// decimal. Used to parse user input from the command line.
//
// Returns 0 on success, -1 on fail.
//
//*****************************************************************************
int
DotDecimalDecoder(char *pcString, uint8_t *pui8Val1, uint8_t *pui8Val2,
                  uint8_t *pui8Val3, uint8_t *pui8Val4)
{
    uint32_t ui32Block1, ui32Block2, ui32Block3, ui32Block4;
    char *pcEndData, *pcStartData;

    // Extract 1st octet of address.
    pcStartData = pcString;
    pcEndData = ustrstr(pcStartData,".");
    ui32Block1 = ustrtoul(pcStartData, 0,10);

    // Extract 2nd octet of address.
    pcStartData = pcEndData +1;
    pcEndData = ustrstr(pcStartData,".");
    ui32Block2 = ustrtoul(pcStartData, 0,10);

    // Extract 3rd octet of address.
    pcStartData = pcEndData +1;
    pcEndData = ustrstr(pcStartData,".");
    ui32Block3 = ustrtoul(pcStartData, 0,10);

    // Extract 4th octet of address.
    pcStartData = pcEndData +1;
    pcEndData = ustrstr(pcStartData,".");
    ui32Block4 = ustrtoul(pcStartData, 0,10);

    // Validate data. Valid values are between 0->255.
    if((ui32Block1 > 255) || (ui32Block2 > 255) || (ui32Block3 > 255) ||
       (ui32Block4 > 255))
    {
        // Exit with failure if any values are > 255
        return(COMMAND_FAIL);
    }

    // Assign address values to variables passed in
    *pui8Val1 = (uint8_t)ui32Block1;
    *pui8Val2 = (uint8_t)ui32Block2;
    *pui8Val3 = (uint8_t)ui32Block3;
    *pui8Val4 = (uint8_t)ui32Block4;

    return(0);
}

int matt_socket()
{
       // Reset global socket type holder.
       g_ui32SocketType = 0;

       // Wait for DHCP process to finish. If you are using a static IP address
       // please delete the wait for DHCP event - ulCC3000DHCP
       while((g_ui32CC3000DHCP == 0) || (g_ui32CC3000Connected == 0))
       {
           hci_unsolicited_event_handler();

           //ROM_SysCtlDelay(1000);
           delay(1000);
       }

       // Socket is of type TCP.
       // Open socket.
       int i32Check = socket(AF_INET, SOCK_DGRAM, IPPROTO_TCP);

       // Set global variable that holds type of socket.
       g_ui32SocketType = IPPROTO_TCP;

       // Error checking.
       if(i32Check >= 0)
       {
           g_ui32Socket = i32Check;
           //return(0);
       }
       else
       {
           printf("    Socket Function error."
                       "    Error code %d.\r\n",i32Check);
           g_ui32SocketType = 0;
           return(-1);
       }

       printf("\r\n");

       return(0);
}

/*
 * Bind a socket
 */
int matt_bind()
{
	// bind to port
	uint32_t ui32Port = 0;
	int8_t i8Check = 0;

	// Validate input.
	if(g_ui32Socket == SENTINEL_EMPTY)
	{
		printf("    Socket: run socket() before bind.\r\n");
		return(1);
	}

	// Family is Always AF_INET on CC3000
	g_tSocketAddr.sa_family = AF_INET;

	// Set the port to bind the socket to.
	ui32Port = 1234;
	g_tSocketAddr.sa_data[0] = (ui32Port & 0xFF00) >> 8;
	g_tSocketAddr.sa_data[1] = (ui32Port & 0x00FF) >> 0;

	// Set IP to 0.0.0.0
	memset (&g_tSocketAddr.sa_data[2], 0, 4);

	// Low Level API call
	i8Check = bind(g_ui32Socket, &g_tSocketAddr, sizeof(sockaddr));

	if(i8Check == 0)
	{
	   // Set global flag variable to indicate the socket has been bound.
	   g_ui32BindFlag = 0;
	}
	else
	{
	   printf("    Bad bind. '%d'\r\n",i8Check);
	   // Set global flag variable to indicate the socket is not bound.
	   g_ui32BindFlag = SENTINEL_EMPTY;
	}

	printf("\r\n");

	return(0);
}

/*
 * Close socket
 */
int matt_close()
{
	// Close socket
	g_bSocketConnected = false;

	// Close the socket.
	int ui32Check = closesocket(g_ui32Socket);

	// Error checking.
	if(COMMAND_SUCCESS == ui32Check)
	{
	   printf("    Closing Socket.\r\n");
	   g_ui32Socket = SENTINEL_EMPTY;
	   return(COMMAND_SUCCESS);
	}
	else
	{
	   printf("    Socked.\r\n");
	}

    printf("\r\n");

    return(0);
}

/*
 * Socket send
 */
int matt_send()
{
	int32_t i32Check = 0;
	char * pui8Data;
	uint32_t ui32DataLength, ui32Port;
	uint8_t ui8IPBlock1, ui8IPBlock2, ui8IPBlock3, ui8IPBlock4;

	//char ipaddr[] = "67.215.65.132";  // weatherunderground
	//char ipaddr[] = "134.67.21.16";   // Airnow
	//char ipaddr[] = "74.125.225.212"; // google
	//char ipaddr[] = "192.220.73.220"; // nice http tutorial

	char ipaddr[] = "168.178.3.11"; 	// airquality.utah.gov

	i32Check = DotDecimalDecoder(ipaddr, &ui8IPBlock1, &ui8IPBlock2,
								 &ui8IPBlock3, &ui8IPBlock4);
	// Validate input.
	if(g_ui32Socket == SENTINEL_EMPTY)
	{
		printf("    Please call socket().\r\n");
		return(1);
	}

	// The data we want to send to the connected server
	char message[] = "GET /aqp/currentconditions.php?id=l4 HTTP/1.1\r\nHOST: www.airquality.utah.gov\r\n\n";

	// Data pointer.
	pui8Data = message;

	// Data length to send.
	ui32DataLength = strlen(message);

	// The family is always AF_INET on CC3000.
	g_tSocketAddr.sa_family = AF_INET;

	// The destination port.
	ui32Port = 80; // matt hard coded to 80
	g_tSocketAddr.sa_data[0] = (ui32Port & 0xFF00) >> 8;
	g_tSocketAddr.sa_data[1] = (ui32Port & 0x00FF) >> 0;

	// The destination IP address.
	g_tSocketAddr.sa_data[2] = ui8IPBlock1;
	g_tSocketAddr.sa_data[3] = ui8IPBlock2;
	g_tSocketAddr.sa_data[4] = ui8IPBlock3;
	g_tSocketAddr.sa_data[5] = ui8IPBlock4;

	// Call low level send Function.
	if(g_ui32SocketType == IPPROTO_TCP)
	{
		// Connect to TCP Socket on Server (if not already conneted)
		if(g_bSocketConnected == false)
		{
			//printf("    Calling Connect...\r\n");
			i32Check = connect(g_ui32Socket, &g_tSocketAddr, sizeof(sockaddr));
			if(i32Check == 0)
			{
				//printf("    Socket Connection Success!\r\n");
				g_bSocketConnected = true;

			}
			else
			{
				// Socket not connected
				//printf("    Connect failed  '%d'\r\n", i32Check);
				return(0);
			}
		}
		// Send TCP Packet.
		//printf("    Send the HTTP GET\r\n");
		i32Check = send(g_ui32Socket, pui8Data, ui32DataLength, 0);
	}

	return(0);
}

/*
 * Receive data
 */
float matt_recv(int *temp)
{
	*temp = 10;
	int32_t i32ReturnValue;
	uint32_t ui32x = 0, ui32Count = 0;
	volatile int count = 0;
	float pollution1 = 0;
	char *safe;

	// Validate Input.
	if((g_ui32Socket == SENTINEL_EMPTY) || (g_ui32BindFlag == SENTINEL_EMPTY))
	{
		printf("    Please Open a socket and Bind it to a port before "
				   "receiving data.\r\n");
		return(-1);
	}

	// Receive TCP data.
	if(g_ui32SocketType == IPPROTO_TCP)
	{
		// We've been asked to receive a TCP packet.
		printf("    Waiting for TCP Packets...\r\n");

		// Get all data received.  This may require multiple calls.
		do
		{
			// Get Data
			i32ReturnValue = recv(g_ui32Socket, g_pui8CC3000_Rx_Buffer,
								CC3000_APP_BUFFER_SIZE, 0);

			// Check Data Validity
			if((0 >= i32ReturnValue))
			{
				// No data received on first try
				printf("    No data received: %d.\r\n", i32ReturnValue);
				return(0);
			}

			printf("    Received %d bytes of data. %d time\r\n",
							i32ReturnValue, count);
			if(i32ReturnValue > 0)
			{
				count++;
			}

			for(ui32x = 0; ui32x < i32ReturnValue; ui32x++, ui32Count++)
			{
				if(g_pui8CC3000_Rx_Buffer[ui32x] == '\'' && g_pui8CC3000_Rx_Buffer[ui32x +1] == 'M' && g_pui8CC3000_Rx_Buffer[ui32x +2] == 'i') //&& g_pui8CC3000_Rx_Buffer[ui32x+2] == 'D')
				{
					printf("GOT ONE! \"Micrograms\" starts at RxBuff[%d]\r\n", ui32x + 1);
					printf("Pollution data digits to scan: [-21](%c) [-20](%c) [-19](%c) [-18](%c) [-17](%c)\r\n" , g_pui8CC3000_Rx_Buffer[ui32x-21], g_pui8CC3000_Rx_Buffer[ui32x-20] ,g_pui8CC3000_Rx_Buffer[ui32x-19],
							g_pui8CC3000_Rx_Buffer[ui32x-18], g_pui8CC3000_Rx_Buffer[ui32x-17]);

					int i = 0;
					int floatPointIndex = -1;
					for (i; i < 21; i++)
					{
						if(g_pui8CC3000_Rx_Buffer[ui32x - i] == '.')
							floatPointIndex = ui32x - i;
						//printf("FloatPointIndex: %d, i: %d \r\n", floatPointIndex, i);
					}

					if (floatPointIndex == 1)
					{
						printf("PARSE ERROR!!!!!!!!!!!!!!!!!!!!!\r\n");
					}

					char polstring[4] = { g_pui8CC3000_Rx_Buffer[floatPointIndex-1], g_pui8CC3000_Rx_Buffer[floatPointIndex] ,g_pui8CC3000_Rx_Buffer[floatPointIndex+1], '\0'};
					safe = NULL;
					printf("Polstring: %c %c %c %c\r\n", polstring[0], polstring[1], polstring[2], polstring[3]);

					pollution1 = ustrtof(polstring, &safe);
					printf("Pollution is: %f\r\n", pollution1);

				}
			}
		}while(count < 34);
	}
	return(pollution1);
}

//*****************************************************************************
//
// Manually connect to an open network. For advanced users.
// Arguments:
// [1]SSID
//
//*****************************************************************************
int
CMD_connect(char *ssid)
{
    uint32_t ui32SsidLen, ui32x;
    char *pui8Ssid;

    // Validate input.
    if(strlen(ssid) >= 255)
    {
        printf("Length of SSID must be less than 255\n");
        return(-1);
    }

    // Extract the SSID from the input parameters and determine the string
    // length.
    pui8Ssid = ssid;
    ui32SsidLen = ustrlen(ssid);

    // Call low level connect function. See documentation for more information.
#ifndef CC3000_TINY_DRIVER
    wlan_connect(WLAN_SEC_UNSEC, pui8Ssid, ui32SsidLen,NULL, NULL, 0);
#else
    wlan_connect(pui8Ssid,ui32SsidLen);
#endif

    printf("    Attempting to connect (5 second timeout)...\r\n");

    // Wait for connect message for 5 seconds,
    for(ui32x = 0; ui32x < 100000000; ui32x++)
    {
        // Check to see if we're connected to a network yet.
        if(g_ui32CC3000DHCP == 1)
        {
        	printf("DHCP Connected!\r\n");
            return(0);
        }

    }
    printf("    Connection Failed. Please check the network name.\n");

    return(0);
}

//*****************************************************************************
//
// main loop
//
//*****************************************************************************

int
main(void)
{
	int32_t webConnected = 0;
	int32_t num_msg_sent = 0;

	volatile float pollution = 0.1;
	volatile float old_pollution = 0.1;
	volatile int temp = 0;
	volatile int *temperature = &temp;

	g_ui32CC3000DHCP = 0;
	g_ui32CC3000Connected = 0;
	g_ui32Socket = SENTINEL_EMPTY;
	g_ui32BindFlag = SENTINEL_EMPTY;
	g_ui32SmartConfigFinished = 0;
	volatile int first = 1;
	volatile int again = 0;

	// Initialize use of GPIOs
	MSS_GPIO_init();

	// Setup the egg LED
	init_weather_air_display();
	insert_default_states();
	start_air_weather_display();

	// show a sequence of colors before
	// attempting to connect
	demo_air_quality_colors();

	// Initialize all board specific components.
	initDriver();

	// Try to connect to wifi
	if(CMD_connect("dd-wrt") < 0)
	{
		printf("Connect Failed\r\n");
	}

	// Loop forever
	while(g_ui32CC3000DHCP == 1)
	{
		// If wlan connect worked make connection with web server
		if(webConnected == 0)
		{
			// Open a TCP socket
			printf("Calling socket()\r\n\n");
			if(matt_socket() < 0)
			{
				exit(1);
			}

			// Bind to hard coded port
			printf("Calling bind()\r\n\n");
			if(matt_bind() < 0)
			{
				exit(1);
			}

			webConnected = 1;
		}

		// If we're connected and it is time to send a HTTP GET
		if((webConnected && (first || again)))
		{
			first = 0; // never use first again
			again = 0; // reset again for next time

			// Send hard coded http request
			printf("Calling send() to send HTTP GET\r\n\n");
			matt_send();
			num_msg_sent++;

			// bind on recv until we get our data
			printf("Calling recv()\r\n\n");
			pollution = matt_recv(temperature);
			printf("Pollution in main: %f\r\n", pollution);
		}

		// only change the pwm if we have something new
		if(pollution != old_pollution)
		{
			old_pollution = pollution;
			update_air_quality(pollution);
		}

		// Wait about 45 seconds before hitting the server again
		if(myTick >= 10)
		{
			printf("Request Again\r\n");
			again = 1;
			myTick = 0;
		}

		// If we are webconnected and we need to do another HTTP GET
		// we must start another connection so close this one
		if ((webConnected == 1) && (again))
		{
			printf("Calling close() to close connection and free socket\r\n\n");
			matt_close();
			webConnected = 0;
		}

	} //end while(1)
} // end main
