#include <board.h>
#include <types.h>
#include "MMU/arm926ej_cp15_drv.h"
#include "MMU/ttbl.h"
#include "interrupt/drv_icoll.h"
#include <stdbool.h>
#include "freescale/iomcimx28.h"

#include "uart/uart.h"
#include "timer/timer.h"
#include "bufop/buffer.h"

#define MMU_ENA

//DUART's buffer
static buffer duart_buffer;
buffer * pointer_duart_buffer = &duart_buffer;

extern void timer_handler_green( void );
extern void timer_handler_red( void );
extern void timer_handler_blue( void );

#define _READ( buf, ret )\
	do {\
		ret = buffer_read( &buf );\
	}\
	while ( !ret )\


int main(void)
{
/*************************************************************************
 *                          MMU configure
 ************************************************************************/
#ifdef MMU_ENA
  // Init MMU
  CP15_Mmu(false);            // Disable MMU
  // Privileged permissions  User permissions AP
  // Read-only               Read-only        0
  CP15_SysProt(FALSE);
  CP15_RomProt(TRUE);
  CP15_InitMmuTtb(TtSB,TtTB); // Build L1 and L2 Translation tables
  CP15_SetTtb(L1Table);       // Set base address of the L1 Translation table
  CP15_SetDomain( (DomainManager << 2*1) | (DomainClient << 0)); // Set domains
  CP15_Mmu(TRUE);             // Enable MMU
  CP15_Cache(TRUE);           // Enable ICache,DCache
#endif
// Interrupt controller init
  ICOLL_Init();

/*************************************************************************
 *                          UART configure
 ************************************************************************/
  duart_interrupt_mask mask;
  mask.packed = 0;
  mask.fields.rxim = 1;

  duart_control_high high_control;
  high_control.packed = 0;
  high_control.fields.wlen = 3;//8bit
//  high_control.fields.fen  = 1;//FIFO enable

  duart_main_control main_control;
  main_control.packed = 0;
  main_control.fields.uarten = 1;
  main_control.fields.txe    = 1;
  main_control.fields.rxe    = 1;

  duart_configure_gpio();
  duart_set_speed( 115200 );
  duart_set_IMSC_reg( mask );
  duart_set_high_control_reg( high_control );
  duart_set_main_control_reg( main_control );

  /*
   * Interrupt setup
   */
  duart_set_interrupt();
  duart_enable_interrupt();
  CP15_exception( false );


/*************************************************************************
 *                          TIMERs configure
 ************************************************************************/
  timer_unit_reset();
  timer_set_reload_bit(TIMER_GREEN);
  timer_interrupt_enable(TIMER_GREEN);

  timer_set_reload_bit(TIMER_RED);
  timer_interrupt_enable(TIMER_RED);

  timer_set_reload_bit(TIMER_BLUE);
  timer_interrupt_enable(TIMER_BLUE);

  /* Interrupt setup */
  timer_set_interrupt( timer_handler_green, INT_TIMER_GREEN );
  timer_enable_interrupt( INT_TIMER_GREEN );

  timer_set_interrupt( timer_handler_red, INT_TIMER_RED );
  timer_enable_interrupt( INT_TIMER_RED );

  timer_set_interrupt( timer_handler_blue, INT_TIMER_BLUE );
  timer_enable_interrupt( INT_TIMER_BLUE );

  timer_start(TIMER_GREEN);//Always tick
  timer_set_carrier_period_in_mcs( TIMER_GREEN, 10000 );

  timer_start(TIMER_RED);//Always tick
  timer_set_carrier_period_in_mcs( TIMER_RED,10000 );

  timer_start(TIMER_BLUE);//Always tick
  timer_set_carrier_period_in_mcs( TIMER_BLUE,10000 );

  __enable_interrupt();




  /*
   *  Prepare array for data
   */
  buffer_construct(&duart_buffer);
  /*
   * GPIO 3_3 GREEN
   */
  HW_PINCTRL_MUXSEL6_bit.BANK3_PIN03    = 3;
  HW_PINCTRL_DRIVE12_bit.BANK3_PIN03_V  = 1; // 3.3V
  HW_PINCTRL_DRIVE12_bit.BANK3_PIN03_MA = 0; // 4mA
  /* Enable output */
  HW_PINCTRL_DOE3_SET = 1 << 3;
  HW_PINCTRL_DOUT3.DOUT03 = 1;
  HW_PINCTRL_DOUT3.DOUT03 = 1;
  HW_PINCTRL_DOUT3.DOUT03 = 0;
  HW_PINCTRL_DOUT3.DOUT03 = 1;

  /*
   *  GPIO 3_5 RED
   */
  HW_PINCTRL_MUXSEL6_bit.BANK3_PIN05    = 3;
  HW_PINCTRL_DRIVE12_bit.BANK3_PIN05_V  = 1; // 3.3V
  HW_PINCTRL_DRIVE12_bit.BANK3_PIN05_MA = 0; // 4mA
  HW_PINCTRL_DOE3_SET = 1 << 5;
  HW_PINCTRL_DOUT3.DOUT05 = 1;
  HW_PINCTRL_DOUT3.DOUT05 = 1;
  HW_PINCTRL_DOUT3.DOUT05 = 0;
  HW_PINCTRL_DOUT3.DOUT05 = 1;
  /*
   *  GPIO 2_14 BLUE
   */
  HW_PINCTRL_MUXSEL4_bit.BANK2_PIN12   = 3;
  HW_PINCTRL_DRIVE9_bit.BANK2_PIN12_V  = 1; // 3.3V
  HW_PINCTRL_DRIVE9_bit.BANK2_PIN12_MA = 0; // 4mA
  HW_PINCTRL_DOE2_SET = 1 << 12;
  HW_PINCTRL_DOUT2.DOUT12 = 1;
  HW_PINCTRL_DOUT2.DOUT12 = 1;
  HW_PINCTRL_DOUT2.DOUT12 = 0;
  HW_PINCTRL_DOUT3.DOUT12 = 1;



  HW_PINCTRL_MUXSEL4_bit.BANK2_PIN14   = 3;
  HW_PINCTRL_DRIVE9_bit.BANK2_PIN14_V  = 1; // 3.3V
  HW_PINCTRL_DRIVE9_bit.BANK2_PIN14_MA = 0; // 4mA
  HW_PINCTRL_DOE2_SET = 1 << 14;
  HW_PINCTRL_DOUT2.DOUT14 = 1;
  HW_PINCTRL_DOUT2.DOUT14 = 1;
  HW_PINCTRL_DOUT2.DOUT14 = 0;
  HW_PINCTRL_DOUT3.DOUT14 = 1;

  Int8U *byte, *current, *new;
  Int8U param[3];
  int e,r = 0;
  int i;
  Int32U speed = 9600;
  timer_set_blue_color_value(0);
  timer_set_green_color_value(0);
  timer_set_red_color_value(0);



//  timer_set_green_color_value( 65535 );
//  duart_buffer.first = duart_buffer.start + 2;
//  duart_buffer.last = duart_buffer.end;
  while(1)
  {
    duart_print( "HEllm\n\r" );
//  	timer_set_green_color_value(1);
//  	timer_set_red_color_value(1);
//  	timer_set_blue_color_value(1);

  	_READ( duart_buffer, byte );
//  	byte =  buffer_read(&duart_buffer);
//  	_READ( duart_buffer, byte );

  	  if (*byte & 0x80 ){
    		switch(*byte & 0x1F ){
    			case 0 : _READ( duart_buffer, current );
    							speed = 0;
    							while( (*current)-- ){
										_READ( duart_buffer, new );
										speed |= (*new) << (8*(*current));
    							}

    							duart_set_speed(speed);
    							break;

    			case 1 : _READ( duart_buffer, current );

    							while( (*current)-- ){

    								_READ( duart_buffer, new );
										param[*current] = *new;
    							}

    							timer_set_blue_color_value( param[0] );
    							timer_set_green_color_value( param[1] );
    							timer_set_red_color_value( param[2] );



    			default: break;

    		}
    	}

    }
}
