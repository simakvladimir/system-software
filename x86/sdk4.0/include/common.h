/*
 * common.h
 *
 *  Created on: 16.02.2014
 *      Author: BoBaHouD
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <port.h>
#include <serial.h>
#include <pio.h>

#define ENDL "\r\n"

#define JUMPER_RS_PIO_PIN 0
#define JUMPER_MODEM_PIO_PIN 1
#define JUMPER_MIF_PIO_PIN 2
#define JUMPER_LOCK_PIO_PIN 3
#define JUMPER_EXT1_PIO_PIN 4
#define JUMPER_PROG_PIO_PIN 5
#define JUMPER_EXT2_PIO_PIN 6


void jumpers_init( void );
uint8_t jumper_get_state( uint16_t pin );

#define SET_BIT_IN_REG( addr, bit ) \
  do{\
    outpw( addr, inpw( addr ) | ( 1 << bit ) );  \
  } while(0)

#define CLEAR_BIT_IN_REG( addr, bit ) \
  do{\
    outpw( addr, inpw( addr ) & ~( 1 << bit ) );  \
  } while(0)

void delay_in_mcs(
  uint32_t mcs
);

uint16_t swap_lo_hi( uint16_t value );

uint32_t swap_lo_hi_32( uint32_t value );

void tracelog_with_num( const char *str, uint32_t value );
void tracelog_with_str( const char *str, const char *arg );
void tracelog( char *str, uint16_t size );

#endif /* COMMON_H_ */
