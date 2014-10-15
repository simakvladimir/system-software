/*
 * wdg.c
 *
 *  Created on: 15.10.2014
 *      Author: BoBaHouD
 */
#include <DSP2802x/DSP2802x_Device.h>
#include <sysctrl/wdg.h>

void watchdog_disable(void)
{
    EALLOW;
    SysCtrlRegs.WDCR= 0x0068;
    EDIS;
}

