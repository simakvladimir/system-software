#define T1CON_pri	0FF5Eh
#define T1CNT_pri	0FF58h
#define EOI_pri		0FF22h

#define TIMER_INTR	0x01		// ��᪠ TIMER � IMASK

extern volatile WORD SystemTime, SystemTimeS; // ��६���� ��⥬���� ⠩���.
extern volatile DWORD time_val;// ������쭮� �६� (�����뢠���� � ������ ����᪠ am186)
extern void InitTime(void);
extern void test_time(DWORD *);

