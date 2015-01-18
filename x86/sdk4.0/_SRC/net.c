/* ****************************************************************************

        24.07.2001 API МПК-1 v 1.2.3


        Функции работы с сетью
        ----------------------

 ------------------------------------------------------------------------------
Автор: Лукичев А.Н.

Изменения:
-------- ------ ----------- --------------------------------------------------- 
 Дата   Версия  Автор                           Описание
-------- ------ ----------- --------------------------------------------------- 
18.08.01 1.1    Изменена логика работы сетевых функций в соответствии с новым
                набором режимов работы последовательного канала (MODEM, RS232,
                RS485)                                                        
20.08.01 1.2.1  Ключев А.О. Смена версии
22.08.01 1.2.2  Лукичев А.Н. Настройка работы с RS485
25.08.01 1.2.3  Ключев А.О. Добавление функций SetIP и GetIP

 **************************************************************************** */

#include <string.h>
#include "186es.h"
#include "mpkdefs.h"
#include "sio.h"
#include "low.h"
#include "error.h"
#include "net.h"
#include "common.h"
#include "eth.h"
#include "eeprom.h"
#include "serial.h"

BYTE mac[6] = {0x00,0x08,0x09,0x0A,0x0B,0x00};

/* ----------------------------------------------------------------------------
                GetNetError
 ----------------------------------------------------------------------------
Информация об ошибках сетевого интерфейса

Результат       Код ошибки.
                0 - нет ошибки.
                1 - подробности об ошибке см. в error_net

Error_net       Указатель на структуру типа ERROR_NET, куда записана информация
                об ошибках.

Входные параметры
Error_net       Ссылка на подготовленную прикладной программой пустую структуру типа ERROR_NET.
Interface       Номер сетевого интерфейса

Описание
Функция заполняет структуру ERROR_NET флагами ошибок последовательного канала.

  ---------------------------------------------------------------------------- */

WORD GetNetError(ERROR_NET far *error,BYTE Interface)
{
        if ( error_net[Interface&0x01].ErrorCount )
        {
                error->Error = error_net[Interface&0x01].Error;
                error->ErrorCount = error_net[Interface&0x01].ErrorCount;
                error->ErrorTr = error_net[Interface&0x01].ErrorTr;
                error->ErrorRec = error_net[Interface&0x01].ErrorRec;
                return 1;
        }
        return 0;
}


/* ----------------------------------------------------------------------------
                  ResetNetError
 ----------------------------------------------------------------------------
Сброс ошибок сетевого интерфейса

void ResetNetError(BYTE Interface)

Результат       Нет

Входные параметры
Interface       Номер сетевого интерфейса

Описание
Функция сбрасывает флаги ошибок и обнуляет структуру ERROR_NET


  ---------------------------------------------------------------------------- */

void ResetNetError(BYTE Interface)
{
        error_net[Interface&0x01].Error = 0;
        error_net[Interface&0x01].ErrorCount = 0;
        error_net[Interface&0x01].ErrorTr = 0;
        error_net[Interface&0x01].ErrorRec = 0;
        if ( Interface==SER0 )
        {
                if ( !error_com[COM0].ErrorCount && !error_com[COM1].ErrorCount )
                        stat.ERROR_IO &= ~SM_ERROR_RS;
        }
        else
                stat.ERROR_IO &= ~SM_ERROR_ETH;
}


/* ----------------------------------------------------------------------------
              ReadPack
 ----------------------------------------------------------------------------
Чтение пакета.

WORD  ReadPack(BYTE interface,VPTR  RTable, BPTR Buf , WPTR N)

Результат       Код ошибки:
                0 - нет ошибки, пакет успешно принят,
                1 - нет пакета,
                2 - см. ошибку в таблице ошибок сетевого интерфейса

Входные параметры
Interface       Номер сетевого интерфейса
RTable  Ссылка на таблицу разрешенных MAC-адресов
N       Ссылка слово, где будет записана длина полученного пакета
Buf     Ссылка на буфер, куда будет записан пакет (Максимальный размер пакета - 1500 байт, минимальный - 46 байт)

Описание
Функция возвращает  адрес и длину принятого пакета. Функция не блокируется.
  ---------------------------------------------------------------------------- */
