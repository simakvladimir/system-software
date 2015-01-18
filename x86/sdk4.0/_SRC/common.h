extern BYTE MPKinfo[128];	// ASCIIZ строка с информацией о системе



extern void Init(void);		// Инициализация модуля вычислителя
extern void ResetWDT(void); 	// Сброс сторожевого таймера (WatchDog-таймера).

#define GetMPKinfo() (MPKinfo)
//extern BPTR * GetMPKinfo();	// Получение информация о контроллере МПК-1     

extern WORD CRC16(BPTR,WORD,WORD);
				// Подсчет CRC
extern void GetStatMPK(STAT_MPK far *);
				// Чтение таблицы текущего состояния МПК-1
extern void UpdateStatMPK(void); 
				// Обновление информации о перемычках 
				// в таблице текущего состояния МПК-1
extern void SetLED(BYTE,BYTE);  // Управление светодиодами МПК-1
extern WORD GetID(BPTR);	// Получение уникального идентификатора МПК-1	





