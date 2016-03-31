

module census_tb;
	
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
		op <= 4'h1;#80
		op <= 4'h1;#50
		op <= 4'h2;#30
		op <= 4'h2;#8
		op <= 4'h2;#8
		op <= 4'h1;#8
		op <= 4'h1;#8
		op <= 4'h1;#8
		op <= 4'h1;#8
		op <= 4'h1;#8
		op <= 4'h8;#8
		op <= 4'h8;#8
		op <= 4'hd;#8
		op <= 4'h7;
	end
	
	census comp0(r, a, op, reset, clk, clk_en);
	
// 	assign b = cnt*3+2;
// 	assign c = cnt*5+7;

	initial begin
		#10
		a<=8'h6;
		#80
		#80
		#50
		a<=10;
		#50
		;
	end
	
	initial begin
		#10
		c <= 0;
	end
	
	
	initial begin
		$dumpfile("out.vcd");
		$dumpvars(0, census_tb);
// 		$monitor(clk);
		
		#400;
		$finish;
	end
	
endmodule
	
