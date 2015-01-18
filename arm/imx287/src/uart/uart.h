#include <types.h>							/* types */
#include <freescale/iomcimx28.h>/* IMX's macros */

#define DUART_CONF_GPIO_MUX_VALUE 2
#define DUART_RX_PIN_ON_GPIO      16
#define DUART_TX_PIN_ON_GPIO      17
#define DUART_MIN_SPEED           9600
#define DUART_MAX_SPEED           115200
#define DUART_CLK                 24000000

#define DUART_FBRD_REG_DIGIT       6
#define DUART_FBRD_REG_MASK        0x3F

#define DUART_RECEIVE_FIFO_FULL    0x40
#define DUART_TRANSMIT_FIFO_FULL   0x20
#define DUART_RECEIVE_FIFO_EMPTY   0x10

/*
 *  Types
 */
typedef union {
	Int32U packed;
  struct{

		Int32U rimim      : 1;
		Int32U ctsmim     : 1;
		Int32U dcdmim     : 1;
		Int32U dsrmim     : 1;
		Int32U rxim       : 1;
		Int32U txim       : 1;
		Int32U rtim       : 1;
		Int32U feim       : 1;

		Int32U peim       : 1;
		Int32U beim       : 1;
		Int32U oeim       : 1;

		Int32U reserved   : 5;
		Int32U unavailable: 16;
  } fields;
} duart_interrupt_mask;

typedef union {
	Int32U packed;
  struct{

		Int32U brk     : 1;
		Int32U pen     : 1;
		Int32U eps     : 1;
		Int32U stp2    : 1;
		Int32U fen     : 1;
		Int32U wlen    : 2;
		Int32U sps     : 1;

		Int32U reserved   : 8;
		Int32U unavailable: 16;
  } fields;
} duart_control_high;

typedef union {
	Int32U packed;
  struct{

		Int32U uarten     : 1;
		Int32U siren      : 1;
		Int32U sirlp      : 1;
		Int32U reserved   : 4;
		Int32U lbe        : 1;
		Int32U txe        : 1;
		Int32U rxe        : 1;
		Int32U dtr        : 1;
		Int32U rts        : 1;
		Int32U out1       : 1;
		Int32U out2       : 1;
		Int32U rtsen      : 1;
		Int32U ctsen      : 1;
		Int32U unavailable: 16;
  } fields;
} duart_main_control;

/*
 *  Interface
 */
void duart_configure_gpio(
  void
);

void duart_set_IMSC_reg(
	duart_interrupt_mask value
);

ret_code duart_set_speed(
		Int32U value
);

void duart_set_high_control_reg(
		duart_control_high value
);

void duart_set_main_control_reg(
	duart_main_control	 value
);

void duart_print(
	char	*data
);

void duart_send(
	char	*data, Int32U length
);

void duart_set_interrupt( void );
void duart_enable_interrupt( void );
