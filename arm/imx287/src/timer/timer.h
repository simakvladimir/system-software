#include <types.h>							/* types */
#include <freescale/iomcimx28.h>/* IMX's macros */

#define TIMER_GREEN 0
#define TIMER_RED   1
#define TIMER_BLUE  2
#define TIMER_1 1
#define TIMER_2 2
#define TIMER_3 3

#define INT_TIMER_GREEN INT_TIMER0
#define INT_TIMER_RED   INT_TIMER1
#define INT_TIMER_BLUE  INT_TIMER2

#define TIMER_XTAL_BASE_FREQ_IN_MHZ 24

#define TIMER_BAM_ARRAY_SIZE 8

#define TIMER_SOFT_RESET_BIT 0x80000000
#define TIMER_CLK_GATE_BIT   0x40000000
#define TIMER_STATUS_IRQ_BIT 0x8000

typedef union {
	Int32U packed;
  struct{
		Int32U select   : 4;
		Int32U prescale : 2;
		Int32U reload   : 1;
		Int32U update   : 1;
		Int32U polarity : 1;
		Int32U reserved1: 2;

		Int32U match_mode: 1;
		Int32U reserved2 : 2;
		Int32U irq_en    : 1;

		Int32U irq   		 : 1;
		Int32U reserved3 : 16;
  } fields;
} timer_control_and_status;

/*
 *
 */
#define timer_set_reload_bit(number)\
		reload_bit(number)\


#define reload_bit( number )\
	(HW_TIMROT_TIMCTRL##number).RELOAD = 1; \

/*
 *
 */
#define timer_interrupt_enable( number )\
		interrupt_enable(number)\

#define interrupt_enable(number)\
	(HW_TIMROT_TIMCTRL##number).IRQ_EN = 1;

/*
 *
 */
#define timer_start( number )\
		start(number)\

#define start(number)\
	(HW_TIMROT_TIMCTRL##number).SELECT = 0xF;\


#define timer_set_period_in_mcs( number, value )\
		set_period_in_mcs(number,value)\


#define 	set_period_in_mcs( number,value )\
	HW_TIMROT_FIXED_COUNT##number = value * TIMER_XTAL_BASE_FREQ_IN_MHZ;\


#define timer_set_carrier_period_in_mcs( number, value )\
		set_carrier_period_in_mcs(number, value)\

#define set_carrier_period_in_mcs(number, value)\
	timer_calculate_bam_periods( value );\
	HW_TIMROT_FIXED_COUNT##number = value * TIMER_XTAL_BASE_FREQ_IN_MHZ;\



/* functions */
void timer_unit_reset(void);
//void timer_set_fixed_count( Int32U value );
void timer_set_interrupt(	void *handler, int vector );
void timer_enable_interrupt( int vector );
