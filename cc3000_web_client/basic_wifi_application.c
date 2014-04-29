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


//*****************************************************************************
//! \addtogroup example_list
//! <h1>CC3000 Basic WiFi Example (cc3000_basic_wifi_application)</h1>
//!
//! This is a basic WiFi application for the CC3000 BoosterPack. This
//! application is a command line wrapper for various functions that the
//! CC3000 can provide. Please refer to the CC3000 wiki at
//! http://processors.wiki.ti.com/index.php/CC3000 for more information on
//! the commands provided.
//!
//! To see available commands type ``help'' at the serial terminal prompt.
//! The terminal is connected in 8-N-1 mode at 115200 baud.
//!
//! To use this example you must first connect to an existing unencrypted
//! wireless network. This can be done by using the ``smartconfig'' command
//! with the associated smartphone application. Alternatively, the connection
//! can be made manually by using the 'connect' command. Once connected you can
//! do any of the following.
//!
//! <b>Configure an IP address:</b>
//!
//! <ol>
//! <li>To use DHCP to allocate a dynamic IP address ``ipconfig'' or
//! ``ipconfig 0 0 0'' or,
//! <li>To allocate a static IP address use ``ipconfig a.b.c.d'' where
//! ``a.b.c.d'' is the required, dotted-decimal format address.
//! </ol>
//!
//! <b>Send and receive UDP data:</b>
//!
//! <ol>
//! <li>Open a UDP socket ``socketopen UDP''.
//! <li>Bind the socket to a local port ``bind 8080''.
//! <li>Send or receive data ``senddata 192.168.1.101 8080 helloworld'' or
//! ``receivedata''.  In the senddata case, the provided parameters identify
//! the IP address of the remote host and the remote port number to which the
//! data is to be sent.
//! </ol>
//!
//! <b>Send and receive TCP data:</b>
//!
//! <ol>
//! <li>Open a TCP socket ``socketopen TCP''.
//! <li>Bind the socket to a local port ``bind 8080''.
//! <li>Send a request to the remote server ``senddata 192.168.1.101 8080
//! helloworld''.  On the first ``senddata'' after opening the socket, the
//! socket is connected to the specified remote host and port.  On further
//! ``senddata'' requests, the remote address and port are ignored and the
//! existing connection is used.
//! <li>Receive data from the remote server ``receivedata''.
//! </ol>
//!
//! Note that, in the current implementation, the application only supports
//! acting as a TCP client.  The CC3000 also supports incoming connections
//! as required to operate as a TCP server but this example does not yet
//! include support for this feature.
//!
//! <b>Send mDNS advertisement:</b>
//!
//! <ol>
//! <li>``mdnsadvertise cc3000''
//! </ol>
//!
//! <b>Close the open socket:</b>
//!
//! <ol>
//! <li>``socketclose''
//! </ol>
//!
//! <b>Disconnect from network:</b>
//!
//! <ol>
//! <li>``disconnect''
//! </ol>
//!
//! <b>Reset the CC3000:</b>
//!
//! <ol>
//! <li>``resetcc3000''
//! </ol>
//!
//! <b>Delete connection policy:</b>
//!
//! This deletes the connection policy from CC3000 memory so that the device
//! won't auto connect whenever it is reset in future.
//!
//! <ol>
//! <li>``deletepolicy''
//! </ol>
//
//*****************************************************************************

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
    printf("Runtime error at line %d of file %s!\n", ui32Line, pcFilename);

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
//
// AES key "smartconfigAES16"
//
//*****************************************************************************
/* matt- not using this
const uint8_t g_pui8smartconfigkey[] = {0x73,0x6d,0x61,0x72,0x74,0x63,0x6f,
                                        0x6e,0x66,0x69,0x67,0x41,0x45,0x53,
                                        0x31,0x36};
*/
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

    //
    // Handle completion of simple config callback
    //
    if(lEventType == HCI_EVNT_WLAN_ASYNC_SIMPLE_CONFIG_DONE)
    {

        printf("\r    Rece'd Asynus Evnt dne "
                         "from CC30\n>");

        g_ui32SmartConfigFinished = 1;
        g_ui8StopSmartConfig = 1;
    }

    //
    // Handle unsolicited connect callback.
    //
    if(lEventType == HCI_EVNT_WLAN_UNSOL_CONNECT)
    {
        //
        // Set global variable to indicate connection.
        //
        g_ui32CC3000Connected = 1;
    }

    //
    // Handle unsolicited disconnect callback. Turn LED Green -> Red.
    //
    if(lEventType == HCI_EVNT_WLAN_UNSOL_DISCONNECT)
    {
        printf("\r    Received Unsot Disconnect from CC3000\n>");
        g_ui32CC3000Connected = 0;
        g_ui32CC3000DHCP = 0;
        g_ui32CC3000DHCP_configured = 0;

        //
        // Turn off the LED3 (Green)
        //
        turnLedOff(LED_1);

        //
        // Turn back on the LED 1 (RED)
        //
        turnLedOn(LED_0);
    }

    //
    // Handle DHCP connection callback.
    //
    if(lEventType == HCI_EVNT_WLAN_UNSOL_DHCP)
    {
        //
        // Notes:
        // 1) IP config parameters are received swapped
        // 2) IP config parameters are valid only if status is OK,
        //      i.e. g_ui32CC3000DHCP becomes 1
        //

        //
        // Only if status is OK, the flag is set to 1 and the addresses are
        // valid
        //

    	/*
    	printf("GOT the UNSOL_DHCP need to check its value is: 0x%x,\r\n",  *(pcData + NETAPP_IPCONFIG_MAC_OFFSET));
    	int i = 0;
    	printf("pcData { ");
    	for (i; i < ucLength; i++)
    	{
    		printf("0x%x, ", *(pcData + i));

    	}
    	printf("}\r\n");
*/

    	// the this is *pcData+ 20
        //if( *(pcData + NETAPP_IPCONFIG_MAC_OFFSET) == 0)
    	if( *(pcData + 4) == 0)
        {
            printf("\r    DHCP DoIP: %d.%d.%d.%d\r\n",
                        pcData[3],pcData[2],pcData[1],pcData[0]);

            //
            // DHCP success, set global accordingly.
            //
            g_ui32CC3000DHCP = 1;

            //
            // Turn on the LED3 (Green).
            //
            turnLedOn(LED_1);
        }
        else
        {
            //
            // DHCP failed, set global accordingly.
            //
            g_ui32CC3000DHCP = 0;
        }
    }

    //
    // Ping event handler
    //
    if(lEventType == HCI_EVNT_WLAN_ASYNC_PING_REPORT)
    {
        //
        // Ping data received, print to screen
        //
        psPingData = (netapp_pingreport_args_t *)pcData;
#ifdef DEBUG
        printf("Data Rec'd='\r\n");
        for(lEventType = 0; lEventType < ucLength; lEventType++)
        {
            printf("%d,",pcData[lEventType]);
        }
        printf("'\n");
#endif

        //
        // Test for ping failure
        //
        if(psPingData->min_round_time == -1)
        {
            printf("\r    Ping Failed\r\n>");
        }
        else
        {
            printf("\r    Ping Rests:\n"
                       "    snt: %d, rec'd: %d, min time: %dms,"
                       " mx tme: %dms, avg time: %dms\n>",
                       psPingData->packets_sent, psPingData->packets_received,
                       psPingData->min_round_time, psPingData->max_round_time,
                       psPingData->avg_round_time);
        }

    }

    //
    // Handle shutdown callback.
    //
    if(lEventType == HCI_EVENT_CC3000_CAN_SHUT_DOWN)
    {
        //
        // Set global variable to indicate the device can be shutdown.
        //
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

    // Initialize use of GPIOs
	MSS_GPIO_init();



    //
    // Initialize device pin configuration and the system clock.
    //
    pio_init();



    //
    // Initialize and configure the SPI.
    //
    init_spi(1000000, 100000000);


    //
    // Enable processor interrupts.
    //
    //MAP_IntMasterEnable();

    MSS_GPIO_enable_irq(SPI_IRQ_PIN); // Do this for each?
#ifdef VERBOSE
	printf("Starting Wlan_init!\r\n");
#endif
    //
    // Initialize and enable WiFi on the CC3000.
    //
    wlan_init(CC3000_UsynchCallback, sendWLFWPatch, sendDriverPatch,
              sendBootLoaderPatch, ReadWlanInterruptPin,
              WlanInterruptEnable, WlanInterruptDisable, WriteWlanPin);

	printf("Wlan_init done!\n\r");

    //
    // Start up the CC3000 stack.
    //
    wlan_start(0);
#ifdef VERBOSE
	printf("Done starting wlan!\r\n");
#endif
    //
    // Turn on the the red LED to indicate that we are active and initiated
    // WLAN successfully.
    //
    turnLedOn(LED_1);

    //
    // Mask out all non-required events from CC3000.
    //
    // Matt - this is completing
    wlan_set_event_mask(HCI_EVNT_WLAN_KEEPALIVE | HCI_EVNT_WLAN_UNSOL_INIT);

    //
    // Initialize UART.
    //
    //DispatcherUARTConfigure(SysCtlClockGet()); // I think our UART is already set up
    //ROM_SysCtlDelay(1000000); // So we don't need this either

    //
    // Print version string.
    //
    //printf("\n\n\rCC3000 Basic Wifi Application!\n\r");

    //
    // Set flag to stop smart config if running.
    //
    g_ui8StopSmartConfig = 0;

    //
    // Configure and enable the system tick.
    //
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

    //
    // Extract 1st octet of address.
    //
    pcStartData = pcString;
    pcEndData = ustrstr(pcStartData,".");
    ui32Block1 = ustrtoul(pcStartData, 0,10);

    //
    // Extract 2nd octet of address.
    //
    pcStartData = pcEndData +1;
    pcEndData = ustrstr(pcStartData,".");
    ui32Block2 = ustrtoul(pcStartData, 0,10);

    //
    // Extract 3rd octet of address.
    //
    pcStartData = pcEndData +1;
    pcEndData = ustrstr(pcStartData,".");
    ui32Block3 = ustrtoul(pcStartData, 0,10);

    //
    // Extract 4th octet of address.
    //
    pcStartData = pcEndData +1;
    pcEndData = ustrstr(pcStartData,".");
    ui32Block4 = ustrtoul(pcStartData, 0,10);

    //
    // Validate data. Valid values are between 0->255.
    //
    if((ui32Block1 > 255) || (ui32Block2 > 255) || (ui32Block3 > 255) ||
       (ui32Block4 > 255))
    {
        //
        // Exit with failure if any values are > 255
        //
        return(COMMAND_FAIL);
    }

    //
    // Assign address values to variables passed in
    //
    *pui8Val1 = (uint8_t)ui32Block1;
    *pui8Val2 = (uint8_t)ui32Block2;
    *pui8Val3 = (uint8_t)ui32Block3;
    *pui8Val4 = (uint8_t)ui32Block4;

    return(0);
}


