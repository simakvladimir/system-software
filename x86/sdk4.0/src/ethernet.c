/*
 * ethernet.c
 *
 *  Created on: 17.06.2013
 *      Author: ALM User
 */
#include <am186es.h>
#include <ethernet.h>
#include <interrupt.h>
#include <port.h>
#include <pio.h>
#include <watchdog.h>
#include <common.h>
#include <serial.h>

static eth_pck_t rx_pck;

//uint8_t mac[6] = {0x00,0x08,0x09,0x0A,0x0B,0x00};
static char mac[6] = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc };

//volatile uint8_t RxBuffer[16384];
static uint16_t TxStatus = 0;

union {
  uint16_t  word[2];
  uint32_t  dword;
  uint8_t  *ptr;
} DMADest;

static void ethernet_process_receive_event(
  uint16_t event
)
{
  unsigned short l, RxStatus = 0;
  int i;

  if( !( event & ETH_RXOK ) )
  {// if no frame received
      rx_pck.errval = ETH_RX_NO_PCK;
      return;
  }

  RxStatus = inpw(ETHDATAL);
  l = inpw(ETHDATAL);
  rx_pck.count = l - (6+6+2); //dst, src, len/type
  l = ++l >> 1; // length in words

  for(i = 0; i < 3; ++i, --l)
    rx_pck.mac_dst[i] = inpw(ETHDATAL);

  for(i = 0; i < 3; ++i, --l)
    rx_pck.mac_src[i] = inpw(ETHDATAL);

  rx_pck.len = inpw(ETHDATAL); --l;

  for(i = 0; i < l && i < 64; ++i)
      ((uint16_t*)rx_pck.data)[i] = inpw(ETHDATAL);

  rx_pck.errval = ETH_SUCCESS;
  return ;
}


/**
 * Interrupt handler routine
 */
static void interrupt far ethernet_int0_handler(
  void
)
{
  uint16_t Event;

  /* Read an event from the Interrupt Status Queue */
  Event = inpw( ETHISQ );

  tracelog_with_num("[Eth] INT0 is happened! Event = ", Event );

   /* Process all the events in the Interrupt Status Queue */
   while ( Event != 0 )
   {
      /* Dispatch to an event handler based on the register number */
      switch ( Event & REG_NUM_MASK )
      {
         case REG_NUM_RX_EVENT:
//            RxStatus = Event&0xFFC0; // 10 high bits
            ethernet_process_receive_event( Event );
            tracelog_with_num( "[Eth] RxEvent. Status = ", Event );
//            ethernet_receive_event( Event );
            break;
         case REG_NUM_TX_EVENT:
           TxStatus = Event&0xFFC0; // 10 high bits
           tracelog_with_num( "[Eth] TxEvent. Status = ", Event );
//           ethernet_transmit_event( Event );
           break;
         case REG_NUM_BUF_EVENT:
            tracelog_with_num( "[Eth] BufEvent. Status = ", Event );
//                 // @jla add for rdy4txnow interrupt
//            ethernet_buffer_event( Event );
//         case REG_NUM_RX_MISS:
//         case REG_NUM_TX_COL:
//           tracelog_with_num( "[Eth] Unused. Status = ", Event );
//            /* Unused events */
            break;
      }

      Event = inpw( ETHISQ );
   }
   outpw( EOI,0x0C ); // write type int
}

/**
 * Reset chip
 */
uint16_t ethernet_reset_chip(
  void
)
{
  uint16_t i;
  uint16_t status;
  // software reset
  outpw( ETHPPPTR  , 0x114 );
  outpw( ETHPPDATAL, 0x0040 );

  tracelog_with_str("[Eth] start delay", "" );
  delay_in_mcs( 1000000 );
  tracelog_with_str("[Eth] cancel delay", "" );

  /* Transition SBHE to switch chip from 8-bit to 16-bit */
  inpw( ETHPPPTR   );
  inpw( ETHPPPTR+1 );
  inpw( ETHPPPTR   );
  inpw( ETHPPPTR+1 );

  for ( i=0; i<ETH_MAX_RESET_COUNTER; i++ ){
    WDT_reset();
    outpw( ETHPPPTR, 0x0136 );
    status = inpw( ETHPPDATAL );
    tracelog_with_num("[Eth] status = ", status );
     if ( status & 0x0080 )
         break;
  }

  if (i == ETH_MAX_RESET_COUNTER)
    return ETH_INVALID_RESET;

  return ETH_SUCCESS;
}

/**
 * Ethernet initialization
 */
