/*
 * led.c
 *
 *  Created on: 05.03.2014
 *      Author: BoBaHouD
 */
#include <led.h>
#include <pio.h>
#include <am186es.h>
#include <ethernet.h>
#include <port.h>

void led_red_init()
{
  pio_configure( PIO_OUT, LED_RED_PIO_PIN );
}

void led_yellow_init()
{
  pio_configure( PIO_OUT, LED_YELLOW_PIO_PIN );
}

void led_green_init()
{

}

void led_red_set_state( uint8_t state )
{
  if (state)
    outpw(P1DATA, (inpw(P1DATA) & ~0x1800) | 0x100);
  else
    outpw(P1DATA, (inpw(P1DATA) & ~0x1800) & ~0x100);
}

void led_yellow_set_state( uint8_t state )
{
  if (state)
    outpw(P1DATA, (inpw(P1DATA) & ~0x1800) & ~0x200);
  else
    outpw(P1DATA, (inpw(P1DATA) & ~0x1800) | 0x200);
}

void led_green_set_state( uint8_t state )
{
  outpw(ETHPPPTR,0x114); // Регистр SelfCTL CS8900A
  if(state)
    outpw(ETHPPDATAL, inpw(ETHPPDATAL) | 0x4000);
  else
    outpw(ETHPPDATAL, inpw(ETHPPDATAL) & ~0x4000);
}
