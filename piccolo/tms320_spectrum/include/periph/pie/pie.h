/*
 * pie.h
 *
 *  Created on: 14.10.2014
 *      Author: Simakov Vladimir
 */

#ifndef PIE_H_
#define PIE_H_

#define PIE_INT_TABLE_SIZE 128

void pie_init(
  void
);

void pie_allow_interrupt(
  Uint16 major,
  Uint16 minor
);

#endif /* PIE_H_ */
