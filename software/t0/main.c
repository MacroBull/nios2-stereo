
#include <stdio.h>
#include <stdint.h>

#include "soc.h"

unsigned long int i = 0, j=0;

int main() {
	while (1) {
		printf("SB Altera!\n");
		i++;
		for (j=0;j<1000;j++){
			IOWR_ALTERA_AVALON_PIO_DATA(SEG6_BASE, i*1000+i);
			usleep(1000);
		}
	}

	return 0;
}
