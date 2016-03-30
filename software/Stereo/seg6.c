/*
 * seg6.c
 *
 *  Created on: Mar 29, 2016
 *      Author: macrobull
 */

#include "soc.h"
#include "seg6.h"

uint8_t dot = 7;

void disp6(uint32_t n){
	IOWR_ALTERA_AVALON_PIO_DATA(SEG6_BASE, n);
}

void disp33(uint16_t a, uint16_t b){
	disp6( (a*1000 + b) | (dot<<20));
}

void setDot(uint8_t p){
	dot = p;
}


