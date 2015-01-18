/*
 * fix_fft.h
 *
 *  Created on: 22.10.2014
 *      Author: Раиса Алексеевна
 */

#ifndef FIX_FFT_H_
#define FIX_FFT_H_

#include <cpu/dsp2802x/DSP28x_Project.h>

int16 FIX_MPY(int16 a, int16 b);

int32 fix_fft_iq(int16 fr[], int16 fi[], int16 m, int16 inverse);

#endif /* FIX_FFT_H_ */
