/* ****************************************************************************

    24.07.2001 API МПК-1 v 1.2.1


    Функции для работы с последовательным каналом (RS232/RS485
    ----------------------------------------------------------

extern WORD GetComError(ERROR_COM far *,BYTE);
                // Информация об ошибках последовательного канала
extern void ResetComError(BYTE);
                // Сброс ошибок последовательного канала

extern WORD SetSioMode(CONFIG_COM far *ConfigCOM,BYTE Interface);
                // Установка параметров и режимов работы последовательного канала. 
extern WORD GetSioMode(CONFIG_COM far *ConfigCOM,BYTE Interface);
                // Информация о текущем режиме работы последовательного канала 

 ------------------------------------------------------------------------------
Автор: Лукичев А.Н.

Изменения:
-------- ------ ----------- --------------------------------------------------- 
 Дата   Версия  Автор               Описание
-------- ------ ----------- --------------------------------------------------- 
18.08.01 1.1    Изменена логика работы модемных функций в соответствии с новым
                набором режимов работы последовательного канала (MODEM, RS232,
                RS485)                                                        
20.08.01 1.2.1  Ключев А.О. Смена версии

 **************************************************************************** */


#include "mpkdefs.h"
#include "186es.h"
#include "low.h"
#include "error.h"
#include "sio.h"
#include "modem.h"
#include "serial.h"
#include "common.h"

/* ----------------------------------------------------------------------------
                GetComError
 ----------------------------------------------------------------------------
 Информация об ошибках последовательного канала

 Результат  Код ошибки.  
        0 - нет ошибки, 
        1 - подробности об ошибке см. в описании таблицы 
        ошибок последовательного канала.

 Входные параметры  
         Error_com  Ссылка на подготовленную прикладной 
                программой пустую структуру типа ERROR_COM.

         Interface  Номер интерфейса   
                0 - COM0
                1 - COM1 

 Описание
 Функция заполняет структуру ERROR_COM флагами ошибок последовательного 
 канала из системной таблицы ошибок последовательного канала. 
  ---------------------------------------------------------------------------- */

WORD GetComError(ERROR_COM far *error,BYTE Interface)
{
    if ( error_com[Interface&0x01].ErrorCount )
    {
        error->Error = error_com[Interface&0x01].Error;
        error->ErrorCount = error_com[Interface&0x01].ErrorCount;
        error->ErrorType = error_com[Interface&0x01].ErrorType;
        return 1;
    }
    return 0;
}

/* ----------------------------------------------------------------------------
                      ResetComError
 ----------------------------------------------------------------------------
Сброс ошибок последовательного канала

Результат   Нет
Входные параметры   
        Interface   Номер интерфейса  
                0 - COM0
                1 - COM1 
Описание

Функция сбрасывает флаги ошибок и обнуляет 
системную таблицу ошибок последовательного канала. 

  ---------------------------------------------------------------------------- */

void ResetComError(BYTE Interface)
{
    error_com[Interface&0x01].Error = 0;
    error_com[Interface&0x01].ErrorCount = 0;
    error_com[Interface&0x01].ErrorType = 0;
    // не осталось ни одной ошибки от последовательного канала
    if ( !error_com[COM0].ErrorCount && !error_com[COM1].ErrorCount && !error_net[SER0].ErrorCount )
        stat.ERROR_IO &= ~SM_ERROR_RS;
}

/* ----------------------------------------------------------------------------
            SetSioMode
 ----------------------------------------------------------------------------
Установка параметров и режимов работы последовательного канала. 

Результат   0 - нет ошибок, 
        1 - ошибка установки режима
    
Входные параметры   

Config_COM  Ссылка на структуру данных содержащую параметры последовательного канала.
Interface   Номер интерфейса  0 - COM01 - COM1 

Описание

Функция позволяет изменить параметры последовательного канала 
  ---------------------------------------------------------------------------- */
