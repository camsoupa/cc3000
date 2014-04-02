// ********************************************************************/ 
// Actel Corporation Proprietary and Confidential
// Copyright 2009 Actel Corporation.  All rights reserved.
//
// ANY USE OR REDISTRIBUTION IN PART OR IN WHOLE MUST BE HANDLED IN 
// ACCORDANCE WITH THE ACTEL LICENSE AGREEMENT AND MUST BE APPROVED 
// IN ADVANCE IN WRITING.  
//  
//
// SPI Top level control.
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

// ------------------------------------------------------
// AS:
// - remove auto fill and auto empty ports/ function
// - remove frame count and associated signals
// - remove tx_fifo_
// ------------------------------------------------------

module spi_control # (
  // AS: added parameters
  parameter     CFG_FRAME_SIZE    = 4
)(  input         pclk,
                     input         presetn,
                     input         psel,
                     input         penable,
                     input         pwrite,
                     input  [6:0]  paddr,
                     input  [CFG_FRAME_SIZE-1:0] wr_data_in,
                     //input  [15:0] cfg_framecnt,
                     //input         auto_fill,
                     //input         auto_empty, 
                     input         cfg_master,
                     //input         rx_fifo_first,               // not really needed anymore
                     input         rx_fifo_empty,
                     // -----------------------
                     // AS: added tx_fifo_empty
                     // -----------------------
                     input         tx_fifo_empty,
                     //input         clear_frame_count,
                     //input         fill_okay,

                     output [CFG_FRAME_SIZE-1:0] tx_fifo_data,            
                     output        tx_fifo_write,           
                     output        tx_fifo_last,             
                     output        rx_fifo_read            
                );



//######################################################################################################

reg tx_fifo_write_sig;
reg rx_fifo_read_sig;

//reg [15:0] txrx_frame_count_d;   //frame count
//reg [15:0] txrx_frame_count_q;
reg tx_last_frame_sig;

//reg auto_write;
//reg auto_read;


// Output assignments.
// AS: modified tx_fifo_last to be dependent on tx_fifo_empty (undone)
assign tx_fifo_last   = tx_last_frame_sig;      
//assign tx_fifo_last   = tx_last_frame_sig && tx_fifo_empty;
assign tx_fifo_data   = wr_data_in;
assign tx_fifo_write  = tx_fifo_write_sig;     
assign rx_fifo_read   = rx_fifo_read_sig;     

// AS: removed counter
//always @(posedge pclk or negedge presetn)
//   begin
//   if (~presetn)
//      begin 
//        txrx_frame_count_q <= 16'h0001;
//      end
//   else
//      begin
//        txrx_frame_count_q <=  txrx_frame_count_d;
//      end
//   end

// Note combinational generation of FIFO read and write signals

always @(*)
   begin
   //defaults
   rx_fifo_read_sig  = 1'b0;      //default no read on rx fifo
   tx_fifo_write_sig = 1'b0;      //default no write on tx fifo
   tx_last_frame_sig = 1'b0;      //default not last frame
   //txrx_frame_count_d = txrx_frame_count_q;

   if (penable && psel)
      begin
      case (paddr) //synthesis parallel_case
      6'h0C:	  //write to transmit fifo
         begin
            if (pwrite)
               begin
                 tx_fifo_write_sig  = 1'b1;   //write to the fifo
                 
                 //txrx_frame_count_d = txrx_frame_count_q + 1'b1;
                 // *************************************************************
                 // AS: what to do here? not sure how tx_last_frame_sig should be
                 // be assigned
                 // ************************************************************* 
                 //if (txrx_frame_count_q == cfg_framecnt)
                 //   begin
                 //     tx_last_frame_sig  = 1'b1;
                 //     txrx_frame_count_d = 16'h0001;
                 //   end
                 
                 // Solution: created Aliased TX DATA register to indicate last frame
                 
               end            
         end
      6'h08:    //read from receive fifo
         begin 
            if (~pwrite) 
            begin
                rx_fifo_read_sig = 1'b1;
            end
         end
      6'h28:    // aliased transmit data, sets last frame bit
        begin
          tx_fifo_write_sig  = 1'b1;   //write to the fifo
          tx_last_frame_sig  = 1'b1;   //last frame
        end
      default:
         begin
         end
      endcase     
   end

   //if (auto_write)
   // begin
   //      tx_fifo_write_sig  = 1'b1;   //write to the fifo
   //      txrx_frame_count_d = txrx_frame_count_q + 1'b1;
   //      if (txrx_frame_count_q == cfg_framecnt)
   //      begin
   //        tx_last_frame_sig  = 1'b1;
   //        txrx_frame_count_d = 16'h0001;
   //      end
   // end
   //
   //if (auto_read)
   // begin
   //     rx_fifo_read_sig = 1'b1;
   // end
   //  
   //if (clear_frame_count) txrx_frame_count_d = 16'h0001; 
   

end
    




//----------------------------------------------------------------------------------

// AS: removed auto write and auto read features
//always@(posedge pclk or negedge presetn)
//begin
//  if (!presetn)
//   begin
//     auto_write <= 1'b0;
//     auto_read  <= 1'b0;
//   end
//  else
// begin
//    auto_write <= 1'b0;
//    auto_read  <= 1'b0;
//   
//    if ((auto_fill) & !auto_write & (fill_okay))       // if room in FIFOs write data
//    begin
//        auto_write <= 1'b1;
//    end
//        
//    //on master empty until all frames done
//    if (cfg_master && auto_empty && !auto_read && !rx_fifo_empty)
//    begin
//        auto_read <= 1'b1;
//    end
//
//    //On slave empty until start of next packet
//    if (!cfg_master && auto_empty && !auto_read && !rx_fifo_first && !rx_fifo_empty)
//    begin
//        auto_read <= 1'b1;
//    end
//
// end
//end






endmodule
   