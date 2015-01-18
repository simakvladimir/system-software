#include "timer.h"
#include "../interrupt/drv_icoll.h"
#include "../gpio/gpio.h"
#include <stdbool.h>

#define HIGH_BIT_MASK (1 << ( TIMER_BAM_ARRAY_SIZE - 1))


static Int32U bam_array[TIMER_BAM_ARRAY_SIZE];

static Int32U cnt_exec_green_led = 0;
static Int32U cnt_exec_red_led = 0;
static Int32U cnt_exec_blue_led = 0;

Int32U timer_green_color;
Int32U timer_red_color;
Int32U timer_blue_color;
/*
 * IRQ handler
 */
void timer_handler_green(
  void
)
{

	HW_TIMROT_TIMCTRL0_CLR_REG &= TIMER_STATUS_IRQ_BIT; //Clear irq bit TODO clear very strangely

	GREEN_GRIO_PIN =  timer_green_color & ( HIGH_BIT_MASK >> ( cnt_exec_green_led % TIMER_BAM_ARRAY_SIZE ) ) ? 1: 0;

	timer_set_period_in_mcs( TIMER_GREEN,bam_array[ cnt_exec_green_led % TIMER_BAM_ARRAY_SIZE ] );
	cnt_exec_green_led++;
}


void timer_handler_red(
  void
)
{
//	RED_GRIO_PIN = 1;
	HW_TIMROT_TIMCTRL1_CLR_REG &= TIMER_STATUS_IRQ_BIT; //Clear irq bit TODO clear very strangely

	RED_GRIO_PIN =  timer_red_color & ( HIGH_BIT_MASK >> ( cnt_exec_red_led % TIMER_BAM_ARRAY_SIZE ) ) ? 1: 0;

	timer_set_period_in_mcs( TIMER_RED,bam_array[ cnt_exec_red_led % TIMER_BAM_ARRAY_SIZE ] );
	cnt_exec_red_led++;
}

void timer_handler_blue(
  void
)
{
//	RED_GRIO_PIN = 1;
	HW_TIMROT_TIMCTRL2_CLR_REG &= TIMER_STATUS_IRQ_BIT; //Clear irq bit TODO clear very strangely

	BLUE_GRIO_PIN =  timer_blue_color & ( HIGH_BIT_MASK >> ( cnt_exec_blue_led % TIMER_BAM_ARRAY_SIZE ) ) ? 1: 0;

	timer_set_period_in_mcs( TIMER_BLUE,bam_array[ cnt_exec_blue_led % TIMER_BAM_ARRAY_SIZE ] );
	cnt_exec_blue_led++;
}



/*
 * Binary angle modulation
 */
void timer_calculate_bam_periods(
		Int32U carrier_period
)
{
	int index;
	for( index = 0; index < TIMER_BAM_ARRAY_SIZE; index++ ){
		bam_array[index] = (carrier_period / ( 1 << (index + 1) ))?(carrier_period / ( 1 << (index + 1) )):1;
	}
}

void timer_set_green_color_value(
		Int32U color
)
{
	timer_green_color = color;
}

void timer_set_red_color_value(
		Int32U color
)
{
	timer_red_color = color;
}

void timer_set_blue_color_value(
		Int32U color
)
{
	timer_blue_color = color;
}



void timer_unit_reset(
	void
)
{
  volatile int i;
  HW_TIMROT_ROTCTRL |= TIMER_SOFT_RESET_BIT;
  for( i = 100; i; i--);
  	HW_TIMROT_ROTCTRL &= ~TIMER_SOFT_RESET_BIT;
  /* Enable clock*/
  for( i = 100; i; i--);
  	HW_TIMROT_ROTCTRL &= ~TIMER_CLK_GATE_BIT; //RESET cjk_gate very important, else timer not start

	HW_TIMROT_TIMCTRL0_REG = 0;
}

/*****************************************************************************
 *                             CONFIGURE functions
 *****************************************************************************/
//void timer_set_period_in_mcs( Int32U value )
//{
//	HW_TIMROT_FIXED_COUNT0 = value * TIMER_XTAL_BASE_FREQ_IN_MHZ;
//}
//
//void timer_set_carrier_period_in_mcs( Int32U value )
//{
//	timer_calculate_bam_periods( value );
//	HW_TIMROT_FIXED_COUNT0 = value * TIMER_XTAL_BASE_FREQ_IN_MHZ;
//}


/***************************************************************************
 *                             INTERRUPT functions
 *****************************************************************************/
void timer_set_interrupt(
	void *handler, int vector
)
{
	ICOLL_SetupIntr(handler, 0, vector, LEVEL3);
}

void timer_enable_interrupt(
  int vector
)
{
	ICOLL_EnableIntr(vector);
}

