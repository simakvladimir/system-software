/*
 * watchdog.c
 *
 *  Created on: 16.02.2014
 *      Author: BoBaHouD
 */

#include <am186es.h>
#include <watchdog.h>
#include <port.h>

void WDT_reset(void)
{
    outpw(WDTCON,0xAAAA);
    outpw(WDTCON,0x5555);
}

