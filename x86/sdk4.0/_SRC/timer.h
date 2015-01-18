#define T1CON_pri	0FF5Eh
#define T1CNT_pri	0FF58h
#define EOI_pri		0FF22h

#define TIMER_INTR	0x01		// маска TIMER в IMASK

extern volatile WORD SystemTime, SystemTimeS; // Переменные системного таймера.
extern volatile DWORD time_val;// глобальное время (отсчитывается с момента запуска am186)
extern void InitTime(void);
extern void test_time(DWORD *);

