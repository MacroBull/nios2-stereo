
module acc23(oOut,
	iCLK);
	
	output [23:0] oOut;
	
	input iCLK;
	
	reg [23:0] rOut;
	
	assign oOut = rOut;
	
	always@(posedge iCLK) begin
		rOut <= rOut + 24'd30201;
	end
	
endmodule

