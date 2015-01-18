#ifndef LOW

/*------------------------------------------------------------------------------
Функции низкого уровня, в т.ч. и дополнительные
------------------------------------------------------------------------------*/
#define LOW

#define di()		__asm{cli};	// Запрещение прерываний
#define ei()		__asm{sti};	// Разрешение прерываний

extern void InitWDT(void);	// Инициализация WDT
extern void EnInterrupt(WORD);  // Функция для разрешения прерываний по маске
extern void DiInterrupt(WORD);	// Функция для запрещения прерываний по маске
extern void Stop(void);		// Остановка системы
extern void outpw(WORD,WORD);	// Запись слова в порт ввода-вывода
#pragma aux outpw = \
             "out DX,AX"        \
	     parm   [dx] [AX];   

extern WORD inpw(WORD);         // Чтение слова из порта ввода-вывода
#pragma aux inpw = \
             "in AX,DX"    \
	     parm   [dx]  \  
	     value   [AX];

extern BYTE setvect(BYTE, void (__interrupt __far *)());//Установка обработчика
				//прерывания

// Критическая секция (производится запрет прерываний, 
// выполнение критического участка кода и разрешение прерываний)

#define GUARD(a)	{			\
				di();		\
				(a);		\
				ei();		\
			}

#endif
