#ifndef ETH
#define ETH

//Область для буфера приема
#define ETH_RX_BUF	0x20000000l
//Базовый адрес блока регистров CS8900A в пространстве ввода-вывода
#define ETHBASE		0x0000
//Блок регистров CS8900A
#define ETHDATAL	(ETHBASE+0x0000)
#define ETHDATAH	(ETHBASE+0x0002)
#define ETHTXCMD	(ETHBASE+0x0004)
#define ETHTXLEN	(ETHBASE+0x0006)
#define ETHISQ		(ETHBASE+0x0008)
#define ETHPPPTR	(ETHBASE+0x000A)
#define ETHPPDATAL	(ETHBASE+0x000C)
#define ETHPPDATAH	(ETHBASE+0x000E)

//Флаги регистров CS8900A
#define ETH_RX_BUSY	0x0001
#define ETH_COLLISION	(0x8000|0x0200)
#define ETH_JABBER	0x0400
#define ETH_TXOK	0x0100
#define ETH_RXOK	(0x0100|0x800|0x400|0x200)
#define ETH_EXTRA_DATA	0x4000
#define ETH_RUNT	0x2000
#define ETH_CRC         0x1000

//Результаты работы функций WritePack и ReadPack
#ifndef PACKET_OK
#define PACKET_OK	0
#define PACKET_NO	1
#define PACKET_ERROR	2
#endif

extern WORD InitCS8900a(void);
extern WORD EthReadPack(VPTR,BPTR,WPTR);
extern WORD EthWritePack(VPTR,BPTR,WORD);



#endif
