#ifndef ERROR_

/*------------------------------------------------------------------------------
Работа с ошибками последовательного канала и сети
------------------------------------------------------------------------------*/

#define ERROR_


extern ERROR_COM error_com[2];
extern ERROR_NET error_net[2];


extern void AddComError(BYTE,BYTE,BYTE);
extern void AddNetError(BYTE,BYTE,BYTE,BYTE);

// Коды для поля ERROR_COM.ErrorType
#define ERROR_COM_OK        0 // Нет ошибки.                                                   
#define ERROR_COM_TIMEOUT   1 // Таймаут.                                                      
#define ERROR_COM_PARITY    2 // Ошибка четности.                                              
#define ERROR_COM_FRAME     3 // Ошибка фрейма.                                                
#define ERROR_COM_MODE      4 // Ошибка режима: использование модемных функций в режиме MCNF=0 
                                  // или пакетных функций в режиме MCNF=1                          
#define ERROR_COM_INTERFACE 5 // Запрос недопустимого интерфейса (COM1)                        


// Коды для поля ERROR_NET.ErrorTr (ERROR_NET.ErrorRec)
#define ERROR_NET_OK        0 // Нет ошибки                                                                  
#define ERROR_NET_COLLISION 1 // Коллизии (для IEEE 802.3 (Ethernet))                                        
#define ERROR_NET_JABBER    2 // Зафиксирован Jabber (для IEEE 802.3 (Ethernet) коллизии длятся дольше 26 мс)
#define ERROR_NET_FULL      3 // Буфер передачи полон                                                        
#define ERROR_NET_FIREWALL  4 // Запрещение от таблицы разрешенных адресов                                   
#define ERROR_NET_LEN       5 // Ошибочная длина пакета                                                      


#define ERROR_NET_TIMEOUT   1 // Таймаут.                                                    
#define ERROR_NET_CRC       2 // Ошибка CRC.                                                 
#define ERROR_NET_MODE      3 // Ошибка режима. Использование сетевых функций в режиме MCNF=1
//#define ERROR_NET_FIREWALL    4 // Запрещение от таблицы разрешенных адресов                   
#define ERROR_NET_LACK      5 // принят пакет короче 64 байт (Ethernet)                      
#define ERROR_NET_SIZE      6 // принят пакет длиннее 1518 байт                              

#endif
