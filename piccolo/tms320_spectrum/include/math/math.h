/*
 * math.h
 *
 *  Created on: 01.11.2014
 *      Author: BoBaHouD
 */

#ifndef MATH_H_
#define MATH_H_

#include <cpu/dsp2802x/DSP28x_Project.h>

int16 FIX_MPY(int16 a, int16 b);
int sqrt_q15(
  int16 in,
  int16 * pOut);

#endif /* MATH_H_ */
