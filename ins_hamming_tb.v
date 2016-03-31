

module hamming_tb;
	
	wire	[31:0]	a;//, b, c, d;
	reg	[31:0]	b, c, d;
	
	reg	[31:0]	cnt;
	wire	clk;
	reg	clk_en, reset;
	reg	[0:0]	op;
	
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
		op <= 0;
		#10
		op <= 1;
		#34
		op <= 0;
	end
	
	hammingAvg4 comp0(a, b, c, op, clk, clk_en, reset);
	
// 	assign b = cnt*3+2;
// 	assign c = cnt*5+7;

	initial begin
		#2
		b <= 32'hf0800000;c<=32'h0a000070;
		#8
		b<= 32'he1000000;c<=32'h85000000;
		#8
		b<=32'hc2800000;c<=32'h19800000;
		#8
		b<=0;c<=0;
	end
	
	
	initial begin
		$dumpfile("out.vcd");
		$dumpvars(0,hamming_tb);
		$monitor(clk);
		
		#100;
		$finish;
	end
	
endmodule
	
