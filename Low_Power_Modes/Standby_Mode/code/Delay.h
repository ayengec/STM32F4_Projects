/*
 * Delay.h
 *
 *  Created on: Jun 22, 2021
 *      Author: alicanyengec
 */

#ifndef SRC_DELAY_H_
#define SRC_DELAY_H_

#include <stdint.h>

void TIM6Config (void);

void Delay_us (uint16_t us);

void Delay_ms (uint16_t ms);


#endif /* SRC_DELAY_H_ */
