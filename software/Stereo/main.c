/*
 * main.c
 * : /opt/Altera/QuartusPrime-15.1/quartus/../nios2eds/nios2_command_shell.sh eclipse-nios2
 *  Created on: Mar 28, 2016
 *      Author: macrobull
 * 
 * 固化函数: 
 */

#include <stdlib.h>
#include <unistd.h>

#include "data.h"
#include "soc.h"
#include "seg6.h"
#include "stereo.h"

image left, right, tof, disp;

uint16_t g_height, g_width;

void validate() {
	g_height = left.height;
	g_width = left.width;
	assert((g_width == right.width) && (g_height == right.height));
	assert((g_width == tof.width) && (g_height == tof.height));
}

int main() {

//	uint32_t i = 0;
//	while (1){
//		fprintf(stderr, "%lu %u\n", i, ALT_CI_HAMMINGP(i-1, i));
//		i+=1;
//		usleep(1000*1000);
//
//	}


	while (1) {
		waitfor(FLAG0);
		waitfor(FLAG1);
		readImage(&left);
		readImage(&right);
		readImage(&tof);
		validate();
		stereoMatch(&disp,
				left, right, tof,
				100, (int32_t)(146.316*3646.308), 100);
		puts(FLAG1);
		waitfor(ACK);
		writeImage(disp);
	}
	return 0;
}

