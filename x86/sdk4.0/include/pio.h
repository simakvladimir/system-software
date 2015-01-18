/*
 * pio.h
 *
 *  Created on: 16.02.2014
 *      Author: BoBaHouD
 */

#ifndef PIO_H_
#define PIO_H_

#define PIN_NUMBER_MIN 0
#define PIN_NUMBER_MAX 31

typedef enum {
  PIO_NORMAL,
  PIO_IN_WITH_PULLUP,
  PIO_OUT,
  PIO_IN_WITHOUT_PULLUP
} pio_mode_t;

enum {
  PIO_SUCCESS,
  PIO_INVALID_PIN_NUMBER,
  PIO_INVALID_MODE
};

int pio_configure( pio_mode_t mode, uint16_t pin_number );

#endif /* PIO_H_ */
