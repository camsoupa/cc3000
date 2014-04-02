//////////////////////////////////////////////////////////////////////
// Created by SmartDesign Tue Apr 01 11:54:43 2014
// Version: 10.1 SP3 10.1.3.1
//////////////////////////////////////////////////////////////////////

`timescale 1 ns/100 ps

// cc3000fpga
module cc3000fpga(
    // Inputs
    MSS_RESET_N,
    SPI_0_DI,
    SPI_1_DI,
    UART_0_RXD,
    UART_1_RXD,
    // Outputs
    LED_0,
    LED_1,
    LED_2,
    SPI_0_DO,
    SPI_1_DO,
    UART_0_TXD,
    UART_1_TXD,
    // Inouts
    SPI_0_CLK,
    SPI_0_SS,
    SPI_1_CLK,
    SPI_1_SS
);

//--------------------------------------------------------------------
// Input
//--------------------------------------------------------------------
input  MSS_RESET_N;
input  SPI_0_DI;
input  SPI_1_DI;
input  UART_0_RXD;
input  UART_1_RXD;
//--------------------------------------------------------------------
// Output
//--------------------------------------------------------------------
output LED_0;
output LED_1;
output LED_2;
output SPI_0_DO;
output SPI_1_DO;
output UART_0_TXD;
output UART_1_TXD;
//--------------------------------------------------------------------
// Inout
//--------------------------------------------------------------------
inout  SPI_0_CLK;
inout  SPI_0_SS;
inout  SPI_1_CLK;
inout  SPI_1_SS;
//--------------------------------------------------------------------
// Nets
//--------------------------------------------------------------------
wire   LED_0_net_0;
wire   LED_1_net_0;
wire   LED_2_net_0;
wire   MSS_RESET_N;
wire   SPI_0_CLK;
wire   SPI_0_DI;
wire   SPI_0_DO_net_0;
wire   SPI_0_SS;
wire   SPI_1_CLK;
wire   SPI_1_DI;
wire   SPI_1_DO_net_0;
wire   SPI_1_SS;
wire   UART_0_RXD;
wire   UART_0_TXD_net_0;
wire   UART_1_RXD;
wire   UART_1_TXD_net_0;
wire   UART_1_TXD_net_1;
wire   UART_0_TXD_net_1;
wire   SPI_1_DO_net_1;
wire   SPI_0_DO_net_1;
wire   LED_2_net_1;
wire   LED_1_net_1;
wire   LED_0_net_1;
//--------------------------------------------------------------------
// Top level output port assignments
//--------------------------------------------------------------------
assign UART_1_TXD_net_1 = UART_1_TXD_net_0;
assign UART_1_TXD       = UART_1_TXD_net_1;
assign UART_0_TXD_net_1 = UART_0_TXD_net_0;
assign UART_0_TXD       = UART_0_TXD_net_1;
assign SPI_1_DO_net_1   = SPI_1_DO_net_0;
assign SPI_1_DO         = SPI_1_DO_net_1;
assign SPI_0_DO_net_1   = SPI_0_DO_net_0;
assign SPI_0_DO         = SPI_0_DO_net_1;
assign LED_2_net_1      = LED_2_net_0;
assign LED_2            = LED_2_net_1;
assign LED_1_net_1      = LED_1_net_0;
assign LED_1            = LED_1_net_1;
assign LED_0_net_1      = LED_0_net_0;
assign LED_0            = LED_0_net_1;
//--------------------------------------------------------------------
// Component instances
//--------------------------------------------------------------------
//--------cc3000fpga_MSS
cc3000fpga_MSS cc3000fpga_MSS_0(
        // Inputs
        .UART_0_RXD  ( UART_0_RXD ),
        .UART_1_RXD  ( UART_1_RXD ),
        .SPI_1_DI    ( SPI_1_DI ),
        .SPI_0_DI    ( SPI_0_DI ),
        .MSS_RESET_N ( MSS_RESET_N ),
        // Outputs
        .UART_0_TXD  ( UART_0_TXD_net_0 ),
        .UART_1_TXD  ( UART_1_TXD_net_0 ),
        .SPI_1_DO    ( SPI_1_DO_net_0 ),
        .SPI_0_DO    ( SPI_0_DO_net_0 ),
        .M2F_GPO_2   ( LED_2_net_0 ),
        .M2F_GPO_1   ( LED_1_net_0 ),
        .M2F_GPO_0   ( LED_0_net_0 ),
        // Inouts
        .SPI_1_CLK   ( SPI_1_CLK ),
        .SPI_1_SS    ( SPI_1_SS ),
        .SPI_0_CLK   ( SPI_0_CLK ),
        .SPI_0_SS    ( SPI_0_SS ) 
        );


endmodule
