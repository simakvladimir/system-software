/* ****************************************************************************

	24.07.2001 API ���-1 v 1.2.2

 	�㭪樨 ��� ࠡ��� � �������
	----------------------------

extern BYTE GetModemSignal(void);	// �⥭�� �������� ᨣ�����
extern void SetDTR(BYTE DTR);           // ��⠭���� �������� ᨣ�����
extern void SendModemString(CPTR initstring);
					// ������ ��ப� ���樠����樨 � �����
extern BYTE Rsio(void);			// �⥭�� ���� �� ������
extern WORD RsioStat(void);		// �஢�ઠ ��⮢���� ���� �� ������.
extern void PurgeModem(void);		// ���⪠ �室���� ���� ������ 
extern void Wsio(BYTE c);		// ��।�� ���� ������.
extern WORD WsioStat(void);		// ��⮢����� ������ ��� �ਥ�� ���� �� ����஫���.

 ------------------------------------------------------------------------------
����: �㪨祢 �.�.

���������:
-------- ------ ----------- --------------------------------------------------- 
 ���	�����	����				���ᠭ��
-------- ------ ----------- --------------------------------------------------- 
18.08.01 1.1	�������� ������ ࠡ��� �������� �㭪権 � ᮮ⢥��⢨� � ����	
                ����஬ ०���� ࠡ��� ��᫥����⥫쭮�� ������ (MODEM, RS232,
                RS485)                                                        
20.08.01 1.2.1  ���祢 �.�. ����� ���ᨨ
22.08.01 1.2.2	�㪨祢 �.�. ���४�஢�� (RS485)
 **************************************************************************** */

#include "186es.h"
#include "mpkdefs.h"
#include "sio.h"
#include "low.h"
#include "error.h"
#include "serial.h"
#include "common.h"
#include "modem.h"
#include "timer.h"

volatile WORD Timeout=0;
volatile BYTE ri=0;//���� ��९��� ����� #RI � ��⨢��� ���ﭨ�, ���뢠����
		//�� ���饭�� � �㭪樨 GetModemSignal

/* ----------------------------------------------------------------------------
			      GetModemSignal
 ----------------------------------------------------------------------------
�⥭�� �������� ᨣ�����

�������	���ଠ�� � �������� ᨣ�����:
		��� 2 - ���ﭨ� ᨣ���� DSR
		��� 1 - ���ﭨ� ᨣ���� DCD
		��� 0 - ���ﭨ� ᨣ���� RI
�室�� ��ࠬ����	���

���ᠭ��

�㭪�� �����頥� ���祭�� �������� ᨣ�����
  ---------------------------------------------------------------------------- */
BYTE GetModemSignal(void)
{
BYTE r;
	if(!(sio_mode&SIO_DEBUG))
		if(!((sio_mode&MCNF) && !(sio_mode&RS485))){//�᫨ �� ०�� MODEM
			AddComError(COM0,1,ERROR_COM_MODE);
			return 0;
			}
		
	r=(BYTE)(inpw(P1DATA)>>13);
	__asm {cli};
	if(ri) {r |= 1; ri = 0;}
	__asm {sti};
	return r;
}

/* ----------------------------------------------------------------------------
		         SetDTR
 ----------------------------------------------------------------------------
��⠭���� �������� ᨣ�����

�������	���
�室�� ��ࠬ����	

DTR		0 - ��⠭����� DTR � ���ᨢ��� ���ﭨ�
		1 - ��⠭����� DTR  � ��⨢��� ���ﭨ�
���ᠭ��

�㭪�� �������� �ࠢ���� ᨣ����� DTR
  ---------------------------------------------------------------------------- */
extern void Blink(long);//!!!
void SetDTR(BYTE DTR) 
{
WORD r;
if(!(sio_mode&SIO_DEBUG))
	if(!((sio_mode&MCNF) && !(sio_mode&RS485))){//�᫨ �� ०�� MODEM
		AddComError(COM0,1,ERROR_COM_MODE);
		return;
		}

	__asm {cli};
	r = inpw(P1DATA);
	if ( DTR )
		outpw(P1DATA,r&0xE7F7);//+��� 2� ����� i2c
	else
		outpw(P1DATA,(r&~0x1800)|0x0008);//+��� 2� ����� i2c
	__asm {sti};
}

/* ----------------------------------------------------------------------------
                 SendModemString
 ----------------------------------------------------------------------------
������ ��ப� ���樠����樨 � �����

�������	���
�室�� ��ࠬ����	

Initstring	ASCIIZ ��ப� � AT ���������

���ᠭ��
�㭪�� �������� ������� � ����� ��᫥����⥫쭮��� AT ������
  ---------------------------------------------------------------------------- */

void SendModemString(CPTR initstring)
{
if(!(sio_mode&SIO_DEBUG))
	if(!((sio_mode&MCNF) && !(sio_mode&RS485))){//�᫨ �� ०�� MODEM
		AddComError(COM0,1,ERROR_COM_MODE);
		return;
		}
	while ( *initstring )
		Wsio(*initstring++);
}


/* ----------------------------------------------------------------------------
         		Rsio
 ----------------------------------------------------------------------------
�⥭�� ���� �� ������

�������	���⠭�� �� ������ ����

�室�� ��ࠬ����	���

���ᠭ��
�㭪�� �।�����祭� ��� �⥭�� ���� �� ������. 
�ਭ��� �� ������ ����� ���ਧ�������. 
�㭪�� Rsio() ����������, �᫨ �室��� ���� ����. 
�஢���� ����稥 ���� � ������ ����� � ������� �㭪樨 RsioStat();
  ---------------------------------------------------------------------------- */

