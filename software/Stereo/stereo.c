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
#include "cesus.h"

uint16_t g_height, g_width;
#define g_totalSize (g_height * g_width)

uint8_t *g_left, *g_right, *g_tof, *g_disp;

uint8_t *g_avgLeft, *g_avgRight;

typedef union {
	uint32_t p;
	struct {
		uint16_t l, r;
	} s;
} hRange;

hRange *g_dispRange;

cStr *g_cStrLeft, *g_cStrRight;

typedef struct regionStruct {
	uint16_t u, d, l, r;
} region;

region *g_cbRegion;

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

void dump32(const uint32_t a[], uint16_t size) {
	while (size--) {
		fprintf(stderr, "%10lu ", *a++);
	}
	putc('\n', stderr);
}

void dump32h(const uint32_t a[], uint16_t size) {
	while (size--) {
		fprintf(stderr, "%08lx ", *a++);
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

void avgFilter(const uint8_t input[], uint8_t output[], uint8_t ws) {

#define width (g_width)
#define height (g_height)

	uint8_t i, j;
	int8_t x, y, sx, sy;
	uint16_t sum, t;
	assert(output != NULL);
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++) { // #TODO 优化:差量v，HDL
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

void avgFilter1(const uint8_t input[], uint8_t output[], uint8_t ws) {

#define width (g_width)
#define height (g_height)

	uint8_t i, j;
	int8_t x, y, sx, sy;
	uint16_t sum, cnt;
	uint16_t fCnt = (ws * 2 + 1) * (ws * 2 + 1);
	uint16_t hCnt = fCnt / 2;
	uint16_t *colSum;

	assert(output != NULL);
	colSum = (uint16_t*) malloc(width * sizeof(uint16_t));
	assert(colSum != NULL);
	for (j = 0; j < width; j++) {
		colSum[j] = 0;
		for (i = 0; i <= ws * 2; i++)
			colSum[j] += input[POS(i, j)];
	}

	for (i = 0; i < height; i++) {
		if ((i > ws) && (i + ws < height))
			for (j = 0; j < width; j++)
				colSum[j] = colSum[j] + input[POS(i + ws, j)]
						- input[POS(i - ws - 1, j)];
		for (j = 0; j < width; j++) {
			if ((i - ws < 1) || (i + ws + 1 >= height) || (j - ws < 1)
					|| (j + ws + 1 >= width)) {
				sum = 0;
				sx = MIN(MIN(i, height-i), ws);
				sy = MIN(MIN(j, width-j), ws);
				for (x = -sx; x <= sx; x++)
					for (y = -sy; y <= sy; y++)
						sum += input[POS(i + x, j + y)];
				cnt = (sx * 2 + 1) * (sy * 2 + 1);
				output[POS(i, j)] = (sum + cnt / 2) / cnt;
			} else {
				sum = sum + colSum[j + ws] - colSum[j - ws - 1];
				output[POS(i, j)] = (sum + hCnt) / fCnt;
			}
			disp33(i, j);
		}
	}

#undef width
#undef height
}

void calcAverage() {
	g_avgLeft = (uint8_t*) malloc(g_totalSize * sizeof(uint8_t));
	assert(g_avgLeft!=NULL);
	g_avgRight = (uint8_t*) malloc(g_totalSize * sizeof(uint8_t));
	assert(g_avgRight!=NULL);

	setDot(3);
	avgFilter1(g_left, g_avgLeft, 5);
	setDot(0);
	avgFilter1(g_right, g_avgRight, 5);
}

void calcCStr(const uint8_t img[], const uint8_t avgImg[], cStr output[]) {

#define width (g_width)
#define height (g_height)

	uint16_t i, j;

	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++) {  // #TODO HDL操作类

			uint8_t avg = avgImg[POS(i, j)];
			cStr cs = { .a = 0, .b = 0, .c = 0, .d = 0 };
			uint8_t shift = 0;

			int8_t x, y;
			uint16_t iy, jx;

			for (y = CensusWin_Up; y <= CensusWin_Down; y++) { //#TODO 并行比较，１１更新
				iy = i + y;
				if ((iy < 0) || (iy >= height))
					shift += CensusWin_Right - CensusWin_Left + 1;
				else
					for (x = CensusWin_Left; x <= CensusWin_Right; x++)
						if (x || y) {
							shift += 1;

							jx = j + x;
							if ((jx > 0) && (jx < width)
									&& (img[POS(iy, jx)] < avg))
								/*cs |= 1 << shift;*/
								*(((uint32_t*) &cs) + (shift >> 5)) |= 1
										<< (shift & 0x1f);
//								*(((uint32_t*) &cs) + ((120-shift) >> 5)) |= 1
//										<< ((120-shift) & 0x1f);
						}

			}
			output[POS(i, j)] = cs;
			disp33(i, j);
		}

#undef width
#undef height

}

void calcCStr1(const uint8_t img[], const uint8_t avgImg[], cStr output[]) {

#define width (g_width)
#define height (g_height)

	uint16_t i, j;

	for (j = 0; j < width; j++)
		for (i = 0; i < height; i++) {  // #TODO HDL操作类

			uint8_t avg = avgImg[POS(i, j)];
			cStr cs = { .a = 0, .b = 0, .c = 0, .d = 0 };
			uint8_t shift = 0;

			int8_t x, y;
			uint16_t iy, jx;

			for (y = CensusWin_Up; y <= CensusWin_Down; y++) { //#TODO 并行比较，１１更新
				iy = i + y;
				if ((iy < 0) || (iy >= height))
					shift += CensusWin_Right - CensusWin_Left + 1;
				else
					for (x = CensusWin_Left; x <= CensusWin_Right; x++)
						if (x || y) {
							shift += 1;

							jx = j + x;
							if ((jx > 0) && (jx < width)
									&& (img[POS(iy, jx)] < avg))
								/*cs |= 1 << shift;*/
								*(((uint32_t*) &cs) + (shift >> 5)) |= 1
										<< (shift & 0x1f);
//								*(((uint32_t*) &cs) + ((120-shift) >> 5)) |= 1
//										<< ((120-shift) & 0x1f);
						}

			}
			output[POS(i, j)] = cs;
			disp33(i, j);
		}

#undef width
#undef height

}

void calcCensusString() {

	g_cStrLeft = (cStr*) malloc(g_totalSize * sizeof(cStr));
	assert(g_cStrLeft!=NULL);
	g_cStrRight = (cStr*) malloc(g_totalSize * sizeof(cStr));
	assert(g_cStrRight!=NULL);

	setDot(3);
	calcCStr1(g_left, g_avgLeft, g_cStrLeft);
	setDot(0);
	calcCStr1(g_right, g_avgRight, g_cStrRight);

}

void calcCbRegion(const uint8_t image[], region *cbRegion) {

#define width (g_width)
#define height (g_height)

	uint8_t colorDis, colorDisPre, gray, grayPre, grayCur;
	uint16_t i, j, t, u, d, l, r;

	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++) {
			u = d = i;
			l = r = j;

			grayPre = gray = image[POS(i, j)];
			for (t = 1; t < MIN(L1, i + 1); t++) {

				grayCur = image[POS(i - t, j)];
				colorDis = ABS(gray, grayCur);
				colorDisPre = ABS(grayPre, grayCur);

				if ((colorDis >= TAO1) || (colorDisPre >= TAO1))
					break;
				if (t >= L2 && colorDis >= TAO2)
					break;

				u = i - t; //
				grayPre = grayCur;
			}
			grayPre = gray;
			for (t = 1; t < MIN(L1, height-i); t++) {

				grayCur = image[POS(i + t, j)];
				colorDis = ABS(gray, grayCur);
				colorDisPre = ABS(grayPre, grayCur);

				if ((colorDis >= TAO1) || (colorDisPre >= TAO1))
					break;
				if (t >= L2 && colorDis >= TAO2)
					break;

				d = i + t; //
				grayPre = grayCur;
			}

			grayPre = gray;
			for (t = 1; t < MIN(L1, j + 1); t++) {

				grayCur = image[POS(i, j - t)];
				colorDis = ABS(gray, grayCur);
				colorDisPre = ABS(grayPre, grayCur);

				if ((colorDis >= TAO1) || (colorDisPre >= TAO1))
					break;
				if (t >= L2 && colorDis >= TAO2)
					break;

				l = j - t; //
				grayPre = grayCur;
			}
			grayPre = gray;
			for (t = 1; t < MIN(L1, width-j); t++) {

				grayCur = image[POS(i, j + t)];
				colorDis = ABS(gray, grayCur);
				colorDisPre = ABS(grayPre, grayCur);

				if ((colorDis >= TAO1) || (colorDisPre >= TAO1))
					break;
				if (t >= L2 && colorDis >= TAO2)
					break;

				r = j + t; //
				grayPre = grayCur;
			}

			cbRegion[POS(i, j)].u = u;
			cbRegion[POS(i, j)].d = d;
			cbRegion[POS(i, j)].l = l;
			cbRegion[POS(i, j)].r = r;
			disp33(i, j);
		}

#undef width
#undef height

}

void calcCrossBasedRegion() {

	g_cbRegion = (region*) malloc(g_totalSize * sizeof(region));
	assert(g_cbRegion!=NULL);

	setDot(0);
	calcCbRegion(g_right, g_cbRegion);

}

void calcDisparity() {

#define width (g_width)
#define height (g_height)
#define tof (g_tof)
#define disp (g_disp)

#define dispRange (g_dispRange)
#define cbRegion (g_cbRegion)
#define cStrLeft (g_cStrLeft)
#define cStrRight (g_cStrRight)

	uint16_t i, j;
	uint16_t x, y;
	int16_t dx, edx;
	uint16_t p, cnt;
	uint32_t cost, minCost;

	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++) {
			if (dispRange[POS(i, j)].p == MAXINT) { //pixels can't be matched
				disp[POS(i, j)] = tof[POS(i, j)];
			} else {
				minCost = MAXINT;
				edx = dispRange[POS(i, j)].s.r - j;
				for (dx = dispRange[POS(i, j)].s.l - j; dx <= edx; dx++) {
					cost = cnt = 0;
					for (y = cbRegion[POS(i, j)].u; y <= cbRegion[POS(i, j)].d;
							y++)
						for (x = cbRegion[POS(i, j)].l;
								x <= MIN(cbRegion[POS(i, j)].r, width-1-dx);
								x++) {
							cost += cesus_hamming(cStrLeft[POS(y, x + dx)],
							cStrRight[POS(y, x)]);
							cnt++;
						}
					cost = (cost + cnt / 2) / cnt;
					if (cost < minCost) {
						minCost = cost;
						p = j + dx;
					}
					setDot(dx & 0x7);
				}

				disp[POS(i, j)] = p - j;
				disp33(i, j);
			}
		}

#undef width
#undef height
#undef tof
#undef disp
#undef dispRange
#undef cbRegion
#undef cStrLeft
#undef cStrRight

}

