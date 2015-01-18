/* ****************************************************************************

	26.07.2001 �ࠩ���� ���-1 v 1.2.2


	���⥬�� ⠩���, �㭪樨 ��� ����७�� �६��� �믮������
	----------------------------------------------------------

 ------------------------------------------------------------------------------
����: �㪨祢 �.�.

���������:
-------- ------ ----------- --------------------------------------------------- 
 ���	�����	����				���ᠭ��
-------- ------ ----------- --------------------------------------------------- 
18.08.01 1.1
20.08.01 1.2.1  ���祢 �.�. ����� ���ᨨ
23.08.01 1.2.2	�㪨祢 �.�. ���४�஢�� (timer1 �� �������� ���뢠���)

 **************************************************************************** */

#include "186es.h"
#include "mpkdefs.h"
#include "time.h"
#include "low.h"
#include "common.h"
#include "timer.h"
#include "modem.h"

volatile WORD SystemTime = 0, SystemTimeS = 0; // ��६���� ��⥬���� ⠩���.
volatile DWORD inc_val;

volatile DWORD time_val = 0;// ������쭮� �६� (�����뢠���� � ������ ����᪠ am186)
volatile BYTE internal_time = 0;// �ணࠬ��� prescaler

// ��ࠡ��稪 ���뢠��� �� TIMER0
void interrupt far timer0_handle(void)
{
WORD eoi=EOI;
	++SystemTime;	// ���६��� ࠧ � 10��
	++internal_time;
	if ( internal_time==100 )// 100 ����⮢ 10�� ⠩��� �� 1 ᥪ㭤�
	{
		++SystemTimeS;
		internal_time = 0;
	}
//	outpw(EOI,0x08);// ��� �ਧ���� ���뢠��� EOI
        __asm {
		push	dx
		push	ax
		mov	dx,eoi
		mov	ax,8h
		out	dx,ax
		pop	ax
		pop	dx
		}
}

/*void interrupt far timer1_handle(void)
{
	__asm{
		push	dx
		push	ax
		mov 	DX, T1CON_pri
		in 	AX, DX
		and 	AX, 0FFDFh	; not 20h
		out 	DX, AX
		mov 	DX, EOI_pri
		mov 	AX, 9
		out 	DX, AX
		pop	ax
		pop	dx
	}
	inc_val += 6554l;//!!!
}*/

// �㭪�� ��� ���樠����樨 ⠩��஢.
// � ��⥬� TIMER0 �ᯮ������ ��� ����� ������쭮�� �६���, � TIMER1
// ���� ��� ᪢������ �६���
void InitTime(void)
{
	__asm {cli};
	outpw(SYSCON,inpw(SYSCON)&~0x1000);	// �� PWD
	// ����ன�� TIMER0 - �ᯮ������ ��� ����� �६���
	outpw(T0CNT,0);		// ���⪠ TIMER0
	outpw(T0CMPA,50000);	// ���ᨬ��쭮� ���祭�� ⠩���,
				//�������� �� �-�� period = t*10^7/2
				//(�६�) = value*2*10^(-7)
				// �� ���� 20MHz ��ਮ� �㤥� 10��
	outpw(T0CON,0x2001);	// 0010000000000001
				// continous, generate intr
	setvect(0x08,(void (__interrupt __far *)(void))&timer0_handle);
	EnInterrupt(TIMER_INTR);		// ��⨥ ��᪨
	outpw(T0CON,inpw(T0CON)|0xC000);	// ����� ⠩���

	// ����ன�� TIMER1 - �ᯮ������ ��� ᪢������ ���
	outpw(T1CNT,0);	// ���⪠ TIMER1
	outpw(T1CMPA,0);
	outpw(T1CON,0x0001);			// 0000000000000001
						// continous
//	setvect(0x12,(void (__interrupt __far *)(void))&timer1_handle);
	outpw(T1CON,inpw(T1CON)|0xC000);	// ����� ⠩���
	__asm {sti};
}


#include "serial.h"
#include "sio.h"

void prepare_param(void)
{
	CONFIG_COM cfg = {1,BAUD_57600,0};
	SetSioMode((CONFIG_COM far *)&cfg,COM0);
}


void call_func(void)
{
	Wsio('0');
}
