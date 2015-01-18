extern BYTE GetModemSignal(void);	// Чтение модемных сигналов
extern void SetDTR(BYTE DTR);           // Установка модемных сигналов
extern void SendModemString(CPTR initstring);
					// Запись строки инициализации в модем
extern BYTE Rsio(void);			// Чтение байта из модема
extern WORD RsioStat(void);		// Проверка готовности байта от модема.
extern void PurgeModem(void);		// Очистка входного буфера модема 
extern void Wsio(BYTE c);		// Передача байта модему.
extern WORD WsioStat(void);		// Готовность модема для приема байта от контроллера.

extern void interrupt RI_handle(void);//Контроль сигнала RI

extern volatile WORD Timeout;	//Системная переменная, содержащая
				//текущее (установленное) значение тайм-аута
				//ответа от модема

// соответствие битов для возврата из функции GetModemSignal()
#define DSR	4
#define DCD	2
#define RI	1

	