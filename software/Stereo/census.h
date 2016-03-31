/*
 * census.h
 *
 *  Created on: Mar 31, 2016
 *      Author: macrobull
 */

#ifndef CENSUS_H_
#define CENSUS_H_

#include "soc.h"

typedef struct cStrStruct {
	uint32_t a, b, c, d;
} cStr;

#define census_setHamAcc(u32)				((uint32_t)ALT_CI_CENSUS(0xf, u32))
#define census_setCounter(u32)				((uint32_t)ALT_CI_CENSUS(0x3, u32))
#define census_getCounter()					((uint32_t)ALT_CI_CENSUS(0x7, 0))

#define hamming(a, b)			((uint32_t)ALT_CI_HAMMING(a, b))
#define hammingPlus(a, b)		((uint32_t)ALT_CI_HAMMINGPLUS(1, a, b))
#define hammingPC()				((uint32_t)ALT_CI_HAMMINGPLUS(0, 0, 0))
#define hammingAvg4(a, b)		((uint32_t)ALT_CI_HAMMINGAVG4(1, a, b))
#define hammingAC4()			((uint32_t)ALT_CI_HAMMINGAVG4(0, 0, 0))

#define census_hammingAC()		(hammingAC4())

//#define census_matrixFillWithMax()				((uint32_t)ALT_CI_CENSUS(0x0, 0))
#define census_matrixAddMax(size)				{(uint32_t)ALT_CI_CENSUS(0x1, size);} //int _i; for(_i=0;_i<size;_i++)fputs("255 ", stderr);}
#define census_matrixAdd(u8)					{(uint32_t)ALT_CI_CENSUS(0x2, u8);} // fprintf(stderr, "%hhu ", u8);}

//#define census_setCodecAL(u8_0_31, u8_32_63)	((uint32_t)ALT_CI_CENSUS(0x4, u8_0_31, u8_32_63))
//#define census_setCodecAH(u8_64_95, u8_96_119)	((uint32_t)ALT_CI_CENSUS(0x5, u8_64_95, u8_96_119))
//#define census_setCodecBL(u8_0_31, u8_32_63)	((uint32_t)ALT_CI_CENSUS(0x6, u8_0_31, u8_32_63))
//
//#define census_setCodecBH_getHamming(u8_64_95, u8_96_119)		((uint32_t)ALT_CI_CENSUS(0xd, u8_64_95, u8_96_119))
//#define census_setCodecBH_getHammingPlus(u8_64_95, u8_96_119)	((uint32_t)ALT_CI_CENSUS(0xe, u8_64_95, u8_96_119))

#define census_setCmp_getCodecLL(u8)				((uint32_t)ALT_CI_CENSUS(0x8, u8)) // fprintf(stderr, "vs %hhu\n", u8);}}

#define census_getCodecLL()						((uint32_t)ALT_CI_CENSUS(0x9, 0))
#define census_getCodecLH()						((uint32_t)ALT_CI_CENSUS(0xa, 0))
#define census_getCodecHL()						((uint32_t)ALT_CI_CENSUS(0xb, 0))
#define census_getCodecHH()						((uint32_t)ALT_CI_CENSUS(0xc, 0))


extern inline uint32_t census_hamming(const cStr cStr0, const cStr cStr1);
extern inline void census_hammingAvg(const cStr cStr0, const cStr cStr1);


#endif /* CENSUS_H_ */
