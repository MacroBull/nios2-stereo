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



#define cesus_hamming(cStr0, cStr1)	({ALT_CI_CESUS(0x8, cStr0.a, cStr0.b);ALT_CI_CESUS(0x9, cStr0.c, cStr0.d);ALT_CI_CESUS(0xa, cStr1.a, cStr1.b);ALT_CI_CESUS(0xb, cStr1.c, cStr1.d);(uint32_t)ALT_CI_CESUS(0xc, 0, 0);})




#endif /* CESUS_H_ */
