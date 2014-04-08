//////////////////////////////////////////////////////////////////////
// Created by SmartDesign Tue Apr 08 15:25:47 2014
// Version: 10.1 SP3 10.1.3.1
//////////////////////////////////////////////////////////////////////

`timescale 1 ns/100 ps

// cc3000fpga
module cc3000fpga(
    // Inputs
    MSS_RESET_N,
    M_MISO,
    UART_0_RXD,
    UART_1_RXD,
    WL_SPI_IRQ,
    // Outputs
    LED_0,
    LED_1,
    LED_2,
    MSTRCLKOUT,
    M_MOSI,
    SPISS,
    SPI_EN_PIN,
    UART_0_TXD,
    UART_1_TXD
);

//--------------------------------------------------------------------
// Input
//--------------------------------------------------------------------
input  MSS_RESET_N;
input  M_MISO;
input  UART_0_RXD;
input  UART_1_RXD;
input  WL_SPI_IRQ;
//--------------------------------------------------------------------
// Output
//--------------------------------------------------------------------
output LED_0;
output LED_1;
output LED_2;
output MSTRCLKOUT;
output M_MOSI;
output SPISS;
output SPI_EN_PIN;
output UART_0_TXD;
output UART_1_TXD;
//--------------------------------------------------------------------
// Nets
//--------------------------------------------------------------------
wire          cc3000fpga_MSS_0_FAB_CLK;
wire          cc3000fpga_MSS_0_M2F_RESET_N;
wire          cc3000fpga_MSS_0_MSS_MASTER_APB_PENABLE;
wire   [31:0] cc3000fpga_MSS_0_MSS_MASTER_APB_PRDATA;
wire          cc3000fpga_MSS_0_MSS_MASTER_APB_PREADY;
wire          cc3000fpga_MSS_0_MSS_MASTER_APB_PSELx;
wire          cc3000fpga_MSS_0_MSS_MASTER_APB_PSLVERR;
wire   [31:0] cc3000fpga_MSS_0_MSS_MASTER_APB_PWDATA;
wire          cc3000fpga_MSS_0_MSS_MASTER_APB_PWRITE;
wire          CoreAPB3_0_APBmslave0_PENABLE;
wire          CoreAPB3_0_APBmslave0_PREADY;
wire          CoreAPB3_0_APBmslave0_PSELx;
wire          CoreAPB3_0_APBmslave0_PSLVERR;
wire          CoreAPB3_0_APBmslave0_PWRITE;
wire          CORESPI_0_SPIOEN;
wire   [7:0]  CORESPI_0_SPISS;
wire          Interrupt;
wire          LED_0_net_0;
wire          LED_1_net_0;
wire          LED_3;
wire          M_MISO;
wire          M_MOSI_net_0;
wire          MSS_RESET_N;
wire          MSTR;
wire          MSTRCLKOUT_net_0;
wire          RXDataAvail;
wire          SPI_EN_PIN_net_0;
wire          SPISS_net_0;
wire          TXRegEmpty;
wire          UART_0_RXD;
wire          UART_0_TXD_net_0;
wire          UART_1_RXD;
wire          UART_1_TXD_net_0;
wire          WL_SPI_IRQ;
wire          UART_1_TXD_net_1;
wire          UART_0_TXD_net_1;
wire          LED_1_net_1;
wire          M_MOSI_net_1;
wire          MSTRCLKOUT_net_1;
wire          LED_3_net_0;
wire          LED_0_net_1;
wire          SPISS_net_1;
wire          SPI_EN_PIN_net_1;
//--------------------------------------------------------------------
// TiedOff Nets
//--------------------------------------------------------------------
wire          GND_net;
wire          VCC_net;
wire   [31:0] IADDR_const_net_0;
wire   [31:0] PRDATAS1_const_net_0;
wire   [31:0] PRDATAS2_const_net_0;
wire   [31:0] PRDATAS3_const_net_0;
wire   [31:0] PRDATAS4_const_net_0;
wire   [31:0] PRDATAS5_const_net_0;
wire   [31:0] PRDATAS6_const_net_0;
wire   [31:0] PRDATAS7_const_net_0;
wire   [31:0] PRDATAS8_const_net_0;
wire   [31:0] PRDATAS9_const_net_0;
wire   [31:0] PRDATAS10_const_net_0;
wire   [31:0] PRDATAS11_const_net_0;
wire   [31:0] PRDATAS12_const_net_0;
wire   [31:0] PRDATAS13_const_net_0;
wire   [31:0] PRDATAS14_const_net_0;
wire   [31:0] PRDATAS15_const_net_0;
wire   [31:0] PRDATAS16_const_net_0;
//--------------------------------------------------------------------
// Constant assignments
//--------------------------------------------------------------------
assign GND_net               = 1'b0;
assign VCC_net               = 1'b1;
assign IADDR_const_net_0     = 32'h00000000;
assign PRDATAS1_const_net_0  = 32'h00000000;
assign PRDATAS2_const_net_0  = 32'h00000000;
assign PRDATAS3_const_net_0  = 32'h00000000;
assign PRDATAS4_const_net_0  = 32'h00000000;
assign PRDATAS5_const_net_0  = 32'h00000000;
assign PRDATAS6_const_net_0  = 32'h00000000;
assign PRDATAS7_const_net_0  = 32'h00000000;
assign PRDATAS8_const_net_0  = 32'h00000000;
assign PRDATAS9_const_net_0  = 32'h00000000;
assign PRDATAS10_const_net_0 = 32'h00000000;
assign PRDATAS11_const_net_0 = 32'h00000000;
assign PRDATAS12_const_net_0 = 32'h00000000;
assign PRDATAS13_const_net_0 = 32'h00000000;
assign PRDATAS14_const_net_0 = 32'h00000000;
assign PRDATAS15_const_net_0 = 32'h00000000;
assign PRDATAS16_const_net_0 = 32'h00000000;
//--------------------------------------------------------------------
// Top level output port assignments
//--------------------------------------------------------------------
assign UART_1_TXD_net_1 = UART_1_TXD_net_0;
assign UART_1_TXD       = UART_1_TXD_net_1;
assign UART_0_TXD_net_1 = UART_0_TXD_net_0;
assign UART_0_TXD       = UART_0_TXD_net_1;
assign LED_1_net_1      = LED_1_net_0;
assign LED_1            = LED_1_net_1;
assign M_MOSI_net_1     = M_MOSI_net_0;
assign M_MOSI           = M_MOSI_net_1;
assign MSTRCLKOUT_net_1 = MSTRCLKOUT_net_0;
assign MSTRCLKOUT       = MSTRCLKOUT_net_1;
assign LED_3_net_0      = LED_3;
assign LED_2            = LED_3_net_0;
assign LED_0_net_1      = LED_0_net_0;
assign LED_0            = LED_0_net_1;
assign SPISS_net_1      = SPISS_net_0;
assign SPISS            = SPISS_net_1;
assign SPI_EN_PIN_net_1 = SPI_EN_PIN_net_0;
assign SPI_EN_PIN       = SPI_EN_PIN_net_1;
//--------------------------------------------------------------------
// Bus Interface Nets - Unequal Pin Widths
//--------------------------------------------------------------------
wire   [31:20]cc3000fpga_MSS_0_MSS_MASTER_APB_PADDR_0_31to20;
wire   [19:0] cc3000fpga_MSS_0_MSS_MASTER_APB_PADDR_0_19to0;
wire   [31:0] cc3000fpga_MSS_0_MSS_MASTER_APB_PADDR_0;
wire   [19:0] cc3000fpga_MSS_0_MSS_MASTER_APB_PADDR;
assign cc3000fpga_MSS_0_MSS_MASTER_APB_PADDR_0_31to20 = 12'h0;
assign cc3000fpga_MSS_0_MSS_MASTER_APB_PADDR_0_19to0 = cc3000fpga_MSS_0_MSS_MASTER_APB_PADDR[19:0];
assign cc3000fpga_MSS_0_MSS_MASTER_APB_PADDR_0 = { cc3000fpga_MSS_0_MSS_MASTER_APB_PADDR_0_31to20, cc3000fpga_MSS_0_MSS_MASTER_APB_PADDR_0_19to0 };

wire   [31:0] CoreAPB3_0_APBmslave0_PADDR;
wire   [6:0]  CoreAPB3_0_APBmslave0_PADDR_0_6to0;
wire   [6:0]  CoreAPB3_0_APBmslave0_PADDR_0;
assign CoreAPB3_0_APBmslave0_PADDR_0_6to0 = CoreAPB3_0_APBmslave0_PADDR[6:0];
assign CoreAPB3_0_APBmslave0_PADDR_0 = { CoreAPB3_0_APBmslave0_PADDR_0_6to0 };

wire   [31:8] CoreAPB3_0_APBmslave0_PRDATA_0_31to8;
wire   [7:0]  CoreAPB3_0_APBmslave0_PRDATA_0_7to0;
wire   [31:0] CoreAPB3_0_APBmslave0_PRDATA_0;
wire   [7:0]  CoreAPB3_0_APBmslave0_PRDATA;
assign CoreAPB3_0_APBmslave0_PRDATA_0_31to8 = 24'h0;
assign CoreAPB3_0_APBmslave0_PRDATA_0_7to0 = CoreAPB3_0_APBmslave0_PRDATA[7:0];
assign CoreAPB3_0_APBmslave0_PRDATA_0 = { CoreAPB3_0_APBmslave0_PRDATA_0_31to8, CoreAPB3_0_APBmslave0_PRDATA_0_7to0 };

wire   [31:0] CoreAPB3_0_APBmslave0_PWDATA;
wire   [7:0]  CoreAPB3_0_APBmslave0_PWDATA_0_7to0;
wire   [7:0]  CoreAPB3_0_APBmslave0_PWDATA_0;
assign CoreAPB3_0_APBmslave0_PWDATA_0_7to0 = CoreAPB3_0_APBmslave0_PWDATA[7:0];
assign CoreAPB3_0_APBmslave0_PWDATA_0 = { CoreAPB3_0_APBmslave0_PWDATA_0_7to0 };

//--------------------------------------------------------------------
// Component instances
//--------------------------------------------------------------------
//--------cc3000fpga_MSS
cc3000fpga_MSS cc3000fpga_MSS_0(
        // Inputs
        .UART_0_RXD  ( UART_0_RXD ),
        .UART_1_RXD  ( UART_1_RXD ),
        .MSS_RESET_N ( MSS_RESET_N ),
        .MSSPREADY   ( cc3000fpga_MSS_0_MSS_MASTER_APB_PREADY ),
        .MSSPSLVERR  ( cc3000fpga_MSS_0_MSS_MASTER_APB_PSLVERR ),
        .FABINT      ( Interrupt ),
        .F2M_GPI_31  ( RXDataAvail ),
        .F2M_GPI_30  ( TXRegEmpty ),
        .GPIO_2_IN   ( WL_SPI_IRQ ),
        .MSSPRDATA   ( cc3000fpga_MSS_0_MSS_MASTER_APB_PRDATA ),
        // Outputs
        .UART_0_TXD  ( UART_0_TXD_net_0 ),
        .UART_1_TXD  ( UART_1_TXD_net_0 ),
        .M2F_GPO_1   ( LED_1_net_0 ),
        .M2F_GPO_0   ( LED_0_net_0 ),
        .FAB_CLK     ( cc3000fpga_MSS_0_FAB_CLK ),
        .M2F_RESET_N ( cc3000fpga_MSS_0_M2F_RESET_N ),
        .MSSPSEL     ( cc3000fpga_MSS_0_MSS_MASTER_APB_PSELx ),
        .MSSPENABLE  ( cc3000fpga_MSS_0_MSS_MASTER_APB_PENABLE ),
        .MSSPWRITE   ( cc3000fpga_MSS_0_MSS_MASTER_APB_PWRITE ),
        .M2F_GPO_3   ( LED_3 ),
        .GPIO_4_OUT  ( SPI_EN_PIN_net_0 ),
        .MSSPADDR    ( cc3000fpga_MSS_0_MSS_MASTER_APB_PADDR ),
        .MSSPWDATA   ( cc3000fpga_MSS_0_MSS_MASTER_APB_PWDATA ) 
        );

//--------CoreAPB3   -   Actel:DirectCore:CoreAPB3:4.0.100
CoreAPB3 #( 
        .APB_DWIDTH      ( 32 ),
        .APBSLOT0ENABLE  ( 1 ),
        .APBSLOT1ENABLE  ( 0 ),
        .APBSLOT2ENABLE  ( 0 ),
        .APBSLOT3ENABLE  ( 0 ),
        .APBSLOT4ENABLE  ( 0 ),
        .APBSLOT5ENABLE  ( 0 ),
        .APBSLOT6ENABLE  ( 0 ),
        .APBSLOT7ENABLE  ( 0 ),
        .APBSLOT8ENABLE  ( 0 ),
        .APBSLOT9ENABLE  ( 0 ),
        .APBSLOT10ENABLE ( 0 ),
        .APBSLOT11ENABLE ( 0 ),
        .APBSLOT12ENABLE ( 0 ),
        .APBSLOT13ENABLE ( 0 ),
        .APBSLOT14ENABLE ( 0 ),
        .APBSLOT15ENABLE ( 0 ),
        .IADDR_OPTION    ( 0 ),
        .MADDR_BITS      ( 12 ),
        .SC_0            ( 0 ),
        .SC_1            ( 0 ),
        .SC_2            ( 0 ),
        .SC_3            ( 0 ),
        .SC_4            ( 0 ),
        .SC_5            ( 0 ),
        .SC_6            ( 0 ),
        .SC_7            ( 0 ),
        .SC_8            ( 0 ),
        .SC_9            ( 0 ),
        .SC_10           ( 0 ),
        .SC_11           ( 0 ),
        .SC_12           ( 0 ),
        .SC_13           ( 0 ),
        .SC_14           ( 0 ),
        .SC_15           ( 0 ),
        .UPR_NIBBLE_POSN ( 6 ) )
CoreAPB3_0(
        // Inputs
        .PRESETN    ( GND_net ), // tied to 1'b0 from definition
        .PCLK       ( GND_net ), // tied to 1'b0 from definition
        .PWRITE     ( cc3000fpga_MSS_0_MSS_MASTER_APB_PWRITE ),
        .PENABLE    ( cc3000fpga_MSS_0_MSS_MASTER_APB_PENABLE ),
        .PSEL       ( cc3000fpga_MSS_0_MSS_MASTER_APB_PSELx ),
        .PREADYS0   ( CoreAPB3_0_APBmslave0_PREADY ),
        .PSLVERRS0  ( CoreAPB3_0_APBmslave0_PSLVERR ),
        .PREADYS1   ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS1  ( GND_net ), // tied to 1'b0 from definition
        .PREADYS2   ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS2  ( GND_net ), // tied to 1'b0 from definition
        .PREADYS3   ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS3  ( GND_net ), // tied to 1'b0 from definition
        .PREADYS4   ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS4  ( GND_net ), // tied to 1'b0 from definition
        .PREADYS5   ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS5  ( GND_net ), // tied to 1'b0 from definition
        .PREADYS6   ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS6  ( GND_net ), // tied to 1'b0 from definition
        .PREADYS7   ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS7  ( GND_net ), // tied to 1'b0 from definition
        .PREADYS8   ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS8  ( GND_net ), // tied to 1'b0 from definition
        .PREADYS9   ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS9  ( GND_net ), // tied to 1'b0 from definition
        .PREADYS10  ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS10 ( GND_net ), // tied to 1'b0 from definition
        .PREADYS11  ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS11 ( GND_net ), // tied to 1'b0 from definition
        .PREADYS12  ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS12 ( GND_net ), // tied to 1'b0 from definition
        .PREADYS13  ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS13 ( GND_net ), // tied to 1'b0 from definition
        .PREADYS14  ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS14 ( GND_net ), // tied to 1'b0 from definition
        .PREADYS15  ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS15 ( GND_net ), // tied to 1'b0 from definition
        .PREADYS16  ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS16 ( GND_net ), // tied to 1'b0 from definition
        .PADDR      ( cc3000fpga_MSS_0_MSS_MASTER_APB_PADDR_0 ),
        .PWDATA     ( cc3000fpga_MSS_0_MSS_MASTER_APB_PWDATA ),
        .PRDATAS0   ( CoreAPB3_0_APBmslave0_PRDATA_0 ),
        .PRDATAS1   ( PRDATAS1_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS2   ( PRDATAS2_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS3   ( PRDATAS3_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS4   ( PRDATAS4_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS5   ( PRDATAS5_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS6   ( PRDATAS6_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS7   ( PRDATAS7_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS8   ( PRDATAS8_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS9   ( PRDATAS9_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS10  ( PRDATAS10_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS11  ( PRDATAS11_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS12  ( PRDATAS12_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS13  ( PRDATAS13_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS14  ( PRDATAS14_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS15  ( PRDATAS15_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS16  ( PRDATAS16_const_net_0 ), // tied to 32'h00000000 from definition
        .IADDR      ( IADDR_const_net_0 ), // tied to 32'h00000000 from definition
        // Outputs
        .PREADY     ( cc3000fpga_MSS_0_MSS_MASTER_APB_PREADY ),
        .PSLVERR    ( cc3000fpga_MSS_0_MSS_MASTER_APB_PSLVERR ),
        .PWRITES    ( CoreAPB3_0_APBmslave0_PWRITE ),
        .PENABLES   ( CoreAPB3_0_APBmslave0_PENABLE ),
        .PSELS0     ( CoreAPB3_0_APBmslave0_PSELx ),
        .PSELS1     (  ),
        .PSELS2     (  ),
        .PSELS3     (  ),
        .PSELS4     (  ),
        .PSELS5     (  ),
        .PSELS6     (  ),
        .PSELS7     (  ),
        .PSELS8     (  ),
        .PSELS9     (  ),
        .PSELS10    (  ),
        .PSELS11    (  ),
        .PSELS12    (  ),
        .PSELS13    (  ),
        .PSELS14    (  ),
        .PSELS15    (  ),
        .PSELS16    (  ),
        .PRDATA     ( cc3000fpga_MSS_0_MSS_MASTER_APB_PRDATA ),
        .PADDRS     ( CoreAPB3_0_APBmslave0_PADDR ),
        .PWDATAS    ( CoreAPB3_0_APBmslave0_PWDATA ) 
        );

//--------CORESPI   -   Actel:DirectCore:CORESPI:4.2.116
CORESPI #( 
        .APB_DWIDTH     ( 8 ),
        .CFG_CLK        ( 7 ),
        .CFG_FRAME_SIZE ( 4 ),
        .CFG_MODE       ( 1 ),
        .CFG_SPH        ( 0 ),
        .CFG_SPO        ( 0 ),
        .CFG_SPS        ( 0 ),
        .FAMILY         ( 18 ),
        .FIFO_DEPTH     ( 4 ) )
CORESPI_0(
        // Inputs
        .PCLK       ( cc3000fpga_MSS_0_FAB_CLK ),
        .PRESETN    ( cc3000fpga_MSS_0_M2F_RESET_N ),
        .PSEL       ( CoreAPB3_0_APBmslave0_PSELx ),
        .PENABLE    ( CoreAPB3_0_APBmslave0_PENABLE ),
        .PWRITE     ( CoreAPB3_0_APBmslave0_PWRITE ),
        .SPISSI     ( GND_net ),
        .SPISDI     ( M_MISO ),
        .SPICLKI    ( GND_net ),
        .PADDR      ( CoreAPB3_0_APBmslave0_PADDR_0 ),
        .PWDATA     ( CoreAPB3_0_APBmslave0_PWDATA_0 ),
        // Outputs
        .PREADY     ( CoreAPB3_0_APBmslave0_PREADY ),
        .PSLVERR    ( CoreAPB3_0_APBmslave0_PSLVERR ),
        .SPIINT     ( Interrupt ),
        .SPIRXAVAIL ( RXDataAvail ),
        .SPITXRFM   ( TXRegEmpty ),
        .SPISCLKO   ( MSTRCLKOUT_net_0 ),
        .SPIOEN     ( CORESPI_0_SPIOEN ),
        .SPISDO     ( M_MOSI_net_0 ),
        .SPIMODE    ( MSTR ),
        .PRDATA     ( CoreAPB3_0_APBmslave0_PRDATA ),
        .SPISS      ( CORESPI_0_SPISS ) 
        );

//--------inverter
inverter inverter_1(
        // Inputs
        .bus  ( CORESPI_0_SPISS ),
        // Outputs
        .line ( SPISS_net_0 ) 
        );


endmodule
