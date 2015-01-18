/*
 * pio.c
 *
 *  Created on: 16.02.2014
 *      Author: BoBaHouD
 */

#include <am186es.h>
#include <common.h>

#include <pio.h>


int pio_configure( pio_mode_t mode, uint16_t pin_number )
{
  uint16_t pio_mode;
  uint16_t pio_dir;
  uint16_t bit, temp;

  if ( pin_number <=PIN_NUMBER_MAX ){

      bit = pin_number % 16;
      // calculate the regs by div on 16
      temp = pin_number >> 4;
      pio_mode = temp ? P1MODE : P0MODE;
      pio_dir  = temp ? P1DIR  : P0DIR;

      switch (mode) {
        case PIO_NORMAL:
          CLEAR_BIT_IN_REG( pio_mode, bit );
          CLEAR_BIT_IN_REG( pio_dir , bit );
          break;

        case PIO_IN_WITH_PULLUP:
          CLEAR_BIT_IN_REG( pio_mode, bit );
          SET_BIT_IN_REG( pio_dir , bit );
          break;

        case PIO_OUT:
          SET_BIT_IN_REG( pio_mode, bit );
          CLEAR_BIT_IN_REG( pio_dir , bit );
          break;

        case PIO_IN_WITHOUT_PULLUP:
          SET_BIT_IN_REG( pio_mode, bit );
          SET_BIT_IN_REG( pio_dir , bit );
          break;

        default:
          return PIO_INVALID_MODE;
      }

      return PIO_SUCCESS;
  }
  return PIO_INVALID_PIN_NUMBER;
}
