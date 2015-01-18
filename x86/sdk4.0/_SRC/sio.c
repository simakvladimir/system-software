/*-----------------------------------------------------------------------------
	
        	24.07.2001 API ���-1 v 1.2.2
		
                
	����� � �㭪�ﬨ ��� ࠡ��� � ��᫥����⥫�� �������
	--------------------------------------------------------

 ------------------------------------------------------------------------------
����: �㪨祢 �.�.

���������:
-------- ------ ----------- --------------------------------------------------- 
 ���	�����	����				���ᠭ��
-------- ------ ----------- --------------------------------------------------- 
18.08.01 1.1    �������� ������ ࠡ��� �������� �㭪権 � ᮮ⢥��⢨� � ����
                ����஬ ०���� ࠡ��� ��᫥����⥫쭮�� ������ (MODEM, RS232,
                RS485)                                                        
20.08.01 1.2.1  ���祢 �.�. ����� ���ᨨ
23.08.01 1.2.2	�㪨祢 �.�. ���४�஢�� �� RS485 (sio_handle)

-----------------------------------------------------------------------------*/
#include "186es.h"
#include "mpkdefs.h"
#include "sio.h"
#include "low.h"
#include "error.h"
#include "serial.h"

volatile WORD sio_mode=0;	// ��६����� ��� �࠭���� ����஥�

volatile BYTE rBuf[rMAX];	// �室��� FIFO
volatile BYTE tBuf[tMAX];	// ��室��� FIFO
volatile WORD rStart, rCount, tStart, tCount;	// 㪠��⥫� FIFO
volatile BYTE transmitting;	// �ਧ��� ⥪�饩 ��।��

volatile BYTE rpack[PACKET_SIZE];
BYTE wpack[PACKET_SIZE];
volatile WORD roffset, wlen, woffset;

// ��ࠡ��稪 ���뢠��� �� DMA1 (�ਥ����)
void interrupt far dma1_handle(void)
{
	sio_mode |= RRDY;
	outpw(SP0CT,inpw(SP0CT)&(~0xE000)); //SP0 normal receive operating
//	outpw(D1CON,(inpw(D1CON)&(~0x2))|0x4);//Stop DMA1
	InitDMA1();
	outpw(EOI,0xB);
}

extern void DSDel(WORD x);//!!!
#pragma aux DSDel = \
"	push	CX	" \
"	mov	CX,AX	" \
"delagn: adc	AX,0h	" \
"	loop	delagn	" \
"	pop	CX	" \
	parm [AX];