//*****************************************************************************
//
// This function triggers smart configuration processing on the CC3000.
//
//*****************************************************************************
/* matt - not doing smartconif at the moment
void StartSmartConfig(void)
{
    g_ui32SmartConfigFinished = 0;
    g_ui32CC3000Connected = 0;
    g_ui32CC3000DHCP = 0;
    g_ui32OkToDoShutDown = 0;

    //
    // Reset all the previous configuration
    //
    wlan_ioctl_set_connection_policy(DISABLE, DISABLE, DISABLE);

    //
    // Delete all previous profiles. 255 means delete all.
    //
    wlan_ioctl_del_profile(255);

    //
    // Inform user of status.
    //
    printf("    Waiting on Smartphone Smart Config App...\n");

    //
    // Wait until CC3000 is disconnected
    //
    while(g_ui32CC3000Connected == 1)
    {
        //ROM_SysCtlDelay(100);
    	delay(100);
        hci_unsolicited_event_handler();
    }

    //
    // Start blinking red LED during Smart Configuration process.
    //
    turnLedOn(LED_0);

    //
    // Create new entry for AES encryption key.
    //
    //nvmem_create_entry(NVMEM_AES128_KEY_FILEID,16);

    //
    // Write AES key to NVMEM.
    //
    //aes_write_key((unsigned char *)(&g_pui8smartconfigkey[0]));


    //
    // Set the prefix used for smart config.
    //
    //wlan_smart_config_set_prefix((char *)g_pcCC3000_prefix); //implement this if we want smart config
    turnLedOff(LED_0);


    //
    // Start the SmartConfig process.
    //
    //wlan_smart_config_start(0); // matt edited this. it was was 1
    turnLedOn(LED_0);

    //
    // Wait for Smart config to finish. Flash LED.
    //
    while(g_ui32SmartConfigFinished == 0)
    {
        turnLedOff(LED_0);
        //ROM_SysCtlDelay(16500000);
        delay(16500000);
        turnLedOn(LED_0);
        //ROM_SysCtlDelay(16500000);
        delay(16500000);
    }
    turnLedOn(LED_0);

    //
    // Create new entry for AES encryption key.
    //
    //nvmem_create_entry(NVMEM_AES128_KEY_FILEID,16);

    //
    // Write AES key to NVMEM.
    //
    //aes_write_key((unsigned char *)(&g_pui8smartconfigkey[0]));

    //
    // Decrypt configuration information and add profile.
    //
    //wlan_smart_config_process();

    //
    // Configure to connect automatically to the AP retrieved in the
    // Smart config process.
    //
    wlan_ioctl_set_connection_policy(DISABLE, DISABLE, ENABLE);

    //
    // Reset the CC3000, necessary to apply configuration.
    //
    wlan_stop();

    //
    // Tell user we're done with smart config.
    //
    printf("\r    Smart Config DONE\n");

    //
    // Mandatory delay between calls to wlan_stop and wlan_start.
    //
    //ROM_SysCtlDelay(100000);
    delay(100000);

    //
    // Start up the CC3000 again.
    //
    wlan_start(0);

    //
    // Mask out all non-required events, these events will be ignored by the
    // asynchronous callback.
    //
    wlan_set_event_mask(HCI_EVNT_WLAN_KEEPALIVE | HCI_EVNT_WLAN_UNSOL_INIT);
}
*/
//*****************************************************************************
//
// Print the help strings for all commands.
//
//*****************************************************************************
/* matt - not used anymore
int
CMD_help(int argc, char **argv)
{
    int32_t i32Index;

    (void)argc;
    (void)argv;

    //
    // Start at the beginning of the command table.
    //
    i32Index = 0;

    //
    // Get to the start of a clean line on the serial output.
    //
    printf("\nAvailable Commands\n------------------\n\n");

    //
    // Display strings until we run out of them.
    //
    while(g_psCmdTable[i32Index].pcCmd)
    {
        //
        // Display help information for a single command.
        //
        printf("%17s %s\n", g_psCmdTable[i32Index].pcCmd,
                   g_psCmdTable[i32Index].pcHelp);
        i32Index++;

        //
        // Make sure we've sent all the UART data before we add more to the
        // buffer.
        //
        UARTFlushTx(false);
    }

    //
    // Leave a blank line after the help strings.
    //
    printf("\n");

    return(0);
}
*/

