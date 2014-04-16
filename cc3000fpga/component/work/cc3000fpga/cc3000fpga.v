//////////////////////////////////////////////////////////////////////
// Created by SmartDesign Wed Apr 16 15:53:55 2014
// Version: 10.1 SP3 10.1.3.1
//////////////////////////////////////////////////////////////////////

`timescale 1 ns/100 ps

// cc3000fpga
module cc3000fpga(
    // Inputs
    MSS_RESET_N,
    SPI_1_DI,
    UART_0_RXD,
    UART_1_RXD,
    cc3000_IRQ,
    // Outputs
    LED_0,
    LED_1,
    LED_2,
    LED_B,
    LED_G,
    LED_R,
    SPI_1_DO,
    SPI_EN_PIN,
    SPI_SS_PIN,
    UART_0_TXD,
    UART_1_TXD,
    // Inouts
    SPI_1_CLK,
    SPI_1_SS
);

//--------------------------------------------------------------------
// Input
//--------------------------------------------------------------------
input  MSS_RESET_N;
input  SPI_1_DI;
input  UART_0_RXD;
input  UART_1_RXD;
input  cc3000_IRQ;
//--------------------------------------------------------------------
// Output
//--------------------------------------------------------------------
output LED_0;
output LED_1;
output LED_2;
output LED_B;
output LED_G;
output LED_R;
output SPI_1_DO;
output SPI_EN_PIN;
output SPI_SS_PIN;
output UART_0_TXD;
output UART_1_TXD;
//--------------------------------------------------------------------
// Inout
//--------------------------------------------------------------------
inout  SPI_1_CLK;
inout  SPI_1_SS;
//--------------------------------------------------------------------
// Nets
//--------------------------------------------------------------------
wire          cc3000_IRQ;
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
wire          CoreAPB3_0_APBmslave0_PSELx;
wire   [31:0] CoreAPB3_0_APBmslave0_PWDATA;
wire          CoreAPB3_0_APBmslave0_PWRITE;
wire   [31:0] CoreAPB3_0_APBmslave1_PRDATA;
wire          CoreAPB3_0_APBmslave1_PREADY;
wire          CoreAPB3_0_APBmslave1_PSELx;
wire          CoreAPB3_0_APBmslave1_PSLVERR;
wire   [31:0] CoreAPB3_0_APBmslave2_PRDATA;
wire          CoreAPB3_0_APBmslave2_PREADY;
wire          CoreAPB3_0_APBmslave2_PSELx;
wire          CoreAPB3_0_APBmslave2_PSLVERR;
wire   [31:0] CoreAPB3_0_APBmslave3_PRDATA;
wire          CoreAPB3_0_APBmslave3_PREADY;
wire          CoreAPB3_0_APBmslave3_PSELx;
wire          CoreAPB3_0_APBmslave3_PSLVERR;
wire   [31:0] CoreAPB3_0_APBmslave4_PRDATA;
wire          CoreAPB3_0_APBmslave4_PREADY;
wire          CoreAPB3_0_APBmslave4_PSELx;
wire          CoreAPB3_0_APBmslave4_PSLVERR;
wire          LED_0_net_0;
wire          LED_1_net_0;
wire          LED_3;
wire          LED_B_net_0;
wire          LED_G_net_0;
wire          LED_R_net_0;
wire          MSS_RESET_N;
wire          SPI_1_CLK;
wire          SPI_1_DI;
wire          SPI_1_DO_1;
wire          SPI_1_SS;
wire          SPI_EN_PIN_0;
wire          SPI_SS_PIN_net_0;
wire          Timer_1_FABINT;
wire          Timer_2_FABINT;
wire          Timer_3_FABINT;
wire          Timer_4_FABINT;
wire          UART_0_RXD;
wire          UART_0_TXD_net_0;
wire          UART_1_RXD;
wire          UART_1_TXD_net_0;
wire          UART_1_TXD_net_1;
wire          UART_0_TXD_net_1;
wire          LED_1_net_1;
wire          LED_3_net_0;
wire          LED_0_net_1;
wire          SPI_1_DO_1_net_0;
wire          LED_B_net_1;
wire          LED_R_net_1;
wire          LED_G_net_1;
wire          SPI_EN_PIN_0_net_0;
wire          SPI_SS_PIN_net_1;
//--------------------------------------------------------------------
// TiedOff Nets
//--------------------------------------------------------------------
wire          GND_net;
wire          VCC_net;
wire   [31:0] IADDR_const_net_0;
wire   [31:0] PRDATAS0_const_net_0;
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
assign PRDATAS0_const_net_0  = 32'h00000000;
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
assign UART_1_TXD_net_1   = UART_1_TXD_net_0;
assign UART_1_TXD         = UART_1_TXD_net_1;
assign UART_0_TXD_net_1   = UART_0_TXD_net_0;
assign UART_0_TXD         = UART_0_TXD_net_1;
assign LED_1_net_1        = LED_1_net_0;
assign LED_1              = LED_1_net_1;
assign LED_3_net_0        = LED_3;
assign LED_2              = LED_3_net_0;
assign LED_0_net_1        = LED_0_net_0;
assign LED_0              = LED_0_net_1;
assign SPI_1_DO_1_net_0   = SPI_1_DO_1;
assign SPI_1_DO           = SPI_1_DO_1_net_0;
assign LED_B_net_1        = LED_B_net_0;
assign LED_B              = LED_B_net_1;
assign LED_R_net_1        = LED_R_net_0;
assign LED_R              = LED_R_net_1;
assign LED_G_net_1        = LED_G_net_0;
assign LED_G              = LED_G_net_1;
assign SPI_EN_PIN_0_net_0 = SPI_EN_PIN_0;
assign SPI_EN_PIN         = SPI_EN_PIN_0_net_0;
assign SPI_SS_PIN_net_1   = SPI_SS_PIN_net_0;
assign SPI_SS_PIN         = SPI_SS_PIN_net_1;
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
wire   [7:0]  CoreAPB3_0_APBmslave0_PADDR_3_7to0;
wire   [7:0]  CoreAPB3_0_APBmslave0_PADDR_3;
wire   [7:0]  CoreAPB3_0_APBmslave0_PADDR_2_7to0;
wire   [7:0]  CoreAPB3_0_APBmslave0_PADDR_2;
wire   [7:0]  CoreAPB3_0_APBmslave0_PADDR_1_7to0;
wire   [7:0]  CoreAPB3_0_APBmslave0_PADDR_1;
wire   [7:0]  CoreAPB3_0_APBmslave0_PADDR_0_7to0;
wire   [7:0]  CoreAPB3_0_APBmslave0_PADDR_0;
assign CoreAPB3_0_APBmslave0_PADDR_3_7to0 = CoreAPB3_0_APBmslave0_PADDR[7:0];
assign CoreAPB3_0_APBmslave0_PADDR_3 = { CoreAPB3_0_APBmslave0_PADDR_3_7to0 };
assign CoreAPB3_0_APBmslave0_PADDR_2_7to0 = CoreAPB3_0_APBmslave0_PADDR[7:0];
assign CoreAPB3_0_APBmslave0_PADDR_2 = { CoreAPB3_0_APBmslave0_PADDR_2_7to0 };
assign CoreAPB3_0_APBmslave0_PADDR_1_7to0 = CoreAPB3_0_APBmslave0_PADDR[7:0];
assign CoreAPB3_0_APBmslave0_PADDR_1 = { CoreAPB3_0_APBmslave0_PADDR_1_7to0 };
assign CoreAPB3_0_APBmslave0_PADDR_0_7to0 = CoreAPB3_0_APBmslave0_PADDR[7:0];
assign CoreAPB3_0_APBmslave0_PADDR_0 = { CoreAPB3_0_APBmslave0_PADDR_0_7to0 };

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
        .F2M_GPI_5   ( Timer_1_FABINT ),
        .F2M_GPI_8   ( Timer_4_FABINT ),
        .F2M_GPI_7   ( Timer_3_FABINT ),
        .F2M_GPI_6   ( Timer_2_FABINT ),
        .SPI_1_DI    ( SPI_1_DI ),
        .GPIO_2_IN   ( cc3000_IRQ ),
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
        .SPI_1_DO    ( SPI_1_DO_1 ),
        .M2F_GPO_13  ( LED_B_net_0 ),
        .M2F_GPO_12  ( LED_G_net_0 ),
        .M2F_GPO_11  ( LED_R_net_0 ),
        .GPIO_4_OUT  ( SPI_EN_PIN_0 ),
        .M2F_GPO_9   ( SPI_SS_PIN_net_0 ),
        .MSSPADDR    ( cc3000fpga_MSS_0_MSS_MASTER_APB_PADDR ),
        .MSSPWDATA   ( cc3000fpga_MSS_0_MSS_MASTER_APB_PWDATA ),
        // Inouts
        .SPI_1_CLK   ( SPI_1_CLK ),
        .SPI_1_SS    ( SPI_1_SS ) 
        );

//--------CoreAPB3   -   Actel:DirectCore:CoreAPB3:4.0.100
CoreAPB3 #( 
        .APB_DWIDTH      ( 32 ),
        .APBSLOT0ENABLE  ( 1 ),
        .APBSLOT1ENABLE  ( 1 ),
        .APBSLOT2ENABLE  ( 1 ),
        .APBSLOT3ENABLE  ( 1 ),
        .APBSLOT4ENABLE  ( 1 ),
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
        .PREADYS0   ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS0  ( GND_net ), // tied to 1'b0 from definition
        .PREADYS1   ( CoreAPB3_0_APBmslave1_PREADY ),
        .PSLVERRS1  ( CoreAPB3_0_APBmslave1_PSLVERR ),
        .PREADYS2   ( CoreAPB3_0_APBmslave2_PREADY ),
        .PSLVERRS2  ( CoreAPB3_0_APBmslave2_PSLVERR ),
        .PREADYS3   ( CoreAPB3_0_APBmslave3_PREADY ),
        .PSLVERRS3  ( CoreAPB3_0_APBmslave3_PSLVERR ),
        .PREADYS4   ( CoreAPB3_0_APBmslave4_PREADY ),
        .PSLVERRS4  ( CoreAPB3_0_APBmslave4_PSLVERR ),
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
        .PRDATAS0   ( PRDATAS0_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS1   ( CoreAPB3_0_APBmslave1_PRDATA ),
        .PRDATAS2   ( CoreAPB3_0_APBmslave2_PRDATA ),
        .PRDATAS3   ( CoreAPB3_0_APBmslave3_PRDATA ),
        .PRDATAS4   ( CoreAPB3_0_APBmslave4_PRDATA ),
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
        .PSELS1     ( CoreAPB3_0_APBmslave1_PSELx ),
        .PSELS2     ( CoreAPB3_0_APBmslave2_PSELx ),
        .PSELS3     ( CoreAPB3_0_APBmslave3_PSELx ),
        .PSELS4     ( CoreAPB3_0_APBmslave4_PSELx ),
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

//--------timerWrapper
timerWrapper Timer_1(
        // Inputs
        .PCLK    ( cc3000fpga_MSS_0_FAB_CLK ),
        .PENABLE ( CoreAPB3_0_APBmslave0_PENABLE ),
        .PSEL    ( CoreAPB3_0_APBmslave1_PSELx ),
        .PRESETN ( cc3000fpga_MSS_0_M2F_RESET_N ),
        .PWRITE  ( CoreAPB3_0_APBmslave0_PWRITE ),
        .PADDR   ( CoreAPB3_0_APBmslave0_PADDR_0 ),
        .PWDATA  ( CoreAPB3_0_APBmslave0_PWDATA ),
        // Outputs
        .PREADY  ( CoreAPB3_0_APBmslave1_PREADY ),
        .PSLVERR ( CoreAPB3_0_APBmslave1_PSLVERR ),
        .FABINT  ( Timer_1_FABINT ),
        .PRDATA  ( CoreAPB3_0_APBmslave1_PRDATA ),
        .TPS     (  ) 
        );

//--------timerWrapper
timerWrapper Timer_2(
        // Inputs
        .PCLK    ( cc3000fpga_MSS_0_FAB_CLK ),
        .PENABLE ( CoreAPB3_0_APBmslave0_PENABLE ),
        .PSEL    ( CoreAPB3_0_APBmslave2_PSELx ),
        .PRESETN ( cc3000fpga_MSS_0_M2F_RESET_N ),
        .PWRITE  ( CoreAPB3_0_APBmslave0_PWRITE ),
        .PADDR   ( CoreAPB3_0_APBmslave0_PADDR_1 ),
        .PWDATA  ( CoreAPB3_0_APBmslave0_PWDATA ),
        // Outputs
        .PREADY  ( CoreAPB3_0_APBmslave2_PREADY ),
        .PSLVERR ( CoreAPB3_0_APBmslave2_PSLVERR ),
        .FABINT  ( Timer_2_FABINT ),
        .PRDATA  ( CoreAPB3_0_APBmslave2_PRDATA ),
        .TPS     (  ) 
        );

//--------timerWrapper
timerWrapper Timer_3(
        // Inputs
        .PCLK    ( cc3000fpga_MSS_0_FAB_CLK ),
        .PENABLE ( CoreAPB3_0_APBmslave0_PENABLE ),
        .PSEL    ( CoreAPB3_0_APBmslave3_PSELx ),
        .PRESETN ( cc3000fpga_MSS_0_M2F_RESET_N ),
        .PWRITE  ( CoreAPB3_0_APBmslave0_PWRITE ),
        .PADDR   ( CoreAPB3_0_APBmslave0_PADDR_2 ),
        .PWDATA  ( CoreAPB3_0_APBmslave0_PWDATA ),
        // Outputs
        .PREADY  ( CoreAPB3_0_APBmslave3_PREADY ),
        .PSLVERR ( CoreAPB3_0_APBmslave3_PSLVERR ),
        .FABINT  ( Timer_3_FABINT ),
        .PRDATA  ( CoreAPB3_0_APBmslave3_PRDATA ),
        .TPS     (  ) 
        );

//--------timerWrapper
timerWrapper Timer_4(
        // Inputs
        .PCLK    ( cc3000fpga_MSS_0_FAB_CLK ),
        .PENABLE ( CoreAPB3_0_APBmslave0_PENABLE ),
        .PSEL    ( CoreAPB3_0_APBmslave4_PSELx ),
        .PRESETN ( cc3000fpga_MSS_0_M2F_RESET_N ),
        .PWRITE  ( CoreAPB3_0_APBmslave0_PWRITE ),
        .PADDR   ( CoreAPB3_0_APBmslave0_PADDR_3 ),
        .PWDATA  ( CoreAPB3_0_APBmslave0_PWDATA ),
        // Outputs
        .PREADY  ( CoreAPB3_0_APBmslave4_PREADY ),
        .PSLVERR ( CoreAPB3_0_APBmslave4_PSLVERR ),
        .FABINT  ( Timer_4_FABINT ),
        .PRDATA  ( CoreAPB3_0_APBmslave4_PRDATA ),
        .TPS     (  ) 
        );


endmodule
