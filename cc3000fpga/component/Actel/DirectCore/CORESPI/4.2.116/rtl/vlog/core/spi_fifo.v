// ********************************************************************/ 
// Actel Corporation Proprietary and Confidential
// Copyright 2009 Actel Corporation.  All rights reserved.
//
// ANY USE OR REDISTRIBUTION IN PART OR IN WHOLE MUST BE HANDLED IN 
// ACCORDANCE WITH THE ACTEL LICENSE AGREEMENT AND MUST BE APPROVED 
// IN ADVANCE IN WRITING.  
//  
//
// SPI Synchronous Fifo
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
// Sept 5th. Fix for reading empty fifo.
//
// *********************************************************************/ 



module spi_fifo(   pclk,           
                   presetn,        
                   fiforst,
                   //fifosize,      
                   data_in,       
                   flag_in,       
                   data_out,      
                   flag_out,      

                   read_in,       
                   write_in,      
                   
                   full_out,      
                   empty_out,     
                   full_next_out, 
                   empty_next_out,
                   overflow_out,  
                   fifo_count
                );

parameter CFG_FRAME_SIZE = 4;       // 4-32
parameter FIFO_DEPTH = 4;       // 2,4,8,16,32

                
input pclk;
input presetn;
input fiforst;
//input [1:0] fifosize;       
input [CFG_FRAME_SIZE-1:0] data_in;
input read_in;
input write_in;
input flag_in;


output [CFG_FRAME_SIZE-1:0] data_out;
output empty_out;
output full_out;
output empty_next_out;
output full_next_out;
output overflow_out;
output flag_out;
output [5:0] fifo_count;

   
reg [4:0] rd_pointer_d; 
reg [4:0] rd_pointer_q;        //read pointer address
reg [4:0] wr_pointer_d;
reg [4:0] wr_pointer_q;        //write pointer address
reg [5:0] counter_d;
reg [5:0] counter_q;           //counter 5 bits


reg [CFG_FRAME_SIZE:0] fifo_mem_d[0:FIFO_DEPTH-1];    //FIFO has extra flag bit (CFG_FRAME_SIZE + 1)
reg [CFG_FRAME_SIZE:0] fifo_mem_q[0:FIFO_DEPTH-1];
reg [CFG_FRAME_SIZE:0] data_out_dx;
reg [CFG_FRAME_SIZE:0] data_out_d;

reg full_out;       
reg empty_out;     
reg full_next_out;  
reg empty_next_out; 

// -----------------------------
// AS: replaced with fixed width
// -----------------------------

//localparam [1:0] FS4  = 2'b00;
//localparam [1:0] FS8  = 2'b01;
//localparam [1:0] FS16 = 2'b10;
//localparam [1:0] FS32 = 2'b11;
//
//
//// AS: replaced with parameter
////reg [5:0] FIFO_DEPTH;
//
//always @(posedge pclk or negedge presetn)    // Register as this feeds into lots of logic
//begin
//   if (~presetn)
//      FIFO_DEPTH <= 4;
//   else
//   begin
//      case (fifosize)
//             FS8   : FIFO_DEPTH <= 8;
//             FS16  : FIFO_DEPTH <= 16;
//             FS32  : FIFO_DEPTH <= 32;
//           default : FIFO_DEPTH <= 4;
//      endcase
//   end
//end
   

wire [CFG_FRAME_SIZE-1:0]  data_out  = data_out_d[CFG_FRAME_SIZE-1:0];
wire                   flag_out  = data_out_d[CFG_FRAME_SIZE];


assign overflow_out = (write_in && (counter_q == FIFO_DEPTH));        /* write and fifo full */


integer i;

//------------------------------------------------------------------------------------------------------------
//infer the FIFO   - no reset required

always @(posedge pclk)
 begin  
   for (i=0; i<FIFO_DEPTH; i=i+1)
      begin
         fifo_mem_q[i] <= fifo_mem_d[i];
      end
 end


//infer the registers and register the flags 
always @(posedge pclk or negedge presetn)
   begin
   if (~presetn)
     begin
       rd_pointer_q   <= 0;
       wr_pointer_q   <= 0;
       counter_q      <= 0;
       full_out       <= 0;
       empty_out      <= 1;
       full_next_out  <= 0;
       empty_next_out <= 0;
     end
   else
     begin
       rd_pointer_q   <= rd_pointer_d;
       wr_pointer_q   <= wr_pointer_d;
       counter_q      <= counter_d;
       full_out       <= (counter_d == FIFO_DEPTH); //is next pointer equal to fifo length
       empty_out      <= (counter_d == 0);
       full_next_out  <= (counter_q == FIFO_DEPTH-1);
       empty_next_out <= (counter_q == 1);
     end
   end


integer j;

always @(*)
begin  
   for (j=0; j<FIFO_DEPTH; j=j+1)    // Hold old values
      begin
         fifo_mem_d[j] = fifo_mem_q[j];
      end

   if (write_in)
      begin
        if (counter_q != FIFO_DEPTH)
          begin
          // -----------------------------------------
          // AS: replaced with fixed size (CFG_FRAME_SIZE)
          // -----------------------------------------
          fifo_mem_d[wr_pointer_q[4:0]][CFG_FRAME_SIZE-1:0] = data_in[CFG_FRAME_SIZE-1:0];
          fifo_mem_d[wr_pointer_q[4:0]][CFG_FRAME_SIZE] = flag_in;
          end
      end

   //Read - data out always available
   data_out_dx = fifo_mem_q[rd_pointer_q[4:0]];
end


// Perform extra read mux on Byte/Half wide reads
always @(*)
  begin
     // AS: removed Byte/Half wide read option
     // flag bits are zero if count zero
     data_out_d = data_out_dx[CFG_FRAME_SIZE:0];
     
     if (counter_q == 0) data_out_d[CFG_FRAME_SIZE] = 1'b0;

  end
   


// Pointers and Flags

always @(*)
   begin
   
     if (fiforst==1'b1)
     begin
        wr_pointer_d  = 5'b00000;
        rd_pointer_d  = 5'b00000;
        counter_d     = 6'b000000;
     end 
     else
     begin
       //defaults
       counter_d    = counter_q;
       rd_pointer_d = rd_pointer_q;
       wr_pointer_d = wr_pointer_q;
    
      if (read_in)
      begin
         if (counter_q != 0)   // ignore read when empty
         begin
            if (~write_in) //if not writing decrement count of the number of objects in fifo else count stays the same
               begin 
                  counter_d = counter_q - 1'b1;
               end
            // AS: Added limits for wrap-around
            if (rd_pointer_q == FIFO_DEPTH - 1)
              rd_pointer_d = 5'b00000;
            else   
              rd_pointer_d = rd_pointer_q + 1'b1; 
         end
            
      end //~read_n

      if (write_in)
      begin
         if (counter_q != FIFO_DEPTH) // ignore write when full
         begin
           if (~read_in)
            begin
               counter_d =  counter_q + 1'b1;
            end
            // AS: Added limits for wrap-around
            if (wr_pointer_q == FIFO_DEPTH-1)
              wr_pointer_d = 5'b00000;
            else
              wr_pointer_d = wr_pointer_q + 1'b1;
              
           end //~write_n
         end
     end
   end
      
wire [5:0] fifo_count = counter_q;

endmodule