extern void Blink(long);
// ��ࠡ��稪 ���뢠��� ��᫥����⥫쭮�� ������ (UART)
void interrupt far sio_handle(void)
{
	WORD c, v;

	c = inpw(SP0STS);
//	Blink(2);

	if ( c&0x638 ) { //�᫨ �����㦥�� �訡�� �ਥ��
		if ( c&0x600 )	// ����ﭨ� BREAK �� ����� RxD, 
		{	// � �� ��� ���, ⠪ �� � error.ErrrType
			outpw(SP0STS,c&~0x600);// �� �����������
			if(!(sio_mode&EARLY))
				AddComError(COM0,1,ERROR_COM_OK);
		}
		if ( c&0x08 ) //�᫨ �訡�� �⭮��
		{
			outpw(SP0STS,c&~0x08);	// ���⪠ 䫠��
			AddComError(COM0,1,ERROR_COM_PARITY);
		}
		if ( c&0x20 ) //�᫨ �訡�� �३��
		{
			outpw(SP0STS,c&~0x20);	// ���⪠ 䫠��
			if(!(sio_mode&EARLY))
				AddComError(COM0,1,ERROR_COM_FRAME);
		}
		if ( c&0x10 )	// 䫠� overrun 
		{	// � �� ��� ���, ⠪ �� � error.ErrrType
			outpw(SP0STS,c&~0x10);// �� �����������
			AddComError(COM0,1,ERROR_COM_OK);
		}
	}

	if ( sio_mode&SER )	//�᫨ ������� ०��
	{
		if ( c&0x80 )
		{
			if ( rCount<rMAX )
			{
				v = inpw(SP0RD);
				rBuf[(rStart+rCount)&rMASK] = (BYTE)v;
				++rCount;
			}
			else {	// ��९������� ���� fifo
				AddComError(COM0,1,ERROR_COM_OK);
				outpw(SP0CT,inpw(SP0CT)&~0x80);
				//����� ���뢠��� �� �ਥ� �� ���⪨
				//����
				}
		}
		if ( (c&0x40) && transmitting )
		{
			if ( tCount )
			{
				outpw(SP0TD,(WORD)(tBuf[tStart&tMASK]));
				++tStart;
				--tCount;
			}
			else
			{
				transmitting = 0;
				outpw(SP0CT,inpw(SP0CT)&~0x100);
				// ��᪨�㥬 ���뢠��� �� ���⮩ ����
			}
		}
	}
	else		// ������ ०��
	{
		if ( c&0x80 ) //�� �ᯥ譮� �ਥ�� ᨬ����
		{
			v = inpw(SP0RD);
			// �� �६� ��ࠡ�⪨ ����� �� ���� ����� �������
			if ( !(sio_mode&RBUSY) )
			{
				// �ਥ� ����� � rpack[]
				switch ( roffset )
				{
				case 0:		// �ॠ��㫠
					if ( (v&0xFF)==0x7E )
					{
						rpack[0] = v;
						++roffset;
					}
					break;
				case 1:		// �ॠ��㫠
					if ( (v&0xFF)==0x55 )
					{
						rpack[1] = v;
						++roffset;
					}
					else
						roffset = 0;
					break;
				case 2:		// �ॠ��㫠
					if ( (v&0xFF)==0xAA )
					{
						rpack[2] = v;
						++roffset;
					}
					else
						roffset = 0;
					break;
				case 3:		// �ॠ��㫠
					if ( (v&0xFF)==0x7E )
					{
						rpack[3] = v;
						++roffset;
					}
					else
						roffset = 0;
					break;
				case 17:	// ����� DMA 
					rpack[roffset++] = v;
					outpw(SP0CT,inpw(SP0CT)|0xA000); 
					//SP0 receive operates through DMA1
					outpw(D1TC,0);
					outpw(D1CON,0xA36E);
					//intr, byte, source-sync
					// src: i/o, no increment 
					// dst: memory, +1 increment
					outpw(D1TC,*((WPTR)&rpack[16])+2);

					roffset = 0;
					break;
				default:	// �ਥ� MAC-���ᮢ
					rpack[roffset++] = v;
				}
			}
		}
		if ( (c&0x40) && transmitting )
		{
			if ( wlen )
			{
				outpw(SP0TD,(WORD)(wpack[woffset++]));
				--wlen;
			}
			else
			{
			if(!(sio_mode&SIO_DEBUG))
				if(sio_mode&RS485){//�᫨ RS485 
//					DSDel(1000);
					if(sio_mode&EARLY)
						outpw(P1DATA,inpw(P1DATA)&~0x1810);
						//#RTS active//+��� 2� ����� i2c
					else
						outpw(P1DATA,(inpw(P1DATA)&~0x1800)|0x10);
						//#RTS passive//+��� 2� ����� i2c
					}
				transmitting = 0;
				outpw(SP0CT,inpw(SP0CT)&~0x100);
			// ��᪨�㥬 ���뢠��� �� ���⮩ ����
			}
		}
	}
	outpw(EOI,0x14);
}

// ���樠������ ���஢ DMA ��� ����⭮�� ������.
// ����� 1 �� �⥭��
void InitDMA1()
{
BPTR ptr;
DWORD dp = (DWORD)((BPTR)rpack+18);//
WORD * p = (WORD *)&ptr;//
	outpw(D1SRCL,SP0RD);		// ���筨� SP0RD
	outpw(D1SRCH,0);
	ptr = (BPTR)rpack+18; //
	dp = p[1];  
	dp <<= 4;
	dp += p[0]; 
	p = (WORD *)&dp; 
		
	outpw(D1DSTL,p[0]);		// �ਥ���� rpack
	outpw(D1DSTH,p[1]);
}

// 0, �᫨ ����� �� ��।�����
WORD SioStatus(void)
{
	return transmitting;
}


