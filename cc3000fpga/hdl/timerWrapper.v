module timerWrapper(
					PCLK,
					PENABLE,
					PSEL,
					PRESETN,
					PWRITE,
					PREADY,
					PSLVERR,
					PADDR,
					PWDATA,
					PRDATA,
					TPS,
                    FABINT);

// APB Bus Interface
input PCLK,PENABLE, PSEL, PRESETN, PWRITE;
input  [31:0] PWDATA;
input  [7:0] PADDR;
output [31:0] PRDATA;
output PREADY, PSLVERR;
output FABINT;


// Test Interface
output [4:0] TPS; // Use for your debugging

 
assign BUS_WRITE_EN = (PENABLE && PWRITE && PSEL);
assign BUS_READ_EN = (!PWRITE && PSEL); //Data is ready during first cycle to make it availble on the bus when PENABLE is asserted

assign PREADY = 1'b1;
assign PSLVERR = 1'b0;

timer timer_0(	.pclk(PCLK),
			    .nreset(PRESETN), 
			    .bus_write_en(BUS_WRITE_EN),
			    .bus_read_en(BUS_READ_EN),
			    .bus_addr(PADDR),
			    .bus_write_data(PWDATA),
			    .bus_read_data(PRDATA),
                .fabint(FABINT)
			);


endmodule