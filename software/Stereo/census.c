/*
 * census.c
 *
 *  Created on: Mar 31, 2016
 *      Author: macrobull
 */

#include "census.h"

inline uint32_t census_hamming(const cStr cStr0, const cStr cStr1){

//	173s
//	ALT_CI_CENSUS(0x4, cStr0.a, cStr0.b);
//	ALT_CI_CENSUS(0x5, cStr0.c, cStr0.d);
//	ALT_CI_CENSUS(0x6, cStr1.a, cStr1.b);
//	return ALT_CI_CENSUS(0xd, cStr1.c, cStr1.d);

//	169s
	return hamming(cStr0.a, cStr1.a) + hamming(cStr0.b, cStr1.b) + hamming(cStr0.c, cStr1.c) + hamming(cStr0.d, cStr1.d);

}



inline void census_hammingAvg(const cStr cStr0, const cStr cStr1){
//	165s
	hammingAvg4(cStr0.a, cStr1.a);
	hammingAvg4(cStr0.b, cStr1.b);
	hammingAvg4(cStr0.c, cStr1.c);
	hammingAvg4(cStr0.d, cStr1.d);

}

