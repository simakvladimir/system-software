/*
 * adc.h
 *
 *  Created on: 14.10.2014
 *      Author: Simakov Vladimir
 */

#ifndef ADC_H_
#define ADC_H_

#include <cpu/dsp2802x/DSP28x_Project.h>

typedef struct{
	Uint16 soc_number;
	Uint16 interrupt_number;
	struct CPUTIMER_VARS *timer;
	Uint16 io_input;
	Uint32 freq_in_Hz;

} adc_options_t;
void adc_init(
  void
);

void adc_register_isr(
  PINT   isr,
  adc_options_t *options
);

void adc_configure(
  adc_options_t *options
);

void adc_start(
  adc_options_t *options
);

Uint16 adc_read_result(
	Uint16 soc_number
);


//void adc_configure_soc(
//  Uint16 soc_number,
//  Uint16 int_number,
//  Uint16 timer_number,
//  Uint16 io_input
//);

#endif /* ADC_H_ */
