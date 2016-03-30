/*
 * main.c
 * : /opt/Altera/QuartusPrime-15.1/quartus/../nios2eds/nios2_command_shell.sh eclipse-nios2
 *  Created on: Mar 28, 2016
 *      Author: macrobull
 * 
 */

#include <stdlib.h>
#include <unistd.h>

#include "data.h"
#include "soc.h"
#include "seg6.h"
#include "stereo.h"

image left, right, tof, disp;

int main() {
	DEBUG("Booted.");

//	while(1){
//		setDot(3);
//		disp33(333,333);
//	}

	int32_t *p;

	while (1) {
		listen(FLAG0, ACK);
		p = readParams();
		listen(FLAG1, ACK);
		setDot(3);
		readImage(&left);
		setDot(0);
		readImage(&right);
		setDot(7);
		readImage(&tof);
		DEBUG("Started.");
		stereoMatch(&disp,
				left, right, tof,
				(int16_t)p[0], p[1], p[2]);
		DEBUG("Done.");
		request(ACK, FLAG1);
		writeImage(disp);
	}
	return 0;
}

