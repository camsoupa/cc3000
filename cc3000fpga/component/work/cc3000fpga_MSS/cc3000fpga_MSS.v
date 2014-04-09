//////////////////////////////////////////////////////////////////////
// Created by SmartDesign Wed Apr 09 17:17:46 2014
// Version: 10.1 SP3 10.1.3.1
//////////////////////////////////////////////////////////////////////

`timescale 1 ns/100 ps

// cc3000fpga_MSS
module cc3000fpga_MSS(
    // Inputs
    F2M_GPI_30,
    F2M_GPI_31,
    F2M_GPI_5,
    F2M_GPI_6,
    F2M_GPI_7,
    F2M_GPI_8,
    FABINT,
    GPIO_2_IN,
    MSSPRDATA,
    MSSPREADY,
    MSSPSLVERR,
    MSS_RESET_N,
    UART_0_RXD,
    UART_1_RXD,
    // Outputs
    FAB_CLK,
    GPIO_4_OUT,
    M2F_GPO_0,
    M2F_GPO_1,
    M2F_GPO_3,
    M2F_RESET_N,
    MSSPADDR,
    MSSPENABLE,
    MSSPSEL,
    MSSPWDATA,
    MSSPWRITE,
    UART_0_TXD,
    UART_1_TXD
);

//--------------------------------------------------------------------
// Input
//--------------------------------------------------------------------
input         F2M_GPI_30;
input         F2M_GPI_31;
input         F2M_GPI_5;
input         F2M_GPI_6;
input         F2M_GPI_7;
input         F2M_GPI_8;
input         FABINT;
input         GPIO_2_IN;
input  [31:0] MSSPRDATA;
input         MSSPREADY;
input         MSSPSLVERR;
input         MSS_RESET_N;
input         UART_0_RXD;
input         UART_1_RXD;
//--------------------------------------------------------------------
// Output
//--------------------------------------------------------------------
output        FAB_CLK;
output        GPIO_4_OUT;
output        M2F_GPO_0;
output        M2F_GPO_1;
output        M2F_GPO_3;
output        M2F_RESET_N;
output [19:0] MSSPADDR;
output        MSSPENABLE;
output        MSSPSEL;
output [31:0] MSSPWDATA;
output        MSSPWRITE;
output        UART_0_TXD;
output        UART_1_TXD;
//--------------------------------------------------------------------
// Nets
//--------------------------------------------------------------------
wire          F2M_GPI_5;
wire          F2M_GPI_6;
wire          F2M_GPI_7;
wire          F2M_GPI_8;
wire          F2M_GPI_30;
wire          F2M_GPI_31;
wire          FABINT;
wire          GPIO_2_IN;
wire          MSS_ADLIB_INST_EMCCLK;
wire          MSS_ADLIB_INST_FCLK;
wire          MSS_ADLIB_INST_MACCLK;
wire          MSS_ADLIB_INST_MACCLKCCC;
wire          MSS_ADLIB_INST_PLLLOCK;
wire          MSS_ADLIB_INST_SYNCCLKFDBK;
wire          MSS_GPIO_0_GPIO_2_IN_Y;
wire   [4:4]  MSS_GPIO_0_GPIO_4_OUT_D;
wire          MSS_RESET_0_MSS_RESET_N_Y;
wire          MSS_RESET_N;
wire          MSS_UART_0_RXD_Y;
wire          MSS_UART_0_TXD_D;
wire          MSS_UART_1_RXD_Y;
wire          MSS_UART_1_TXD_D;
wire          MSSINT_GPI_5_Y;
wire          MSSINT_GPI_6_Y;
wire          MSSINT_GPI_7_Y;
wire          MSSINT_GPI_8_Y;
wire          MSSINT_GPI_30_Y;
wire          MSSINT_GPI_31_Y;
wire   [0:0]  MSSINT_GPO_0_A;
wire   [1:1]  MSSINT_GPO_1_A;
wire   [3:3]  MSSINT_GPO_3_A;
wire          net_71;
wire          net_72;
wire          net_73;
wire          net_74;
wire   [19:0] net_75_PADDR;
wire          net_75_PENABLE;
wire   [31:0] MSSPRDATA;
wire          MSSPREADY;
wire          net_75_PSELx;
wire          MSSPSLVERR;
wire   [31:0] net_75_PWDATA;
wire          net_75_PWRITE;
wire          PAD;
wire          UART_0_RXD;
wire          UART_0_TXD_net_0;
wire          UART_1_RXD;
wire          UART_1_TXD_net_0;
wire          net_72_net_0;
wire          net_71_net_0;
wire          MSS_ADLIB_INST_SYNCCLKFDBK_net_0;
wire          net_74_net_0;
wire          net_75_PSELx_net_0;
wire          net_75_PENABLE_net_0;
wire          net_75_PWRITE_net_0;
wire          net_73_net_0;
wire   [19:0] net_75_PADDR_net_0;
wire   [31:0] net_75_PWDATA_net_0;
wire          UART_0_TXD_net_1;
wire          UART_1_TXD_net_1;
wire          PAD_net_0;
wire   [31:0] GPI_net_0;
wire   [31:0] GPO_net_0;
//--------------------------------------------------------------------
// TiedOff Nets
//--------------------------------------------------------------------
wire          GND_net;
wire          VCC_net;
wire   [1:0]  DMAREADY_const_net_0;
wire   [1:0]  MACF2MRXD_const_net_0;
wire   [1:0]  MACRXD_const_net_0;
wire   [15:0] EMCRDB_const_net_0;
wire   [31:0] FABPADDR_const_net_0;
wire   [31:0] FABPWDATA_const_net_0;
//--------------------------------------------------------------------
// Constant assignments
//--------------------------------------------------------------------
assign GND_net               = 1'b0;
assign VCC_net               = 1'b1;
assign DMAREADY_const_net_0  = 2'h0;
assign MACF2MRXD_const_net_0 = 2'h0;
assign MACRXD_const_net_0    = 2'h0;
assign EMCRDB_const_net_0    = 16'h0000;
assign FABPADDR_const_net_0  = 32'h00000000;
assign FABPWDATA_const_net_0 = 32'h00000000;
//--------------------------------------------------------------------
// Top level output port assignments
//--------------------------------------------------------------------
assign net_72_net_0                     = net_72;
assign M2F_GPO_1                        = net_72_net_0;
assign net_71_net_0                     = net_71;
assign M2F_GPO_0                        = net_71_net_0;
assign MSS_ADLIB_INST_SYNCCLKFDBK_net_0 = MSS_ADLIB_INST_SYNCCLKFDBK;
assign FAB_CLK                          = MSS_ADLIB_INST_SYNCCLKFDBK_net_0;
assign net_74_net_0                     = net_74;
assign M2F_RESET_N                      = net_74_net_0;
assign net_75_PSELx_net_0               = net_75_PSELx;
assign MSSPSEL                          = net_75_PSELx_net_0;
assign net_75_PENABLE_net_0             = net_75_PENABLE;
assign MSSPENABLE                       = net_75_PENABLE_net_0;
assign net_75_PWRITE_net_0              = net_75_PWRITE;
assign MSSPWRITE                        = net_75_PWRITE_net_0;
assign net_73_net_0                     = net_73;
assign M2F_GPO_3                        = net_73_net_0;
assign net_75_PADDR_net_0               = net_75_PADDR;
assign MSSPADDR[19:0]                   = net_75_PADDR_net_0;
assign net_75_PWDATA_net_0              = net_75_PWDATA;
assign MSSPWDATA[31:0]                  = net_75_PWDATA_net_0;
assign UART_0_TXD_net_1                 = UART_0_TXD_net_0;
assign UART_0_TXD                       = UART_0_TXD_net_1;
assign UART_1_TXD_net_1                 = UART_1_TXD_net_0;
assign UART_1_TXD                       = UART_1_TXD_net_1;
assign PAD_net_0                        = PAD;
assign GPIO_4_OUT                       = PAD_net_0;
//--------------------------------------------------------------------
// Slices assignments
//--------------------------------------------------------------------
assign MSS_GPIO_0_GPIO_4_OUT_D[4] = GPO_net_0[4:4];
assign MSSINT_GPO_0_A[0]          = GPO_net_0[0:0];
assign MSSINT_GPO_1_A[1]          = GPO_net_0[1:1];
assign MSSINT_GPO_3_A[3]          = GPO_net_0[3:3];
//--------------------------------------------------------------------
// Concatenation assignments
//--------------------------------------------------------------------
assign GPI_net_0 = { MSSINT_GPI_31_Y , MSSINT_GPI_30_Y , 21'h000000 , MSSINT_GPI_8_Y , MSSINT_GPI_7_Y , MSSINT_GPI_6_Y , MSSINT_GPI_5_Y , 2'h0 , MSS_GPIO_0_GPIO_2_IN_Y , 2'h0 };
//--------------------------------------------------------------------
// Component instances
//--------------------------------------------------------------------
//--------MSS_APB
MSS_APB #( 
        .ACT_CONFIG ( 0 ),
        .ACT_DIE    ( "IP4X3M1" ),
        .ACT_FCLK   ( 100000000 ),
        .ACT_PKG    ( "fg484" ) )
MSS_ADLIB_INST(
        // Inputs
        .MSSPRDATA      ( MSSPRDATA ),
        .MSSPREADY      ( MSSPREADY ),
        .MSSPSLVERR     ( MSSPSLVERR ),
        .FABPADDR       ( FABPADDR_const_net_0 ), // tied to 32'h00000000 from definition
        .FABPWDATA      ( FABPWDATA_const_net_0 ), // tied to 32'h00000000 from definition
        .FABPWRITE      ( GND_net ), // tied to 1'b0 from definition
        .FABPSEL        ( GND_net ), // tied to 1'b0 from definition
        .FABPENABLE     ( GND_net ), // tied to 1'b0 from definition
        .SYNCCLKFDBK    ( MSS_ADLIB_INST_SYNCCLKFDBK ),
        .CALIBIN        ( GND_net ), // tied to 1'b0 from definition
        .FABINT         ( FABINT ),
        .F2MRESETn      ( VCC_net ), // tied to 1'b1 from definition
        .DMAREADY       ( DMAREADY_const_net_0 ), // tied to 2'h0 from definition
        .RXEV           ( GND_net ), // tied to 1'b0 from definition
        .VRON           ( GND_net ), // tied to 1'b0 from definition
        .GPI            ( GPI_net_0 ),
        .UART0CTSn      ( GND_net ), // tied to 1'b0 from definition
        .UART0DSRn      ( GND_net ), // tied to 1'b0 from definition
        .UART0RIn       ( GND_net ), // tied to 1'b0 from definition
        .UART0DCDn      ( GND_net ), // tied to 1'b0 from definition
        .UART1CTSn      ( GND_net ), // tied to 1'b0 from definition
        .UART1DSRn      ( GND_net ), // tied to 1'b0 from definition
        .UART1RIn       ( GND_net ), // tied to 1'b0 from definition
        .UART1DCDn      ( GND_net ), // tied to 1'b0 from definition
        .I2C0SMBUSNI    ( GND_net ), // tied to 1'b0 from definition
        .I2C0SMBALERTNI ( GND_net ), // tied to 1'b0 from definition
        .I2C0BCLK       ( GND_net ), // tied to 1'b0 from definition
        .I2C1SMBUSNI    ( GND_net ), // tied to 1'b0 from definition
        .I2C1SMBALERTNI ( GND_net ), // tied to 1'b0 from definition
        .I2C1BCLK       ( GND_net ), // tied to 1'b0 from definition
        .MACF2MRXD      ( MACF2MRXD_const_net_0 ), // tied to 2'h0 from definition
        .MACF2MCRSDV    ( GND_net ), // tied to 1'b0 from definition
        .MACF2MRXER     ( GND_net ), // tied to 1'b0 from definition
        .MACF2MMDI      ( GND_net ), // tied to 1'b0 from definition
        .FABSDD0D       ( GND_net ), // tied to 1'b0 from definition
        .FABSDD1D       ( GND_net ), // tied to 1'b0 from definition
        .FABSDD2D       ( GND_net ), // tied to 1'b0 from definition
        .FABSDD0CLK     ( GND_net ), // tied to 1'b0 from definition
        .FABSDD1CLK     ( GND_net ), // tied to 1'b0 from definition
        .FABSDD2CLK     ( GND_net ), // tied to 1'b0 from definition
        .FABACETRIG     ( GND_net ), // tied to 1'b0 from definition
        .LVTTL0EN       ( GND_net ), // tied to 1'b0 from definition
        .LVTTL1EN       ( GND_net ), // tied to 1'b0 from definition
        .LVTTL2EN       ( GND_net ), // tied to 1'b0 from definition
        .LVTTL3EN       ( GND_net ), // tied to 1'b0 from definition
        .LVTTL4EN       ( GND_net ), // tied to 1'b0 from definition
        .LVTTL5EN       ( GND_net ), // tied to 1'b0 from definition
        .LVTTL6EN       ( GND_net ), // tied to 1'b0 from definition
        .LVTTL7EN       ( GND_net ), // tied to 1'b0 from definition
        .LVTTL8EN       ( GND_net ), // tied to 1'b0 from definition
        .LVTTL9EN       ( GND_net ), // tied to 1'b0 from definition
        .LVTTL10EN      ( GND_net ), // tied to 1'b0 from definition
        .LVTTL11EN      ( GND_net ), // tied to 1'b0 from definition
        .FCLK           ( MSS_ADLIB_INST_FCLK ),
        .MACCLKCCC      ( MSS_ADLIB_INST_MACCLKCCC ),
        .RCOSC          ( GND_net ), // tied to 1'b0 from definition
        .MACCLK         ( MSS_ADLIB_INST_MACCLK ),
        .PLLLOCK        ( MSS_ADLIB_INST_PLLLOCK ),
        .MSSRESETn      ( MSS_RESET_0_MSS_RESET_N_Y ),
        .SPI0DI         ( GND_net ), // tied to 1'b0 from definition
        .SPI0CLKI       ( GND_net ), // tied to 1'b0 from definition
        .SPI0SSI        ( GND_net ), // tied to 1'b0 from definition
        .UART0RXD       ( MSS_UART_0_RXD_Y ),
        .I2C0SDAI       ( GND_net ), // tied to 1'b0 from definition
        .I2C0SCLI       ( GND_net ), // tied to 1'b0 from definition
        .SPI1DI         ( GND_net ), // tied to 1'b0 from definition
        .SPI1CLKI       ( GND_net ), // tied to 1'b0 from definition
        .SPI1SSI        ( GND_net ), // tied to 1'b0 from definition
        .UART1RXD       ( MSS_UART_1_RXD_Y ),
        .I2C1SDAI       ( GND_net ), // tied to 1'b0 from definition
        .I2C1SCLI       ( GND_net ), // tied to 1'b0 from definition
        .MACRXD         ( MACRXD_const_net_0 ), // tied to 2'h0 from definition
        .MACCRSDV       ( GND_net ), // tied to 1'b0 from definition
        .MACRXER        ( GND_net ), // tied to 1'b0 from definition
        .MACMDI         ( GND_net ), // tied to 1'b0 from definition
        .EMCCLKRTN      ( MSS_ADLIB_INST_EMCCLK ),
        .EMCRDB         ( EMCRDB_const_net_0 ), // tied to 16'h0000 from definition
        .ADC0           ( GND_net ), // tied to 1'b0 from definition
        .ADC1           ( GND_net ), // tied to 1'b0 from definition
        .ADC2           ( GND_net ), // tied to 1'b0 from definition
        .ADC3           ( GND_net ), // tied to 1'b0 from definition
        .ADC4           ( GND_net ), // tied to 1'b0 from definition
        .ADC5           ( GND_net ), // tied to 1'b0 from definition
        .ADC6           ( GND_net ), // tied to 1'b0 from definition
        .ADC7           ( GND_net ), // tied to 1'b0 from definition
        .ADC8           ( GND_net ), // tied to 1'b0 from definition
        .ADC9           ( GND_net ), // tied to 1'b0 from definition
        .ADC10          ( GND_net ), // tied to 1'b0 from definition
        .ADC11          ( GND_net ), // tied to 1'b0 from definition
        .ABPS0          ( GND_net ), // tied to 1'b0 from definition
        .ABPS1          ( GND_net ), // tied to 1'b0 from definition
        .ABPS2          ( GND_net ), // tied to 1'b0 from definition
        .ABPS3          ( GND_net ), // tied to 1'b0 from definition
        .ABPS4          ( GND_net ), // tied to 1'b0 from definition
        .ABPS5          ( GND_net ), // tied to 1'b0 from definition
        .ABPS6          ( GND_net ), // tied to 1'b0 from definition
        .ABPS7          ( GND_net ), // tied to 1'b0 from definition
        .ABPS8          ( GND_net ), // tied to 1'b0 from definition
        .ABPS9          ( GND_net ), // tied to 1'b0 from definition
        .ABPS10         ( GND_net ), // tied to 1'b0 from definition
        .ABPS11         ( GND_net ), // tied to 1'b0 from definition
        .TM0            ( GND_net ), // tied to 1'b0 from definition
        .TM1            ( GND_net ), // tied to 1'b0 from definition
        .TM2            ( GND_net ), // tied to 1'b0 from definition
        .TM3            ( GND_net ), // tied to 1'b0 from definition
        .TM4            ( GND_net ), // tied to 1'b0 from definition
        .TM5            ( GND_net ), // tied to 1'b0 from definition
        .CM0            ( GND_net ), // tied to 1'b0 from definition
        .CM1            ( GND_net ), // tied to 1'b0 from definition
        .CM2            ( GND_net ), // tied to 1'b0 from definition
        .CM3            ( GND_net ), // tied to 1'b0 from definition
        .CM4            ( GND_net ), // tied to 1'b0 from definition
        .CM5            ( GND_net ), // tied to 1'b0 from definition
        .GNDTM0         ( GND_net ), // tied to 1'b0 from definition
        .GNDTM1         ( GND_net ), // tied to 1'b0 from definition
        .GNDTM2         ( GND_net ), // tied to 1'b0 from definition
        .VAREF0         ( GND_net ), // tied to 1'b0 from definition
        .VAREF1         ( GND_net ), // tied to 1'b0 from definition
        .VAREF2         ( GND_net ), // tied to 1'b0 from definition
        .GNDVAREF       ( GND_net ), // tied to 1'b0 from definition
        .PUn            ( GND_net ), // tied to 1'b0 from definition
        // Outputs
        .MSSPADDR       ( net_75_PADDR ),
        .MSSPWDATA      ( net_75_PWDATA ),
        .MSSPWRITE      ( net_75_PWRITE ),
        .MSSPSEL        ( net_75_PSELx ),
        .MSSPENABLE     ( net_75_PENABLE ),
        .FABPRDATA      (  ),
        .FABPREADY      (  ),
        .FABPSLVERR     (  ),
        .CALIBOUT       (  ),
        .MSSINT         (  ),
        .WDINT          (  ),
        .M2FRESETn      ( net_74 ),
        .DEEPSLEEP      (  ),
        .SLEEP          (  ),
        .TXEV           (  ),
        .GPO            ( GPO_net_0 ),
        .UART0RTSn      (  ),
        .UART0DTRn      (  ),
        .UART1RTSn      (  ),
        .UART1DTRn      (  ),
        .I2C0SMBUSNO    (  ),
        .I2C0SMBALERTNO (  ),
        .I2C1SMBUSNO    (  ),
        .I2C1SMBALERTNO (  ),
        .MACM2FTXD      (  ),
        .MACM2FTXEN     (  ),
        .MACM2FMDO      (  ),
        .MACM2FMDEN     (  ),
        .MACM2FMDC      (  ),
        .ACEFLAGS       (  ),
        .CMP0           (  ),
        .CMP1           (  ),
        .CMP2           (  ),
        .CMP3           (  ),
        .CMP4           (  ),
        .CMP5           (  ),
        .CMP6           (  ),
        .CMP7           (  ),
        .CMP8           (  ),
        .CMP9           (  ),
        .CMP10          (  ),
        .CMP11          (  ),
        .LVTTL0         (  ),
        .LVTTL1         (  ),
        .LVTTL2         (  ),
        .LVTTL3         (  ),
        .LVTTL4         (  ),
        .LVTTL5         (  ),
        .LVTTL6         (  ),
        .LVTTL7         (  ),
        .LVTTL8         (  ),
        .LVTTL9         (  ),
        .LVTTL10        (  ),
        .LVTTL11        (  ),
        .PUFABn         (  ),
        .VCC15GOOD      (  ),
        .VCC33GOOD      (  ),
        .GPOE           (  ),
        .SPI0DO         (  ),
        .SPI0DOE        (  ),
        .SPI0CLKO       (  ),
        .SPI0MODE       (  ),
        .SPI0SSO        (  ),
        .UART0TXD       ( MSS_UART_0_TXD_D ),
        .I2C0SDAO       (  ),
        .I2C0SCLO       (  ),
        .SPI1DO         (  ),
        .SPI1DOE        (  ),
        .SPI1CLKO       (  ),
        .SPI1MODE       (  ),
        .SPI1SSO        (  ),
        .UART1TXD       ( MSS_UART_1_TXD_D ),
        .I2C1SDAO       (  ),
        .I2C1SCLO       (  ),
        .MACTXD         (  ),
        .MACTXEN        (  ),
        .MACMDO         (  ),
        .MACMDEN        (  ),
        .MACMDC         (  ),
        .EMCCLK         ( MSS_ADLIB_INST_EMCCLK ),
        .EMCAB          (  ),
        .EMCWDB         (  ),
        .EMCRWn         (  ),
        .EMCCS0n        (  ),
        .EMCCS1n        (  ),
        .EMCOEN0n       (  ),
        .EMCOEN1n       (  ),
        .EMCBYTEN       (  ),
        .EMCDBOE        (  ),
        .SDD0           (  ),
        .SDD1           (  ),
        .SDD2           (  ),
        .VAREFOUT       (  ) 
        );

//--------cc3000fpga_MSS_tmp_MSS_CCC_0_MSS_CCC   -   Actel:SmartFusionMSS:MSS_CCC:2.0.106
cc3000fpga_MSS_tmp_MSS_CCC_0_MSS_CCC MSS_CCC_0(
        // Inputs
        .CLKA           ( GND_net ), // tied to 1'b0 from definition
        .CLKA_PAD       ( GND_net ), // tied to 1'b0 from definition
        .CLKA_PADP      ( GND_net ), // tied to 1'b0 from definition
        .CLKA_PADN      ( GND_net ), // tied to 1'b0 from definition
        .CLKB           ( GND_net ), // tied to 1'b0 from definition
        .CLKB_PAD       ( GND_net ), // tied to 1'b0 from definition
        .CLKB_PADP      ( GND_net ), // tied to 1'b0 from definition
        .CLKB_PADN      ( GND_net ), // tied to 1'b0 from definition
        .CLKC           ( GND_net ), // tied to 1'b0 from definition
        .CLKC_PAD       ( GND_net ), // tied to 1'b0 from definition
        .CLKC_PADP      ( GND_net ), // tied to 1'b0 from definition
        .CLKC_PADN      ( GND_net ), // tied to 1'b0 from definition
        .MAINXIN        ( GND_net ), // tied to 1'b0 from definition
        .LPXIN          ( GND_net ), // tied to 1'b0 from definition
        .MAC_CLK        ( GND_net ), // tied to 1'b0 from definition
        // Outputs
        .GLA            (  ),
        .GLB            (  ),
        .GLC            (  ),
        .FAB_CLK        ( MSS_ADLIB_INST_SYNCCLKFDBK ),
        .YB             (  ),
        .YC             (  ),
        .FAB_LOCK       (  ),
        .RCOSC_CLKOUT   (  ),
        .MAINXIN_CLKOUT (  ),
        .LPXIN_CLKOUT   (  ),
        .GLA0           ( MSS_ADLIB_INST_FCLK ),
        .MSS_LOCK       ( MSS_ADLIB_INST_PLLLOCK ),
        .MAC_CLK_CCC    ( MSS_ADLIB_INST_MACCLKCCC ),
        .MAC_CLK_IO     ( MSS_ADLIB_INST_MACCLK ) 
        );

//--------INBUF_MSS
INBUF_MSS #( 
        .ACT_CONFIG ( 0 ),
        .ACT_PIN    ( "W1" ) )
MSS_GPIO_0_GPIO_2_IN(
        // Inputs
        .PAD ( GPIO_2_IN ),
        // Outputs
        .Y   ( MSS_GPIO_0_GPIO_2_IN_Y ) 
        );

//--------OUTBUF_MSS
OUTBUF_MSS #( 
        .ACT_CONFIG ( 0 ),
        .ACT_PIN    ( "AA1" ) )
MSS_GPIO_0_GPIO_4_OUT(
        // Inputs
        .D   ( MSS_GPIO_0_GPIO_4_OUT_D ),
        // Outputs
        .PAD ( PAD ) 
        );

//--------INBUF_MSS
INBUF_MSS #( 
        .ACT_CONFIG ( 0 ),
        .ACT_PIN    ( "R1" ) )
MSS_RESET_0_MSS_RESET_N(
        // Inputs
        .PAD ( MSS_RESET_N ),
        // Outputs
        .Y   ( MSS_RESET_0_MSS_RESET_N_Y ) 
        );

//--------INBUF_MSS
INBUF_MSS #( 
        .ACT_CONFIG ( 0 ),
        .ACT_PIN    ( "U18" ) )
MSS_UART_0_RXD(
        // Inputs
        .PAD ( UART_0_RXD ),
        // Outputs
        .Y   ( MSS_UART_0_RXD_Y ) 
        );

//--------OUTBUF_MSS
OUTBUF_MSS #( 
        .ACT_CONFIG ( 0 ),
        .ACT_PIN    ( "Y22" ) )
MSS_UART_0_TXD(
        // Inputs
        .D   ( MSS_UART_0_TXD_D ),
        // Outputs
        .PAD ( UART_0_TXD_net_0 ) 
        );

//--------INBUF_MSS
INBUF_MSS #( 
        .ACT_CONFIG ( 0 ),
        .ACT_PIN    ( "W22" ) )
MSS_UART_1_RXD(
        // Inputs
        .PAD ( UART_1_RXD ),
        // Outputs
        .Y   ( MSS_UART_1_RXD_Y ) 
        );

//--------OUTBUF_MSS
OUTBUF_MSS #( 
        .ACT_CONFIG ( 0 ),
        .ACT_PIN    ( "V20" ) )
MSS_UART_1_TXD(
        // Inputs
        .D   ( MSS_UART_1_TXD_D ),
        // Outputs
        .PAD ( UART_1_TXD_net_0 ) 
        );

//--------MSSINT
MSSINT MSSINT_GPI_5(
        // Inputs
        .A ( F2M_GPI_5 ),
        // Outputs
        .Y ( MSSINT_GPI_5_Y ) 
        );

//--------MSSINT
MSSINT MSSINT_GPI_6(
        // Inputs
        .A ( F2M_GPI_6 ),
        // Outputs
        .Y ( MSSINT_GPI_6_Y ) 
        );

//--------MSSINT
MSSINT MSSINT_GPI_7(
        // Inputs
        .A ( F2M_GPI_7 ),
        // Outputs
        .Y ( MSSINT_GPI_7_Y ) 
        );

//--------MSSINT
MSSINT MSSINT_GPI_8(
        // Inputs
        .A ( F2M_GPI_8 ),
        // Outputs
        .Y ( MSSINT_GPI_8_Y ) 
        );

//--------MSSINT
MSSINT MSSINT_GPI_30(
        // Inputs
        .A ( F2M_GPI_30 ),
        // Outputs
        .Y ( MSSINT_GPI_30_Y ) 
        );

//--------MSSINT
MSSINT MSSINT_GPI_31(
        // Inputs
        .A ( F2M_GPI_31 ),
        // Outputs
        .Y ( MSSINT_GPI_31_Y ) 
        );

//--------MSSINT
MSSINT MSSINT_GPO_0(
        // Inputs
        .A ( MSSINT_GPO_0_A ),
        // Outputs
        .Y ( net_71 ) 
        );

//--------MSSINT
MSSINT MSSINT_GPO_1(
        // Inputs
        .A ( MSSINT_GPO_1_A ),
        // Outputs
        .Y ( net_72 ) 
        );

//--------MSSINT
MSSINT MSSINT_GPO_3(
        // Inputs
        .A ( MSSINT_GPO_3_A ),
        // Outputs
        .Y ( net_73 ) 
        );


endmodule
