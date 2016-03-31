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

//	puts("000000");
//	usleep(100*1000);
//	puts("111111");
//	cesus_hammingAC();
//	cStr a = {.a=0xf0800000,.b=0xe1000000,.c=0xc2800000,.d=0}, b={.a=0x0a000070,.b=0x85000000,.c=0x19800000,.d=0};
//	while(1){
//		cesus_hammingAvg(a, b);
//		uint32_t r = cesus_hammingAC();
////		fprintf(stdout, "res=%ld\n", hammingPC());
//		fprintf(stdout, "res=%ld\n", r);
//		fprintf(stderr, "res=%ld\n", r);
//		usleep(1000*100);
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

