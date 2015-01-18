/*
 * ethernet.h
 *
 *  Created on: 17.06.2013
 *      Author: ALM User
 */

#ifndef ETHERNET_H_
#define ETHERNET_H_

#include <types.h>

#define ETH_MAX_TRIES 20

#define ETH_RX_BUF      0x20000000l

#define ETHBASE         0x0000

#define ETH_VENDOR      0x630E

#define ETH_PKTPG_IND_ADDR     0x0158

#define ETH_MAX_RESET_COUNTER 1000

#define ETHDATAL        (ETHBASE+0x0000)
#define ETHDATAH        (ETHBASE+0x0002)
#define ETHTXCMD        (ETHBASE+0x0004)
#define ETHTXLEN        (ETHBASE+0x0006)
#define ETHISQ          (ETHBASE+0x0008)
#define ETHPPPTR        (ETHBASE+0x000A)
#define ETHPPDATAL      (ETHBASE+0x000C)
#define ETHPPDATAH      (ETHBASE+0x000E)

#define ETH_RX_BUSY     0x0001
#define ETH_COLLISION   (0x8000|0x0200)
#define ETH_JABBER      0x0400
#define ETH_TXOK        0x0100
#define ETH_RXOK        (0x0100|0x800|0x400|0x200)
#define ETH_EXTRA_DATA  0x4000
#define ETH_RUNT        0x2000
#define ETH_CRC         0x1000

/* Register Numbers */

#define REG_NUM_MASK       0x003F
#define REG_NUM_RX_EVENT   0x0004
#define REG_NUM_TX_EVENT   0x0008
#define REG_NUM_BUF_EVENT  0x000C
#define REG_NUM_RX_MISS    0x0010
#define REG_NUM_TX_COL     0x0012

enum{
  ETH_SUCCESS,
  ETH_WRONG_VENDOR,
  ETH_INVALID_RESET,
  ETH_TX_FRAME_ERROR,
  ETH_TX_ERROR,
  ETH_RX_NO_PCK
};

typedef struct {
  uint16_t mac_src[3];
  uint16_t mac_dst[3];
  char     data[64];
  uint16_t len;
  uint16_t count;
  uint16_t errval;
} eth_pck_t;


uint16_t ethernet_reset_chip(
  void
);

uint16_t ethernet_init(
  void
);

uint16_t ethernet_send_packet(
  uint16_t *dst_mac,
  uint16_t *src_mac,
  char *data,
  uint16_t size
);

int ethernet_receive_packet(
  uint16_t *dst_mac,
  uint16_t *src_mac,
  char *buf,
  uint16_t *len,
  uint16_t *count
);


void ethernet_get_mac( char *mac );

static void ethernet_write_pp_data(
  uint16_t PPAddr,
  uint16_t PPData
);

static uint16_t ethernet_read_pp_data( uint16_t Offset );

static char ethernet_get_revision(
  void
);

//static void ethernett_process_receive_event( uint16_t event );

#endif /* ETHERNET_H_ */
