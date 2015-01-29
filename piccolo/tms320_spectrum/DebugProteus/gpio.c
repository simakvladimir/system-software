/*
 * gpio.c
 *
 *  Created on: 21.10.2014
 *      Author: Simakov Vladimir
 */
#include <cpu/dsp2802x/DSP28x_Project.h>
#include <periph/gpio/gpio.h>

void gpio_aio_configure(
  Uint16 pin_num,
  Uint32 function
)
{
	Uint16 shift = pin_num << 1;
	EALLOW;
	GpioCtrlRegs.AIOMUX1.all |= function << shift;
	EDIS;
}

void gpio_dig_configure(
  Uint16 pin_num,
  Uint32 function
)
{

}
