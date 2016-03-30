/*
* 8 segment LED encoding modules
* This converts unsigned interger to code of common anode 8 segment LED, in 1 digit, 4 digits and 8 digits
* @Author: Macrobull
* @Project: DE2-70 Audio Effector and Visualization
* @Date: July 2014
* @Github: https://github.com/MacroBull/10189020-FPGA_application_work
*/

module hex1(oHex, 
	iNum, iDot);
	
	output	[7:0]	oHex;
	
	input	[3:0]	iNum;
	input	iDot;
	
	assign	oHex[7] = ~iDot;
	assign	oHex[6:0] = (iNum == 4'h0)?7'b1000000:
		(iNum == 4'h1)?7'b1111001:
		(iNum == 4'h2)?7'b0100100:
		(iNum == 4'h3)?7'b0110000:
		(iNum == 4'h4)?7'b0011001:
		(iNum == 4'h5)?7'b0010010:
		(iNum == 4'h6)?7'b0000010:
		(iNum == 4'h7)?7'b1111000:
		(iNum == 4'h8)?7'b0000000:
		(iNum == 4'h9)?7'b0010000:
		(iNum == 4'ha)?7'b0001000:
		(iNum == 4'hb)?7'b0000011:
		(iNum == 4'hc)?7'b0100111:
		(iNum == 4'hd)?7'b0100001:
		(iNum == 4'he)?7'b0000110:
		(iNum == 4'hf)?7'b0001110:
		7'b1111111;
	
endmodule
