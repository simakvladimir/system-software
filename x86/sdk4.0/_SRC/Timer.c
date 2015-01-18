/* ****************************************************************************

	26.07.2001 драйверы МПК-1 v 1.2.2


	Системный таймер, функции для измерения времени выполнения
	----------------------------------------------------------

 ------------------------------------------------------------------------------
Автор: Лукичев А.Н.

Изменения:
-------- ------ ----------- --------------------------------------------------- 
 Дата	Версия	Автор				Описание
-------- ------ ----------- --------------------------------------------------- 
18.08.01 1.1
20.08.01 1.2.1  Ключев А.О. Смена версии
23.08.01 1.2.2	Лукичев А.Н. Корректировки (timer1 не генерирует прерывания)

 **************************************************************************** */

#include "186es.h"
#include "mpkdefs.h"
#include "time.h"
#include "low.h"
#include "common.h"
#include "timer.h"
#include "modem.h"

volatile WORD SystemTime = 0, SystemTimeS = 0; // Переменные системного таймера.
volatile DWORD inc_val;

volatile DWORD time_val = 0;// глобальное время (отсчитывается с момента запуска am186)
volatile BYTE internal_time = 0;// программный prescaler

// Обработчик прерывания от TIMER0
void interrupt far timer0_handle(void)
{
WORD eoi=EOI;
	++SystemTime;	// инкремент раз в 10мс
	++internal_time;
	if ( internal_time==100 )// 100 оборотов 10мс таймера за 1 секунду
	{
		++SystemTimeS;
		internal_time = 0;
	}
//	outpw(EOI,0x08);// сброс признака прерывания EOI
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

// Функция для инициализации таймеров.
// В системе TIMER0 используется для отсчета глобального времени, а TIMER1
// просто для сквозного времени
void InitTime(void)
{
	__asm {cli};
	outpw(SYSCON,inpw(SYSCON)&~0x1000);	// не PWD
	// настройка TIMER0 - используется для отсчета времени
	outpw(T0CNT,0);		// очистка TIMER0
	outpw(T0CMPA,50000);	// максимальное значение таймера,
				//вычисляется по ф-ле period = t*10^7/2
				//(время) = value*2*10^(-7)
				// на частоте 20MHz период будет 10мс
	outpw(T0CON,0x2001);	// 0010000000000001
				// continous, generate intr
	setvect(0x08,(void (__interrupt __far *)(void))&timer0_handle);
	EnInterrupt(TIMER_INTR);		// снятие маски
	outpw(T0CON,inpw(T0CON)|0xC000);	// запуск таймера

	// настройка TIMER1 - используется для сквозного счета
	outpw(T1CNT,0);	// очистка TIMER1
	outpw(T1CMPA,0);
	outpw(T1CON,0x0001);			// 0000000000000001
						// continous
//	setvect(0x12,(void (__interrupt __far *)(void))&timer1_handle);
	outpw(T1CON,inpw(T1CON)|0xC000);	// запуск таймера
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
