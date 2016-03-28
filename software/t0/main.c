
#include <stdio.h>
#include <stdint.h>

#include "soc.h"

unsigned long int i = 0, j=0;

int dude(int n){
	int r = 0, a, b;
	for (a=0;a<n;a++) for (b=0;b<n;b++){
		r += a*b;
	}
	return r;
}

int main() {
	while (1) {
		printf("SB Altera%d!@%d\n", i, NIOS2_CPU_FREQ);
		i++;
		for (j=0;j<1000;j++){
			IOWR_ALTERA_AVALON_PIO_DATA(SEG6_BASE, i*1000+i);
			usleep(1000);
		}
	}

	return 0;
}
