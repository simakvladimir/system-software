extern BYTE MPKinfo[128];	// ASCIIZ ��ப� � ���ଠ樥� � ��⥬�



extern void Init(void);		// ���樠������ ����� ���᫨⥫�
extern void ResetWDT(void); 	// ���� ��஦����� ⠩��� (WatchDog-⠩���).

#define GetMPKinfo() (MPKinfo)
//extern BPTR * GetMPKinfo();	// ����祭�� ���ଠ�� � ����஫��� ���-1     

extern WORD CRC16(BPTR,WORD,WORD);
				// ������ CRC
extern void GetStatMPK(STAT_MPK far *);
				// �⥭�� ⠡���� ⥪�饣� ���ﭨ� ���-1
extern void UpdateStatMPK(void); 
				// ���������� ���ଠ樨 � ��६�窠� 
				// � ⠡��� ⥪�饣� ���ﭨ� ���-1
extern void SetLED(BYTE,BYTE);  // ��ࠢ����� ᢥ⮤������ ���-1
extern WORD GetID(BPTR);	// ����祭�� 㭨���쭮�� �����䨪��� ���-1	





