/*
 * interrupt.c
 *
 *  Created on: 17.06.2013
 *      Author: Simakov V.V.
 */

#include <am186es.h>
#include <types.h>
#include <interrupt.h>
#include <port.h>

uint8_t interrupt_vector_catch(
  uint8_t intnum,
  void (__interrupt __far *handler)()
)
{
  uint32_t far *isr_table = (uint32_t far *)INTERRUPT_VECTOR_TABLE_ENTRY;

  if( (intnum==0x9) || ((intnum>0x15) && (intnum<0x1F)) )
    return INTERRUPT_ERROR;

  if( !handler )
    return INTERRUPT_ERROR;

  isr_table[intnum] = (uint32_t)handler;

  return INTERRUPT_SUCCESS;
}

void interrupt_enable(
  uint16_t mask
)
{
  outpw(IMASK,inpw(IMASK)&~mask);
}
