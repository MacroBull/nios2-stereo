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
#include "cesus.h"

image left, right, tof, disp;

int main() {
	DEBUG("Booted.");

	cStr a = {.a=1,.b=0,.c=0,.d=0}, b={.a=0,.b=0,.c=0,.d=0};
	while(1){
//		fprintf(stderr, "%lu\n", cesus_hamming(a, b));
		fprintf(stdout, "res=%lu\n", cesus_hamming(a, b));
		usleep(1000*100);
	}

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

