/*
 * soc.h
 *
 *  Created on: Mar 28, 2016
 *      Author: macrobull
 */

#ifndef SOC_H_
#define SOC_H_

#include <stdint.h>
#include <assert.h>

#include "system.h"
#include "../BSP/drivers/inc/altera_avalon_pio_regs.h"

#define hamming(a,b)	(uint8_t)(ALT_CI_HAMMING(a, b))


#endif /* SOC_H_ */
