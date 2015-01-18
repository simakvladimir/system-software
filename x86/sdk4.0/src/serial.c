/*
 * serial.c
 *
 *  Created on: 16.02.2014
 *      Author: BoBaHouD
 */
#include <types.h>
#include <common.h>
#include <port.h>
#include <am186es.h>
#include <pio.h>
#include <watchdog.h>
#include <serial.h>


/**
 * Serial configuration table
 */
static serial_cfg_t serial_cfg = {
  SERIAL_BAUDRATE_57600,
  SERIAL_CTRL_TRANSIVER_ENABLE | SERIAL_CTRL_8BITS_WITHOUT_PARITY
};

static serial_bdrt_t serial_bdrt_table[] = {
    {SERIAL_BAUDRATE_9600  , 130 },
    {SERIAL_BAUDRATE_57600 ,  22 },
    {SERIAL_BAUDRATE_115200,  10 },
    {0,0}
};

/**
 * Initialization serial
 */
int serial_init(
  void
)
{
  // configure uart pins
  pio_configure( PIO_NORMAL, SERIAL_PIO_TXD_PIN );
  pio_configure( PIO_NORMAL, SERIAL_PIO_RXD_PIN );

  serial_set_control_reg( serial_cfg.ctrl );
  serial_setbdrt( serial_cfg.speed );

  return SERIAL_SUCCESS;
}

int serial_set_control_reg(
  uint16_t ctrl
)
{
  outpw( SP0CT, ctrl );
  return SERIAL_SUCCESS;
}

/**
 * Configure serial speed
 */
int serial_setbdrt(
  uint32_t speed
)
{
  uint16_t bauddiv;

  if ( speed < SERIAL_BAUDRATE_9600 || speed > SERIAL_BAUDRATE_115200 )
    return SERIAL_INVALID_SPEED;

  bauddiv = serial_search_bdrt_by_speed( speed );

  outpw( SP0BAUD, bauddiv );

  tracelog_with_num( "[uart] UART speed = ", speed );

  return SERIAL_SUCCESS;
}

/**
 * Print single symbol
 */
void serial_putc(
  char c
)
{
  while( !(inpw(SP0STS) & SERIAL_STATUS_THRE) )
  {
    WDT_reset();
  }
  outpw( SP0TD, (uint16_t)c );
}

/**
 * Print const string
 */
void serial_puts(
  const char *str
)
{
  while(*str)
  {
     serial_putc( *str );
     str++;
  }
}

void serial_print_num(
  uint32_t num,
  uint16_t base,
  uint8_t  sign
)
{
  uint32_t unsigned_num;
  uint32_t n;
  uint32_t count;
  char toPrint[20];

  if ( sign ) {
    serial_putc('-');
    unsigned_num = (uint32_t) -num;
  } else {
    unsigned_num = (uint32_t) num;
  }

  count = 0;
  while ((n = unsigned_num / base) > 0) {
    toPrint[count++] = (char) (unsigned_num - (n * base));
    unsigned_num = n;
  }
  toPrint[count++] = (char) unsigned_num;

  for (n = 0; n < count; n++) {
      serial_putc("0123456789ABCDEF"[(int)(toPrint[count-(n+1)])]);
  }
}

static uint16_t serial_search_bdrt_by_speed( uint32_t speed )
{
  uint16_t bdrt;
  serial_bdrt_t *p_table = serial_bdrt_table;

  // am186es formula: bauddiv = cpu_freq / ( 16 * baudrate )
  bdrt = ( CPU_FREQ >> 4 ) / speed;

  while(p_table->speed)
  {
      if (p_table->speed == speed)
        return p_table->bdrt;
      p_table++;
  }
  return bdrt;
}
