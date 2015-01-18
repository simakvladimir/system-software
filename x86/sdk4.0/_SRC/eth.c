/*-----------------------------------------------------------------------------

    25.07.2001  API ���-1 v 1.2.3

    ������ ��� ������ � ������������ Ethernet CS8900a
    -------------------------------------------------

WORD InitCS8900a(void); //������������� ����������� CS8900a � ��� ���������
            //(DMA, IRQ)
WORD EthReadPack(VPTR,BPTR,WPTR);//��������� ������ �� ������ Ethernet
WORD EthWritePack(VPTR,BPTR,WORD);//�������� ������ �� ������ Ethernet

 ------------------------------------------------------------------------------
�����: ������� �.�.

���������:
-------- ------ ----------- --------------------------------------------------- 
 ����   ������  �����               ��������
-------- ------ ----------- --------------------------------------------------- 
18.08.01 1.1
20.08.01 1.2.1  ������ �.�. ����� ������
20.08.01 1.2.2  ������� �.�. ��������� ��������� � ���������� ���������� INT0
22.08.01 1.2.2  ������� �.�. �������������
26.12.01 1.2.3  ������� �.�. ����� CS8900a �������� � ����� ���������� ���

-----------------------------------------------------------------------------*/

#include "mpkdefs.h"
#include "186es.h"
#include "eth.h"
#include "low.h"
#include "serial.h"
#include "error.h"
#include "net.h"
#include "modem.h"
#include "common.h"

#include "print.h"
#include "vcom.h"

#include <string.h>

//----------------------------------------------------------------------
//              ����������
//----------------------------------------------------------------------
#define MAX_TRIES 2000

extern BYTE mac[];
volatile BYTE RxBuffer[16384];//����� ������ 16K
//volatile BPTR RxBuffer=(BPTR)ETH_RX_BUF;//����� ������ 16K
volatile WORD RxCount=0;//���������� �������� �������, ������������ � ������
volatile WORD RxHead=0;//�������� ������������ RxBuffer �� ������� ������������ ������
volatile WORD RxLen=0;//��������� ����� ������� � ������
volatile WORD RxTail=0;//�������� �� ������� ���������� ����� (4-�������� ������������)
volatile WORD RxStatus=0, TxStatus=0;
union {
    WORD word[2];
    DWORD dword;
    BPTR ptr;
    } DMADest; //������� ����� ���������� DMA (����� ���������� ������ �
//������ ������)

void interrupt far int0_handle(void);
void interrupt far dma0_handle(void);





//������� ������ � ������� CS8900A � ���������� ������� PPAddr
void WritePPData(WORD PPAddr, WORD PPData)
{
outpw(ETHPPPTR, PPAddr);
outpw(ETHPPDATAL, PPData);
}

/*----------------------------------------------------------------------
                InitCS8900
  ----------------------------------------------------------------------
    ������������� ����������� CS8900a � ��� ���������

    ��. ���������:  ���
    ���������:  0 - �������
            1 - �������
  ��������:
    ������������� ������ 0 DMA, ���������� ���������� �� DMA,
  ������������ PCS0, PCS1 �� �������� ������������ �-�, �����. �����������
  CS8900a, ����������� ����� �����������, ��������� ��� ���������
  ----------------------------------------------------------------------*/
