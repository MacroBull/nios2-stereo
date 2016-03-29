

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
	
module hammingPlus(oOut,
	iA, iB,
	iClk,
	iReset
	);
	
	output	[31:0]	oOut;
	
	input	[31:0]	iA, iB;
	input	iClk, iReset;
	
	wire	[31:0]	wD;
	
	reg	[31:0]	rOut;
	
	assign oOut = rOut;
	assign wD = iA ^ iB;
	
	always@(negedge iClk or negedge iReset)begin
		if (!iReset) rOut = 0;
		else rOut = rOut + ((((wD[0]+wD[1])+(wD[2]+wD[3]))+((wD[4]+wD[5])+(wD[6]+wD[7])))+
		(((wD[8]+wD[9])+(wD[10]+wD[11]))+((wD[12]+wD[13])+(wD[14]+wD[15]))))+
		((((wD[16]+wD[17])+(wD[18]+wD[19]))+((wD[20]+wD[21])+(wD[22]+wD[23])))+
		(((wD[24]+wD[25])+(wD[26]+wD[27]))+((wD[28]+wD[29])+(wD[30]+wD[31]))));
	end

endmodule
	
	
	
