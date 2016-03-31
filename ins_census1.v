
module census(oRes,
	oDone,
	iA, iB,
	iOp,
	iClk, iClk_en, 
	iStart, iReset);

	output	[31:0]	oRes;

	output	oDone;
	
	input	[31:0]	iA, iB;
	input	[3:0]	iOp;
	
	input	iClk, iClk_en, iStart, iReset;
	
	reg	[31:0]	rRes;
	reg	[967:0]	rMatrix; //8*121-1
	reg	[3:0]	rMask;
	reg	[7:0]	rCmp;
	reg	[119:0]	rCodec0, rCodec1;
	reg	[31:0]	rAcc;
	reg	[3:0]	rMux;
	
	reg	rDone;
	
	wire	[119:0]	wCodec, wHamming;
	wire	[7:0]	wSum;
	
	assign	oRes = (rMux==4'd1)?wCodec[31:0]:(rMux==4'd2)?{24'd0, wSum}:(rMux==4'd3)?(rAcc + {24'd0, wSum}):rRes;
	assign	oDone = rDone;
	assign	wHamming = rCodec0 ^ rCodec1;
	assign	wSum = wHamming[0]+wHamming[1]+wHamming[2]+wHamming[3]+wHamming[4]+wHamming[5]+wHamming[6]+wHamming[7]+wHamming[8]+wHamming[9]+wHamming[10]+wHamming[11]+wHamming[12]+wHamming[13]+wHamming[14]+wHamming[15]+wHamming[16]+wHamming[17]+wHamming[18]+wHamming[19]+wHamming[20]+wHamming[21]+wHamming[22]+wHamming[23]+wHamming[24]+wHamming[25]+wHamming[26]+wHamming[27]+wHamming[28]+wHamming[29]+wHamming[30]+wHamming[31]+wHamming[32]+wHamming[33]+wHamming[34]+wHamming[35]+wHamming[36]+wHamming[37]+wHamming[38]+wHamming[39]+wHamming[40]+wHamming[41]+wHamming[42]+wHamming[43]+wHamming[44]+wHamming[45]+wHamming[46]+wHamming[47]+wHamming[48]+wHamming[49]+wHamming[50]+wHamming[51]+wHamming[52]+wHamming[53]+wHamming[54]+wHamming[55]+wHamming[56]+wHamming[57]+wHamming[58]+wHamming[59]+wHamming[60]+wHamming[61]+wHamming[62]+wHamming[63]+wHamming[64]+wHamming[65]+wHamming[66]+wHamming[67]+wHamming[68]+wHamming[69]+wHamming[70]+wHamming[71]+wHamming[72]+wHamming[73]+wHamming[74]+wHamming[75]+wHamming[76]+wHamming[77]+wHamming[78]+wHamming[79]+wHamming[80]+wHamming[81]+wHamming[82]+wHamming[83]+wHamming[84]+wHamming[85]+wHamming[86]+wHamming[87]+wHamming[88]+wHamming[89]+wHamming[90]+wHamming[91]+wHamming[92]+wHamming[93]+wHamming[94]+wHamming[95]+wHamming[96]+wHamming[97]+wHamming[98]+wHamming[99]+wHamming[100]+wHamming[101]+wHamming[102]+wHamming[103]+wHamming[104]+wHamming[105]+wHamming[106]+wHamming[107]+wHamming[108]+wHamming[109]+wHamming[110]+wHamming[111]+wHamming[112]+wHamming[113]+wHamming[114]+wHamming[115]+wHamming[116]+wHamming[117]+wHamming[118]+wHamming[119];
	
	genvar i;
	
	generate
		for (i=0;i<60;i=i+1) begin: codec_low
			assign wCodec[i] = rMatrix[(i+1)*8-1:i*8]<rCmp;
		end
		for (i=61;i<=120;i=i+1) begin: codec_high
			assign wCodec[i-1] = rMatrix[(i+1)*8-1:i*8]<rCmp;
		end
	endgenerate
	
	always@(posedge iClk or negedge iReset)begin
		if (!iReset) begin
		end else if (iClk_en) if (iStart) begin
			case (iOp)
				4'h1:begin // add 3 data and valid rMask
					rMask <= iB[3:0];
					rMatrix[967:24] <= rMatrix[967-24:0];
					rMatrix[23:0] <= iA[23:0] | (24'hffffff << (iB*8));
				end
				4'h2:begin // add 8 data with rMask
					rMatrix[967:64] <= rMatrix[967-64:0];
					rMatrix[63:0] <= {iB, iA} | ({64{1'b1}} << ((rMask-3)*6'd8));
				end
				4'h3:begin // compare with iA
					rCmp <= iA[7:0];
					rMux <= 4'd1;  // get wCodec[31:0] immediately
					rCodec0 <= wCodec; // move last to rCodec0 trick?
				end
// 				4'h4:begin // get output 31:0
// 					rMux <= 4'd0;
// 					rRes <= wCodec[31:0];
// 				end
				4'h5:begin // get output 63:32
					rMux <= 4'd0;
					rRes <= wCodec[63:32];
				end
				4'h6:begin // get output 95:64
					rMux <= 4'd0;
					rRes <= wCodec[95:64];
				end
				4'h7:begin // get output 119:96
					rMux <= 4'd0;
					rRes <= {8'd0, wCodec[119:96]};
				end
				4'h8:begin // read codec0 63:0
					rCodec0[63:0] <= {iB, iA};
				end
				4'h9:begin // read codec0 119:0
					rCodec0[119:64] <= {iB[23:0], iA};
				end
				4'ha:begin // read codec1 63:0
					rCodec1[63:0] <= {iB, iA};
				end
				4'hb:begin // read codec1 119:0, hamming !
					rCodec1[119:64] <= {iB[23:0], iA};
					rMux <= 4'd2;  // get wSum immediately
				end
				4'hc:begin // read codec1 119:0, hamming plus!
					rCodec1[119:64] <= {iB[23:0], iA};
					rAcc <= rAcc + {24'd0, wSum};
					rMux <= 4'd3;  // get wSum immediately
				end
				4'hd:begin // set rAcc
					rAcc <= iA;
				end
				end
				4'he:begin // add rAcc
					rMux <= 4'd0;
					rAcc <= rAcc + iA;
					rRes <= rAcc + iA;
				end
				default:begin// (0):clear
					rMatrix <= {968{1'b1}};
				end
			endcase
		end
	end
	
	always@(negedge iClk or negedge iReset)begin
		if (!iReset) begin
			rDone <= 1'd0;
		end else begin
			rDone <= iClk_en;
		end
	end
	
	
endmodule

	