extern void Blink(long);
WORD InitCS8900a(void)
{
WORD i,j;
WORD tmp;

//��������� DMA
__asm {cli};
setvect(0x0A,(void (__interrupt __far *)(void))&dma0_handle);
outpw(DMA0CON,0x5);//5-priority, must be the highest of all
EnInterrupt(0x4);//
__asm {sti};
outpw(D0SRCH,0x0000);
outpw(D0SRCL,ETHDATAL);//
DMADest.ptr=(BPTR)RxBuffer;
DMADest.dword=(DMADest.word[1]<<4)+DMADest.word[0];
outpw(D0DSTH,DMADest.word[1]);
outpw(D0DSTL,DMADest.word[0]);
outpw(D0TC,0);
outpw(D0CON,0xA30D);//Start bit is off, unsynchronized, interrupt

Width = 4;
Radix = 16;
Direction = PRINT_COM;

//���������� ����� CS8900A
for(j=0; j<MAX_TRIES; j++){
    ResetWDT();
    __asm {cli};
    tmp=inpw(P1DATA);
    outpw(P1DATA,(tmp&~0x1800)|0x2000);//Ether_Res High //+����� 2� ����� i2c
    for(i=0; i<1024; i++)continue;//��������
    outpw(P1DATA,tmp&0xC7FF);//Ether_Res Low//+����� 2� ����� i2c
    __asm {sti};
    for(i=0; i<1024; i++)continue;//��������
    outpw(ETHPPPTR,0x136); //SelfST
    for(i=0; i<1024; i++)continue;//��������
    outpw(ETHPPPTR,0x136);
    outpw(ETHPPPTR,0x136);
    i=inpw(ETHPPDATAL);
//    print(i);
    if(i&0x80)break;
//    Rsio();
    }

if(j==MAX_TRIES) {
    stat.ERROR_IO |= SM_ERROR_ETH;
    return 1;
    }

//��������� ��� ���������
outpw(ETHPPPTR,0x8158);//Individual Address, Auto-increment
for(i=0; i<3; i++) //
    outpw(ETHPPDATAL,((WPTR)mac)[i]);

WritePPData(0x116,0);//BusCTL
WritePPData(0x112,0);//LineCTL
WritePPData(0x114,0x1000);//SelfCTL
WritePPData(0x102,0x1100);//RxCFG
WritePPData(0x104,0x500);//RxCTL
WritePPData(0x106,0x8740);//TxCFG
WritePPData(0x10A,0x0000);//BufCFG
WritePPData(0x22,0);//Interrupt Number
WritePPData(0x112,0xC0);//LineCTL
//��������� ���������� ������
__asm {cli};
RxHead=0;
RxCount=0;
RxStatus=TxStatus=0;
__asm {sti};
WritePPData(0x116,0x8000);//BusCTL

//��������� INT0
__asm {cli};
setvect(0xC,(void (__interrupt __far *)(void))*int0_handle);
outpw(I0CON,0x16);//Level-Triggered, 6-priority
EnInterrupt(0x10);
__asm {sti};

stat.ERROR_IO &= ~SM_ERROR_ETH;

return 0;
}

/*----------------------------------------------------------------------
                EthReadPack
  ----------------------------------------------------------------------
    ��������� ������ �� ������ Ethernet

    ��. ���������:  RTable  - ��������� �� ��������� ���� rTable,
                  ���������� ������� ����������� MAC
                  ������� �����������
            Buf - ��������� �� �����, � ������� �����
                  ���������� ������� ����� �����������
                  ���������� ����� ����������� (1 ����),
                  ������ �� ������� RTable � ������,
                  ������������ � ��������� ������
            n   - ��������� �� �����, ���� ����� ��������
                  ����� ������ Buf � ������

    ���������:  0 - �������
            1 - ��� ������
            2 - ������, ������ ���������� � ������� �������
                GetNetError
  ----------------------------------------------------------------------*/
extern void Blink(long);
WORD EthReadPack(VPTR RTable,BPTR Buf,WPTR n)
{ 
WORD PacketLen=0,RxEvent=0;//����� ������, ���������� � ���
BYTE i;
//WORD qq=0;//!!!

if(RxStatus&ETH_CRC) {
    AddNetError(ETH0,1,ERROR_NET_CRC,0);
    return PACKET_ERROR;
    }
if(!RxCount) return PACKET_NO;

__asm {cli};
RxCount--;//���������� �������� ������������ � ������ �������
RxEvent=*((WPTR)(RxBuffer+RxHead));
PacketLen=*((WPTR)(RxBuffer+RxHead+2));

for(i=0; i<MAX_ENTRIES; i++) {//����� MAC-������ ����������� � ������� ����������� MAC-�������
    if( !_fmemcmp( (VPTR)(((rTable far *)RTable)->MACaddress[i]), (VPTR)(RxBuffer+RxHead+2+2+6), 6) )
        break;
    }

if(i==MAX_ENTRIES){ 
    *n=0;
    AddNetError(ETH0,1,ERROR_NET_FIREWALL,0);
    if(PacketLen&0x3){
        PacketLen+=4;
        PacketLen&=0xFFFC; //Double-word alignment
        }
    PacketLen+=4;//���� 2 ����� (������ � �����)
    RxHead+=PacketLen; //��������� "������" ������
    RxLen-=PacketLen;//���������� ����� ������
    RxHead&=0x3FFF;
    __asm {sti};
//  Wsio('2');
//  Blink(200);
//Wsio((BYTE)PacketLen);  //!!!
//Wsio((BYTE)(PacketLen>>8));
//for(qq=4; qq<18; qq++)
//  Wsio(RxBuffer[RxHead+qq]);
//for(qq=0; qq<6; qq++)
//  Wsio(((rTable far*)RTable)->MACaddress[0][qq]);
    return PACKET_ERROR;
    }

//Wsio((BYTE)PacketLen);  //!!!
//Wsio((BYTE)(PacketLen>>8));
//for(qq=0; qq<(PacketLen-6-6-2); qq++)
//  Wsio(RxBuffer[RxHead+2+2+6+6+2+qq]);

Buf[0]=((rTable*)RTable)->LogicalAddress[i]; Buf++;
*n=PacketLen-6-6-2;
_fmemmove(Buf,(BPTR)RxBuffer+RxHead+2+2+6+6+2,PacketLen-6-6-2);
if(PacketLen&0x3){
    PacketLen+=4;
    PacketLen&=0xFFFC; //Double-word alignment
    }
PacketLen+=4;
RxLen-=PacketLen;
RxHead+=PacketLen;//��������� "������" ������
RxHead&=0x3FFF; //Wrap-around
__asm   {sti};
return PACKET_OK;
}

