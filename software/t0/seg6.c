

void disp6(uint32_t n){
	IOWR_ALTERA_AVALON_PIO_DATA(SEG6_BASE, n);
}

void disp33(uint16_t a, uint16_t b){
	disp6(a*1000 + b);
}
