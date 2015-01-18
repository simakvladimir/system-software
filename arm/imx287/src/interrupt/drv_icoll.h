/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2011
 *
 *    File name   : drv_icoll.h
 *    Description : IMX23 interrupt collector driver
 *
 *    History :
 *    1. Data        : May 2011
 *       Author      : Stoyan Choynev
 *       Description : Initial revision
 *    $Revision: 44589 $
 *
 **************************************************************************/
#ifndef __DRV_ICOLL_H
#define __DRV_ICOLL_H

/*
 *  DEfine INTERRUPT
 */
#define __enable_irq()\
	asm(\
			"MRS R0, CPSR\n"\
			"BIC R0,R0,#128\n"\
			"MSR CPSR_c, R0\n"\
	);\

#define __disable_irq()\
	asm(\
			"MRS R0, CPSR\n"\
			"ORR R0,R0,#128\n"\
			"MSR CPSR_c, R0\n"\
	);\

#define __enable_interrupt()\
	asm(\
			"MRS R0, CPSR\n"\
			"BIC R0,R0,#192\n"\
			"MSR CPSR, R0\n"\
	);\

#define __disable_interrupt()
	asm(\
			"MRS R0, CPSR\n"\
			"ORR R0,R0,#192\n"\
			"MSR CPSR, R0\n"\
	);\

typedef void(*IntrFunc_t)(void);

typedef enum
{
  LEVEL0 = 0,
  LEVEL1,
  LEVEL2,
  LEVEL3
} ICOLL_priority_t;

/*************************************************************************
 * Function Name: ICOLL_Init
 * Parameters: none
 *
 * Return: none
 *
 * Description: Init Interrupt controller
 *
 *************************************************************************/
void ICOLL_Init(void);

/*************************************************************************
 * Function Name: ICOLL_SetupIntr
 * Parameters: IntrFunc_t pIntrSub - pointer to the interrupt service routine
 *             Boolean FastIntr    - 0 -IRQ : 1- FIQ
 *             Int32U IntrSouce     - Interrupt intedx (0 - 127)
 *             ICOLL_priority_t Priority  - priority level 
 *
 * Return: none
 *
 * Description: Setup the interrupt priority, type and service routine.
 *              The interrupt will be disabled.
 *************************************************************************/
void ICOLL_SetupIntr(IntrFunc_t pIntrSub, Boolean FastIntr,
                    Int32U IntrSource, ICOLL_priority_t Priority);

/*************************************************************************
 * Function Name: ICOLL_EnableIntr
 * Parameters: Int32U IntrSource - Interrupt intedx (0 - 127)
 *
 * Return: none
 *
 * Description: Enable Interrupt 
 *
 *************************************************************************/
void ICOLL_EnableIntr(Int32U IntrSource);

/*************************************************************************
 * Function Name: ICOLL_DisableIntr
 * Parameters: Int32U IntrSource - Interrupt intedx (0 - 127)
 *
 * Return: none
 *
 * Description: Disable Interrupt 
 *
 *************************************************************************/
void ICOLL_DisableIntr(Int32U IntrSource);

#endif // __DRV_ICOLL_H
