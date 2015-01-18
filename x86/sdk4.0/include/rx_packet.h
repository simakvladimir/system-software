/*
 * rx_packet.h
 *
 *  Created on: 05.03.2014
 *      Author: BoBaHouD
 */

#ifndef RX_PACKET_H_
#define RX_PACKET_H_

typedef struct{
  char mac_dst[6];
  char mac_src[6];
  char pkt_length[2];
  char op_code[2];
  char param[4];
  char timePC[9];
}rx_pkt_t;

enum {
    CMD_RED_LED,
    CMD_GREEN_LED,
    CMD_YELLOW_LED,
    CMD_TELEMETRY
};
enum {
    OFF,
    ON
};


#endif /* RX_PACKET_H_ */
