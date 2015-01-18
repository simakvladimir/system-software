/* ****************************************************************************

        24.07.2001 API ���-1 v 1.2.3


        ������� ������ � �����
        ----------------------

 ------------------------------------------------------------------------------
�����: ������� �.�.

���������:
-------- ------ ----------- --------------------------------------------------- 
 ����   ������  �����                           ��������
-------- ------ ----------- --------------------------------------------------- 
18.08.01 1.1    �������� ������ ������ ������� ������� � ������������ � �����
                ������� ������� ������ ����������������� ������ (MODEM, RS232,
                RS485)                                                        
20.08.01 1.2.1  ������ �.�. ����� ������
22.08.01 1.2.2  ������� �.�. ��������� ������ � RS485
25.08.01 1.2.3  ������ �.�. ���������� ������� SetIP � GetIP

 **************************************************************************** */

#include <string.h>
#include "186es.h"
#include "mpkdefs.h"
#include "sio.h"
#include "low.h"
#include "error.h"
#include "net.h"
#include "common.h"
#include "eth.h"
#include "eeprom.h"
#include "serial.h"

BYTE mac[6] = {0x00,0x08,0x09,0x0A,0x0B,0x00};

/* ----------------------------------------------------------------------------
                GetNetError
 ----------------------------------------------------------------------------
���������� �� ������� �������� ����������

���������       ��� ������.
                0 - ��� ������.
                1 - ����������� �� ������ ��. � error_net

Error_net       ��������� �� ��������� ���� ERROR_NET, ���� �������� ����������
                �� �������.

������� ���������
Error_net       ������ �� �������������� ���������� ���������� ������ ��������� ���� ERROR_NET.
Interface       ����� �������� ����������

��������
������� ��������� ��������� ERROR_NET ������� ������ ����������������� ������.

  ---------------------------------------------------------------------------- */

WORD GetNetError(ERROR_NET far *error,BYTE Interface)
{
        if ( error_net[Interface&0x01].ErrorCount )
        {
                error->Error = error_net[Interface&0x01].Error;
                error->ErrorCount = error_net[Interface&0x01].ErrorCount;
                error->ErrorTr = error_net[Interface&0x01].ErrorTr;
                error->ErrorRec = error_net[Interface&0x01].ErrorRec;
                return 1;
        }
        return 0;
}


/* ----------------------------------------------------------------------------
                  ResetNetError
 ----------------------------------------------------------------------------
����� ������ �������� ����������

void ResetNetError(BYTE Interface)

���������       ���

������� ���������
Interface       ����� �������� ����������

��������
������� ���������� ����� ������ � �������� ��������� ERROR_NET


  ---------------------------------------------------------------------------- */

void ResetNetError(BYTE Interface)
{
        error_net[Interface&0x01].Error = 0;
        error_net[Interface&0x01].ErrorCount = 0;
        error_net[Interface&0x01].ErrorTr = 0;
        error_net[Interface&0x01].ErrorRec = 0;
        if ( Interface==SER0 )
        {
                if ( !error_com[COM0].ErrorCount && !error_com[COM1].ErrorCount )
                        stat.ERROR_IO &= ~SM_ERROR_RS;
        }
        else
                stat.ERROR_IO &= ~SM_ERROR_ETH;
}


/* ----------------------------------------------------------------------------
              ReadPack
 ----------------------------------------------------------------------------
������ ������.

WORD  ReadPack(BYTE interface,VPTR  RTable, BPTR Buf , WPTR N)

���������       ��� ������:
                0 - ��� ������, ����� ������� ������,
                1 - ��� ������,
                2 - ��. ������ � ������� ������ �������� ����������

������� ���������
Interface       ����� �������� ����������
RTable  ������ �� ������� ����������� MAC-�������
N       ������ �����, ��� ����� �������� ����� ����������� ������
Buf     ������ �� �����, ���� ����� ������� ����� (������������ ������ ������ - 1500 ����, ����������� - 46 ����)

��������
������� ����������  ����� � ����� ��������� ������. ������� �� �����������.
  ---------------------------------------------------------------------------- */
