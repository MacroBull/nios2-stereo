/*
 * data.c
 *
 *  Created on: Mar 28, 2016
 *      Author: macrobull
 */

#include "data.h"


#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "crc32.h"
#include "seg6.h"

#define debug(x) fprintf(stderr, "DEBUG/%s:%d:%s\n", __FILE__, __LINE__, x)

void waitfor(const char target[]){
	char s[9];
	s[0] = '\0';
	while (strcmp(s, target)) {
		gets(s);
		debug(s);
	}

	puts(ACK);
}

void readImage(image *img){
	char magic[9], ack[9];
	ack[0] = '\0';
	while (strcmp(ack, ACK)) {
		gets(magic);
		assert(strcmp(magic, PGM_MAGIC) == 0);

		uint16_t width, height;
		scanf("%hu %hu\n", &width, &height);
		disp33(height, width);
		scanf("%hu\n", &img->white);
		assert((0<img->white)&&(img->white<256));

		if ((img->width == width) && (img->height == height)){
			fprintf(stderr, "Update image@%08lx\n", (uint32_t)img);
		} else {
			fprintf(stderr, "New image@%08lx:%ux%u\n", (uint32_t)img, width, height);
			img->width = width;
			img->height = height;
			if (img->data) free(img->data);
			img->data =  (uint8_t *)malloc(img->width * img->height);
			assert(img->data != NULL);
		}
		uint16_t i, j;
		for(i=0;i<height;i++) for(j=0;j<width;j++){
			img->data[POS(i,j)] = getchar();
			disp33(i, j);
		}

		uint32_t checksum = crc32(0, img->data, width * height);
		fprintf(stderr, "CRC:%08lx/%lu\n", checksum, checksum);
		printf("%08lx\n", checksum);

		fputs("GGG", stderr);
		gets(ack);
		debug(ack);
	}

}

void writeImage(const image img){
	uint32_t checksum = 0, resp_cs = 1;
	while (checksum != resp_cs) {
		printf("%s\n%u %u\n%u\n", PGM_MAGIC, img.width, img.height, img.white);
		uint16_t i, j, width = img.width;
		for(i=0;i<img.height;i++) for(j=0;j<width;j++){
			putchar(img.data[POS(i,j)]);
			disp33(i, j);
		}

		checksum = crc32(0, img.data, img.width * img.height);
		scanf("%lx", &resp_cs);
		if (checksum != resp_cs) {
			fprintf(stderr, "CRC mismatch, expected:%08lx, response:%08lx\n", checksum, resp_cs);
			puts(NAK);
		}
	}
	puts(ACK);

}
