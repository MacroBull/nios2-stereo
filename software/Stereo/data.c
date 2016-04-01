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

#include "soc.h"
#include "crc32.h"
#include "seg6.h"

char *request(char e[], const char r[]){
	char resp[9];
	SET_EOS(resp, 0);
	while (strcmp(resp, e) ^ (e[0] == '\0')) {
		if (r[0]){
			puts(r);
			fprintf(stderr, "request	tx->:%s\n", r);
		}
		gets(resp);
		fprintf(stderr, "request	rx<-:%s\n", resp);
		SET_EOS(resp, 8);
	}
	strcpy(e, resp);
	return e;
}

char *listen(char e[], const char r[]){
	char req[9];
	SET_EOS(req, 0);
	while (strcmp(req, e) ^ (e[0] == '\0')) {
		gets(req);
		fprintf(stderr, "listen	rx<-:%s\n", req);
		SET_EOS(req, 8);
	}
	if (r[0]) {
		puts(r);
		fprintf(stderr, "listen	tx->:%s\n", r);
	}
	strcpy(e, req);
	return e;
}

int32_t *readParams(){
	uint8_t l, i;
	int32_t *p;
	scanf("%hhu", &l);
	fprintf(stderr, "Reading %u parameters...\n", l);
	p = (int32_t*)malloc(l*sizeof(int32_t));
	assert(p != NULL);
	i = l;
	while (i--) scanf("%ld", p++);
	return p - l;
}

image *readImage(image *img) {
	char magic[9], resp[9];
	SET_EOS(resp, 0);
	while (strcmp(resp, ACK)) {
		gets(magic);
		SET_EOS(magic, 8);
		assert(strcmp(magic, PGM_MAGIC) == 0);

		uint16_t width, height;
		scanf("%hu %hu\n", &width, &height);
		disp33(height, width);
		scanf("%hu\n", &img->white);
		assert((0 < img->white) && (img->white < 256));

		if ((img->width == width) && (img->height == height)) {
			fprintf(stderr, "Update image@%08lx\n", (uint32_t) img);
		} else {
			fprintf(stderr, "New image@%08lx:%ux%u\n", (uint32_t) img, width,
					height);
			img->width = width;
			img->height = height;
			if (img->data)
				FREE(img->data);
			img->data = (uint8_t *) malloc(
					img->width * img->height * sizeof(uint8_t));
			assert(img->data != NULL);
		}
		uint16_t i, j;
		for (i = 0; i < height; i++)
			for (j = 0; j < width; j++) {
				img->data[POS(i, j)] = getchar();
				disp33(i, j);
			}

		uint32_t checksum = crc32(0, img->data, width * height);
		fprintf(stderr, "CRC:%08lx\n", checksum);
		printf("%08lx\n", checksum);

		gets(resp);
		SET_EOS(resp, 8);
		DEBUG(resp);
	}
	return img;

}

void writeImage(const image img) {
	uint32_t checksum = 0, resp_cs = 1;
	while (checksum != resp_cs) {
		printf("%s\n%u %u\n%u\n", PGM_MAGIC, img.width, img.height, img.white);
		uint16_t i, j, width = img.width;
		for (i = 0; i < img.height; i++)
			for (j = 0; j < width; j++) {
				putchar(img.data[POS(i,j)]);
				disp33(i, j);
			}

		checksum = crc32(0, img.data, img.width * img.height);
		scanf("%lx", &resp_cs);
//		char c = '\0';while (c != '\n') c = getchar();
		if (checksum != resp_cs) {
			fprintf(stderr, "CRC mismatch, expected:%08lx, response:%08lx\n",
					checksum, resp_cs);
			puts(NAK);
		}
	}
	puts(ACK);

}