WORD ReadPack(BYTE Interface,VPTR RTable,BPTR Buf,WPTR n)
{
        WORD i, len, crc;
        if ( Interface==SER0 )
        {
                if ( sio_mode&SER ) { //���� �� ������ ������������ ���. �����
                        while(transmitting)ResetWDT();
                        __asm {cli};
                        roffset = woffset = 0;
                        __asm {sti};
                        sio_mode &= ~SER;
                        return PACKET_NO;
                        }

                if ( !(sio_mode&RRDY) ) {
                        return PACKET_NO;
                        }

                // ���� �����, ��������� ��� "������������"
                __asm {cli};
                sio_mode |= RBUSY;              // ��������� �����
                __asm {sti};

                if ( _fmemcmp((const void far*)(rpack+4),(VPTR)mac,6) ){//�� ��� dst MAC
                        goto fw_fail;
                        }
                // �������� ����������� ������� �����������
                for ( i=0; i<MAX_ENTRIES; ++i )
                        if ( !_fmemcmp((const void far*)(rpack+4+6),(VPTR)(((rTable far *)RTable)->MACaddress[i]),6) )
                                break;
                if ( i==MAX_ENTRIES )   // ��� �� ������ ���������
                        goto fw_fail;

                Buf[0] = ((rTable far*)RTable)->LogicalAddress[i];
                len = *((WPTR)&rpack[16]);
                crc = *((WPTR)(rpack+4+6+6+2+len));

//              for(i=0; i<(4+6+6+2+len+2); i++) //!!!
//                      print(rpack[i]);

//              if ( CRC16((BPTR)rpack,4+6+6+2+len,0)!=crc )
//              {
//� ���� ������ �������� CRC16 ������ MPCRS ���������
//                      AddNetError(Interface,0,0,ERROR_NET_CRC);
//                      return PACKET_ERROR;
//              }

                _fmemcpy(Buf+1,(BPTR)(rpack+4+6+6+2),len);
                *n = len;
                roffset = 0;
                __asm {cli};
                sio_mode &= ~(RRDY|RBUSY);      // ������ ������ ��� � ����� ��������
                __asm {sti};

                return PACKET_OK;
        }
        else            // ������ � Ethernet
        {
//      return 0;
        return EthReadPack(RTable,Buf,n);
        }
fw_fail:
        AddNetError(Interface,0,0,ERROR_NET_FIREWALL);
        roffset = 0;
        __asm {cli};
        sio_mode &= ~(RRDY|RBUSY);      // ������ ������ ��� � ����� ��������
        __asm {sti};
        return PACKET_ERROR;
}


/* ----------------------------------------------------------------------------
                         WritePack
 ----------------------------------------------------------------------------
�������� ������

���������       ��� ������.
                0 - ��� ������, ����� ������� �������,
                1 - ����� �����,
                2 - ��. ������ � ������� ������ �������� ����������

������� ���������
Interface       ����� �������� ����������
RTable  ������ �� ������� ����������� MAC-�������
N       ����� ������
Buf     ������ �� �����, ��� ����������� ������������ �����
        (����. ������ ������ - 1500 ����, ����������� - 46 ���� )

��������
������� ��������� �������� ����� �� ������ �� ������� �����������. ������� �� ����������� �� ����� �������� ��� ������ � ����������� SER0 � ����������� ��� ������ � ����������� ETH0. ���������� ����� ���������� ������� �� ��������� ����������� ������ � ������������� � ������� RTable � ���������� (MAC) ����� ����������.

  ---------------------------------------------------------------------------- */
