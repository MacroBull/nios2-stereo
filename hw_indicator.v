

module hw_indicator(
    iCLK, 
    iRST_n,
    oLED
);

    input iCLK;
    input iRST_n;
    output oLED;
    
	parameter FREQ = 100*1000000;
    
    reg	[31:0] rCnt;
	 reg	rLED;
    
    assign oLED = rLED;
    
    always@( posedge iCLK or negedge iRST_n) begin
	    if(!iRST_n) begin
	        rCnt <= 0;
	        rLED <= 1;
		end
		else if (rCnt == FREQ/2) begin
	        rCnt <= 0;
	        rLED <= ~rLED;
		end
	    else rCnt <= rCnt + 1;
	end
	    

endmodule
