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

#define POS(row, col) (row*width+col)


#define DEBUG(x) fprintf(stderr, "DEBUG/%s:%d:%s\n", __FILE__, __LINE__, x)

typedef struct {
	uint16_t width, height, white;
	uint8_t *data;
} image;

extern void waitfor(const char target[]);
extern void readImage(image *img);
extern void writeImage(const image img);


#endif /* DATA_H_ */
