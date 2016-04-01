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
#include "census.h"


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

int16_t dxMin, dxMax;

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

void calcRange(int16_t offset, int32_t bf, int32_t deta, uint8_t checkboard) {

#define width (g_width)
#define height (g_height)
#define tof (g_tof)
#define disp (g_disp)
#define dispRange (g_dispRange)

	int32_t start, end;
	uint8_t vDisp;
	uint16_t i, j;

	if (dispRange)
		FREE(dispRange);
	dispRange = (hRange*) malloc(g_totalSize * sizeof(hRange));
	assert(dispRange!=NULL);

	dxMin = width;
	dxMax = 0;

	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++) {
			vDisp = tof[POS(i, j)];
			if ((vDisp == 0) || (vDisp + j >= width)) {
				dispRange[POS(i, j)].p = MAXINT;
				disp[POS(i, j)] = checkboard?(((i/checkboard+j/checkboard)&1)?85:170):tof[POS(i, j)];
			} else {
				vDisp += offset;
				start = bf * vDisp / (bf + deta * vDisp) + j - offset;
				end = bf * vDisp / (bf - deta * vDisp) + j - offset;

				dispRange[POS(i, j)].s.l = MIN(MAX(start, 0), width);
				dispRange[POS(i, j)].s.r = MIN(MAX(end, 0), width);

				if (dispRange[POS(i, j)].s.l - j > dxMax)
					dxMax = dispRange[POS(i, j)].s.l - j;
				if (dispRange[POS(i, j)].s.l - j < dxMin)
					dxMin = dispRange[POS(i, j)].s.l - j;
			}
		}

	fprintf(stderr, "dMin, dMax = %d, %d\n", dxMin, dxMax);

#undef width
#undef height
#undef tof
#undef disp
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

void avgFilter1(const uint8_t input[], uint8_t output[], uint8_t ws) {

#define width (g_width)
#define height (g_height)

	uint8_t i, j;
	int8_t x, y, sx, sy;
	uint16_t sum = 0, cnt;
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
				sum += + colSum[j + ws] - colSum[j - ws - 1];
				output[POS(i, j)] = (sum + hCnt) / fCnt;
			}
			disp33(i, j);
		}
	}
	FREE(colSum);

#undef width
#undef height
}

void calcAverage() {
	if (g_avgLeft)
		FREE(g_avgLeft);
	g_avgLeft = (uint8_t*) malloc(g_totalSize * sizeof(uint8_t));
	assert(g_avgLeft!=NULL);
	if (g_avgRight)
		FREE(g_avgRight);
	g_avgRight = (uint8_t*) malloc(g_totalSize * sizeof(uint8_t));
	assert(g_avgRight!=NULL);

	setDot(3);
	avgFilter(g_left, g_avgLeft, 5);
	setDot(0);
	avgFilter(g_right, g_avgRight, 5);
}

