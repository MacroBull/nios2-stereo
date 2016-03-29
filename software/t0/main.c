
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
	char s[80];
	while (1) {
		gets(s);
		puts("Echo:");
		puts(s);
		printf("SB%d@%dHz!\n", i, NIOS2_CPU_FREQ);
		i++;
		for (j=0;j<1000;j++){
			IOWR_ALTERA_AVALON_PIO_DATA(SEG6_BASE, i*1000+i);
			usleep(1000);
		}
	}

	return 0;
}