/*----------------------------------------------------------------------
                EthWritePack
  ----------------------------------------------------------------------
    �������� ������ �� ������ Ethernet

    ��. ���������:  RTable  - ��������� �� ��������� ���� rTable,
                  ���������� ������� ����������� MAC
                  ������� ����������
            Buf - ��������� �� �����, � ������� ����������:
                  ���������� ����� ���������� (1 ����),
                  ������ �� ������� RTable � ������,
                  ������� ���������� ���������
            n   - �����, ���������� ����� ������ �
                  ������

    ���������:  0 - �������
            2 - ������, ������ ���������� � ������� �������
                GetNetError
  ----------------------------------------------------------------------*/
WORD EthWritePack(VPTR RTable,BPTR Buf,WORD n)
{ 
BYTE i;
WORD j;
WORD BusST,TxEvent;//���������� ��������� BusST, TxEvent CS8900A
for(i=0; i<MAX_ENTRIES; i++)
    if(((rTable*)RTable)->LogicalAddress[i]==*Buf) //���� �����. ����� ����������
        break;//(������ ���� � Buf) ������ � �������
if(i==MAX_ENTRIES){ //�����. ����� ���������� �� ������ � �������
    AddNetError(ETH0,1,ERROR_NET_FIREWALL,0);
    return 2;
    }
Buf++;//����� 1 ����, �������������� ����� ���������� ����� ����������

//����������� CS8900A � ������� ������
outpw(ETHTXCMD,0x00C0);
outpw(ETHTXLEN,6+6+2+n);
outpw(ETHPPPTR,0x0138);
for(j=0; j<1000; j++)continue; //��������

do{
    BusST=inpw(ETHPPDATAL);
    if(BusST & 0x0080) {//if TxBidErr
        AddNetError(ETH0,1,ERROR_NET_LEN,0);
        return PACKET_ERROR; //
        }
    }while( !(BusST & 0x0100) ); //while not Rdy4TxNOW
for(j=0; j<3; j++) //����� ������ ����������
    outpw(ETHDATAL,((WPTR)(((rTable*)RTable)->MACaddress[i]))[j] ); //������� �������
for(j=0; j<3; j++) //����� ������ �����������
    outpw(ETHDATAL,((WPTR)mac)[j]);


    outpw(ETHDATAL,n);

__asm {cli};
TxStatus=0;
__asm {sti};
j=ETHDATAL;
    __asm {
        pushf
        push    AX
        push    CX
        push    DX
        push    SI
        push    DS
        cld
        mov DX,j
        mov CX,n
        shr cx,1
        lds SI,Buf
    rep     outsw
        pop DS
        pop SI
        pop DX
        pop CX
        pop AX
        popf
        }
if(n&0x1)//���� ���������� ���� � ������ ��������,
    outpw(ETHDATAL,Buf[n-1]);//�� ������� ��������� ����

for(j=0; j<1000; j++)continue;

//������ ����������
do {
    __asm {cli};
    TxEvent=TxStatus;
    __asm {sti};
    ResetWDT();
    } while(!(TxEvent&0xFFC0));
if(TxEvent & ETH_TXOK) return PACKET_OK;
else {
    if(TxEvent & ETH_JABBER)
        AddNetError(ETH0,1,ERROR_NET_JABBER,0);
    else if(TxEvent & ETH_COLLISION)
        AddNetError(ETH0,1,ERROR_NET_COLLISION,0);
    return PACKET_ERROR;
    }
}

#if 0
int sendCS8900a(short *dst_mac, short *src_mac, char *buf, short count)
{ 
int i, j;
unsigned short BusST;//���������� �������� BusST CS8900A

    //����������� CS8900A � ������� ������
    outpw(ETHTXCMD,0x00C0);
    outpw(ETHTXLEN,6+6+2+count); // src + dst + len + data
    outpw(ETHPPPTR,0x0138);

    for(j = 0; j < 1000; j++) continue; // ��������

    do
    {
        BusST = inpw(ETHPPDATAL);
        if(BusST & 0x0080) //if TxBidErr
            return PACKET_ERROR; //
    }while( !(BusST & 0x0100) ); //while not Rdy4TxNOW

    for(j = 0; j < 3; j++) //����� ������ ����������
        outpw( ETHDATAL, dst_mac[j] ); //������� �������
    for(j = 0; j < 3; j++) //����� ������ �����������
        outpw( ETHDATAL, src_mac[j] );

    outpw(ETHDATAL, count); // packet length

    count = ++count >> 1; // ����� � ������
    for(j = 0; j < count; ++j)
        outpw(ETHDATAL, ((short*)buf)[j]);

    //������ ����������
    while( !(TxStatus & 0xFFC0) );
    if(TxStatus & ETH_TXOK) 
        return PACKET_OK;

    return PACKET_ERROR;
}

