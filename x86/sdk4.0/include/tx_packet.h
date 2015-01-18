/*
 * tx_packet.h
 *
 *  Created on: 05.03.2014
 *      Author: BoBaHouD
 */

#ifndef TX_PACKET_H_
#define TX_PACKET_H_

static unsigned tx_i;

typedef struct{
  char mac_dst[6];
  char mac_src[6];
  char pkt_length[2];
  char rled_state;
  char gled_state;
  char yled_state;
  char jmp_modem_state;
  char jmp_rs_state;
  char jmp_lock_state;
  char jmp_mif_state;
  char jmp_prog_state;
  char jmp_ext1_state;
  char jmp_ext2_state;
  char telem_state;
  char sdk_time[4];
}tx_pkt_t;


#define TX_PCKT_SET_MAC_DST( pkt, mac )\
  for(tx_i=0;tx_i<6;tx_i++) \
    pkt.mac_dst[tx_i] = mac[tx_i];

#define TX_PCKT_SET_MAC_SRC( pkt, mac )\
  for(tx_i=0;tx_i<6;tx_i++) \
    pkt.mac_src[tx_i] = mac[tx_i];

#define TX_PCKT_SET_TIME( pkt, value )\
  char *tx_pckt_c = (char*)value;\
  for(tx_i=0;tx_i<4;tx_i++) \
    pkt.sdk_time[tx_i] = tx_pckt_c[3-tx_i];


#endif
