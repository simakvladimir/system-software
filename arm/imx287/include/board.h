/***************************************************************************
 **
 **    This file defines the board specific definition
 **
 **    Used with ARM IAR C/C++ Compiler and Assembler.
 **
 **    (c) Copyright IAR Systems 2011
 **
 **    $Revision: 44589 $
 **
 ***************************************************************************/
#include <types.h>
#include <freescale/iomcimx28.h>
#ifndef __BOARD_H
#define __BOARD_H

/* OSC [MHz] */
#define FOSC1               24MHZ
#define FOSC2               327680HZ

/* Core clk [MHz]  454 MHz */
#define MPLL_CLK            454MHZ
#define FCLK                266MHZ
#define HCLK                133MHZ
#define SPLL_CLK            300MHZ

/* Tick per second*/
#define TICK_PER_SEC        10
#define TIMER_CLOCK_FREQ    24000000

#define LED2_ON()           (HW_PINCTRL_DOUT3_SET = 1ul << 5)
#define LED2_OFF()          (HW_PINCTRL_DOUT3_CLR = 1ul << 5)
#define LED2_TOGGLE()       (HW_PINCTRL_DOUT3_TOG = 1ul << 5)

#define LED3_ON()           (HW_PINCTRL_DOUT3_SET = 1ul << 4)
#define LED3_OFF()          (HW_PINCTRL_DOUT3_CLR = 1ul << 4)
#define LED3_TOGGLE()       (HW_PINCTRL_DOUT3_TOG = 1ul << 4)

#endif /* __BOARD_H */
