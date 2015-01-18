/*
 * timer.h
 *
 *  Created on: 20.02.2014
 *      Author: BoBaHouD
 */

#ifndef TIMER_H_
#define TIMER_H_

#define TIMER_INTR      0x01

static void interrupt far timer_handle(void);

void timer_init(void);

uint32_t timer_get_tick(
  void
);

#endif /* TIMER_H_ */
