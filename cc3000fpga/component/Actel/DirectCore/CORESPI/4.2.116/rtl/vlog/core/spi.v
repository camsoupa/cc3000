// ********************************************************************/ 
// Actel Corporation Proprietary and Confidential
// Copyright 2009 Actel Corporation.  All rights reserved.
//
// ANY USE OR REDISTRIBUTION IN PART OR IN WHOLE MUST BE HANDLED IN 
// ACCORDANCE WITH THE ACTEL LICENSE AGREEMENT AND MUST BE APPROVED 
// IN ADVANCE IN WRITING.  
 //  
//
// spi.v -- top level module for spi core
//
//
// Revision Information:
// Date     Description
//
//
// SVN Revision Information:
// SVN $Revision: 21608 $
// SVN $Date: 2013-12-02 16:03:36 -0800 (Mon, 02 Dec 2013) $
//
// Resolved SARs
// SAR      Date     Who   Description
//
// Notes: 
//
//
// *********************************************************************/ 


module spi(   //inputs
//              TESTMODE,
              PCLK,       //system clock
              PRESETN,    //system reset
              PADDR,      //address line
              PSEL,       //device select
              PENABLE,    //enable
              PWRITE,     //write
              PWDATA,     //write data
              SPISSI,     //slave select
              SPISDI,     //serial data in
              SPICLKI,    //serial clock in

              //outputs
              PRDDATA,    //data read
              SPIINT,     //interrupt
              SPISS,      //slave select
              SPISCLKO,   //serial clock out
              SPIRXAVAIL, //data ready to be read (dma mode)
              SPITXRFM,   //room for more (dma mode)
              SPIOEN,     //output enable
              SPISDO,     //serial data out
              SPIMODE     //1 -> master, 0 -> slave
              );

// AS: Added Parameters to replace 
// configuration bits / registers
parameter               APB_DWIDTH      =   8;
parameter               CFG_FRAME_SIZE  =   4;
parameter               FIFO_DEPTH      =   4;
parameter               CFG_CLK         =   7;
parameter               CFG_SPO         =   0;
parameter               CFG_SPH         =   0;
parameter               CFG_SPS         =   0;
parameter               CFG_MODE        =   0;
              
//input TESTMODE;
input PCLK;
input PRESETN;
input [6:0] PADDR;
input PSEL;
input PENABLE;
input PWRITE;
input [APB_DWIDTH-1:0] PWDATA;
input SPISSI;
input SPISDI;
input SPICLKI;


output [APB_DWIDTH-1:0] PRDDATA;

output SPIINT;
output [7:0] SPISS;
output SPISCLKO;
output SPIRXAVAIL;
output SPITXRFM;
output SPIOEN;
output SPIMODE;
output SPISDO;


// Do not declare single bit block connections unless required

wire [APB_DWIDTH-1:0] prdata_regs;            
// ---------------------
// AS: removed features:
// ---------------------
//wire [1:0]  cfg_mode;        
//wire [5:0]  cfg_framesize;        
//wire [7:0]  cfg_clkrate;        
//wire [15:0] cfg_framecnt;  
wire [7:0]  cfg_ssel;             
//wire [1:0]  cfg_fifosize;             
wire        cfg_master;
wire        cfg_enable;
//wire [5:0]  cfg_pktsize;
wire [2:0]  cfg_cmdsize;
//wire [1:0]  cfg_userstatus;

wire [CFG_FRAME_SIZE-1:0] tx_fifo_data_in;      
wire [CFG_FRAME_SIZE-1:0] tx_fifo_data_out;     
wire [CFG_FRAME_SIZE-1:0] rx_fifo_data_in;      
wire [CFG_FRAME_SIZE-1:0] rx_fifo_data_out;     

wire rx_fifo_empty;
wire tx_fifo_full;
wire master_ssel_out;
wire [5:0]   rx_fifo_count;
wire [5:0]   tx_fifo_count;


//##########################################################################################
//APB Signals 


