/*
 * serial.h
 *
 *  Created on: 16.02.2014
 *      Author: BoBaHouD
 */
#ifndef __SERIAL_H__
#define __SERIAL_H__

#define SERIAL_INTERNAL_BUF_SIZE 20

#define SERIAL_PIO_RXD_PIN 23
#define SERIAL_PIO_TXD_PIN 22

/* status bits */
#define SERIAL_STATUS_THRE 0x0040

/* ctrl bits */
#define SERIAL_CTRL_8BITS_WITHOUT_PARITY 0x1
#define SERIAL_CTRL_TRANSIVER_ENABLE     0x40

#define SERIAL_BAUDRATE_9600   9600
#define SERIAL_BAUDRATE_57600  57600
#define SERIAL_BAUDRATE_115200 115200

typedef struct {
  uint32_t speed;
  uint16_t ctrl;
} serial_cfg_t;

typedef struct {
  uint32_t speed;
  uint16_t bdrt;
} serial_bdrt_t;

/**
 * Error types
 */
enum {
  SERIAL_SUCCESS,
  SERIAL_INVALID_SPEED
};

/**********************************************************
 *                    Serial device API                   *
 **********************************************************/
int serial_init(
  void
);

int serial_set_control_reg(
  uint16_t ctrl
);

int serial_setbdrt(
  uint32_t bdrt
);

void serial_putc(
  char c
);

void serial_puts(
  const char *str
);

void serial_print_num(
  uint32_t num,
  uint16_t base,
  uint8_t  sign
);

static uint16_t serial_search_bdrt_by_speed( uint32_t speed );

#endif /* __SERIAL_H__ */
