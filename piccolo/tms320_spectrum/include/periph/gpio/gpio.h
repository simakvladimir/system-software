/*
 * gpio.h
 *
 *  Created on: 21.10.2014
 *      Author: Simakov Vladimir
 */

#ifndef GPIO_H_
#define GPIO_H_

#define GPIO_AIO_ADC_FN 0x2
#define GPIO_AIO_AIO_FN 0x0

void gpio_aio_configure(
  Uint16 pin_num,
  Uint32 function
);

void gpio_dig_configure(
  Uint16 pin_num,
  Uint32 function
);

#endif /* GPIO_H_ */
