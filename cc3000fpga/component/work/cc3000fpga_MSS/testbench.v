//////////////////////////////////////////////////////////////////////
// Created by Actel SmartDesign Tue Apr 01 11:54:33 2014
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
// Instantiate Unit Under Test:  cc3000fpga_MSS
//////////////////////////////////////////////////////////////////////
cc3000fpga_MSS cc3000fpga_MSS_0 (
    // Inputs
    .UART_0_RXD({1{1'b0}}),
    .UART_1_RXD({1{1'b0}}),
    .SPI_0_DI({1{1'b0}}),
    .SPI_1_DI({1{1'b0}}),
    .MSS_RESET_N(NSYSRESET),

    // Outputs
    .M2F_GPO_2( ),
    .M2F_GPO_1( ),
    .M2F_GPO_0( ),
    .UART_0_TXD( ),
    .UART_1_TXD( ),
    .SPI_0_DO( ),
    .SPI_1_DO( ),

    // Inouts
    .SPI_0_CLK( ),
    .SPI_0_SS( ),
    .SPI_1_CLK( ),
    .SPI_1_SS( )

);

endmodule

