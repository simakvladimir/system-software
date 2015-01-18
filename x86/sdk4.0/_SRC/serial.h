#ifndef SERIAL_
#define SERIAL_

#define MODE_MASK   0x0018  // модифицируемые биты в регистре SP0CT

#define MCNF        0x0001  // маска для режима MODEM в sio_mode
#define RS485       0x4000  //Маска для режима RS(485)
#define EARLY       0x8000  //Маска для определения версии контроллера
                //(работа в режиме RS485 на ранних платах МПК
                //отличается от поздних плат - используется
                //на уровне драйвера, 1 - ранняя версия)
#define SER     0x2000  //Флаг для определения работы драйвера в 
                //режиме MODEM (посылка пакетов/отдельных
                //байтов, 1 - байты) 
#define SIO_DEBUG   0x1000  //Если этот флаг установлен, то все 
                //последовательный канал настраивается,
                //независимо от значения внешних перемычек,
                //в режиме No HS, 8, N, 232, Modem AND Net
extern WORD GetComError(ERROR_COM far *,BYTE);
                // Информация об ошибках последовательного канала
extern void ResetComError(BYTE);
                // Сброс ошибок последовательного канала

extern WORD SetSioMode(CONFIG_COM far *ConfigCOM,BYTE Interface);
                // Установка параметров и режимов работы последовательного канала. 
extern WORD GetSioMode(CONFIG_COM far *ConfigCOM,BYTE Interface);
                // Информация о текущем режиме работы последовательного канала 

#endif //SERIAL_