BYTE Rsio(void)
{
	BYTE c;
	WORD time_out;
if(!(sio_mode&SIO_DEBUG))
	if(!((sio_mode&MCNF) && !(sio_mode&RS485))){//�᫨ �� ०�� MODEM
		AddComError(COM0,1,ERROR_COM_MODE);
		return 0;
		}

	if(!(sio_mode&SER)){ //�᫨ �� �맮�� �⮩ �㭪樨 ��।������� 
				//������
		//PurgeModem();
		__asm {cli};
		rCount = tCount = 0;
		__asm {sti};
		while(transmitting)ResetWDT();
		sio_mode|=SER;
		}

	time_out = SystemTime;//����饥 ���祭�� ��⥬���� ���稪�
	while ( !rCount ) {
		if((SystemTime-time_out)>=Timeout){
			AddComError(COM0,1,ERROR_COM_TIMEOUT);
			return 0;
			}
		ResetWDT(); 
		}
	__asm {cli};
	c = rBuf[rStart&rMASK];
	++rStart;
	--rCount;
	if( !rCount ) {
		outpw(SP0CT,inpw(SP0CT)|0x80);
		//����襭�� ���뢠��� �� �ਥ� ��᫥ ������ ���⪨ ����
		}
	__asm {sti};
	return c;
}


/* ----------------------------------------------------------------------------
         		RsioStat
 ----------------------------------------------------------------------------
�஢�ઠ ��⮢���� ���� �� ������.

�������		������⢮ ���⮢ � �ਥ���� ����
�室�� ��ࠬ����	���

���ᠭ��
�㭪�� ᮮ�頥� �ਪ������ �ணࠬ�� � ��⮢���� �뤠�� �� ���� ��।���� ����, �ਭ�⮣� �� ������.

  ---------------------------------------------------------------------------- */

WORD RsioStat(void)
{
if(!(sio_mode&SIO_DEBUG))
	if(!((sio_mode&MCNF) && !(sio_mode&RS485))){//�᫨ �� ०�� MODEM
		AddComError(COM0,1,ERROR_COM_MODE);
		return 0;
		}

	if(!(sio_mode&SER)){ //�᫨ �� �맮�� �⮩ �㭪樨 ��।������� 
				//������
		//PurgeModem();
		__asm {cli};
		rCount = tCount = 0;
		__asm {sti};
		while(transmitting)ResetWDT();
		sio_mode|=SER;
		}

	return rCount;
}


/* ----------------------------------------------------------------------------
         		PurgeModem
 ----------------------------------------------------------------------------
���⪠ �室���� ���� ������ 

�������		���
�室�� ��ࠬ����	���

���ᠭ��
�㭪�� 㤠��� �� �室���� � ��室���� ���� �� ����祭�� �� ������ �����.

  ---------------------------------------------------------------------------- */

void PurgeModem(void)
{
WORD w;
if(!(sio_mode&SIO_DEBUG))
	if(!((sio_mode&MCNF) && !(sio_mode&RS485))){//�᫨ �� ०�� MODEM
		AddComError(COM0,1,ERROR_COM_MODE);
		return;
		}

	__asm {cli};
	rCount = tCount = 0;
	transmitting = 0;
	w = inpw(SP0CT);
	outpw(SP0CT,w & ~0x140);// ��⠭���� ��।�� (+����� ���뢠���)
	outpw(SP0CT,w | 0x40);// ࠧ�襭�� ��।��
	__asm {sti};
}


/* ----------------------------------------------------------------------------
         		Wsio
 ----------------------------------------------------------------------------
��।�� ���� ������.

�������		���

�室�� ��ࠬ����	

�			��।������ ����

���ᠭ��
�㭪�� �।�����祭� ��� ��।�� ������ ���� ������. ��।������ ����� ���ਧ�������. �㭪�� Wsio() ����������, �᫨ ��室��� ���� ��९�����. �஢���� ����稥 ᢮������� ���� � ���� ����� � ������� �㭪樨 WsioStat();
  ---------------------------------------------------------------------------- */
void Wsio(BYTE c)
{
  while( !(inpw(SP0STS) & 0x0040) )
  {
    ResetWDT();
  }
  outpw( SP0TD, (BYTE)c );
}

/* ----------------------------------------------------------------------------
         		WsioStat
 ----------------------------------------------------------------------------
��⮢����� ������ ��� �ਥ�� ���� �� ����஫���.

�������		������⢮ ᢮������ ���⮢ � ���� ��।��.
�室�� ��ࠬ����	���

���ᠭ��
�㭪�� ᮮ�頥� �ਪ������ �ணࠬ�� � ��⮢���� ������ 
� �ਥ�� �� ����஫��� ��।���� ����.
  ---------------------------------------------------------------------------- */
WORD WsioStat(void)
{
if(!(sio_mode&SIO_DEBUG))
	if(!((sio_mode&MCNF) && !(sio_mode&RS485))){//�᫨ �� ०�� MODEM
		AddComError(COM0,1,ERROR_COM_MODE);
		return 0;
		}
	if(!(sio_mode&SER)){ //�᫨ �� �맮�� �⮩ �㭪樨 ��।������� 
				//������
		while(transmitting)ResetWDT();
		PurgeModem();
		sio_mode|=SER;
		}


	return tMAX-tCount;
}

/*---------------------------------------------------------------------------
		��ࠡ��稪 ���뢠��� �� ᨣ���� ri
  ---------------------------------------------------------------------------*/
void interrupt far ri_handle(void)
{
ri=1;
outpw(EOI,0xF);
}