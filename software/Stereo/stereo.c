/*
 * stereo.c
 *
 *  Created on: Mar 29, 2016
 *      Author: macrobull
 */

#include <stdlib.h>
#include <assert.h>

#include "stereo.h"

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

void dump8(uint8_t a[], uint16_t size){
	while (size--){
		fprintf(stderr, "%3u ", *a++);
	}
	putc('\n', stderr);
}

void dump16(uint16_t a[], uint16_t size){
	while (size--){
		fprintf(stderr, "%5u ", *a++);
	}
	putc('\n', stderr);
}

void twotoone(const uint16_t a[], uint8_t b[], uint8_t mode){
	uint16_t i;
	for (i=0;i<g_totalSize;i++){
		switch (mode){
		case 1:
			b[i] = a[i] & 0xff;
			break;
		case 2:
			b[i] = MIN(0xff,a[i]);
			break;
		default:
			b[i] = a[i] >> 8;
		}
	}
}

void quadtoone(const uint32_t a[], uint8_t b[], uint8_t mode){
	uint16_t i;
	for (i=0;i<g_totalSize;i++){
		switch (mode){
		case 1:
			b[i] = a[i] & 0xff;
			break;
		case 2:
			b[i] = MIN(0xff,a[i]);
			break;
		default:
			b[i] = a[i] >> 24;
		}
	}
}


void calcRange(int16_t offset, int32_t bf, int32_t deta) {

#define width (g_width)
#define height (g_height)
#define tof (g_tof)
#define disp (g_disp)
#define dispRange (g_dispRange)

	int32_t start, end;
	uint8_t vDisp;
	uint16_t i, j;

	hRange maxRange = {.p = MAXINT};

	dispRange = (hRange*) malloc(g_totalSize*sizeof(hRange));
	assert(dispRange!=NULL);

	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++) {
			vDisp = tof[POS(i, j)];
			if ((vDisp == 0) || (vDisp + j >= width)){
				dispRange[POS(i, j)] = maxRange;
				disp[POS(i, j)] = tof[POS(i, j)];// fill disp with tof
			}else {
//				vDisp += offset;
				start = bf * vDisp / (bf + deta * vDisp) + j - offset;
				end = bf * vDisp / (bf - deta * vDisp) + j - offset;

				dispRange[POS(i, j)].s.l = MIN(MAX(start, 0), width);
				dispRange[POS(i, j)].s.r = MIN(MAX(end, 0), width);
			}
		}

#undef width
#undef height
#undef tof
#undef disp
#undef dispRange

}

void stereoMatch(image *disp, image left, image right, image tof,
		int16_t offset, int32_t bf, int32_t deta)
{
	g_width = left.width;
	g_height = left.height;
	disp->width = g_width;
	disp->height = g_height;
	disp->white = left.white;
	if (disp->data) free(disp->data);
	disp->data = (uint8_t*)malloc(g_width*g_height);
	assert(disp->data != NULL);

	g_left = left.data;
	g_right = right.data;
	g_tof = tof.data;
	g_disp = disp->data;

	calcRange(offset, bf, deta);

//	dump16(g_dRangeStart, 200);
//	dump16(g_dRangeEnd, 20);

	quadtoone((uint32_t*)g_dispRange, g_disp, 1);
}

