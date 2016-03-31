

module cesus_tb;
	
	wire	[31:0]	r;//, b, c, d;
	reg	[31:0]	a, b, c, d;
	
	reg	[31:0]	cnt;
	wire	clk;
	reg	clk_en, reset;
	reg	[3:0]	op;
	
	initial cnt <= 0;
	always #1 cnt<=cnt+1;
	assign clk = cnt[0];
	
	initial begin
		reset <= 1;
		#2
		reset <= 0;
	end
	
	initial clk_en <= 0;
	always #4 clk_en <= ~clk_en;
	
	initial begin
		op <= 4'h0;#10
		op <= 4'h1;#8
		op <= 4'h2;#8
		op <= 4'h1;#8
		op <= 4'h2;#8
		op <= 4'h1;#8
		op <= 4'h2;#8
		op <= 4'h1;#8
		op <= 4'h2;#8
		op <= 4'h1;#8
		op <= 4'h2;#8
		op <= 4'h1;#8
		op <= 4'h2;#8
		op <= 4'hd;#8
		op <= 4'h8;#8
		op <= 4'h8;#8
		op <= 4'hd;#8
		op <= 4'h7;
	end
	
	cesus comp0(r, a, b, op, reset, clk, clk_en);
	
// 	assign b = cnt*3+2;
// 	assign c = cnt*5+7;

	initial begin
		#10
		a<= 32'h00808080;b<=32'ha1234567;
		#8
		a<= 32'he7654321;b<=32'h8500000a;
		#80
		a<=32'hc2800000;b<=32'h0;#10
		a<=32'h2;b<=32'h3;
		#80
		a<=0;b<=0;
	end
	
	initial begin
		#10
		c <= 0;
	end
	
	
	initial begin
		$dumpfile("out.vcd");
		$dumpvars(0, cesus_tb);
// 		$monitor(clk);
		
		#200;
		$finish;
	end
	
endmodule
	
