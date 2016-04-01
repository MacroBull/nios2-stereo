
module displayScanner(oSel,
	iCLK);
	
	output [5:0] oSel;
	
	input iCLK;
	
	reg [5:0] rSel;
	reg [15:0] rCnt;
	
	assign oSel = rSel;
	
	always@(posedge iCLK) begin
		if (!rCnt) begin
			if (rSel) rSel <= rSel << 1;
			else rSel <= 6'b1;
		end
		rCnt <= rCnt + 1'd1;
	end
	
endmodule

module seg_hex(oSeg, oSel,
	iNum,
	iCLK);
	
	output	[7:0]	oSeg;
	output	[5:0]	oSel;
	
	input	[23:0]	iNum;
	input	iCLK;
	
	wire	[5:0]	wSel;
	
	assign oSel = ~wSel;
	
	hex1 comp0(oSeg,
		wSel[5-0]?iNum[3:0]:wSel[5-1]?iNum[7:4]:wSel[5-2]?iNum[11:8]:
		wSel[5-3]?iNum[15:12]:wSel[5-4]?iNum[19:16]:wSel[5-5]?iNum[23:20]
		:0, 0);	
	displayScanner comp1(wSel, iCLK);
	
endmodule

module seg_hex_dot(oSeg, oSel,
	iNum, iDot,
	iCLK);
	
	output	[7:0]	oSeg;
	output	[5:0]	oSel;
	
	input	[23:0]	iNum;
	input	[2:0]	iDot;
	input	iCLK;
	
	wire	[5:0]	wSel, wTmp;
	
	assign oSel = ~wSel;
	assign wTmp = wSel >> (5-iDot);
	
	hex1 comp0(oSeg,
		wSel[5-0]?iNum[3:0]:wSel[5-1]?iNum[7:4]:wSel[5-2]?iNum[11:8]:
		wSel[5-3]?iNum[15:12]:wSel[5-4]?iNum[19:16]:wSel[5-5]?iNum[23:20]
		:0, wTmp[0]);	
	displayScanner comp1(wSel, iCLK);
	
endmodule

module bcd(oSeg,
	iDec);
	
	output	[23:0]	oSeg;
	
	input	[19:0]	iDec;
	
	assign oSeg[3:0] = iDec % 20'd10;
	assign oSeg[7:4] = iDec / 20'd10 % 20'd10;
	assign oSeg[11:8] = iDec / 20'd100 % 20'd10;
	assign oSeg[15:12] = iDec / 20'd1000 % 20'd10;
	assign oSeg[19:16] = iDec / 20'd10000 % 20'd10;
	assign oSeg[23:20] = iDec / 20'd100000 % 20'd10;
	
endmodule


module seg_dual(oSeg, oSel,
	iNum, iIsHex,
	iCLK);
	
	output	[7:0]	oSeg;
	output	[5:0]	oSel;
	
	input	[23:0]	iNum;
	input	iIsHex;
	input	iCLK;
	
	wire [23:0]	wHex;
	wire [7:0]	wSeg0, wSeg1;
	wire [5:0]	wSel0, wSel1;
	
	assign oSeg = iIsHex?wSeg0:wSeg1;
	assign oSel = iIsHex?wSel0:wSel1;
	
	seg_hex	comp0(wSeg0, wSel0, iNum, iCLK);
	seg_hex_dot	comp1(wSeg1, wSel1, wHex, iNum[22:20], iCLK);
	bcd		comp2(wHex, iNum[19:0]);
	
endmodule


