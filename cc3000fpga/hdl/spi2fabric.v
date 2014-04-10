
module spi2fabric(in_from_fabric_di, out_to_spi_di, in_from_spi_do, out_to_fabric_do, in_from_spi_clk, out_to_fabric_clk, in_from_spi_ss, out_to_fabric_ss);

input in_from_fabric_di, in_from_spi_do, in_from_spi_clk, in_from_spi_ss;
output out_to_spi_di, out_to_fabric_do, out_to_fabric_clk, out_to_fabric_ss;

assign out_to_spi_di = in_from_fabric_di;
assign out_to_fabric_do = in_from_spi_do;
assign out_to_fabric_clk = in_from_spi_clk;
assign out_to_fabric_ss = in_from_spi_ss;

endmodule