int matt_socket()
{
       //
       // Reset global socket type holder.
       //
       g_ui32SocketType = 0;

       //
       // Wait for DHCP process to finish. If you are using a static IP address
       // please delete the wait for DHCP event - ulCC3000DHCP
       //
       while((g_ui32CC3000DHCP == 0) || (g_ui32CC3000Connected == 0))
       {
           hci_unsolicited_event_handler();

           //ROM_SysCtlDelay(1000);
           delay(1000);
       }

       //
       // Socket is of type TCP.
       //
           //
           // Open socket.
           //
           int i32Check = socket(AF_INET, SOCK_DGRAM, IPPROTO_TCP);

           //
           // Set global variable that holds type of socket.
           //
           g_ui32SocketType = IPPROTO_TCP;

           //
           // Inform user of the socket being opened.
           //
           //printf("    Socket Type: TCP\r\n");

       //
       // Error checking.
       //
       if(i32Check >= 0)
       {
           //printf("    Socket Handle is '%d'\r\n",i32Check);
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


int matt_bind()
{
       // bind to port
            uint32_t ui32Port = 0;
           int8_t i8Check = 0;

           //
           // Validate input.
           //

           if(g_ui32Socket == SENTINEL_EMPTY)
           {
               printf("    Socket: run socket() before bind.\r\n");
               return(1);
           }

           //
           // Family is Always AF_INET on CC3000
           //
           g_tSocketAddr.sa_family = AF_INET;

           //
           // Set the port to bind the socket to.
           //
           ui32Port = 1234;
           g_tSocketAddr.sa_data[0] = (ui32Port & 0xFF00) >> 8;
           g_tSocketAddr.sa_data[1] = (ui32Port & 0x00FF) >> 0;

           //
           // Set IP to 0.0.0.0
           //
           memset (&g_tSocketAddr.sa_data[2], 0, 4);

           //
           // Low Level API call
           //
           i8Check = bind(g_ui32Socket, &g_tSocketAddr, sizeof(sockaddr));

           if(i8Check == 0)
           {
              // printf("    Bind Success on port: %d\r\n",
                          //(g_tSocketAddr.sa_data[0] << 8) + g_tSocketAddr.sa_data[1]);

               //
               // Set global flag variable to indicate the socket has been bound.
               //
               g_ui32BindFlag = 0;
           }
           else
           {
               printf("    Bad bind. '%d'\r\n",i8Check);

               //
               // Set global flag variable to indicate the socket is not bound.
               //
               g_ui32BindFlag = SENTINEL_EMPTY;
           }


           printf("\r\n");

           return(0);
}


int matt_close()
{

       // Close socket
       g_bSocketConnected = false;

       //
       // Close the socket.
       //
       int ui32Check = closesocket(g_ui32Socket);

       //
       // Error checking.
       //
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

int matt_send()
{
	    int32_t i32Check = 0;
	    char * pui8Data;
	    uint32_t ui32DataLength, ui32Port;
	    uint8_t ui8IPBlock1, ui8IPBlock2, ui8IPBlock3, ui8IPBlock4;

	    //char ipaddr[] = "67.215.65.132";  //weatherunderground
	    //char ipaddr[] = "134.67.21.16";   //Airnow
	    //char ipaddr[] = "74.125.225.212"; // google
	    //char ipaddr[] = "192.220.73.220"; // nice http tutorial
	    char ipaddr[] = "168.178.3.11"; 	// airquality.utah.gov

	    i32Check = DotDecimalDecoder(ipaddr, &ui8IPBlock1, &ui8IPBlock2,
	                                 &ui8IPBlock3, &ui8IPBlock4);
	    //
	    // Validate input.
	    //
	    if(g_ui32Socket == SENTINEL_EMPTY)
	    {
	        printf("    Please call socket().\r\n");
	        return(1);
	    }

	    // matt - might want to add this back in if we start sending different messages
	    //
	    // Validate message size, between 1 and 1460 bytes.
	    //
	    //else if((strlen(argv[3]) > 1460) || (strlen(argv[3]) < 1) )
	    //{
	    //    printf("Invalid Message, must send 1-1460 bytes\n");
	    //    return(1);
	    //}

	    // The data we want to send to the connected server
	    //char message[] = "GET /aqp/currentconditions.php?id=slc HTTP/1.1\r\nHOST: www.airquality.utah.gov\r\n\n";
	    char message[] = "GET /aqp/currentconditions.php?id=l4 HTTP/1.1\r\nHOST: www.airquality.utah.gov\r\n\n";

	    //
	    // Data pointer.
	    //
	    pui8Data = message;

	    //
	    // Data length to send.
	    //
	    ui32DataLength = strlen(message);

	    //
	    // The family is always AF_INET on CC3000.
	    //
	    g_tSocketAddr.sa_family = AF_INET;

	    //
	    // The destination port.
	    //
	    ui32Port = 80; // matt hard coded to 80
	    g_tSocketAddr.sa_data[0] = (ui32Port & 0xFF00) >> 8;
	    g_tSocketAddr.sa_data[1] = (ui32Port & 0x00FF) >> 0;

	    //
	    // The destination IP address.
	    //
	    g_tSocketAddr.sa_data[2] = ui8IPBlock1;
	    g_tSocketAddr.sa_data[3] = ui8IPBlock2;
	    g_tSocketAddr.sa_data[4] = ui8IPBlock3;
	    g_tSocketAddr.sa_data[5] = ui8IPBlock4;

	    //
	    // Call low level send Function.
	    //

	    if(g_ui32SocketType == IPPROTO_TCP)
	    {
	        //
	        // Connect to TCP Socket on Server (if not already conneted)
	        //
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
	                //
	                // Socket not connected
	                //
	                //printf("    Connect failed  '%d'\r\n", i32Check);
	                return(0);
	            }
	        }
	        //
	        // Send TCP Packet.
	        //
	        //printf("    Send the HTTP GET\r\n");
	        i32Check = send(g_ui32Socket, pui8Data, ui32DataLength, 0);
	    }

	    //
	    // Validate completion of send.
	    //
	    if(i32Check == -1)
	    {
	        //printf("    Send ode '%d'\n",i32Check);
	    }
	    else
	    {
	        //printf("    Sent Data: %d bytes.\r\n", i32Check);
	    }

	    return(0);
}


float matt_recv(int *temp)
{

		*temp = 10;
	    int32_t i32ReturnValue;
	    uint32_t ui32x = 0, ui32Count = 0;
	    volatile int count = 0;
	    float pollution1 = 0;
        char *safe;
        char parseBuffer[300];
        int parseCounter = 0;

	    //
	    // Validate Input.
	    //
	    if((g_ui32Socket == SENTINEL_EMPTY) || (g_ui32BindFlag == SENTINEL_EMPTY))
	    {
	        printf("    Please Open a socket and Bind it to a port before "
	                   "receiving data.\r\n");
	        return(-1);
	    }

	    //
	    // Receive TCP data.
	    //
	    if(g_ui32SocketType == IPPROTO_TCP)
	    {
	        //
	        // We've been asked to receive a TCP packet.
	        //
	        printf("    Waiting for next request...\r\n");

	        //
	        // Get all data received.  This may require multiple calls.
	        //
	        do
	        {

	            //
	            // Get Data
	            //

	            i32ReturnValue = recv(g_ui32Socket, g_pui8CC3000_Rx_Buffer,
	                                CC3000_APP_BUFFER_SIZE, 0);

	           // volatile int i = 1000000;
	           // while(i !=0)
	           // i--;

	            //
	            // Check Data Validity
	            //
	            if((0 >= i32ReturnValue))
	            {
	                //
	                // No data received on first try
	                //
	                printf("    No data received: %d.\r\n", i32ReturnValue);
	                return(0);
	            }

	            //
	            // Print data to screen
	            //
	                printf("    Received %d bytes of data. %d time\r\n",
	                            i32ReturnValue, count);
	            if(i32ReturnValue > 0)
	            {
	                count++;
	            }




	            for(ui32x = 0; ui32x < i32ReturnValue; ui32x++, ui32Count++)
	            {
	                //
	                // Add column wrapping to make output pretty.
	                //
	               // if( ((ui32Count % 60) == 0) && (ui32Count > 0))
	               // {
	               //     printf("\r\n    ");
	               // }

	                //
	                // Print text to screen
	                //

	             	//if(g_pui8CC3000_Rx_Buffer[ui32x] == '2' && g_pui8CC3000_Rx_Buffer[ui32x +1] == '.' && g_pui8CC3000_Rx_Buffer[ui32x +2] == '5') //&& g_pui8CC3000_Rx_Buffer[ui32x+2] == 'D')
	            	//{
	                    //parseBuffer[parseCounter] = g_pui8CC3000_Rx_Buffer[ui32x +2];
	                    //parseCounter++;

	            	//}

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
	                     //char polstring[7] = { g_pui8CC3000_Rx_Buffer[ui32x-21], g_pui8CC3000_Rx_Buffer[ui32x-20] ,g_pui8CC3000_Rx_Buffer[ui32x-19],
	             		//		g_pui8CC3000_Rx_Buffer[ui32x-18], g_pui8CC3000_Rx_Buffer[ui32x-17], '\0'};

	             		pollution1 = ustrtof(polstring, &safe);
	                    printf("Pollution is: %f\r\n", pollution1);

	            	}

	                 //UARTprintf("%c",g_pui8CC3000_Rx_Buffer[ui32x]);
	            }



	        }while(count < 34);   //while(i32ReturnValue == CC3000_APP_BUFFER_SIZE); //while(found == 0);
	    }

        //	    printf("Leaving Recv\r\n\n");
       // printf("Pollution right before return: %f\r\n", pollution1);
	    return(pollution1);




	/*
    int32_t i32ReturnValue;
    uint32_t ui32x = 0, ui32Count = 0;
    volatile int count = 0;
    //after_rec = 1;

    //
    // Validate Input.
    //
    if((g_ui32Socket == SENTINEL_EMPTY) || (g_ui32BindFlag == SENTINEL_EMPTY))
    {
        printf("    Please Ore "
                   "receiving data.\n");
        return(1);
    }

    //
    // Receive TCP data.
    //
    if(g_ui32SocketType == IPPROTO_TCP)
    {
        //
        // We've been asked to receive a TCP packet.
        //
        //printf("    Looking for TCP Packets...\r\n");

        //
        // Get all data received.  This may require multiple calls.
        //
        do
        {

            //
            // Get Data
            //
        	//printf("calling recv\r\n");
            i32ReturnValue = recv(g_ui32Socket, g_pui8CC3000_Rx_Buffer,
                                CC3000_APP_BUFFER_SIZE, 0);


            //ROM_SysCtlDelay(10000000);     // TODO not urgent but change receive so it does not need to wait
            //delay(10000000);				 // or so it waits until the whole http request is sent

            //
            // Check Data Validity
            //
            if((0 >= i32ReturnValue))
            {
                //
                // No data received on first try
                //
                printf("    No data received: %d.\n", i32ReturnValue);
                return(0);
            }

            //
            // Print data to screen
            //
                printf("    Received %d bytes of data. %d time\n",
                            i32ReturnValue, count);

                count++;

            for(ui32x = 0; ui32x < i32ReturnValue; ui32x++, ui32Count++)
            {
                //
                // Add column wrapping to make output pretty.
                //
                if( ((ui32Count % 60) == 0) && (ui32Count > 0))
                {
                    printf("\n    ");
                }

                // Looks for PM in the rec'd text. TODO Change this to capture what comes after PM and temperature. There are nice
                // functions in the utils.c to compare strings
            	if(g_pui8CC3000_Rx_Buffer[ui32x] == 'P' && g_pui8CC3000_Rx_Buffer[ui32x +1] == 'M') //&& g_pui8CC3000_Rx_Buffer[ui32x+2] == 'D')
            	{
            		printf("GOT ONE!\r\n");
            		printf("%c%c%c%c%c" ,g_pui8CC3000_Rx_Buffer[ui32x], g_pui8CC3000_Rx_Buffer[ui32x+1], g_pui8CC3000_Rx_Buffer[ui32x+2],
            				g_pui8CC3000_Rx_Buffer[ui32x+3] ,g_pui8CC3000_Rx_Buffer[ui32x+4] );

            	}

                //
                // Print text to screen
                //
                 printf("%c",g_pui8CC3000_Rx_Buffer[ui32x]);
            }

            //UARTFlushTx(false);  //TODO we did not have a flush in our driver but we're probably fine without it

        }while(i32ReturnValue == CC3000_APP_BUFFER_SIZE); //while(found == 0); //while(i32ReturnValue == CC3000_APP_BUFFER_SIZE);
    }

    printf("'\n\n");

    return(0);
*/
}

