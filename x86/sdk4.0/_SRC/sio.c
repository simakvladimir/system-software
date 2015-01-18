/*-----------------------------------------------------------------------------
	
        	24.07.2001 API МПК-1 v 1.2.2
		
                
	Модуль с функциями для работы с последовательным каналом
	--------------------------------------------------------

 ------------------------------------------------------------------------------
Автор: Лукичев А.Н.

Изменения:
-------- ------ ----------- --------------------------------------------------- 
 Дата	Версия	Автор				Описание
-------- ------ ----------- --------------------------------------------------- 
18.08.01 1.1    Изменена логика работы модемных функций в соответствии с новым
                набором режимов работы последовательного канала (MODEM, RS232,
                RS485)                                                        
20.08.01 1.2.1  Ключев А.О. Смена версии
23.08.01 1.2.2	Лукичев А.Н. Корректировки по RS485 (sio_handle)

-----------------------------------------------------------------------------*/
#include "186es.h"
#include "mpkdefs.h"
#include "sio.h"
#include "low.h"
#include "error.h"
#include "serial.h"

volatile WORD sio_mode=0;	// переменная для хранения настроек

volatile BYTE rBuf[rMAX];	// входной FIFO
volatile BYTE tBuf[tMAX];	// выходной FIFO
volatile WORD rStart, rCount, tStart, tCount;	// указатели FIFO
volatile BYTE transmitting;	// признак текущей передачи

volatile BYTE rpack[PACKET_SIZE];
BYTE wpack[PACKET_SIZE];
volatile WORD roffset, wlen, woffset;

// Обработчик прерывания от DMA1 (приемник)
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
// Обработчик прерываний последовательного канала (UART)
void interrupt far sio_handle(void)
{
	WORD c, v;

	c = inpw(SP0STS);
//	Blink(2);

	if ( c&0x638 ) { //Если обнаружена ошибка приема
		if ( c&0x600 )	// Состояние BREAK на линии RxD, 
		{	// в ТЗ его нет, так что в error.ErrrType
			outpw(SP0STS,c&~0x600);// не заполоняется
			if(!(sio_mode&EARLY))
				AddComError(COM0,1,ERROR_COM_OK);
		}
		if ( c&0x08 ) //Если ошибка четности
		{
			outpw(SP0STS,c&~0x08);	// очистка флага
			AddComError(COM0,1,ERROR_COM_PARITY);
		}
		if ( c&0x20 ) //Если ошибка фрейма
		{
			outpw(SP0STS,c&~0x20);	// очистка флага
			if(!(sio_mode&EARLY))
				AddComError(COM0,1,ERROR_COM_FRAME);
		}
		if ( c&0x10 )	// флаг overrun 
		{	// в ТЗ его нет, так что в error.ErrrType
			outpw(SP0STS,c&~0x10);// не заполоняется
			AddComError(COM0,1,ERROR_COM_OK);
		}
	}

	if ( sio_mode&SER )	//Если модемный режим
	{
		if ( c&0x80 )
		{
			if ( rCount<rMAX )
			{
				v = inpw(SP0RD);
				rBuf[(rStart+rCount)&rMASK] = (BYTE)v;
				++rCount;
			}
			else {	// переполнение буфера fifo
				AddComError(COM0,1,ERROR_COM_OK);
				outpw(SP0CT,inpw(SP0CT)&~0x80);
				//Запрет прерывания на прием до очистки
				//буфера
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
				// маскируем прерывание на пустой буфер
			}
		}
	}
	else		// пакетный режим
	{
		if ( c&0x80 ) //При успешном приеме символа
		{
			v = inpw(SP0RD);
			// во время обработки пакета все новые данные теряются
			if ( !(sio_mode&RBUSY) )
			{
				// прием пакета в rpack[]
				switch ( roffset )
				{
				case 0:		// преамбула
					if ( (v&0xFF)==0x7E )
					{
						rpack[0] = v;
						++roffset;
					}
					break;
				case 1:		// преамбула
					if ( (v&0xFF)==0x55 )
					{
						rpack[1] = v;
						++roffset;
					}
					else
						roffset = 0;
					break;
				case 2:		// преамбула
					if ( (v&0xFF)==0xAA )
					{
						rpack[2] = v;
						++roffset;
					}
					else
						roffset = 0;
					break;
				case 3:		// преамбула
					if ( (v&0xFF)==0x7E )
					{
						rpack[3] = v;
						++roffset;
					}
					else
						roffset = 0;
					break;
				case 17:	// запуск DMA 
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
				default:	// прием MAC-адресов
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
				if(sio_mode&RS485){//Если RS485 
//					DSDel(1000);
					if(sio_mode&EARLY)
						outpw(P1DATA,inpw(P1DATA)&~0x1810);
						//#RTS active//+сброс 2х линий i2c
					else
						outpw(P1DATA,(inpw(P1DATA)&~0x1800)|0x10);
						//#RTS passive//+сброс 2х линий i2c
					}
				transmitting = 0;
				outpw(SP0CT,inpw(SP0CT)&~0x100);
			// маскируем прерывание на пустой буфер
			}
		}
	}
	outpw(EOI,0x14);
}

// Инициализация буферов DMA для пакетного обмена.
// канал 1 на чтение
void InitDMA1()
{
BPTR ptr;
DWORD dp = (DWORD)((BPTR)rpack+18);//
WORD * p = (WORD *)&ptr;//
	outpw(D1SRCL,SP0RD);		// источник SP0RD
	outpw(D1SRCH,0);
	ptr = (BPTR)rpack+18; //
	dp = p[1];  
	dp <<= 4;
	dp += p[0]; 
	p = (WORD *)&dp; 
		
	outpw(D1DSTL,p[0]);		// приемник rpack
	outpw(D1DSTH,p[1]);
}

// 0, если пакет не передается
WORD SioStatus(void)
{
	return transmitting;
}


