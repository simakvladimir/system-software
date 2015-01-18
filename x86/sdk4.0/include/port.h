/*
 * port.h
 *
 *  Created on: 16.02.2014
 *      Author: BoBaHouD
 */

#ifndef __PORT_H__
#define __PORT_H__

#include "types.h"

extern void outpw(uint16_t,uint16_t);
#pragma aux outpw = \
            "out DX,AX"        \
            parm   [dx] [AX];

extern unsigned short inpw(uint16_t);
#pragma aux inpw = \
            "in AX,DX"    \
            parm    [dx]  \
            value   [AX];

#endif /* __PORT_H__ */
