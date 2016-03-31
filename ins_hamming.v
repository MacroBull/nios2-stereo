

module hamming(oOut,
	iA, iB
	);
	
	output	[31:0]	oOut;
	
	input	[31:0]	iA, iB;
	
	wire	[31:0]	wC;
	
	assign oOut = ((((wC[0]+wC[1])+(wC[2]+wC[3]))+((wC[4]+wC[5])+(wC[6]+wC[7])))+
		(((wC[8]+wC[9])+(wC[10]+wC[11]))+((wC[12]+wC[13])+(wC[14]+wC[15]))))+
		((((wC[16]+wC[17])+(wC[18]+wC[19]))+((wC[20]+wC[21])+(wC[22]+wC[23])))+
		(((wC[24]+wC[25])+(wC[26]+wC[27]))+((wC[28]+wC[29])+(wC[30]+wC[31]))));
	assign wC = iA ^ iB;
	
endmodule
	
module hammingPlus(oRes,
	iA, iB,
	iOp,
	iClk, iClk_en,
	iReset);

	output	[31:0]	oRes;
	
	input	[31:0]	iA, iB;
	input	iOp;
	
	input	iClk, iReset, iClk_en;
	
	reg	[31:0]	rAcc, rCnt, rRes;
	reg	rDone;
	
	wire	[31:0]	wH;
	
	assign	oRes = rRes;
	
	hamming	comp0(wH, iA, iB);
	
	always@(posedge iClk or posedge iReset or negedge iClk_en) begin
		if (iReset) begin
			rRes <= 32'h0;
			rDone <= 0;
		end else if (!iClk_en) rDone <= 0;
		else begin
			rDone <= 1;
			if (!rDone) begin
				if (!iOp) begin // clear and report result
					rRes <= rAcc;
				end
			end
		end
	end
	
	always@(negedge iClk or posedge iReset) begin
		if (iReset) begin
			rAcc <= 32'h0;
			rCnt <= 32'h0;
		end else if (iClk_en && rDone) begin
			if (!iOp) begin
				rAcc <= 32'h0;
				rCnt <= 32'h0;
			end else begin
				rAcc <= rAcc + wH;
				rCnt <= rCnt + 32'h1;
			end
		end
	end

endmodule

	
module hammingAvg4(oRes,
	iA, iB,
	iOp,
	iClk, iClk_en,
	iReset);

	output	[31:0]	oRes;
	
	input	[31:0]	iA, iB;
	input	iOp;
	
	input	iClk, iReset, iClk_en;
	
	reg	[31:0]	rAcc, rCnt, rRes;
	reg	rDone;
	
	wire	[31:0]	wH;
	
	assign	oRes = rRes;
	
	hamming	comp0(wH, iA, iB);
	
	always@(posedge iClk or posedge iReset or negedge iClk_en) begin
		if (iReset) begin
			rRes <= 32'h0;
			rDone <= 0;
		end else if (!iClk_en) rDone <= 0;
		else begin
			rDone <= 1;
			if (!rDone) begin
				if (!iOp) begin // clear and report result
					rRes <= (rCnt>0)?(rAcc*4 + rCnt/2)/rCnt:0;
				end
			end
		end 
	end
	
	always@(negedge iClk or posedge iReset) begin
		if (iReset) begin
			rAcc <= 32'h0;
			rCnt <= 32'h0;
		end else if (iClk_en && rDone) begin
			if (!iOp) begin
				rAcc <= 32'h0;
				rCnt <= 32'h0;
			end else begin
				rAcc <= rAcc + wH;
				rCnt <= rCnt + 32'h1;
			end
		end
	end

endmodule

	