void calcAverage1() {
	if (g_avgLeft)
		FREE(g_avgLeft);
	g_avgLeft = (uint8_t*) malloc(g_totalSize * sizeof(uint8_t));
	assert(g_avgLeft!=NULL);
	if (g_avgRight)
		FREE(g_avgRight);
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
		for (j = 0; j < width; j++) {

			uint8_t avg = avgImg[POS(i, j)];
			cStr cs = { .a = 0, .b = 0, .c = 0, .d = 0 };
			uint8_t shift = 0;

			int8_t x, y;
			uint16_t iy, jx;

			for (y = CensusWin_Up; y <= CensusWin_Down; y++) {
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
//								*(((uint32_t*) &cs) + (shift >> 5)) |= 1
//										<< (shift & 0x1f);
								*(((uint32_t*) &cs) + ((120-shift) >> 5)) |= 1
										<< ((120-shift) & 0x1f);
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

	uint16_t i, j, x, y, top, bottom, front, back;
	cStr cs;

	census_setCounter(0);

	for (i = 0; i < height; i++) {
		//census_matrixFillWithMax();
		for (j = 0; j < 5; j++) {
			census_matrixAddMax(6);
			census_matrixAddMax(5);
		}

		top = MAX(0, i - 5);
		bottom = MIN(height-1, i + 5);
		front = top + 5 - i;
		back = i + 5 - bottom;
		for (x = 0; x <= 5; x++) {
			if (front)
				census_matrixAddMax(front);
			for (y = top; y <= bottom; y++)
				census_matrixAdd(img[POS(y, x)]);
			if (back)
				census_matrixAddMax(back);
		}

		cs.a = census_setCmp_getCodecLL(avgImg[POS(i, 0)]);
		cs.b = census_getCodecLH();
		cs.c = census_getCodecHL();
		cs.d = census_getCodecHH();
		output[POS(i, 0)] = cs;

		for (j = 1; j <= width - 5; j++) {
			if (front)
				census_matrixAddMax(front);
			for (y = top; y <= bottom; y++)
				census_matrixAdd(img[POS(y, j + 5)]);
			if (back)
				census_matrixAddMax(back);

			cs.a = census_setCmp_getCodecLL(avgImg[POS(i, j)]);
			cs.b = census_getCodecLH();
			cs.c = census_getCodecHL();
			cs.d = census_getCodecHH();
			output[POS(i, j)] = cs;

		}

		for (; j < width; j++) {
			census_matrixAddMax(6);
			census_matrixAddMax(5);
			cs.a = census_setCmp_getCodecLL(avgImg[POS(i, j)]);
			cs.b = census_getCodecLH();
			cs.c = census_getCodecHL();
			cs.d = census_getCodecHH();
			output[POS(i, j)] = cs;
		}
		disp33(i, j);
	}

	fprintf(stderr, "Census instructions invoked %lu times.\n",
	census_getCounter());

#undef width
#undef height

}

void calcCensusString() {
	if (g_cStrLeft)
		FREE(g_cStrLeft);
	g_cStrLeft = (cStr*) malloc(g_totalSize * sizeof(cStr));
	assert(g_cStrLeft!=NULL);
	if (g_cStrRight)
		FREE(g_cStrRight);
	g_cStrRight = (cStr*) malloc(g_totalSize * sizeof(cStr));
	assert(g_cStrRight!=NULL);

	setDot(3);
	calcCStr(g_left, g_avgLeft, g_cStrLeft);
	setDot(0);
	calcCStr(g_right, g_avgRight, g_cStrRight);
}

void calcCensusString1() {
	if (g_cStrLeft)
		FREE(g_cStrLeft);
	g_cStrLeft = (cStr*) malloc(g_totalSize * sizeof(cStr));
	assert(g_cStrLeft!=NULL);
	if (g_cStrRight)
		FREE(g_cStrRight);
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
	if (g_cbRegion)
		FREE(g_cbRegion);
	g_cbRegion = (region*) malloc(g_totalSize * sizeof(region));
	assert(g_cbRegion!=NULL);

	setDot(0);
	calcCbRegion(g_right, g_cbRegion);

}

void calcDisparity() {

#define width (g_width)
#define height (g_height)
#define disp (g_disp)

#define dispRange (g_dispRange)
#define cbRegion (g_cbRegion)
#define cStrLeft (g_cStrLeft)
#define cStrRight (g_cStrRight)

	uint16_t i, j;
	uint16_t x, y, u, d, l, r;
	int16_t dx, edx;
	uint16_t p = 0;
	uint32_t cost, minCost, cnt;

	census_hammingAC();
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++) {
			if (dispRange[POS(i, j)].p < MAXINT) {
				minCost = MAXINT;
				edx = dispRange[POS(i, j)].s.r - j;
				u = cbRegion[POS(i, j)].u;
				d = cbRegion[POS(i, j)].d;

				for (dx = dispRange[POS(i, j)].s.l - j; dx <= edx; dx++) {
					cost = cnt = 0;
					for (y = u; y <= d; y++) {
						l = cbRegion[POS(y, j)].l;
						r = MIN(cbRegion[POS(y, j)].r, width-1-dx);
						for (x = l; x <= r; x++) {
							cost += census_hamming(cStrLeft[POS(y, x + dx)], cStrRight[POS(y, x)]);
							cnt ++;
						}
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
#undef disp
#undef dispRange
#undef cbRegion
#undef cStrLeft
#undef cStrRight

}

void calcDisparity1() {

#define width (g_width)
#define height (g_height)
#define disp (g_disp)

#define dispRange (g_dispRange)
#define cbRegion (g_cbRegion)
#define cStrLeft (g_cStrLeft)
#define cStrRight (g_cStrRight)

	uint16_t i, j, k;
	uint16_t y, u, d, l, r1, dxRange;
	int16_t dx, edx;
	uint16_t p = 0;
	uint16_t cost, minCost, cnt;

	uint16_t accStep0;
	uint32_t accStep1;
	uint16_t *lineAcc;

	dxRange = dxMax + 1 - dxMin;
	lineAcc = (uint16_t*) malloc(
	height * dxRange * (width + 1) * sizeof(uint16_t));
	assert(lineAcc!=NULL);

	setDot(3);
	accStep0 = (width + 1);
	accStep1 = dxRange * accStep0;
	for (i = 0; i < height; i++)
		for (j = 0; j < dxRange; j++) {
			dx = j + dxMin;
			cost = 0;
			lineAcc[i * accStep1 + j * accStep0] = 0;
			for (k = MAX(0, -dx); k < MIN(width, width-dx); k++) {
				cost += census_hamming(cStrLeft[POS(i, k + dx)],
				cStrRight[POS(i, k)]);
				lineAcc[i * accStep1 + j * accStep0 + k + 1] = cost;
			}
//			assert(cost<60000); // max width:250
			disp33(i, j);
		}

	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++) {
			if (dispRange[POS(i, j)].p < MAXINT) {
				minCost = MAXINT;
				edx = dispRange[POS(i, j)].s.r - j;
				u = cbRegion[POS(i, j)].u;
				d = cbRegion[POS(i, j)].d;

				for (dx = dispRange[POS(i, j)].s.l - j; dx <= edx; dx++) {
					cost = cnt = 0;
					for (y = u; y <= d; y++) {
						l = cbRegion[POS(y, j)].l;
						r1 = MIN(cbRegion[POS(i, j)].r+1, width-dx);
						cost += lineAcc[y * accStep1 + (dx - dxMin) * accStep0
								+ r1]
								- lineAcc[y * accStep1 + (dx - dxMin) * accStep0
										+ l];
						cnt += r1 - l;
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

	FREE(lineAcc);

#undef width
#undef height
#undef disp
#undef dispRange
#undef cbRegion
#undef cStrLeft
#undef cStrRight

}

void stereoMatch(image *disp, image *left, image *right, image *tof,
		int16_t offset, int32_t deta, int32_t bf,
		uint8_t checkboard) {

	g_width = tof->width;
	g_height = tof->height;
	disp->width = g_width;
	disp->height = g_height;
	disp->white = tof->white;
	if (disp->data)
		FREE(disp->data);
	disp->data = (uint8_t*) malloc(g_width * g_height);
	assert(disp->data != NULL);

	g_left = left->data;
	g_right = right->data;
	g_tof = tof->data;
	g_disp = disp->data;

	puts("validate");
	validate(*left, *right);

	puts("calcRange");
	calcRange(offset, bf, deta, checkboard); // standalone
	FREE(tof->data);

//	calcAverage(); // 3.6s@150p // standalone
	puts("calcAverage1");
	calcAverage1(); // 0.7s@150p // standalone
//	calcCensusString(); //6.4s@150p // depend on average
	puts("calcCensusString1");
	calcCensusString1(); //0.4s@150p // depend on average

	FREE(g_avgLeft);
	FREE(g_avgRight);
	FREE(left->data);

	puts("calcCrossBasedRegion");
	calcCrossBasedRegion(); //1.4s@150p // standalone

	FREE(right->data);

	left->width = right->width = tof->width = 0;
//
//	calcDisparity(); // 160s@150p
	puts("calcDisparity1");
	calcDisparity1(); // 18s@150p

	/*
	 * opt-off:95s
	 * +avg:-0.9s
	 * +census:-2.1s
	 * opt-full:9.5s
	 */
//////////////////////////////////

//	memcpy(g_disp, g_avgLeft, g_totalSize);

//	dump16((uint16_t*)g_dispRange, 20); // 95	111	22	27	112	130	121	141	60	70	57	66	27	32	88	101	119	137	48	55

//	dump32h((uint32_t*) g_cStrLeft + 4 * 30, 4 * 20); //Left vs left!//120-shift//00000000 f0800000 00000001 00000000 00000000 e1000000 00000001 00000000 00000000 c2800000 00000001 00000000 00000000 85000000 00000001 00000000 00000000 0a000000 00000001 00000000 00000000 00000000 00000000 00000000 00000000 19800000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 57800000 00000000 00000000 00000000 af800000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 f7800000 00000001 00000000 00000000 ef800000 00000001 00000000

//	dump16((uint16_t*)g_cbRegion, 4*20);//  0  14   0  14   0   1   0  15   0   0   0  16   0   0   0  17   0   0   0  18   0   0   0  19   0   0   0  20   0   0   0  21   0   0   0  22   0   0   0  23   0   0   0  24   0   0   0  25   0   0   0  26   0   0   0  27   0   0   0  28   0   0   1  29   0   0   2  30   0   0   3  31   0   0   4  32   0   0   5  33

//	quadtoone((uint32_t*)g_dispRange, g_disp, 1);
//	avgFilter(g_left, g_disp, 5); //1.8s

}

