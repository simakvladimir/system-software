/*
 * timer.c
 *
 *  Created on: 20.02.2014
 *      Author: BoBaHouD
 */

#include <am186es.h>
#include <port.h>
#include <interrupt.h>
#include <common.h>
#include <timer.h>

static uint32_t timer_count = 0;

static void interrupt far timer_handle(void)
{
  timer_count++;
//  tracelog_with_str( "[timer] System tick", "" );
  outpw(EOI,0x08);
}

void timer_init(void)
{
  __asm {cli};
  outpw(SYSCON,inpw(SYSCON)&~0x1000);     // не PWD

  // настройка TIMER0 - используется для отсчета времени
  outpw(T0CNT,0);         // очистка TIMER0
  outpw(T0CMPA,50000);    // максимальное значение таймера,
                          //вычисляется по ф-ле period = t*10^7/2
                          //(время) = value*2*10^(-7)
                          // на частоте 20MHz период будет 10мс
  outpw(T0CON,0x2001);    // 0010000000000001
                          // continous, generate intr
  interrupt_vector_catch(0x08,(void (__interrupt __far *)(void))&timer_handle);
  interrupt_enable(TIMER_INTR);                // снятие маски
  outpw(T0CON,inpw(T0CON)|0xC000);        // запуск таймера

  __asm {sti};
}

uint32_t timer_get_tick(
  void
)
{
  return timer_count;
}
