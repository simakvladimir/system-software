/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2011
 *
 *    File name   : drv_icoll.c
 *    Description : IMX23 interrupt collector driver
 *
 *    History :
 *    1. Data        : May 2011
 *       Author      : Stoyan Choynev
 *       Description : Initial revision
 *    $Revision: 44589 $
 *
 **************************************************************************/
#define ICOLL_GLOBAL
#include "board.h"
#include "types.h"
#include "drv_icoll.h"
/** local definitions **/

typedef struct _VectorEntry_t
{
	IntrFunc_t 	Callback;
	Int32U 			PtiorityLevel;
} VectorEntry_t, * pVectorEntry_t;

/** external functions **/

/** external data **/

/** internal functions **/


/** public data **/

/** private data **/
static VectorEntry_t VectorTable[128];
static IntrFunc_t FastIsr;

/** public functions **/

/*************************************************************************
 * Function Name: IRQ_Handler
 * Parameters: none
 *
 * Return: none
 *
 * Description: IRQ interrupt handler
 *
 *************************************************************************/
void IRQ_Handler(void)
{
//	asm( "MOVS     sp, lr\n"
//			" ADD     sp,sp,#(4) ");
VectorEntry_t * entry = (VectorEntry_t *)HW_ICOLL_VECTOR;


  (*entry->Callback)();  	/*Call installed interrupt service routine*/

//  __enable_irq();
//    __disable_irq();

  HW_ICOLL_LEVELACK = entry->PtiorityLevel;	/*Priority Level Restore*/
//asm( "LDR     lr,[sp]\n"
//		"MOVS pc, lr");
}

/*************************************************************************
 * Function Name: FIQ_Handler
 * Parameters: none
 *
 * Return: none
 *
 * Description: FIQ interrupt handler
 *
 *************************************************************************/
void FIQ_Handler(void)
{
  (FastIsr)();    /*Call installed interrupt service routine*/
}

/*************************************************************************
 * Function Name: ICOLL_Init
 * Parameters: none
 *
 * Return: none
 *
 * Description: Init Interrupt controller
 *
 *************************************************************************/
void ICOLL_Init(void)
{
  __disable_interrupt();
  /*Soft reset*/
  HW_ICOLL_CTRL_bit.SFTRST = 1;
  volatile int i;
  for( i = 100; i; i--);
  HW_ICOLL_CTRL_bit.SFTRST = 0;
  /*Enable interrupt collector*/
  for( i = 100; i; i--);
  HW_ICOLL_CTRL_bit.CLKGATE = 0;
  /*Init Interrupt collector*/
  HW_ICOLL_CTRL_bit.ARM_RSE_MODE = 1;   	/* ARM-style, interrupt inservice is
                                          	 signalled by the read of the
                                          	 HW_ICOLL_VECTOR register to acquire
                                          	 the interrupt vector address */
  HW_ICOLL_CTRL_bit.NO_NESTING = 0;     	/* Enable nested interrupts */
  HW_ICOLL_CTRL_bit.VECTOR_PITCH = 2;  		/* two word pitch */

  HW_ICOLL_VBASE = (Int32U) VectorTable;  /* Set vector table base address */

  HW_ICOLL_CTRL_bit.FIQ_FINAL_ENABLE = 1; /* enable the final FIQ output */
  HW_ICOLL_CTRL_bit.IRQ_FINAL_ENABLE = 1; /* enable the final IRQ output */

  __enable_interrupt();
}

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
                    Int32U IntrSource, ICOLL_priority_t Priority)
{
Int32U * Interrupt = ((Int32U *)&HW_ICOLL_INTERRUPT0) + 4*IntrSource;
pVectorEntry_t entry;

  /*Sanitary check*/
  if(IntrSource > 127)
  {
    return;
  }

  /*If not fast interrupt*/
  if(!FastIntr)
  {
  	entry = VectorTable + IntrSource;
    /*updated interrupt register. This will disable the corresponding interrupt*/
    *Interrupt = Priority;
    /*Set ISR*/
    entry->Callback = pIntrSub;
    entry->PtiorityLevel = 1ul<<Priority;
  }
  else
  {
    /*updated interrupt register. This will disable the corresponding interrupt*/
    *Interrupt = 1<<4;
    /*Set ISR*/
    FastIsr = pIntrSub;
  }
}

/*************************************************************************
 * Function Name: ICOLL_EnableIntr
 * Parameters: Int32U IntrSource - Interrupt intedx (0 - 127)
 *
 * Return: none
 *
 * Description: Enable Interrupt
 *
 *************************************************************************/
void ICOLL_EnableIntr(Int32U IntrSource)
{
Int32U * Interrupt = ((Int32U *)&HW_ICOLL_INTERRUPT0_SET) + 4*IntrSource;

  /*Sanitary check*/
  if(IntrSource > 127)
  {
    return;
  }

  *Interrupt = 1<<2;
}

/*************************************************************************
 * Function Name: ICOLL_DisableIntr
 * Parameters: Int32U IntrSource - Interrupt intedx (0 - 127)
 *
 * Return: none
 *
 * Description: Disable Interrupt
 *
 *************************************************************************/
void ICOLL_DisableIntr(Int32U IntrSource)
{
Int32U * Interrupt = ((Int32U *)&HW_ICOLL_INTERRUPT0_CLR) + 4*IntrSource;

  /*Sanitary check*/
  if(IntrSource > 127)
  {
    return;
  }

  *Interrupt = 1<<2;
}

/** private functions **/