extern void Blink(long);//!!!
WORD WritePack(BYTE Interface,VPTR RTable,BPTR Buf,WORD n)
{
        BYTE i;
        BPTR cur;
        cur = wpack;
        // �������� ����������
        if ( Interface==SER0 )
        {
                if ( sio_mode&SER ) { //���� �� ������ ������������ ���. �����
                        while(transmitting)ResetWDT();
                        __asm {cli};
                        roffset = woffset = 0;
                        transmitting = 0;
                        sio_mode &= ~SER;
                        __asm {sti};
                        }
                else if ( transmitting )
                        return PACKET_BUSY;

                // �������� ����������� �������
                for ( i=0; i<MAX_ENTRIES; ++i )
                        if(((rTable*)RTable)->LogicalAddress[i]==*Buf)
                        //���� �����. ����� ����������
                                break;//(������ ���� � Buf) ������ � �������

                if ( i==MAX_ENTRIES )           // �� ����� �� ������ ���������� ������
                {
                        AddNetError(Interface,0,ERROR_NET_FIREWALL,0);
                        return PACKET_ERROR;
                }

                *((DPTR)cur) = 0x7EAA557E;      // ���������
                cur += 4;

                _fmemcpy((BPTR)cur,(BPTR)&(((rTable*)RTable)->MACaddress[i]),6);
                cur += 6;

                _fmemcpy((BPTR)cur,(BPTR)mac,6);
                cur += 6;
                *(WORD*)cur = n;
                cur += 2;

                //
                _fmemcpy((BPTR)cur,(Buf+1),n);  // data
                *((WORD *)(cur+n)) = CRC16((BPTR)wpack,4+6+6+2+n,0);    // crc
                __asm {cli};
                transmitting = 1;
                woffset = 0;
                wlen = 4+6+6+2+n+2-1;   // ����� ������-1

                if(!(sio_mode&SIO_DEBUG))
                        if(sio_mode&RS485){//���� RS485
                                if(sio_mode&EARLY){
                                        outpw(P1DATA,(inpw(P1DATA)&~0x1800)|0x10);//+����� 2� ����� i2c
                                        //#RTS passive
                                        while(!(inpw(P1DATA)&0x20));
                                        }
                                else {
//                                      Blink(4);
                                        outpw(P1DATA,inpw(P1DATA)&~0x1810);//+����� 2� ����� i2c
                                        //#RTS active
                                        while(inpw(P1DATA)&0x20);
                                        }
                                }
                outpw(SP0TD,(WORD)(wpack[woffset++]));
                outpw(SP0CT,inpw(SP0CT)|0x100);//��������� ���������� �� ������ �����
                __asm {sti};
        }
        else    // ������ � �����
        {
//      return 0;
        return EthWritePack(RTable,Buf,n);
        //
        }
        return PACKET_OK;
}


/* ----------------------------------------------------------------------------
                 InitEthernet
 ----------------------------------------------------------------------------
������������� ����������� Ethernet

���������       ��� ������: 0 - ��� ������, 1 - ������������� ��������
������� ���������
��������
������� �������������� ���������� Ethernet � ��� ��������� (���������� ��� � ���������� ����������). ����� ������� �������� � �������� ��������� ��������� ����������, ����������� � ���������� ETH0. ����������� � ������������ � ������ ������������� ������ - ��������. �������� ����������� ����������������  ����� ��� ����� ������ ���-1.

  ---------------------------------------------------------------------------- */

WORD InitEthernet(void)  
{
        if(GetMAC((BPTR)mac))
                return 1;//�� ������� ���������� MAC-�����
        return InitCS8900a();
}


/* ----------------------------------------------------------------------------
                        SetMAC
 ----------------------------------------------------------------------------
��������� MAC-������ ���-1

WORD SetMAC(BPTR Buf)

���������               ��� ������:
                        0 - ��� ������,
                        1 - MAC ����� �� ����������

������� ���������
Buf                     ������ �� ������ � MAC-������� ���-1

��������
������� ���������� MAC-����� � ����������������� ������ �������� �����������.

  ---------------------------------------------------------------------------- */

WORD SetMAC(BPTR Buf)
{
BYTE i;
WORD j;
long a,b=1;
        for(i=0; i<6; i++){
                if(WriteByteEEPROM(Buf[i],i+10))
                        return 1;
                for(j=0; j<4000; j++)//�������� ~40ms
                        a=a*b;
                ResetWDT();
                }
        return 0;
}


/* ----------------------------------------------------------------------------
                        GetMAC
 ----------------------------------------------------------------------------
������ MAC-������ ���-1


���������       ��� ������:
                0 - ��� ������,
                1 - MAC-����� �� ���������

������� ���������
Buf             ������ �� ������ � ������� ����� ���������� MAC-����� ���-1

��������
������� ������ MAC ����� �� ����������������� ������ �������� �����������.

  ---------------------------------------------------------------------------- */

WORD GetMAC(BPTR Buf)
{
BYTE i,a;
        for(i=0; i<6; i++){
                if(ReadByteEEPROM(&a,i+10))
                        return 1;
                Buf[i]=a;
                }

        return 0;
}



