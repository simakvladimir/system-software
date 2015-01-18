#include "uart.h"				/* uart const */
#include "../bufop/buffer.h"
#include "../interrupt/drv_icoll.h"

extern buffer* pointer_duart_buffer;

/*UART's routine*/
static void duart_handler(void)
{
  Int8U byte;
	while ( !( HW_UARTDBG_FR & DUART_RECEIVE_FIFO_EMPTY ) ) {

   byte = HW_UARTDBG_DR;

   buffer_write( pointer_duart_buffer, byte);

	}
}

/******************************************************************
 *                           SETTINGS
 *****************************************************************/

/*
 *  Configure GPIO's MUX for DUART
 */
void duart_configure_gpio(
  void
)
{
	/* Configure GPIO's MUX for RX*/
  HW_PINCTRL_MUXSEL7_bit.BANK3_PIN16 = DUART_CONF_GPIO_MUX_VALUE;
  /* RX DURT enable on input */
  HW_PINCTRL_DOE3_SET			    			 = 0 << DUART_RX_PIN_ON_GPIO;

	/* Configure GPIO's MUX for TX*/
  HW_PINCTRL_MUXSEL7_bit.BANK3_PIN17 = DUART_CONF_GPIO_MUX_VALUE;
  /* RX DURT enable on output */
  HW_PINCTRL_DOE3_SET			    			 = 0 << DUART_TX_PIN_ON_GPIO;
}

/*
 *  UART set Interrupt Mask Set/Clear Register
 */
void duart_set_IMSC_reg(
	duart_interrupt_mask value
)
{
	HW_UARTDBG_IMSC = value.packed;
}

/*
 *  UART set Speed
 */
ret_code duart_set_speed(
		Int32U value
)
{
	if ( value >= DUART_MIN_SPEED && value <= DUART_MAX_SPEED ) {

		unsigned divisor;

		divisor = ( DUART_CLK * 4 )/ value;

		HW_UARTDBG_IBRD = divisor >> DUART_FBRD_REG_DIGIT;
		HW_UARTDBG_FBRD = divisor  & DUART_FBRD_REG_MASK;

		return SUCCESS;
	}
	return ERROR;
}

/*
 *  UART set Control REG High Byte
 */
void duart_set_high_control_reg(
		duart_control_high value
)
{
	HW_UARTDBG_H = value.packed;
}

/*
 *  UART set Control REG
 */
void duart_set_main_control_reg(
	duart_main_control	 value
)
{
	HW_UARTDBG_CR = value.packed;
}

/******************************************************************
 *                           SEND
 *****************************************************************/

/*
 *  UART print text
 */
void duart_print(
	char	*data
)
{
	while( *data != '\0' ){
		while( HW_UARTDBG_FR & DUART_TRANSMIT_FIFO_FULL )
			asm( "nop:nop:nop:nop:nop:nop:nop:nop:nop:nop:nop:nop" );
		HW_UARTDBG_DR = *(data++);
	}

}

/*
 *  UART print text
 */
void duart_send(
	char	*data, Int32U length
)
{
	while( length-- ){
		while( HW_UARTDBG_FR & 0x20 )
			asm( "nop" );
		HW_UARTDBG_DR = *data;
	}
}

/***************************************************************************
 *                             INTERRUPT functions
 *****************************************************************************/
void duart_set_interrupt(
	void
)
{
	ICOLL_SetupIntr(duart_handler, 0, INT_DUART, LEVEL3);
}

void duart_enable_interrupt(
  void
)
{
	ICOLL_EnableIntr(INT_DUART);
}
