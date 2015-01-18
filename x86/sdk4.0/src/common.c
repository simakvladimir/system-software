/*
 * common.c
 *
 *  Created on: 17.06.2013
 *      Author: BoBaHouD
 */
#include <common.h>
#include <watchdog.h>
#include <serial.h>
#include <am186es.h>

/*Jumpers*/
void jumpers_init( void ){
  pio_configure( PIO_IN_WITH_PULLUP, JUMPER_RS_PIO_PIN );
  pio_configure( PIO_IN_WITHOUT_PULLUP, JUMPER_MODEM_PIO_PIN );
  pio_configure( PIO_IN_WITH_PULLUP, JUMPER_MIF_PIO_PIN );
  pio_configure( PIO_IN_WITH_PULLUP, JUMPER_LOCK_PIO_PIN );
  pio_configure( PIO_IN_WITH_PULLUP, JUMPER_EXT1_PIO_PIN );
  pio_configure( PIO_IN_WITH_PULLUP, JUMPER_PROG_PIO_PIN );
  pio_configure( PIO_IN_WITHOUT_PULLUP, JUMPER_EXT2_PIO_PIN );
}

uint8_t jumper_get_state( uint16_t pin )
{
  uint16_t value;
  value = inpw(P0DATA);
  return ((value & 0xFF) & (1<<pin)) ? 0x0: 0x1;
}

void delay_in_mcs(
  uint32_t mcs
)
{
  while (mcs--){
    __asm{
      nop
    }; // 3 clocks
    WDT_reset(); // 9 clocks * 2 = 18 clocks
    // summa = 18 closks + 3 clocks = 21 clocks ~ 1 mcs
  }
}

void tracelog_with_num( const char *str, uint32_t value )
{
  serial_puts( str );
  serial_print_num( value, 10, 0 );
  serial_puts( "\r\n" );
}

void tracelog( char *str, uint16_t size ){
  while(size--){
    serial_print_num((uint32_t)*str, 16, 0);
    str++;
  }
  serial_puts( "\r\n" );
}

void tracelog_with_str( const char *str,  const char *arg )
{
  serial_puts( str );
  serial_puts( arg );
  serial_puts( "\r\n" );
}

uint16_t swap_lo_hi( uint16_t value )
{
  return ((value >> 8) | ((value & 0xFF) << 8));
}

uint32_t swap_lo_hi_32( uint32_t value )
{
  return (value<<24) | ((value<<8) & 0x00ff0000) |
      ((value>>8) & 0x0000ff00) | (value>>24);
}
