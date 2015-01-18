/*
 * fir.h
 *
 *  Created on: 29.10.2014
 *      Author: Simakov Vladimir
 */

#ifndef FIR_H_
#define FIR_H_

#include <cpu/dsp2802x/DSP28x_Project.h>


#define FIR_LENGTH 37 /* 128000 / 40000 */

void fir_init(
  void
);

void fir_process_real(
  int16 input[],
  int16 length
);


#endif /* FIR_H_ */