wire  [6:0]  PADDR32 = { PADDR[6:2], 2'b00 };


//read data: either from the register file or the fifo.
// ------------------------
// AS: adjusted PADDR value, changed to async
// ------------------------
assign PRDDATA = ~(PADDR32[6:0]==7'h08) ? prdata_regs :  rx_fifo_data_out;

assign SPIMODE    = cfg_master;
assign SPIRXAVAIL = ~rx_fifo_empty;
assign SPITXRFM   = ~tx_fifo_full;


// ----------------------------------------------------------------------------------
// Channel Outputs

//Pass the slave select to the selected devices. If no slave select asserted then everything off

reg [7:0]  master_ssel_all;
assign SPISS = master_ssel_all;

integer i;
always @(*)
   begin
      if (cfg_enable && cfg_master)
         begin
            for (i=0; i<8; i=i+1)
             begin
               if (cfg_ssel[i])
                  master_ssel_all[i] = master_ssel_out;
               else
                  master_ssel_all[i] = (CFG_MODE != 1);   //Send low in TIMODE to deselect
             end
         end
      else
         begin
            for (i =0; i<8; i=i+1)
                 master_ssel_all[i] = (CFG_MODE != 1);    //Send low in TIMODE to deselect
         end
   end      
      
wire  ssel_both = ( cfg_master ? master_ssel_out : SPISSI ); 


//-----------------------------------------------------------------------------------------


// The Register Set
spi_rf # (
          .APB_DWIDTH(APB_DWIDTH)
)
URF    ( .pclk                 (PCLK),                           
                .presetn              (PRESETN),                        
                .paddr                (PADDR32[6:0]),                   
                .psel                 (PSEL),
                .penable              (PENABLE),                             
                .pwrite               (PWRITE),                         
                .wrdata               (PWDATA),                         
                .prdata               (prdata_regs),                    
                .interrupt            (SPIINT),                         
              
                .tx_channel_underflow (tx_channel_underflow),           
                .rx_channel_overflow  (rx_channel_overflow),           
                .tx_done              (tx_done),                        
                .rx_done              (rx_done),                        
                .rx_fifo_read         (rx_fifo_read),                   
                .tx_fifo_write        (tx_fifo_write),                  
                .tx_fifo_read         (tx_fifo_read),                  
                .rx_fifo_full         (rx_fifo_full),                   
                .rx_fifo_full_next    (rx_fifo_full_next),              
                .rx_fifo_empty        (rx_fifo_empty),                  
                .rx_fifo_empty_next   (rx_fifo_empty_next),             
                .tx_fifo_full         (tx_fifo_full),                   
                .tx_fifo_full_next    (tx_fifo_full_next),              
                .tx_fifo_empty        (tx_fifo_empty), 
                .tx_fifo_empty_next   (tx_fifo_empty_next), 
                .first_frame          (first_frame_out), 
                //.frames_done_fill     (tx_fifo_last_in),                       
                //.frames_done_empty    (rx_done),
                .ssel                 (ssel_both),                       
                //.hw_txbusy            (hw_txbusy),                         
                //.hw_rxbusy            (hw_rxbusy),                        
                .rx_pktend            (rx_pktend),
                .rx_cmdsize           (rx_cmdsize),
                .active               (active),

                .cfg_enable           (cfg_enable),                    
                //.cfg_mode             (cfg_mode),                       
                .cfg_master           (cfg_master),
// AS: removed a bunch of register bits
//     (now parameter-driven)
                //.cfg_spo              (cfg_spo),                       
                //.cfg_sph              (cfg_sph),                       
                //.cfg_sps              (cfg_sps),                       
                //.cfg_framesize        (cfg_framesize),                 
                //.cfg_clkmode          (cfg_clkmode),                   
                //.cfg_clkrate          (cfg_clkrate),                   
                //.cfg_framecnt         (cfg_framecnt),                
                .cfg_ssel             (cfg_ssel),                      
                //.cfg_fifosize         (cfg_fifosize),
                .cfg_cmdsize          (cfg_cmdsize),                    
                //.cfg_pktsize          (cfg_pktsize),                    
                //.cfg_userstatus       (cfg_userstatus),
                .clr_txfifo           (fiforsttx),                     
                .clr_rxfifo           (fiforstrx),                  
                //.auto_fill            (auto_fill),
                //.auto_empty           (auto_empty),
                //.auto_stall           (auto_stall),
                //.auto_txnow           (auto_txnow),
                //.cfg_autopoll         (cfg_autopoll),
                //.cfg_autostatus       (cfg_autostatus),
                .cfg_frameurun        (cfg_frameurun),
                //.clr_framecnt         (clr_framecnt),
                .cfg_oenoff           (cfg_oenoff)
              );


// APB side of FIFOs Control    

spi_control # (
                   .CFG_FRAME_SIZE       (CFG_FRAME_SIZE)
)           UCON ( .pclk                 (PCLK),
                   .presetn              (PRESETN),
                   .psel                 (PSEL),
                   .penable              (PENABLE),
                   .pwrite               (PWRITE),
                   .paddr                (PADDR32[6:0]),
                   .wr_data_in           (PWDATA[CFG_FRAME_SIZE-1:0]),          // AS: use only FRAME_SIZE bits for data
                   //.auto_fill            (auto_fill),                 
                   //.auto_empty           (auto_empty),
                   .cfg_master           (cfg_master),
                   //.cfg_framecnt         (cfg_framecnt), 
                   .tx_fifo_data         (tx_fifo_data_in),     
                   .tx_fifo_write        (tx_fifo_write),       
                   .tx_fifo_last         (tx_fifo_last_in),        
                   .tx_fifo_empty        (tx_fifo_empty),
                   .rx_fifo_read         (rx_fifo_read),
                   //.clear_frame_count    (clr_framecnt),
                   //.fill_okay            (fill_okay),
                   .rx_fifo_empty        (rx_fifo_empty)             
                   //.rx_fifo_first        (first_frame_out)             
                   );


//Transmit Fifo