int receiveCS8900a(short *dst_mac, short *src_mac, char *buf, short *len, int *count)
{ 
unsigned short l = 0, RxStatus = 0;//����� ������, ���������� � ���
int i;

    if( !( readCS8900aReg( 0x124 ) & ETH_RX_OK ) ) // if no frame received
        return 1;

    RxStatus = inpw(ETHDATAL);
    l = inpw(ETHDATAL);
    *count = l - (6+6+2); //dst, src, len/type
    l = ++l >> 1; // length in words

    for(i = 0; i < 3; ++i, --l)
        dst_mac[i] = inpw(ETHDATA);

    for(i = 0; i < 3; ++i, --l)
        src_mac[i] = inpw(ETHDATA);

    *len = inpw(ETHDATA); --l;

    for(i = 0; i < l; ++i)
        ((short*)buf)[i] = inpw(ETHDATA);

    return PACKET_OK;
}

void interrupt far int0_handler(void)
{
short status;

    while( (status = inpw(ETHISQ)) != 0 )
    {
        switch(status&0x3F) //����� ������� ��������� � ISQ
        {
            case 0xC: //BufEvent
                break;
            case 0x8: //TxEvent
                TxStatus=status&0xFFC0;
                break;
            case 0x4: //RxEvent
                if(status&ETH_RXOK)
                    RxStatus = ETH_RX_PACKET; // ������ �����
        }
    }

    outpw(EOI,0x0C);
}


#endif


void interrupt far int0_handle(void)
{
WORD status,len;
if(!(RxStatus&ETH_RX_BUSY)){//��������� ������ �����������, �����
                //���������� ���������� �� DMA
    while(status=inpw(ETHISQ)){
        switch(status&0x3F){//����� ������� ��������� � ISQ
        case 0xC: //BufEvent
            break;
        case 0x8: //TxEvent
            TxStatus=status&0xFFC0;
            break;
        case 0x4: //RxEvent
            if(status&ETH_RXOK){
                RxStatus=ETH_RX_BUSY;//���� ��������� �� ��������� DMA
    
                *((WPTR)(RxBuffer+RxTail))=inpw(ETHDATAL);
                *((WPTR)(RxBuffer+RxTail+2))=inpw(ETHDATAL);
                len=*((WPTR)(RxBuffer+RxTail+2));
                len+=(len&0x1)?2:0;
    
                //�������� ������������ ������
                if( (RxLen+len+4+4)>=16384 ) {
                    outpw(ETHPPPTR,0x102);//RxCFG
                    outpw(ETHPPDATAL,inpw(ETHPPDATAL)|0x40);
                    //Skip Command - ����� ��������
                    RxStatus&=~ETH_RX_BUSY;
                    }
                else {
                    len>>=1;//����� � ������
                    //������ DMA0
                    DMADest.ptr=(BPTR)RxBuffer+RxTail+4;
                    DMADest.dword=(DMADest.word[1]<<4) + DMADest.word[0];
                    outpw(D0DSTH,DMADest.word[1]);
                    outpw(D0DSTL,DMADest.word[0]);
                    outpw(D0TC,len);
                    outpw(D0CON,inpw(D0CON)|0x6);//������
                    }
                }
            else RxStatus=status;
            }
        }
    }
outpw(EOI,0x0C);
}

void interrupt far dma0_handle(void)
{
WORD len;
outpw(D0CON,(inpw(D0CON)|0x4)&0xFFFD);//Disable DMA0
len=*((WPTR)(RxBuffer+RxTail+2))+4;
RxStatus=(*((WPTR)(RxBuffer+RxTail)))&0xFFC0;
len=(len&0x3)?(len+0x4)&0xFFFC:len;//DWORD alignment
RxLen+=len;
RxTail+=len;
RxTail&=0x3FFF;//wrap-around

//Wsio((BYTE)RxTail);
//Wsio((BYTE)(RxTail>>8));
//Wsio((BYTE)RxHead);
//Wsio((BYTE)(RxHead>>8));

RxCount++;
outpw(EOI,0xA);
}

