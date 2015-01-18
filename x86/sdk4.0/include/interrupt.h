/*
 * interrupt.h
 *
 *  Created on: 17.06.2013
 *      Author: ALM User
 */

#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#define INTERRUPT_DMA0_TYPE 0x0A
#define INTERRUPT_INT0_TYPE 0xC

#define INTERRUPT_VECTOR_TABLE_ENTRY 0x00000000l

enum {
  INTERRUPT_SUCCESS,
  INTERRUPT_ERROR
};

uint8_t interrupt_vector_catch(
  uint8_t intnum,
  void (__interrupt __far *handler)()
);

void interrupt_enable(
  uint16_t mask
);

#endif /* INTERRUPT_H_ */