WORD ReadPack(BYTE Interface,VPTR RTable,BPTR Buf,WPTR n)
{
        WORD i, len, crc;
        if ( Interface==SER0 )
        {
                if ( sio_mode&SER ) { //Если до вызова передавались отд. байты
                        while(transmitting)ResetWDT();
                        __asm {cli};
                        roffset = woffset = 0;
                        __asm {sti};
                        sio_mode &= ~SER;
                        return PACKET_NO;
                        }

                if ( !(sio_mode&RRDY) ) {
                        return PACKET_NO;
                        }

                // есть пакет, проверяем его "подходимость"
                __asm {cli};
                sio_mode |= RBUSY;              // блокируем буфер
                __asm {sti};

                if ( _fmemcmp((const void far*)(rpack+4),(VPTR)mac,6) ){//не тот dst MAC
                        goto fw_fail;
                        }
                // проверка разрешенных адресов отправителя
                for ( i=0; i<MAX_ENTRIES; ++i )
                        if ( !_fmemcmp((const void far*)(rpack+4+6),(VPTR)(((rTable far *)RTable)->MACaddress[i]),6) )
                                break;
                if ( i==MAX_ENTRIES )   // нет ни одного вхождения
                        goto fw_fail;

                Buf[0] = ((rTable far*)RTable)->LogicalAddress[i];
                len = *((WPTR)&rpack[16]);
                crc = *((WPTR)(rpack+4+6+6+2+len));

//              for(i=0; i<(4+6+6+2+len+2); i++) //!!!
//                      print(rpack[i]);

//              if ( CRC16((BPTR)rpack,4+6+6+2+len,0)!=crc )
//              {
//В этой версии проверка CRC16 пакета MPCRS отключена
//                      AddNetError(Interface,0,0,ERROR_NET_CRC);
//                      return PACKET_ERROR;
//              }

                _fmemcpy(Buf+1,(BPTR)(rpack+4+6+6+2),len);
                *n = len;
                roffset = 0;
                __asm {cli};
                sio_mode &= ~(RRDY|RBUSY);      // больше пакета нет и буфер свободен
                __asm {sti};

                return PACKET_OK;
        }
        else            // работа с Ethernet
        {
//      return 0;
        return EthReadPack(RTable,Buf,n);
        }
fw_fail:
        AddNetError(Interface,0,0,ERROR_NET_FIREWALL);
        roffset = 0;
        __asm {cli};
        sio_mode &= ~(RRDY|RBUSY);      // больше пакета нет и буфер свободен
        __asm {sti};
        return PACKET_ERROR;
}


/* ----------------------------------------------------------------------------
                         WritePack
 ----------------------------------------------------------------------------
Передача пакета

Результат       Код ошибки.
                0 - нет ошибки, пакет успешно передан,
                1 - буфер занят,
                2 - см. ошибку в таблице ошибок сетевого интерфейса

Входные параметры
Interface       Номер сетевого интерфейса
RTable  Ссылка на таблицу разрешенных MAC-адресов
N       Длина пакета
Buf     Ссылка на буфер, где подготовлен передаваемый пакет
        (Макс. размер пакета - 1500 байт, минимальный - 46 байт )

Описание
Функция позволяет передать пакет по одному из сетевых интерфейсов. Функция не блокируется на время передачи при работе с интерфейсом SER0 и блокируется при работе с интерфейсом ETH0. Логический адрес назначения берется из заголовка прикладного пакета и преобразуется с помощью RTable в физический (MAC) адрес назначения.

  ---------------------------------------------------------------------------- */
