
module cesus(oRes,
	iA, iB,
	iOp, iReset,
	iClk, iClk_en);

	output	[31:0]	oRes;
	
	input	[31:0]	iA, iB;
	input	[3:0]	iOp;
	
	input	iClk, iClk_en, iReset;
	
	reg	[31:0]	rRes;
	reg	[967:0]	rMatrix; //8*121-1
	reg	[3:0]	rMask;
	reg	[7:0]	rCmp;
	reg	[119:0]	rCodec0, rCodec1;
	reg	[31:0]	rAcc;

	reg	[31:0]	rCnt;
	reg	rDone;
	
	wire	[119:0]	wCodec, wH;
	wire	[7:0]	wSum;
	
	assign	oRes = rRes;
	assign	wH = rCodec0 ^ rCodec1;
	assign	wSum = wH[0]+wH[1]+wH[2]+wH[3]+wH[4]+wH[5]+wH[6]+wH[7]+wH[8]+wH[9]+wH[10]+wH[11]+wH[12]+wH[13]+wH[14]+wH[15]+wH[16]+wH[17]+wH[18]+wH[19]+wH[20]+wH[21]+wH[22]+wH[23]+wH[24]+wH[25]+wH[26]+wH[27]+wH[28]+wH[29]+wH[30]+wH[31]+wH[32]+wH[33]+wH[34]+wH[35]+wH[36]+wH[37]+wH[38]+wH[39]+wH[40]+wH[41]+wH[42]+wH[43]+wH[44]+wH[45]+wH[46]+wH[47]+wH[48]+wH[49]+wH[50]+wH[51]+wH[52]+wH[53]+wH[54]+wH[55]+wH[56]+wH[57]+wH[58]+wH[59]+wH[60]+wH[61]+wH[62]+wH[63]+wH[64]+wH[65]+wH[66]+wH[67]+wH[68]+wH[69]+wH[70]+wH[71]+wH[72]+wH[73]+wH[74]+wH[75]+wH[76]+wH[77]+wH[78]+wH[79]+wH[80]+wH[81]+wH[82]+wH[83]+wH[84]+wH[85]+wH[86]+wH[87]+wH[88]+wH[89]+wH[90]+wH[91]+wH[92]+wH[93]+wH[94]+wH[95]+wH[96]+wH[97]+wH[98]+wH[99]+wH[100]+wH[101]+wH[102]+wH[103]+wH[104]+wH[105]+wH[106]+wH[107]+wH[108]+wH[109]+wH[110]+wH[111]+wH[112]+wH[113]+wH[114]+wH[115]+wH[116]+wH[117]+wH[118]+wH[119];
	
	genvar i;
	
	generate
		for (i=0;i<60;i=i+1) begin: codec_low
			assign wCodec[i] = rMatrix[(i+1)*8-1:i*8]<rCmp;
		end
		for (i=61;i<=120;i=i+1) begin: codec_high
			assign wCodec[i-1] = rMatrix[(i+1)*8-1:i*8]<rCmp;
		end
	endgenerate
	
	always@(posedge iClk or posedge iReset or negedge iClk_en)begin
		if (iReset) begin
			rCnt <= 0;
			rDone <= 0;
		end else if (!iClk_en) rDone <= 0;
		else begin
			rDone <= 1;
			if (!rDone) begin
				rCnt <= rCnt + 1;
				case (iOp)
					4'h1:begin // add 3 data and valid rMask
						rMask <= iB[3:0];
						rMatrix[967:24] <= rMatrix[967-24:0];
						rMatrix[23:0] <= iA[23:0] | (24'hffffff << (iB[3:0]*8));
					end
					4'h2:begin // add 8 data with rMask
						rMatrix[967:64] <= rMatrix[967-64:0];
						rMatrix[63:0] <= {iB, iA} | ({64{1'b1}} << (((rMask>4'd3)?rMask-4'd3:4'd0)*6'd8));
					end
					4'h3:begin // set counter
						rCnt <= rCnt * iA + iB;
					end
					
					4'h4:begin // read codec0 63:0
// 						rCodec0[63:0] <= {iB, iA};
					end
					4'h5:begin // read codec0 119:64
// 						rCodec0[119:64] <= {iB[23:0], iA};
					end
					4'h6:begin // read codec1 63:0
						rCodec1[63:0] <= {iB, iA};
					end
					
					////////////////////With result///////////////////////////
					4'h7:begin // get counter
					end
					4'h8:begin // compare with iA, get output 31:0
						rCmp <= iA[7:0];
						rCodec1 <= wCodec; // move last to rCodec1
					end
					4'h9:begin // get output 31:0
					end
					4'ha:begin // get output 63:32
					end
					4'hb:begin // get output 95:64
					end
					4'hc:begin // get output 119:96
					end
					4'hd:begin // read codec1 119:64, hamming !
						rCodec1[119:64] <= {iB[23:0], iA};
					end
					4'he:begin // read codec1 119:64, hamming plus!
						rCodec1[119:64] <= {iB[23:0], iA};
					end
					4'hf:begin // muladd rAcc: set and add
					end
					default:begin// (0):clear
						rMatrix <= {968{1'b1}};
					end
				endcase
			end
		end
	end
	
	
	always@(negedge iClk or posedge iReset)begin
		if (iReset) begin
			rAcc <= 31'd0;
			rRes <= 31'd0;
		end else if (iClk_en && rDone) case (iOp)
			4'h4:begin // read codec0 63:0
				rCodec0[63:0] <= {iB, iA};
			end
			4'h5:begin // read codec0 119:64
				rCodec0[119:64] <= {iB[23:0], iA};
			end
			4'h8:begin // compare with iA, get output 31:0
				rCodec0 <= wCodec; // move this to rCodec0
			end
		//////////////////////////////////////
			4'hf:begin
				rAcc <= rAcc * iA + iB;
				rRes <= rAcc * iA + iB;
			end
			4'he:begin
				rAcc <= rAcc + {24'd0, wSum};
				rRes <= rAcc + {24'd0, wSum};
			end
			4'hd:begin
				rRes <= {24'd0, wSum};
			end
			4'h9:begin // get output 31:0
				rRes <= wCodec[31:0];
			end
			4'ha:begin // get output 63:32
				rRes <= wCodec[63:32];
			end
			4'hb:begin // get output 95:64
				rRes <= wCodec[95:64];
			end
			4'hc:begin // get output 119:96
				rRes <= {8'd0, wCodec[119:96]};
			end
			4'h8:begin // get output 31:0
				rRes <= wCodec[31:0];
			end
			4'h7:begin // get counter
				rRes <= rCnt;
			end
			default: begin
				rRes <= 0;
			end
		endcase
	end
	
endmodule

	
