extern WORD GetNetError(ERROR_NET far *error,BYTE Interface);
            // Информация об ошибках сетевого интерфейса
extern void ResetNetError(BYTE Interface);
            // Сброс ошибок сетевого интерфейса
extern WORD ReadPack(BYTE Interface,VPTR RTable,BPTR Buf,WPTR n);
            // Чтение пакета.
extern WORD WritePack(BYTE Interface,VPTR RTable,BPTR Buf,WORD n);
            // Передача пакета
extern WORD InitEthernet(void);
            // Инициализация контроллера Ethernet
extern WORD SetMAC(BPTR Buf);
            // Установка MAC-адреса МПК-1
extern WORD GetMAC(BPTR Buf);
            // Чтение MAC-адреса МПК-1



extern WORD SetIP(BPTR Buf);
extern WORD GetIP(BPTR Buf);


extern BYTE mac[6];     // MAC адрес МПК-1
