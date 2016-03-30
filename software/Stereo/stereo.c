/*
 * stereo.c
 *
 *  Created on: Mar 29, 2016
 *      Author: macrobull
 */

#include <stdlib.h>
#include <assert.h>

#include "stereo.h"
#include "seg6.h"

uint16_t g_height, g_width;
#define g_totalSize (g_height * g_width)

uint8_t *g_left, *g_right, *g_tof, *g_disp;

typedef union {
	uint32_t p;
	struct {
		uint16_t l, r;
	} s;
} hRange;

hRange *g_dispRange;

uint8_t *g_avgLeft, *g_avgRight;

void dump8(const uint8_t a[], uint16_t size) {
	while (size--) {
		fprintf(stderr, "%3u ", *a++);
	}
	putc('\n', stderr);
}

void dump16(const uint16_t a[], uint16_t size) {
	while (size--) {
		fprintf(stderr, "%5u ", *a++);
	}
	putc('\n', stderr);
}

void twotoone(const uint16_t a[], uint8_t b[], uint8_t mode) {
	uint16_t i;
	for (i = 0; i < g_totalSize; i++) {
		switch (mode) {
		case 1:
			b[i] = a[i] & 0xff;
			break;
		case 2:
			b[i] = MIN(0xff, a[i]);
			break;
		default:
			b[i] = a[i] >> 8;
		}
	}
}

void quadtoone(const uint32_t a[], uint8_t b[], uint8_t mode) {
	uint16_t i;
	for (i = 0; i < g_totalSize; i++) {
		switch (mode) {
		case 1:
			b[i] = a[i] & 0xff;
			break;
		case 2:
			b[i] = MIN(0xff, a[i]);
			break;
		default:
			b[i] = a[i] >> 24;
		}
	}
}

////////////////////////////////////////

void validate(image left, image right) {
	assert((g_width == left.width) && (g_height == left.height));
	assert((g_width == right.width) && (g_height == right.height));
}

void calcRange(int16_t offset, int32_t bf, int32_t deta) {

#define width (g_width)
#define height (g_height)
#define tof (g_tof)
#define dispRange (g_dispRange)

	int32_t start, end;
	uint8_t vDisp;
	uint16_t i, j;

	hRange maxRange = { .p = MAXINT };

	dispRange = (hRange*) malloc(g_totalSize * sizeof(hRange));
	assert(dispRange!=NULL);

	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++) {
			vDisp = tof[POS(i, j)];
			if ((vDisp == 0) || (vDisp + j >= width)) {
				dispRange[POS(i, j)] = maxRange;
			} else {
				vDisp += offset;
				start = bf * vDisp / (bf + deta * vDisp) + j - offset;
				end = bf * vDisp / (bf - deta * vDisp) + j - offset;

				dispRange[POS(i, j)].s.l = MIN(MAX(start, 0), width);
				dispRange[POS(i, j)].s.r = MIN(MAX(end, 0), width);
			}
		}

#undef width
#undef height
#undef tof
#undef dispRange

}

void avgFilter(const uint8_t *input, uint8_t *output, uint8_t ws) {

#define width (g_width)
#define height (g_height)

	uint8_t i, j;
	int8_t x, y, sx, sy;
	uint16_t sum, t;
	assert(output != NULL);
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++) {
			sum = 0;
			sx = MIN(MIN(i, height-i), ws);
			sy = MIN(MIN(j, width-j), ws);
			for (x = -sx; x <= sx; x++)
				for (y = -sy; y <= sy; y++)
					sum += input[POS(i + x, j + y)];
			t = (sx * 2 + 1) * (sy * 2 + 1);
			output[POS(i, j)] = (sum + t / 2) / t;
			disp33(i, j);
		}

#undef width
#undef height
}

void calcAverage(){
	g_avgLeft = (uint8_t*) malloc(g_totalSize * sizeof(uint8_t));
	assert(g_avgLeft!=NULL);
	g_avgRight = (uint8_t*) malloc(g_totalSize * sizeof(uint8_t));
	assert(g_avgRight!=NULL);

	setDot(3);
	avgFilter(g_left, g_avgLeft, 5);
	setDot(0);
	avgFilter(g_right, g_avgRight, 5);
}

void stereoMatch(image *disp, image left, image right, image tof,
		int16_t	offset, int32_t deta, int32_t bf) {

	g_width = tof.width;
	g_height = tof.height;
	disp->width = g_width;
	disp->height = g_height;
	disp->white = tof.white;
	if (disp->data)
		free(disp->data);
	disp->data = (uint8_t*) malloc(g_width * g_height);
	assert(disp->data != NULL);

	g_left = left.data;
	g_right = right.data;
	g_tof = tof.data;
	g_disp = disp->data;

	validate(left, right);
	calcRange(offset, bf, deta);
	calcAverage();//3.6s@150p

//////////////////////////////////

	memcpy(g_disp, g_avgLeft, g_totalSize);

//	dump16((uint16_t*)g_dispRange, 20); // 95	111	22	27	112	130	121	141	60	70	57	66	27	32	88	101	119	137	48	55

//	quadtoone((uint32_t*)g_dispRange, g_disp, 1);
//	avgFilter(g_left, g_disp, 5); //1.8s

}

