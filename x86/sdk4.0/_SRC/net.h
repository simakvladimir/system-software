extern WORD GetNetError(ERROR_NET far *error,BYTE Interface);
            // ���ଠ�� �� �訡��� �⥢��� ����䥩�
extern void ResetNetError(BYTE Interface);
            // ���� �訡�� �⥢��� ����䥩�
extern WORD ReadPack(BYTE Interface,VPTR RTable,BPTR Buf,WPTR n);
            // �⥭�� �����.
extern WORD WritePack(BYTE Interface,VPTR RTable,BPTR Buf,WORD n);
            // ��।�� �����
extern WORD InitEthernet(void);
            // ���樠������ ����஫��� Ethernet
extern WORD SetMAC(BPTR Buf);
            // ��⠭���� MAC-���� ���-1
extern WORD GetMAC(BPTR Buf);
            // �⥭�� MAC-���� ���-1



extern WORD SetIP(BPTR Buf);
extern WORD GetIP(BPTR Buf);


extern BYTE mac[6];     // MAC ���� ���-1