void stereoMatch(image *disp, image left, image right, image tof,
		int16_t offset, int32_t deta, int32_t bf) {

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
	calcRange(offset, bf, deta); // standalone
//	calcAverage(); // 3.6s@150p // standalone
	calcAverage(); // 0.7s@150p // standalone
	calcCrossBasedRegion(); //1.4s@150p // standalone
//	calcCensusString(); //6.4s@150p // depend on average
	calcCensusString(); //6.6s@150p // depend on average
//
	calcDisparity();

//////////////////////////////////

//	memcpy(g_disp, g_avgLeft, g_totalSize);

//	dump16((uint16_t*)g_dispRange, 20); // 95	111	22	27	112	130	121	141	60	70	57	66	27	32	88	101	119	137	48	55

//	dump32h((uint32_t*) g_cStrLeft + 4 * 30, 4 * 20); //Left vs left!//120-shift//00000000 f0800000 00000001 00000000 00000000 e1000000 00000001 00000000 00000000 c2800000 00000001 00000000 00000000 85000000 00000001 00000000 00000000 0a000000 00000001 00000000 00000000 00000000 00000000 00000000 00000000 19800000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 57800000 00000000 00000000 00000000 af800000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 f7800000 00000001 00000000 00000000 ef800000 00000001 00000000

//	dump16((uint16_t*)g_cbRegion, 4*20);//  0  14   0  14   0   1   0  15   0   0   0  16   0   0   0  17   0   0   0  18   0   0   0  19   0   0   0  20   0   0   0  21   0   0   0  22   0   0   0  23   0   0   0  24   0   0   0  25   0   0   0  26   0   0   0  27   0   0   0  28   0   0   1  29   0   0   2  30   0   0   3  31   0   0   4  32   0   0   5  33

//	quadtoone((uint32_t*)g_dispRange, g_disp, 1);
//	avgFilter(g_left, g_disp, 5); //1.8s

}

