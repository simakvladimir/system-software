extern BYTE GetModemSignal(void);	// �⥭�� �������� ᨣ�����
extern void SetDTR(BYTE DTR);           // ��⠭���� �������� ᨣ�����
extern void SendModemString(CPTR initstring);
					// ������ ��ப� ���樠����樨 � �����
extern BYTE Rsio(void);			// �⥭�� ���� �� ������
extern WORD RsioStat(void);		// �஢�ઠ ��⮢���� ���� �� ������.
extern void PurgeModem(void);		// ���⪠ �室���� ���� ������ 
extern void Wsio(BYTE c);		// ��।�� ���� ������.
extern WORD WsioStat(void);		// ��⮢����� ������ ��� �ਥ�� ���� �� ����஫���.

extern void interrupt RI_handle(void);//����஫� ᨣ���� RI

extern volatile WORD Timeout;	//���⥬��� ��६�����, ᮤ�ঠ��
				//⥪�饥 (��⠭��������) ���祭�� ⠩�-���
				//�⢥� �� ������

// ᮮ⢥��⢨� ��⮢ ��� ������ �� �㭪樨 GetModemSignal()
#define DSR	4
#define DCD	2
#define RI	1

	