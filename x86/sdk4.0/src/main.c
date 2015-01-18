/*
 * main.c
 *
 *  Created on: 16.02.2014
 *      Author: BoBaHouD
 */
#include <am186es.h>
#include <types.h>
#include <common.h>
// devices
#include <watchdog.h>
#include <port.h>
#include <serial.h>
#include <ethernet.h>
#include <timer.h>
#include <led.h>

/* protocol */
#include <tx_packet.h>
#include <rx_packet.h>

static char mac_src[6];
static char mac_dst[6] = { 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54 };
//static char data[64] = { "1234567890123456789012345678901234567890123456789012345678901234" };

static tx_pkt_t telemetry_pckt;
static rx_pkt_t command_pckt;

struct {
  uint16_t mac_src[3];
  uint16_t mac_dst[3];
  char     data[64];
  uint16_t len;
  uint16_t count;
} receive_packet;

int main(
  void
)
{
  uint16_t errval;
  uint16_t sw;
  // Led configure
//  outpw(P1MODE,inpw(P1MODE) | 0x0200);
//  outpw(P1DIR ,inpw(P1DIR)&~0x0200);


  led_red_init();
  led_yellow_init();
  led_green_init();

  // Debug uart start
  serial_init();
  serial_puts("[main] DUART is configured...\r\n");

  tracelog_with_num( "[main] tick = ", timer_get_tick() );
  timer_init();
  tracelog_with_num( "[main] Eth init status = ", ethernet_init() );

  WDT_reset();

  ethernet_get_mac( &mac_src );

  TX_PCKT_SET_MAC_DST( telemetry_pckt, mac_dst );
  TX_PCKT_SET_MAC_SRC( telemetry_pckt, mac_src );

  /*  */
  jumpers_init();

  telemetry_pckt.telem_state = 0x0;

  while(1)
  {
      uint32_t time = timer_get_tick();
      TX_PCKT_SET_TIME( telemetry_pckt, &time );


//    tracelog_with_num( "[main] INSERV reg = ", inpw(INSERV) );
//
//    outpw(P1DATA ,inpw(P1DATA)&~0x0200);
//    serial_puts("[main] LED is on\r\n");

//    led_red_set_state( 0);
//    led_yellow_set_state( 0  );
//    led_green_set_state(0 );

    delay_in_mcs( 100000 ); // 0.1 s

    errval = ethernet_receive_packet( (uint16_t*)command_pckt.mac_dst,
        (uint16_t*)command_pckt.mac_src,
        command_pckt.op_code,
        &receive_packet.count,
        (uint16_t*)command_pckt.pkt_length );
//&receive_packet.count
    tracelog_with_num( "[main] RX pck = ", errval );
//
    if (errval==0){

        sw = swap_lo_hi(*((uint16_t*)&command_pckt.op_code));
        tracelog_with_num( "[main] RX pck data[1] = ",sw);
        tracelog_with_num( "[main] PARAM = ",*((uint32_t*)&command_pckt.param));
        switch (sw) {
          case CMD_RED_LED:
            led_red_set_state(command_pckt.param[3]);
            telemetry_pckt.rled_state = command_pckt.param[3];
            break;
          case CMD_GREEN_LED:
            led_green_set_state(command_pckt.param[3]);
            telemetry_pckt.gled_state = command_pckt.param[3];
            break;
          case CMD_YELLOW_LED:
            led_yellow_set_state(command_pckt.param[3]);
            telemetry_pckt.yled_state = command_pckt.param[3];
            break;
          case CMD_TELEMETRY:
            telemetry_pckt.telem_state = command_pckt.param[3];
            break;
          default:
            break;
        }


//        command_pckt.unused = 0x0;
        tracelog_with_str( "[main] RX = ", "");
        tracelog( command_pckt.mac_dst, 6 );
        tracelog( command_pckt.mac_src, 6 );
        tracelog( command_pckt.op_code, 2 );
        tracelog( command_pckt.param, 4 );
        tracelog( command_pckt.pkt_length, 2);
        tracelog( command_pckt.timePC, 9 );
//        tracelog_with_num( "[main] RX pck data[1] = ", (uint32_t)command_pckt.param );
//        tracelog_with_str( "[main] RX pck data[1] = ", command_pckt.timePC );
//        tracelog_with_num( "[main] RX pck data[2] = ", receive_packet.data[2] );
//        tracelog_with_num( "[main] RX pck data[3] = ", receive_packet.data[3] );
//
//
//        tracelog_with_num( "[main] RX pck mac_src[0] = ", receive_packet.mac_src[0] );
//        tracelog_with_num( "[main] RX pck mac_src[1] = ", receive_packet.mac_src[1] );
//        tracelog_with_num( "[main] RX pck mac_src[2] = ", receive_packet.mac_src[2] );
//
//        tracelog_with_num( "[main] RX pck mac_dst[0] = ", receive_packet.mac_dst[0] );
//        tracelog_with_num( "[main] RX pck mac_dst[1] = ", receive_packet.mac_dst[1] );
//        tracelog_with_num( "[main] RX pck mac_dst[2] = ", receive_packet.mac_dst[2] );
//
//        tracelog_with_num( "[main] RX pck len = ", receive_packet.len );
//        tracelog_with_num( "[main] RX pck count = ", receive_packet.count );
    }
//
//    outpw(P1DATA ,inpw(P1DATA) | 0x0200);
//    serial_puts("[main] LED is off\r\n");
//
//    led_red_set_state( 1);
//    led_yellow_set_state( 1);
//    led_green_set_state(1 );
    telemetry_pckt.jmp_modem_state = jumper_get_state( JUMPER_MODEM_PIO_PIN );
    telemetry_pckt.jmp_rs_state    = jumper_get_state( JUMPER_RS_PIO_PIN );
    telemetry_pckt.jmp_lock_state  = jumper_get_state( JUMPER_LOCK_PIO_PIN );
    telemetry_pckt.jmp_mif_state   = jumper_get_state( JUMPER_MIF_PIO_PIN );
    telemetry_pckt.jmp_prog_state  = jumper_get_state( JUMPER_PROG_PIO_PIN );
    telemetry_pckt.jmp_ext1_state  = jumper_get_state( JUMPER_EXT1_PIO_PIN );
    telemetry_pckt.jmp_ext2_state  = jumper_get_state( JUMPER_EXT2_PIO_PIN );

    delay_in_mcs( 100000 ); // 0.1 s

//    tracelog_with_str( "[main] JuMpers = ", "");
//    tracelog( telemetry_pckt.jmp_modem_state, 7 );



    tracelog_with_num( "[main] tick = ", timer_get_tick() );

    if ( telemetry_pckt.telem_state )
      tracelog_with_num( "[main] TX pck = ", ethernet_send_packet( (uint16_t*)mac_dst, (uint16_t*)mac_src, &telemetry_pckt.rled_state,  sizeof(tx_pkt_t)-6-6-2) );
  }
  return 0;
}
