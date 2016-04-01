/*
 * data.h
 *
 *  Created on: Mar 28, 2016
 *      Author: macrobull
 */

#ifndef DATA_H_
#define DATA_H_

#include <stdint.h>
#include <stdio.h>

#define PGM_MAGIC "P5"
#define FLAG0 "START"
#define FLAG1 "BOOM"
#define ACK "ACK"
#define NAK "NAK"


#define	MIN(a, b)	((a)<(b)?(a):(b))
#define	MAX(a, b)	((a)>(b)?(a):(b))
#define	MAXINT	(~0)
#define	ABS(x,y) ((x)>(y)?(x-y):(y-x))
#define	POS(row, col) ((row)*width+(col))
#define SET_EOS(s, pos) {(s)[(pos)] = '\n';}

#define DEBUG(x) fprintf(stderr, "DEBUG/%s:%d:%s\n", __FILE__, __LINE__, x)

typedef struct {
	uint16_t width, height, white;
	uint8_t *data;
} image;

extern char *request(char e[], const char r[]);
extern char *listen(char e[], const char r[]);
extern void waitfor(const char target[]);

extern int32_t *readParams();
extern void readParams1(int32_t *p);
extern image *readImage(image *img);
extern void writeImage(const image img);


#endif /* DATA_H_ */
