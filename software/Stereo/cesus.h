/*
 * cesus.h
 *
 *  Created on: Mar 31, 2016
 *      Author: macrobull
 */

#ifndef CESUS_H_
#define CESUS_H_

#include "soc.h"

typedef struct cStrStruct {
	uint32_t a, b, c, d;
} cStr;

#define cesuse_getCycles()		(ALT_CI_CESUS(0x7, 0, 0))

#define hamming(a, b)			(ALT_CI_HAMMING(a, b))
#define hammingPlus(a, b)		(ALT_CI_HAMMINGPLUS(1, a, b))
#define hammingPC()				(ALT_CI_HAMMINGPLUS(0, 0, 0))
#define hammingAvg4(a, b)		(ALT_CI_HAMMINGAVG4(1, a, b))
#define hammingAC4()			(ALT_CI_HAMMINGAVG4(0, 0, 0))

#define cesus_hammingAC()		(hammingAC4())

extern inline uint32_t cesus_hamming(const cStr cStr0, const cStr cStr1);
extern inline void cesus_hammingAvg(const cStr cStr0, const cStr cStr1);


#endif /* CESUS_H_ */
