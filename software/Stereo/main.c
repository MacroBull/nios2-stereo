/*
 * main.c
 * : /opt/Altera/QuartusPrime-15.1/quartus/../nios2eds/nios2_command_shell.sh eclipse-nios2
 *  Created on: Mar 28, 2016
 *      Author: macrobull
 * 
 * 固化函数: 
 */

#include <stdlib.h>

#include "data.h"
#include "soc.h"
#include "seg6.h"

image left, right, tof;

uint16_t height, width;

void validate() {
	height = left.height;
	width = left.width;
	assert((width == right.width) && (height == right.height));
	assert((width == tof.width) && (height == tof.height));
}

int main() {
	while (1) {
		waitfor(FLAG0);
		waitfor(FLAG1);
		readImage(&left);
		readImage(&right);
		readImage(&tof);
		validate();
		puts(FLAG1);
		waitfor(ACK);
		writeImage(tof);
	}
	return 0;
}

