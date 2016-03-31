
module test(oRes,
	iA, iB,
	iOp,
	iClk, iClk_en,
	iReset);

	output	[31:0]	oRes;
	
	input	[31:0]	iA, iB;
	input	[3:0]	iOp;
	
	input	iClk, iReset, iClk_en;
	
	reg	[31:0]	rRes, rM, rN, rCnt;
	
	assign oRes = rRes;

	
	always@(negedge iClk or posedge iReset) begin
		if (iReset) begin
			rCnt <= 32'h0;
		end else if (iClk_en) begin
			rCnt <= rCnt + 1;
			if (iOp == 4'd1) rM <= iA;
		end
		
	end
	
	always@(posedge iClk) begin
		if (iReset) begin
			rRes <= 32'h0;
		end else if (iClk_en) begin
			if (iOp == 4'd2) rRes <= rM;
			if (iOp == 4'd3) rRes <= rCnt;
		end
	end
	
endmodule

	