WORD SetSioMode(CONFIG_COM far *ConfigCOM,BYTE Interface)
{
    WORD n;
    if ( Interface!=COM0 )          // поддерживается только COM0
    {
        AddComError(Interface,0,ERROR_COM_INTERFACE);
        return 1;
    }

    __asm {cli};
    outpw(SP0CT,inpw(SP0CT)&~0x0060);   // на момент настройки отключаем TMODE, RMODE

    outpw(SP0STS,0);

    // настройка линий TX, RX, CTS, RTR
        outpw(P1MODE,inpw(P1MODE)&~0x0030);
        outpw(P1DIR,inpw(P1DIR)&~0x0030);

    // установка прерывания UART
    setvect(0x14,(void (__interrupt __far *)())&sio_handle);
    EnInterrupt(0x0400);        // разрешение прерывания

    // установка прерывания DMA
    setvect(0xB,(void (__interrupt __far *)())&dma1_handle);
    EnInterrupt(0x08);          // снятие маски

    //установка прерывания RI
    setvect(0xF,(void (__interrupt __far *)())&RI_handle);
    EnInterrupt(0x80);      // снятие маски

    // настройка SIO    
    UpdateStatMPK();

    if(!(sio_mode&SIO_DEBUG)){
        sio_mode &= ~(MCNF|RS485);
//        sio_mode |= (stat.RS==SM_RS485)?RS485:0;
//        sio_mode |= (stat.MODEM==SM_MODEM)?MCNF:0;
        sio_mode |= MCNF;
        }
    else sio_mode&=(SIO_DEBUG|EARLY);

    rStart = rCount = tStart = tCount = 0;  // сброс буферов FIFO
    transmitting = 0;
    sio_mode &= ~(RRDY|RBUSY);
    wlen = woffset = roffset = 0;

    n = 0x10E0;         // RSIE, RXIE, TMODE, RMODE
    if(!(sio_mode&SIO_DEBUG)){
        n |= ConfigCOM->Mode&MODE_MASK; //Определение четности
        if(ConfigCOM->Mode&0x0002)  //Если 7 бит данных
            if(ConfigCOM->Mode&0x8) //Если контроль четности включен
                n |= 0x1;   //SP0 mode 1
            else
                n |= 0x4;   //SP0 mode 4
        else                            //Если 8 бит данных
            if(ConfigCOM->Mode&0x8) //и контроль четности включен
                n |= 0x3;
            else 
                n |= 0x1;
    

        //Установка HS 
        if(sio_mode&MCNF){//Если режим MODEM
            n|=0x200;//Вкл. HS 
            outpw(AUXCON,inpw(AUXCON) & ~0x18);//CTS/RTR
//            outpw(AUXCON,(inpw(AUXCON)&~0x10)|0x8);//DTE
            }
        outpw(P1MODE,inpw(P1MODE)&~0x30);//CTS/RTR normal
        outpw(P1DIR,inpw(P1DIR)&~0x30);
        outpw(P1DATA,inpw(P1DATA)&~0x1C00);//Установка #RS232+сброс 2х линий i2c
        }
    else n|=0x1; //SioMode 1
    
    outpw(SP0CT,n);
    outpw(SP0BAUD,ConfigCOM->Baud);

    Timeout = ConfigCOM->Timeout;   //Установка тайм-аута ответа от модема

    ResetComError(Interface);       // очистка ошибок

    InitDMA1();
    __asm {sti};

    return 0;
}

/* ----------------------------------------------------------------------------
                     GetSioMode
 ----------------------------------------------------------------------------
Информация о текущем режиме работы последовательного канала 


Результат   0 - нет ошибок, 1 - ошибка

Config_COM  Ссылка на структуру данных, в которую записаны параметры последовательного канала.

Входные параметры   

Config_COM  Ссылка на структуру данных подготовленную прикладной программой для записи параметров последовательного канала.
Interface   Номер интерфейса  0 - COM01 - COM1  

Описание
Функция позволяет получить параметры последовательного канала. 
Информация о параметрах копируется в структуру предоставляемую 
пользовательской программой. 
 ---------------------------------------------------------------------------- */

WORD GetSioMode(CONFIG_COM far *ConfigCOM,BYTE Interface)
{
    if ( Interface!=COM0 )
    {
        AddComError(Interface,0,ERROR_COM_INTERFACE);
        return 1;
    }
    ConfigCOM->Mode = inpw(SP0CON)&MODE_MASK;
    switch(inpw(SP0CON)&0xF){
    case 0x9:
    case 0x4: ConfigCOM->Mode |= 0x2;
        }
    ConfigCOM->Baud = inpw(SP0BAUD);
    ConfigCOM->Timeout = Timeout;
    return 0;
}