//*****************************************************************************
//
//
//
//*****************************************************************************
/*
int
CMD_matt(int argc, char **argv)
{
    printf("Calling socket()\n\n");
	matt_socket();
    printf("Calling bind()\n\n");
	matt_bind();
    printf("Calling send() to send HTTP GET\n\n");
	matt_send();
    printf("Calling recv()\n\n");
    matt_recv();
    printf("Calling close() to close connection and free socket\n\n");
	matt_close();

return 0;
}
*/
//*****************************************************************************
//
// This function runs the Smart Configuration process.
// This function is meant to be run in tandem with the smartphone
// application. The user enters the network information on the smart phone and
// then saves it to the microcontroller. Easiest way to connect CC3000 to a
// network.
//
//*****************************************************************************
/* Now directly calling connect from main
int
CMD_smartConfig(int argc, char **argv)
{
    StartSmartConfig();

    return(0);
}
*/
//*****************************************************************************
//
// Manually connect to an open network. For advanced users.
// Arguments:
// [1]SSID
//
//*****************************************************************************
int
//CMD_connect(int argc, char **argv)
CMD_connect(char *ssid)
{
    uint32_t ui32SsidLen, ui32x;
    char *pui8Ssid;

    //
    // Validate input.
    //


    if(strlen(ssid) >= 255)
    {
        printf("Length of SSID must be less than 255\n");
        return(-1);
    }

    //
    // Extract the SSID from the input parameters and determine the string
    // length.
    //
    //pui8Ssid = argv[1];
    pui8Ssid = ssid;
    //ui32SsidLen = ustrlen(argv[1]);
    ui32SsidLen = ustrlen(ssid);

    //
    // Call low level connect function. See documentation for more information.
    //
#ifndef CC3000_TINY_DRIVER
    wlan_connect(WLAN_SEC_UNSEC, pui8Ssid, ui32SsidLen,NULL, NULL, 0);
#else
    wlan_connect(pui8Ssid,ui32SsidLen);
#endif

    //
    // Tell user what we're doing.
    //
    printf("    Attempting to connect (5 second timeout)...\r\n");

    //
    // Wait for connect message for 5 seconds,
    //
    for(ui32x = 0; ui32x < 100000000; ui32x++)
    {
        //
        // Check to see if we're connected to a network yet.
        //
        if(g_ui32CC3000DHCP == 1)
        {
        	printf("DHCP Connected!\r\n");
            return(0);
        }

        //
        // Delay 1ms
        //
        //ROM_SysCtlDelay(ROM_SysCtlClockGet() / 3000);
    }
    printf("    Connection Failed. Please check the network name.\n");

    return(0);
}

