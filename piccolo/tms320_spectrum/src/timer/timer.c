/*
 * timer.c
 *
 *  Created on: 14.10.2014
 *      Author: BoBaHouD
 */
#include <DSP2802x/DSP2802x_Device.h>
#include <DSP2802x/DSP2802x_Examples.h>
#include <timer/timer.h>

struct CPUTIMER_VARS CpuTimer0;

//  CpuTimer2 is used by DSP BIOS & other RTOS. Comment out if using DSP BIOS or other RTOS.
struct CPUTIMER_VARS CpuTimer1;
struct CPUTIMER_VARS CpuTimer2;


void timer_init(
   void
)
{
    CpuTimer0.RegsAddr = &CpuTimer0Regs;
    // Initialize timer period to maximum:
    CpuTimer0Regs.PRD.all  = 0xFFFFFFFF;
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT):
    CpuTimer0Regs.TPR.all  = 0;
    CpuTimer0Regs.TPRH.all = 0;
    // Make sure timer is stopped:
    CpuTimer0Regs.TCR.bit.TSS = 1;
    // Reload all counter register with period value:
    CpuTimer0Regs.TCR.bit.TRB = 1;
    // Reset interrupt counters:
    CpuTimer0.InterruptCount = 0;

// CpuTimer 1 and CpuTimer2 are reserved for DSP BIOS & other RTOS
// Do not use these two timers if you ever plan on integrating
// DSP-BIOS or another realtime OS.
//
// Initialize address pointers to respective timer registers:
    CpuTimer1.RegsAddr = &CpuTimer1Regs;
    CpuTimer2.RegsAddr = &CpuTimer2Regs;
    // Initialize timer period to maximum:
    CpuTimer1Regs.PRD.all  = 0xFFFFFFFF;
    CpuTimer2Regs.PRD.all  = 0xFFFFFFFF;
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT):
    CpuTimer1Regs.TPR.all  = 0;
    CpuTimer1Regs.TPRH.all = 0;
    CpuTimer2Regs.TPR.all  = 0;
    CpuTimer2Regs.TPRH.all = 0;
    // Make sure timers are stopped:
    CpuTimer1Regs.TCR.bit.TSS = 1;
    CpuTimer2Regs.TCR.bit.TSS = 1;
    // Reload all counter register with period value:
    CpuTimer1Regs.TCR.bit.TRB = 1;
    CpuTimer2Regs.TCR.bit.TRB = 1;
    // Reset interrupt counters:
    CpuTimer1.InterruptCount = 0;
    CpuTimer2.InterruptCount = 0;
}


void timer_configure(
  struct CPUTIMER_VARS *timer,
  Uint32 periodInHz
)
{
    Uint32  temp;

    // Initialize timer period:
    timer->CPUFreqInHz = NANOSEC_IN_SEC / CPU_RATE;
    timer->PeriodInHz  = periodInHz;
    temp = (long) (timer->CPUFreqInHz / periodInHz);
    timer->RegsAddr->PRD.all = temp;

    // Set pre-scale counter to divide by 1 (SYSCLKOUT):
    timer->RegsAddr->TPR.all  = 0;
    timer->RegsAddr->TPRH.all = 0;

    // Initialize timer control register:
    timer->RegsAddr->TCR.bit.TSS  = 1;      // 1 = Stop timer, 0 = Start/Restart Timer
    timer->RegsAddr->TCR.bit.TRB  = 1;      // 1 = reload timer
    timer->RegsAddr->TCR.bit.SOFT = 0;
    timer->RegsAddr->TCR.bit.FREE = 0;     // Timer Free Run Disabled
    timer->RegsAddr->TCR.bit.TIE  = 1;      // 0 = Disable/ 1 = Enable Timer Interrupt

    // Reset interrupt counter:
    timer->InterruptCount = 0;
}
