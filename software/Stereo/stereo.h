
#ifndef STEREO_H_
#define STEREO_H_

#include "data.h"

#define CensusWin_Left -5 //census transfor window size
#define CensusWin_Right 5
#define CensusWin_Up -5
#define CensusWin_Down 5

#define TAO1 20    //parameters of cross-based region
#define TAO2 6
#define L1 15
#define L2 8


extern void stereoMatch(image *disp,
				image left, image right, image tof,
				int16_t	offset,
				int32_t deta, int32_t bf); // BF and DETA has same unit
				
				
				
#endif
