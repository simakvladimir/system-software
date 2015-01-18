/*
 * wdg.c
 *
 *  Created on: 15.10.2014
 *      Author: Simakov Vladimir
 */
#include <cpu/dsp2802x/DSP2802x_Device.h>
#include <periph/watchdog/wdg.h>

void watchdog_disable(void)
{
    EALLOW;
    SysCtrlRegs.WDCR= 0x0068;
    EDIS;
}