//*****************************************************************************
//
// Open a UDP or TCP socket.
// Arguments:
// [1] 'TCP' or 'UDP' to specify socket type
//
//*****************************************************************************
/* matt_socket is based off this
int
CMD_socketOpen (int argc, char **argv)
{
    int32_t i32Check = 0;

    //
    // Validate input.
    //
    if(argc < 2)
    {
        printf("    Please specify socket type, 'TCP' or 'UDP'\n");
        return(CMDLINE_TOO_FEW_ARGS);
    }
    else if(argc > 2)
    {
        return(CMDLINE_TOO_MANY_ARGS);
    }
    else if(argc == 2)
    {
        if((argv[1][0] != 'u') && (argv[1][0] != 'U') &&
           (argv[1][0] != 't') && (argv[1][0] != 'T'))
        {
            printf("    Please provide Type of Socket 'UDP' or 'TCP' .\n");
            return(CMDLINE_INVALID_ARG);
        }
    }

    //
    // Reset global socket type holder.
    //
    g_ui32SocketType = 0;

    //
    // Wait for DHCP process to finish. If you are using a static IP address
    // please delete the wait for DHCP event - ulCC3000DHCP
    //
    while((g_ui32CC3000DHCP == 0) || (g_ui32CC3000Connected == 0))
    {
        hci_unsolicited_event_handler();

        ROM_SysCtlDelay(1000);
    }

    //
    // Socket is of type UDP.
    //
    if((argv[1][0] == 'U') || (argv[1][0] == 'u'))
    {
        //
        // Open socket.
        //
        i32Check = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        //
        // Set global variable that holds type of socket.
        //
        g_ui32SocketType = IPPROTO_UDP;

        //
        // Inform user of the socket being opened.
        //
        printf("    Socket is of type UDP\n");
    }

    //
    // Socket is of type TCP.
    //
    else if((argv[1][0] == 't') || (argv[1][0] == 'T'))
    {
        //
        // Open socket.
        //
        i32Check = socket(AF_INET, SOCK_DGRAM, IPPROTO_TCP);

        //
        // Set global variable that holds type of socket.
        //
        g_ui32SocketType = IPPROTO_TCP;

        //
        // Inform user of the socket being opened.
        //
        printf("    Socket is of type TCP\n");
    }

    //
    // Error checking.
    //
    if(i32Check >= 0)
    {
        printf("    Socket Handle is '%d'\n",i32Check);
        g_ui32Socket = i32Check;
        return(0);
    }
    else
    {
        printf("    Socket Function returned an error."
                    "   Socket not opened. Error code %d.\n",i32Check);
        g_ui32SocketType = 0;
        return(0);
    }
}
*/
//*****************************************************************************
//
// Close the open socket.
// Arguments: None
//
//*****************************************************************************
/* matt_close is based off this
int
CMD_socketClose (int argc, char **argv)
{
    uint32_t ui32Check;

    //
    // Shut down open socket connection for TCP
    //
    g_bSocketConnected = false;

    //
    // Close the socket.
    //
    ui32Check = closesocket(g_ui32Socket);

    //
    // Error checking.
    //
    if(COMMAND_SUCCESS == ui32Check)
    {
        printf("    Socket closed successfully.\n");
        g_ui32Socket = SENTINEL_EMPTY;
        return(COMMAND_SUCCESS);
    }
    else
    {
        printf("    Socket close Failed.\n");
    }
    return(0);
}
*/
//*****************************************************************************
//
// Send Data to a destination port at a given IP Address.
// Arguments:
// [1] IP Address in the form xxx.XXX.yyy.YYY
// [2] Port as an integer 0->65536
// [3] Data to send as a string, < 255 bytes, no spaces
//
//*****************************************************************************
/* matt_send is based off this
int
CMD_sendData (int argc, char **argv)
{
    int32_t i32Check = 0;
    char * pui8Data;
    uint32_t ui32DataLength, ui32Port;
    uint8_t ui8IPBlock1, ui8IPBlock2, ui8IPBlock3, ui8IPBlock4;

    //
    // Extract IP address.
    //
    i32Check = DotDecimalDecoder(argv[1], &ui8IPBlock1, &ui8IPBlock2,
                                 &ui8IPBlock3, &ui8IPBlock4);

    //
    // Validate input.
    //
    if(g_ui32Socket == SENTINEL_EMPTY)
    {
        printf("    Please Open a socket before tying this command.\n");
        return(CMDLINE_INVALID_ARG);
    }
    else if(argc < 4)
    {
        return(CMDLINE_TOO_FEW_ARGS);
    }
    else if(argc > 4)
    {
        return(CMDLINE_TOO_MANY_ARGS);
    }
    //
    // Validate message size, between 1 and 1460 bytes.
    //
    else if((strlen(argv[3]) > 1460) || (strlen(argv[3]) < 1) )
    {
        printf("Invalid Message, must send 1-1460 bytes\n");
        return(CMDLINE_INVALID_ARG);
    }
    //
    // Validate port is between 0 and 65535.
    //
    else if( ustrtoul(argv[2],0,10) > 65536)
    {
        printf("    Port must be between 0 and  65535.\n");
        return(CMDLINE_INVALID_ARG);
    }
    //
    // Check return value of dot decimal converter.
    //
    else if(i32Check == COMMAND_FAIL)
    {
        printf("    Invalid IP Address. Valid IP is 0.0.0.0 -> "
                    "255.255.255.255\n");
        return(CMDLINE_INVALID_ARG);
    }

    //
    // Data pointer.
    //
    pui8Data = argv[3];

    //
    // Data length to send.
    //
    ui32DataLength = strlen(argv[3]);

    //
    // The family is always AF_INET on CC3000.
    //
    g_tSocketAddr.sa_family = AF_INET;

    //
    // The destination port.
    //
    ui32Port = ustrtoul(argv[2],0,10);
    g_tSocketAddr.sa_data[0] = (ui32Port & 0xFF00) >> 8;
    g_tSocketAddr.sa_data[1] = (ui32Port & 0x00FF) >> 0;

    //
    // The destination IP address.
    //
    g_tSocketAddr.sa_data[2] = ui8IPBlock1;
    g_tSocketAddr.sa_data[3] = ui8IPBlock2;
    g_tSocketAddr.sa_data[4] = ui8IPBlock3;
    g_tSocketAddr.sa_data[5] = ui8IPBlock4;

    //
    // Call low level send Function.
    //
    if(g_ui32SocketType == IPPROTO_UDP)
    {
        //
        // Send UDP packet.
        //
        printf("    Sending UDP Packet...\n");
        i32Check = sendto(g_ui32Socket, pui8Data, ui32DataLength, 0,
                          &g_tSocketAddr,sizeof(sockaddr));
    }
    else if(g_ui32SocketType == IPPROTO_TCP)
    {
        //
        // Connect to TCP Socket on Server (if not already conneted)
        //
        if(g_bSocketConnected == false)
        {
            printf("    Connecting to TCP Socket on Server...\n");
            i32Check = connect(g_ui32Socket, &g_tSocketAddr, sizeof(sockaddr));
            if(i32Check != 0)
            {
                printf("    Connect failed with error code '%d'\n", i32Check);
                printf("    Please make sure there is a server with the "
                           "specified socket open to connect to\n");
                return(0);
            }
            else
            {
                //
                // Socket connected successfully
                //
                g_bSocketConnected = true;
            }
        }
        //
        // Send TCP Packet.
        //
        printf("    Sending TCP Packet...\n");
        i32Check = send(g_ui32Socket, pui8Data, ui32DataLength, 0);
    }

    //
    // Validate completion of send.
    //
    if(i32Check == -1)
    {
        printf("    Send Data Failed with code '%d'\n",i32Check);
    }
    else
    {
        printf("    Send Data Success: sent %d bytes.\n", i32Check);
    }

    return(0);
}
*/
//*****************************************************************************
//
// Receives data from the opened socket on the binded port. Prints received
// data to the terminal.
//
// Arguments: None
//
// Requires SocketOpen and Bind to have been previously called.
//
//*****************************************************************************
/*
int
CMD_receiveData (int argc, char **argv)
{
    int32_t i32ReturnValue;
    socklen_t tRxPacketLength;
    uint32_t ui32x = 0, ui32Count = 0;
    bool bRunOnce = true;

    //
    // Validate Input.
    //
    if((g_ui32Socket == SENTINEL_EMPTY) || (g_ui32BindFlag == SENTINEL_EMPTY))
    {
        printf("    Please Open a socket and Bind it to a port before "
                   "receiving data.\n");
        return(CMDLINE_BAD_CMD);
    }

    //
    // Receive UDP Data.
    //
    if(g_ui32SocketType == IPPROTO_UDP)
    {
        //
        // Tell user what we're doing.
        //
        printf("    Looking for UDP Packets...\n");

        //
        // Get all data received.  This may require multiple calls.
        //
        do
        {
            //
            // Get Data
            //
            i32ReturnValue = recvfrom(g_ui32Socket, g_pui8CC3000_Rx_Buffer,
                                    CC3000_APP_BUFFER_SIZE, 0, &g_tSocketAddr,
                                    &tRxPacketLength);

            //
            // Check Data Validity
            //
            if(bRunOnce && (0 >= i32ReturnValue))
            {
                //
                // No data received on first try
                //
                printf("    No data received.\n");
                return(0);
            }

            //
            // Print data to screen
            //
            if(bRunOnce)
            {
                printf("    Received %d bytes of data: \n    '",
                            i32ReturnValue);
            }
            for(ui32x = 0; ui32x < i32ReturnValue; ui32x++, ui32Count++)
            {
                //
                // Add column wrapping to make output pretty
                //
                if( ((ui32Count % 60) == 0) && (ui32Count > 0))
                {
                    printf("\n    ");
                }

                //
                // Print text to screen
                //
                printf("%c",g_pui8CC3000_Rx_Buffer[ui32x]);
            }
            bRunOnce = false;

        }while(i32ReturnValue == CC3000_APP_BUFFER_SIZE);
    }
    //
    // Receive TCP data.
    //
    else if(g_ui32SocketType == IPPROTO_TCP)
    {
        //
        // We've been asked to receive a TCP packet.
        //
        printf("    Looking for TCP Packets...\n");

        //
        // Get all data received.  This may require multiple calls.
        //
        do
        {
            //
            // Get Data
            //
            i32ReturnValue = recv(g_ui32Socket, g_pui8CC3000_Rx_Buffer,
                                CC3000_APP_BUFFER_SIZE, 0);

            //
            // Check Data Validity
            //
            if(bRunOnce && (0 >= i32ReturnValue))
            {
                //
                // No data received on first try
                //
                printf("    No data received.\n");
                return(0);
            }

            //
            // Print data to screen
            //
            if(bRunOnce)
            {
                printf("    Received %d bytes of data: \n    '",
                            i32ReturnValue);
            }
            for(ui32x = 0; ui32x < i32ReturnValue; ui32x++, ui32Count++)
            {
                //
                // Add column wrapping to make output pretty.
                //
                if( ((ui32Count % 60) == 0) && (ui32Count > 0))
                {
                    printf("\n    ");
                }

                //
                // Print text to screen
                //
                printf("%c",g_pui8CC3000_Rx_Buffer[ui32x]);
            }
            bRunOnce = false;

        }while(i32ReturnValue == CC3000_APP_BUFFER_SIZE);
    }

    printf("'\n\n");

    return(0);
}
*/
//*****************************************************************************
//
// Bind the open socket to a selected port.
// Arguments:
// [1] Port to Bind to
//
// Requires Socket Open before running.
//
//*****************************************************************************
/*
int
CMD_bind (int argc, char **argv)
{
    uint32_t ui32Port = 0;
    int8_t i8Check = 0;

    //
    // Validate input.
    //
    if( (ustrtoul(argv[1],0,10)) > (65536) )
    {
        printf("    Invalid Port, must be 0->65536\n");
        return(CMDLINE_INVALID_ARG);
    }
    else if(g_ui32Socket == SENTINEL_EMPTY)
    {
        printf("    Socket not open, please run socketopen.\n");
        return(0);
    }
    else if( argc < 2 )
    {
        return(CMDLINE_TOO_FEW_ARGS);
    }
    else if( argc > 2 )
    {
        return(CMDLINE_TOO_MANY_ARGS);
    }

    //
    // Family is Always AF_INET on CC3000
    //
    g_tSocketAddr.sa_family = AF_INET;

    //
    // Set the port to bind the socket to.
    //
    ui32Port = ustrtoul(argv[1],0,10);
    g_tSocketAddr.sa_data[0] = (ui32Port & 0xFF00) >> 8;
    g_tSocketAddr.sa_data[1] = (ui32Port & 0x00FF) >> 0;

    //
    // Set IP to 0.0.0.0
    //
    memset (&g_tSocketAddr.sa_data[2], 0, 4);

    //
    // Low Level API call
    //
    i8Check = bind(g_ui32Socket, &g_tSocketAddr, sizeof(sockaddr));

    if(i8Check == 0)
    {
        printf("    Bind Successful to port %d, 0x%x\n",
                   (g_tSocketAddr.sa_data[0] << 8) + g_tSocketAddr.sa_data[1],
                   (g_tSocketAddr.sa_data[0] << 8) + g_tSocketAddr.sa_data[1]);

        //
        // Set global flag variable to indicate the socket has been bound.
        //
        g_ui32BindFlag = 0;
    }
    else
    {
        printf("    Bind Failed. bind() returned code '%d'\n",i8Check);

        //
        // Set global flag variable to indicate the socket is not bound.
        //
        g_ui32BindFlag = SENTINEL_EMPTY;
    }

    return(0);
}
*/
//*****************************************************************************
//
// This command configures the IP address for the CC3000.
// This configuration can also be accomplished by running smartconfig.
// For DHCP give no arguments or specify 0 for all arguments.
// For a static IP specify the arguments
// Arguments:
// [1]  IPaddress in dot-decimal format xxx.XXX.yyy.YYY
// [2]  Default Gateway in dot-decimal format xxx.XXX.yyy.YYY
// [3]  Network Mask (optional, if not given assumed to be 255.255.255.0)
//
// To test connectivity ping the address from a computer.
//
//*****************************************************************************
/* use DHCP instead
int
CMD_ipConfig (int argc, char **argv)
{
    int32_t i32Check;
    uint8_t ui8IP[4], ui8Gateway[4], ui8NetMask[4];
    uint32_t ui32IP = 0, ui32Gateway = 0, ui32NetMask = 0, ui32DNS = 0;

    //
    // Validate input.
    //
    if(1 == argc)
    {
        //
        // Auto DNS, set all variables = 0.
        //
        ui8NetMask[0] = 0;
        ui8NetMask[1] = 0;
        ui8NetMask[2] = 0;
        ui8NetMask[3] = 0;

        ui8Gateway[0] = 0;
        ui8Gateway[1] = 0;
        ui8Gateway[2] = 0;
        ui8Gateway[3] = 0;

        ui8IP[0] = 0;
        ui8IP[1] = 0;
        ui8IP[2] = 0;
        ui8IP[3] = 0;
    }
    else if(argc < 3)
    {
        return(CMDLINE_TOO_FEW_ARGS);
    }
    else if(argc > 4)
    {
        return(CMDLINE_TOO_MANY_ARGS);
    }
    else
    {
        //
        // Handle IP Address
        //
        i32Check = DotDecimalDecoder(argv[1], &ui8IP[0], &ui8IP[1],&ui8IP[2],
                                     &ui8IP[3]);
        if(i32Check == COMMAND_FAIL)
        {
            printf("    Invalid IP Address\n");
            return(CMDLINE_INVALID_ARG);
        }

        //
        // Handle Default Gateway
        //
        i32Check = DotDecimalDecoder(argv[2], &ui8Gateway[0], &ui8Gateway[1],
                                     &ui8Gateway[2], &ui8Gateway[3]);
        if(i32Check == COMMAND_FAIL)
        {
            printf("    Invalid Gateway Address\n");
            return(CMDLINE_INVALID_ARG);
        }

        //
        // Handle Network Mask
        //
        if(argc == 4)
        {
            //
            // If provided, fill out Network Mask
            //
            i32Check = DotDecimalDecoder(argv[3], &ui8NetMask[0],
                                         &ui8NetMask[1], &ui8NetMask[2],
                                         &ui8NetMask[3]);

            if(i32Check == COMMAND_FAIL)
            {
                printf("    Invalid Network Mask\n");
                return(CMDLINE_INVALID_ARG);
            }
        }
        else
        {
            //
            // No Network Mask given, default to 255.255.255.0
            //
            ui8NetMask[0] = 255;
            ui8NetMask[1] = 255;
            ui8NetMask[2] = 255;
            ui8NetMask[3] = 0;
        }
    }

    //
    // Fill the variables
    //
    ui32IP = ((ui8IP[0] << 24) + (ui8IP[1] << 16) +
              (ui8IP[2] << 8 ) + (ui8IP[3] << 0 ));

    ui32Gateway = ((ui8Gateway[0] << 24) + (ui8Gateway[1] << 16) +
                   (ui8Gateway[2] <<  8) + (ui8Gateway[3] <<  0));

    ui32NetMask = ((ui8NetMask[0] << 24) + (ui8NetMask[1] << 16) +
                   (ui8NetMask[2] <<  8) + (ui8NetMask[3] <<  0));

    //
    // API Call
    //
    i32Check = netapp_dhcp((unsigned long *)&ui32IP,
                           (unsigned long *)&ui32NetMask,
                           (unsigned long *)&ui32Gateway,
                           (unsigned long *)&ui32DNS);

    //
    // Validate return for success / failure.
    //
    if(i32Check == COMMAND_SUCCESS)
    {
        printf("    IPConfig completed successfully.\n");
    }
    else
    {
        printf("    IPConfig Failed. netapp_dhcp() returned code '%d'\n",
                   i32Check);
    }

    return(0);
}
*/
//*****************************************************************************
//
// Disconnect from Access Point.
// Arguments: None
//
//*****************************************************************************
/*
int
CMD_disconnect (int argc, char **argv)
{
    int32_t i32Check;

    //
    // Tell the CC3000 to disconnect.
    //
    i32Check = wlan_disconnect();

    //
    // Check disconnection.
    //
    if(i32Check == COMMAND_SUCCESS)
    {
        printf("    CC3000 Disconnected Successfully.\n");

        //
        // Set LED to Red
        //
        turnLedOn(LED_0);
    }
    else
    {
        printf("    CC3000 already disconnected.\n");

        //
        // Set LED to Red, just in case it wasn't already
        //
        turnLedOn(LED_0);
    }

    return(0);
}
*/
//*****************************************************************************
//
// Remove the automatic connection policy from the CC3000. On reset it will no
// longer attempt to automatically connect to the access point.
// Arguments: None
//
//*****************************************************************************
/*
int
CMD_deletePolicy (int argc, char **argv)
{
    int32_t i32Check;

    //
    // API Call to Disconnect
    //
    i32Check = wlan_ioctl_set_connection_policy(DISABLE, DISABLE, DISABLE);

    if(i32Check == COMMAND_SUCCESS)
    {
        printf("    Policy Deleted Successfully.\n");
    }
    else
    {
        printf("    Delete Policy failed with error code '%d'\n",i32Check);
    }

    return(0);
}
*/
//*****************************************************************************
//
// Send a mDNS query packet.
// Arguments:
// [1] (Optional) specify the name to advertise with.
//
//*****************************************************************************
/*
int
CMD_mdnsadvertise (int argc, char **argv)
{
    uint32_t ui32x;
    int32_t i32Check;

    ui32x = 0;
    i32Check = 0;

    //
    // Validate input.
    //
    if(argc < 1)
    {
        return(-1);
    }
    else if(argc > 2)
    {
        return(-1);
    }

    //
    // Choose mDNS Name
    //
    if(1 == argc)
    {
        //
        // Use hard coded name
        //
        printf("    mdns advertising as: '%s'...\n",g_pcdevice_name);
        for(ui32x = 0; ((ui32x < 100) && (i32Check != 0)); ui32x++)
        {
            //i32Check = mdnsAdvertiser(1, g_pcdevice_name,
            //                          sizeof(g_pcdevice_name));
        }
    }
    else
    {
        //
        // Use argument as name
        //
        printf("    mdns advertising as: '%s'...\n",argv[1]);
        for(ui32x = 0; ((ui32x < 100) && (i32Check != 0)); ui32x++)
        {
            //i32Check = mdnsAdvertiser(1, argv[1], strlen(argv[1]));
        }

    }
    //
    // Check return code
    //
    if(i32Check == COMMAND_SUCCESS)
    {
        printf("    mDNS Advertised successfully.\n");
    }
    else
    {
        printf("    mDNS Advertising failed with error code '%d'\n",
                   i32Check);
    }

    return(0);
}
*/
//*****************************************************************************
//
// Stops and then starts the CC3000 unit. Necessary to apply profiles. Useful
// for restarting sockets and other things.
//
//*****************************************************************************
/*
int
CMD_cc3000reset(int argc, char **argv)
{
    //
    // Reset Sockets, Ports, and all connections
    //
    g_ui32BindFlag = SENTINEL_EMPTY;
    g_ui32Socket = SENTINEL_EMPTY;
    g_ui32SocketType = SENTINEL_EMPTY;

    //
    // Stop the CC3000. No return value provided.
    //
    wlan_stop();

    //
    // Wait a bit.
    //
    //ROM_SysCtlDelay(100000);
    delay(100000);
    //
    // Turn off Green LED, set to Red
    //
    turnLedOff(LED_1);
    turnLedOn(LED_0);

    //
    // Restart the CC3000.
    //
    wlan_start(0);

    return(0);
}
*/
//*****************************************************************************
//
// Ping an IP Address
// Arguments:
//  [1] IP address to ping
//  [2] (optional) max number of tries
//  [3] (optional) timeout in milliseconds
//
//*****************************************************************************
/*
int
CMD_ping(int argc, char **argv)
{
    int32_t i32Check = 0;
    uint32_t ui32Tries = 0, ui32Timeout = 0, ui32IP = 0;
    uint8_t ui8IPBlock1, ui8IPBlock2, ui8IPBlock3, ui8IPBlock4;

    //
    // Validate input.
    //
    if(argc <= 1)
    {
        return(-1);
    }
    else if(argc > 4)
    {
        return(-1);
    }

    //
    // Extract IP address.
    //
    i32Check = DotDecimalDecoder(argv[1], &ui8IPBlock1, &ui8IPBlock2,
                                 &ui8IPBlock3, &ui8IPBlock4);

    //
    // Validate IP address.
    //
    if(i32Check == COMMAND_FAIL)
    {
        printf("    Invalid IP Address, Please try again.\n");
        return(-1);
    }
    else
    {
        //
        // Concatenate IP blocks together.
        //
        ui32IP = (ui8IPBlock4 << 24) + (ui8IPBlock3 << 16) +
                 ( ui8IPBlock2 << 8) + (ui8IPBlock1 << 0);
    }

    //
    // Extract the maximum number of tries.
    //
    if(argc >= 3)
    {
        //ui32Tries = ustrtoul(argv[2], 0,10);
        ui32Tries = 4;
    }
    else
    {
        ui32Tries = 4;
    }

    //
    // Fill in timeout.
    //
    if(argc == 4)
    {
        //ui32Timeout = ustrtoul(argv[3], 0,10);
        ui32Timeout = 500;
    }
    else
    {
        ui32Timeout = 500;
    }

    //
    // Notify user of settings.
    //
    printf("    Pinging %d.%d.%d.%d, 0x%x, Max Tries: %d, Timeout: %dms...",
               ui8IPBlock1, ui8IPBlock2, ui8IPBlock3, ui8IPBlock4, ui32IP,
               ui32Tries, ui32Timeout);

    //
    // Send Ping request to CC3000
    //
    i32Check = netapp_ping_send((unsigned long *)&ui32IP, ui32Tries, 255,
                                ui32Timeout);

    //
    // Validate that the Ping completed successfully
    //
    if(i32Check != 0)
    {
        printf("    Ping request failed with error code: '%d'\n",i32Check);
    }

    return(0);
}
*/
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
	    int32_t num_msg_to_send = 1;
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

	    //
	    // Initialize all board specific components.
	    //
	    initDriver();

	    //setup the output
	    init_weather_air_display();

	    // Try to connect to wifi
	    if(CMD_connect("dd-wrt") < 0)
	    {
	    	printf("Connect Failed\r\n");
	    }
	    else
	    {
		    //init_pwm(); // do whatever we need to do for the led library here
	    }

	    //
	    // Loop forever
	    //
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


	          // for manual pollution changing in testing
	          //pollution = 200;

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