spi_fifo # (
                   .CFG_FRAME_SIZE  (CFG_FRAME_SIZE),
                   .FIFO_DEPTH      (FIFO_DEPTH)
) UTXF    (        .pclk            (PCLK), 
                   .presetn         (PRESETN), 
                   .fiforst         (fiforsttx),
                   //.fifosize        (cfg_fifosize),
                   .data_in         (tx_fifo_data_in),               
                   .flag_in         (tx_fifo_last_in),                  
                   .data_out        (tx_fifo_data_out),              
                   .flag_out        (tx_fifo_last_out),              
                   .read_in         (tx_fifo_read),                  
                   .write_in        (tx_fifo_write),                 
                   .full_out        (tx_fifo_full),                  
                   .empty_out       (tx_fifo_empty),                 
                   .full_next_out   (tx_fifo_full_next),             
                   .empty_next_out  (tx_fifo_empty_next),            
                   .overflow_out    (not_used1),              
                   .fifo_count      (tx_fifo_count)
                   );



//Receive Fifo

spi_fifo # (
          .CFG_FRAME_SIZE(CFG_FRAME_SIZE),
          .FIFO_DEPTH(FIFO_DEPTH)
          
) URXF     ( .pclk            (PCLK), 
                    .presetn         (PRESETN), 
                    .fiforst         (fiforstrx),
                    //.fifosize        (cfg_fifosize),
                    .data_in         (rx_fifo_data_in),             
                    .write_in        (rx_fifo_write), 
                    .flag_in         (rx_fifo_first_in),             
                    .data_out        (rx_fifo_data_out),            
                    .read_in         (rx_fifo_read),                
                    .flag_out        (first_frame_out),             
                    .full_out        (rx_fifo_full),                
                    .empty_out       (rx_fifo_empty),               
                    .empty_next_out  (rx_fifo_empty_next),          
                    .full_next_out   (rx_fifo_full_next),           
                    .overflow_out    (rx_channel_overflow),         
                    .fifo_count      (rx_fifo_count)
                    );


//Channel controll

spi_chanctrl # (
  // parameters (AS)
  .CFG_SPH            (CFG_SPH),
  .CFG_SPO            (CFG_SPO),
  .CFG_SPS            (CFG_SPS),
  .CFG_MODE           (CFG_MODE),
  .CFG_CLKRATE        (CFG_CLK),
  // no clock mode parameter (removed)
  .CFG_FRAME_SIZE      (CFG_FRAME_SIZE)
)UCC   (               .pclk               (PCLK),      
                       .presetn            (PRESETN), 
//                       .testmode           (TESTMODE),         
                       .spi_clk_in         (SPICLKI),      
                       .spi_clk_out        (SPISCLKO),        
                       .spi_ssel_in        (SPISSI),
                       .spi_ssel_out       (master_ssel_out),    
                       .spi_data_in        (SPISDI),
                       .spi_data_out       (SPISDO), 
                       .spi_data_oen       (SPIOEN),
                       .txfifo_count       (tx_fifo_count),
                       .txfifo_empty       (tx_fifo_empty),       
                       .txfifo_read        (tx_fifo_read), 
                       .txfifo_data        (tx_fifo_data_out),
                       .txfifo_last        (tx_fifo_last_out),
                       .rxfifo_count       (rx_fifo_count),
                       .rxfifo_write       (rx_fifo_write),
                       .rxfifo_data        (rx_fifo_data_in),
                       .rxfifo_first       (rx_fifo_first_in), 
                       .cfg_enable         (cfg_enable), 
                       .cfg_master         (cfg_master), 
                       //.cfg_mode           (cfg_mode),        
                       //.cfg_sph            (cfg_sph),        
                       //.cfg_spo            (cfg_spo),       
                       //.cfg_sps            (cfg_sps),
                       //.cfg_autopoll       (cfg_autopoll),
                       .cfg_frameurun      (cfg_frameurun),
                       //.cfg_autostatus     (cfg_autostatus), 
                       //.cfg_clkmode        (cfg_clkmode), 
                       //.cfg_clkrate        (cfg_clkrate),
                       //.cfg_fifosize       (cfg_fifosize), 
                       //.cfg_framesize      (cfg_framesize), 
                       //.cfg_userstatus     (cfg_userstatus),         
                       .cfg_cmdsize        (cfg_cmdsize),
                       //.cfg_pktsize        (cfg_pktsize),
                       .cfg_oenoff         (cfg_oenoff),
                       //.auto_stall         (auto_stall),
                       //.auto_txnow         (auto_txnow),
                       //.hw_rxbusy          (hw_rxbusy),
                       //.hw_txbusy          (hw_txbusy),
                       .tx_alldone         (tx_done), 
                       .rx_alldone         (rx_done), 
                       .rx_pktend          (rx_pktend),
                       .rx_cmdsize         (rx_cmdsize),
                       .tx_underrun        (tx_channel_underflow),
                       //.fill_okay          (fill_okay),
                       .active             (active) 
                     );                 



endmodule


   