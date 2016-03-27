
module soc (
	clk_clk,
	reset_reset_n,
	seg6_export,
	uart_rxd,
	uart_txd);	

	input		clk_clk;
	input		reset_reset_n;
	output	[23:0]	seg6_export;
	input		uart_rxd;
	output		uart_txd;
endmodule
