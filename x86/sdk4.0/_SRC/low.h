#ifndef LOW

/*------------------------------------------------------------------------------
�㭪樨 ������� �஢��, � �.�. � �������⥫��
------------------------------------------------------------------------------*/
#define LOW

#define di()		__asm{cli};	// ����饭�� ���뢠���
#define ei()		__asm{sti};	// ����襭�� ���뢠���

extern void InitWDT(void);	// ���樠������ WDT
extern void EnInterrupt(WORD);  // �㭪�� ��� ࠧ�襭�� ���뢠��� �� ��᪥
extern void DiInterrupt(WORD);	// �㭪�� ��� ����饭�� ���뢠��� �� ��᪥
extern void Stop(void);		// ��⠭���� ��⥬�
extern void outpw(WORD,WORD);	// ������ ᫮�� � ���� �����-�뢮��
#pragma aux outpw = \
             "out DX,AX"        \
	     parm   [dx] [AX];   

extern WORD inpw(WORD);         // �⥭�� ᫮�� �� ���� �����-�뢮��
#pragma aux inpw = \
             "in AX,DX"    \
	     parm   [dx]  \  
	     value   [AX];

extern BYTE setvect(BYTE, void (__interrupt __far *)());//��⠭���� ��ࠡ��稪�
				//���뢠���

// ����᪠� ᥪ�� (�ந�������� ����� ���뢠���, 
// �믮������ ����᪮�� ���⪠ ���� � ࠧ�襭�� ���뢠���)

#define GUARD(a)	{			\
				di();		\
				(a);		\
				ei();		\
			}

#endif