extern void Blink(long);//!!!
WORD WritePack(BYTE Interface,VPTR RTable,BPTR Buf,WORD n)
{
        BYTE i;
        BPTR cur;
        cur = wpack;
        // Проверка интерфейса
        if ( Interface==SER0 )
        {
                if ( sio_mode&SER ) { //Если до вызова передавались отд. байты
                        while(transmitting)ResetWDT();
                        __asm {cli};
                        roffset = woffset = 0;
                        transmitting = 0;
                        sio_mode &= ~SER;
                        __asm {sti};
                        }
                else if ( transmitting )
                        return PACKET_BUSY;

                // проверка разрешенных адресов
                for ( i=0; i<MAX_ENTRIES; ++i )
                        if(((rTable*)RTable)->LogicalAddress[i]==*Buf)
                        //Если логич. адрес назначения
                                break;//(первый байт в Buf) найден в таблице

                if ( i==MAX_ENTRIES )           // не нашли ни одного совпавшего адреса
                {
                        AddNetError(Interface,0,ERROR_NET_FIREWALL,0);
                        return PACKET_ERROR;
                }

                *((DPTR)cur) = 0x7EAA557E;      // преамбула
                cur += 4;

                _fmemcpy((BPTR)cur,(BPTR)&(((rTable*)RTable)->MACaddress[i]),6);
                cur += 6;

                _fmemcpy((BPTR)cur,(BPTR)mac,6);
                cur += 6;
                *(WORD*)cur = n;
                cur += 2;

                //
                _fmemcpy((BPTR)cur,(Buf+1),n);  // data
                *((WORD *)(cur+n)) = CRC16((BPTR)wpack,4+6+6+2+n,0);    // crc
                __asm {cli};
                transmitting = 1;
                woffset = 0;
                wlen = 4+6+6+2+n+2-1;   // длина пакета-1

                if(!(sio_mode&SIO_DEBUG))
                        if(sio_mode&RS485){//Если RS485
                                if(sio_mode&EARLY){
                                        outpw(P1DATA,(inpw(P1DATA)&~0x1800)|0x10);//+сброс 2х линий i2c
                                        //#RTS passive
                                        while(!(inpw(P1DATA)&0x20));
                                        }
                                else {
//                                      Blink(4);
                                        outpw(P1DATA,inpw(P1DATA)&~0x1810);//+сброс 2х линий i2c
                                        //#RTS active
                                        while(inpw(P1DATA)&0x20);
                                        }
                                }
                outpw(SP0TD,(WORD)(wpack[woffset++]));
                outpw(SP0CT,inpw(SP0CT)|0x100);//разрешаем прерывание на пустой буфер
                __asm {sti};
        }
        else    // работа с сетью
        {
//      return 0;
        return EthWritePack(RTable,Buf,n);
        //
        }
        return PACKET_OK;
}


/* ----------------------------------------------------------------------------
                 InitEthernet
 ----------------------------------------------------------------------------
Инициализация контроллера Ethernet

Результат       Код ошибки: 0 - нет ошибки, 1 - инициализация неудачна
Входные параметры
Описание
Функция инициализирует контроллер Ethernet и его окружение (контроллер ПДП и контроллер прерываний). Также функция приводит в исходное состояние системные переменные, относящиеся к интерфейсу ETH0. Принимаемые и передаваемые в момент инициализации пакеты - теряются. Регистры контроллера инициализируются  также как после старта МПК-1.

  ---------------------------------------------------------------------------- */

WORD InitEthernet(void)  
{
        if(GetMAC((BPTR)mac))
                return 1;//Не удается установить MAC-адрес
        return InitCS8900a();
}


/* ----------------------------------------------------------------------------
                        SetMAC
 ----------------------------------------------------------------------------
Установка MAC-адреса МПК-1

WORD SetMAC(BPTR Buf)

Результат               Код ошибки:
                        0 - нет ошибки,
                        1 - MAC адрес не установить

Входные параметры
Buf                     Ссылка на массив с MAC-адресом МПК-1

Описание
Функция записывает MAC-адрес в энергонезависимую память сетевого контроллера.

  ---------------------------------------------------------------------------- */

WORD SetMAC(BPTR Buf)
{
BYTE i;
WORD j;
long a,b=1;
        for(i=0; i<6; i++){
                if(WriteByteEEPROM(Buf[i],i+10))
                        return 1;
                for(j=0; j<4000; j++)//Задержка ~40ms
                        a=a*b;
                ResetWDT();
                }
        return 0;
}


/* ----------------------------------------------------------------------------
                        GetMAC
 ----------------------------------------------------------------------------
Чтение MAC-адреса МПК-1


Результат       Код ошибки:
                0 - нет ошибки,
                1 - MAC-адрес не прочитать

Входные параметры
Buf             Ссылка на массив в котором будет находиться MAC-адрес МПК-1

Описание
Функция читает MAC адрес из энергонезависимой памяти сетевого контроллера.

  ---------------------------------------------------------------------------- */

WORD GetMAC(BPTR Buf)
{
BYTE i,a;
        for(i=0; i<6; i++){
                if(ReadByteEEPROM(&a,i+10))
                        return 1;
                Buf[i]=a;
                }

        return 0;
}



