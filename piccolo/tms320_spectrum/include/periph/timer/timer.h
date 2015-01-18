/*
 * timer.h
 *
 *  Created on: 14.10.2014
 *      Author: Simakov Vladimir
 */


#ifndef TIMER_H_
#define TIMER_H_

#define NANOSEC_IN_SEC 1000000000

void timers_init(
   void
);

void timer_configure(
  struct CPUTIMER_VARS *timer,
  Uint32 periodInHz
);

void timer_register_isr(
  int16 number,
  PINT   isr
);

void timer_start(
   struct CPUTIMER_VARS *timer
);

#endif /* TIMER_H_ */