uint16_t ethernet_init(
  void
)
{
  uint16_t errval;

  // Software reset
  // read product id
  outpw( ETHPPPTR, 0x0 );
  if ( inpw( ETHPPDATAL ) != ETH_VENDOR ){
    tracelog_with_num("[Eth] wrong vendor = ",  inpw( ETHPPDATAL ) );
    return ETH_WRONG_VENDOR;
  }

  // check revision TODO
  serial_puts( "[Eth] Founded cs8900a Revision = " );
  serial_putc( ethernet_get_revision() );
  serial_puts( ENDL );

  // software reset
  errval =  ethernet_reset_chip();
  tracelog_with_num( "[Eth] Reset code = ", errval );

  if (errval)
    return errval;

  // set default MAC address TODO
//  outpw(ETHPPPTR,0x8158);//Individual Address, Auto-increment
//  for(index=0; index<3; index++) //
//    outpw(ETHPPDATAL,((uint16_t*)mac)[index]);
  ethernet_write_pp_data( ETH_PKTPG_IND_ADDR,   *((uint16_t*)&mac[0]) );
  ethernet_write_pp_data( ETH_PKTPG_IND_ADDR+2, *((uint16_t*)&mac[2]) );
  ethernet_write_pp_data( ETH_PKTPG_IND_ADDR+4, *((uint16_t*)&mac[4]) );

  // read MAC
  tracelog_with_num( "[Eth] MAC read1 =", ethernet_read_pp_data(ETH_PKTPG_IND_ADDR) );
  tracelog_with_num( "[Eth] MAC read2 =", ethernet_read_pp_data(ETH_PKTPG_IND_ADDR+2) );
  tracelog_with_num( "[Eth] MAC read3 =", ethernet_read_pp_data(ETH_PKTPG_IND_ADDR+4) );

  // Next configure
  ethernet_write_pp_data(0x116,0);//BusCTL
  ethernet_write_pp_data(0x112,0);//LineCTL
  ethernet_write_pp_data(0x114,0x1000);//SelfCTL
  ethernet_write_pp_data(0x102,0x1100);//RxCFG // int when CRC err and receive OK
  ethernet_write_pp_data(0x104,0x500);//RxCTL // dest == MAC, CRC and length are valid
  ethernet_write_pp_data(0x106,0x8740);//TxCFG // if transmited then interrupt
  ethernet_write_pp_data(0x10A,0x0000);//BufCFG
  ethernet_write_pp_data(0x22,0);//Interrupt Number
  ethernet_write_pp_data(0x112,0xC0);//LineCTL

//  __asm {cli};
//  __asm {sti};

  __asm {cli};

  ethernet_write_pp_data(0x116,0x8000);//BusCTL interrupt cs8900 on

  interrupt_vector_catch( INTERRUPT_INT0_TYPE,
                          (void (__interrupt __far *)(void))&ethernet_int0_handler);
  outpw(I0CON,0x16);//Level-Triggered, 6-priority
  interrupt_enable(0x10);

  rx_pck.errval = ETH_RX_NO_PCK;

  __asm {sti};

  return ETH_SUCCESS;
}

uint16_t ethernet_send_packet(
  uint16_t *dst_mac,
  uint16_t *src_mac,
  char *data,
  uint16_t size
)
{
  uint16_t BusST;
  uint16_t j;
  // transmit cmd
  outpw(ETHTXCMD,0x00C0);
  outpw(ETHTXLEN,6+6+2+size);
  outpw(ETHPPPTR,0x0138);

  for(j = 0; j < 1000; j++) continue;

  do
  {
    WDT_reset();
    BusST = inpw(ETHPPDATAL);
    if(BusST & 0x0080) //if TxBidErr
        return ETH_TX_FRAME_ERROR; //
  }while( !(BusST & 0x0100) ); //while not Rdy4TxNOW

//  __asm {cli};
  TxStatus=0;
//  __asm {sti};

  for(j = 0; j < 3; j++) //Вывод адреса назначения
    outpw( ETHDATAL, dst_mac[j] ); //Выводим словами

  for(j = 0; j < 3; j++) //Вывод адреса отправителя
    outpw( ETHDATAL, src_mac[j] );

  outpw(ETHDATAL, swap_lo_hi(size)); // packet length

  size = ++size >> 1; // длина в словах
  for(j = 0; j < size; ++j)
    outpw(ETHDATAL, ((uint16_t*)data)[j]);

   //Анализ результата
   while( !(TxStatus & 0xFFC0) )
   {
      WDT_reset();
   }

   if(TxStatus & ETH_TXOK)
     return ETH_SUCCESS;

   return ETH_TX_ERROR;
}


int ethernet_receive_packet(
  uint16_t *dst_mac,
  uint16_t *src_mac,
  char *buf,
  uint16_t *len,
  uint16_t *count
)
{
  uint16_t i, status;

 __asm {cli};

  status = rx_pck.errval;

  if ( !rx_pck.errval )
  {
      WDT_reset();

      for ( i=0; i<3; i++ ){
        dst_mac[i] =   rx_pck.mac_dst[i] ;
        src_mac[i] =   rx_pck.mac_src[i] ;
      }

      for ( i=0; i<64; i++ ){
        buf[i] =   rx_pck.data[i];
      }

      *len     =  rx_pck.len ;
      *count   =  rx_pck.count ;

      rx_pck.errval = ETH_RX_NO_PCK;
  }

  __asm {sti};

  return status;
}

void ethernet_get_mac( char *mac )
{
  uint16_t temp,i;
  for (i=0;i<3;i++){
    temp = ethernet_read_pp_data(ETH_PKTPG_IND_ADDR+i*2);
    mac[i*2]   = temp & 0xFF;
    mac[i*2+1] = (temp >> 8) & 0xFF;
  }

}

/**
 * Write data to cs8900a register
 */
static void ethernet_write_pp_data(
  uint16_t PPAddr,
  uint16_t PPData
)
{
  outpw(ETHPPPTR  , PPAddr);
  outpw(ETHPPDATAL, PPData);
}

static uint16_t ethernet_read_pp_data( uint16_t Offset )
{
  outpw( ETHPPPTR, Offset );
  return inpw( ETHPPDATAL );
}

/**
 * Decode chip revision
 */
static char ethernet_get_revision(
  void
)
{
  uint16_t rev;

  outpw( ETHPPPTR, 0x2 );
  rev = inpw( ETHPPDATAL );

  return ( (rev >> 8) + 0x3B);
}

