/*
 * seg6.c
 *
 *  Created on: Mar 29, 2016
 *      Author: macrobull
 */

#include "soc.h"
#include "seg6.h"

void disp6(uint32_t n){
	IOWR_ALTERA_AVALON_PIO_DATA(SEG6_BASE, n);
}

void disp33(uint16_t a, uint16_t b){
	disp6(a*1000 + b);
}


