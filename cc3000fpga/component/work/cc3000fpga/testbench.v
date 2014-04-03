//////////////////////////////////////////////////////////////////////
// Created by Actel SmartDesign Thu Apr 03 14:44:27 2014
// Testbench Template
// This is a basic testbench that instantiates your design with basic 
// clock and reset pins connected.  If your design has special
// clock/reset or testbench driver requirements then you should 
// copy this file and modify it. 
//////////////////////////////////////////////////////////////////////

`timescale 1ns/100ps

module testbench;

parameter SYSCLK_PERIOD = 100; // 10MHz

reg SYSCLK;
reg NSYSRESET;

initial
begin
    SYSCLK = 1'b0;
    NSYSRESET = 1'b0;
end

//////////////////////////////////////////////////////////////////////
// Reset Pulse
//////////////////////////////////////////////////////////////////////
initial
begin
    #(SYSCLK_PERIOD * 10 )
        NSYSRESET = 1'b1;
end


//////////////////////////////////////////////////////////////////////
// 10MHz Clock Driver
//////////////////////////////////////////////////////////////////////
always @(SYSCLK)
    #(SYSCLK_PERIOD / 2.0) SYSCLK <= !SYSCLK;


//////////////////////////////////////////////////////////////////////
// Instantiate Unit Under Test:  cc3000fpga
//////////////////////////////////////////////////////////////////////
cc3000fpga cc3000fpga_0 (
    // Inputs
    .UART_1_RXD({1{1'b0}}),
    .UART_0_RXD({1{1'b0}}),
    .MSS_RESET_N(NSYSRESET),
    .M_MISO({1{1'b0}}),
    .WL_SPI_IRQ({1{1'b0}}),

    // Outputs
    .UART_1_TXD( ),
    .UART_0_TXD( ),
    .LED_1( ),
    .LED_0( ),
    .M_MOSI( ),
    .MSTRCLKOUT( ),
    .LED_2( ),
    .SPISS( ),
    .IO_4_PADOUT( )

    // Inouts

);

endmodule

