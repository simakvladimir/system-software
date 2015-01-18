/* ****************************************************************************

        24.07.2001 API МПК-1 v 1.2.3

    Функции для работы с модулями расширения
    ----------------------------------------

WORD WriteMP(WORD AddrMP,WORD Addr,BPTR Buf,WORD n);
            // Запись массива данных в МР-26
WORD ReadMP(WORD AddrMP,WORD Addr,BPTR Buf,WORD n);
            // Чтение массива данных из МР-26
WORD DOUT(WORD AddrMP,BYTE d,BYTE c);
            // Управление исполнительным устройством (дискретным выходом)
WORD DIN(WORD AddrMP, BYTE d, BPTR c);
            // Чтение  датчика (дискретного входа)
WORD GetRD30(BYTE AddrMP, BYTE d, BPTR Buf);
            // Чтение  RD30         
WORD SetRD30(WORD AddrMP,BYTE d,BYTE Command,BYTE Time);
            // Управление RD30 (Изменение состояния светодиода)

Низкоуровневые функции:

WORD InitMR26(STAT_MPK *s);
            // Заполнение таблицы STAT_MPK информацией о 
            // модулях расширения

 ------------------------------------------------------------------------------
Автор: Ключев А.О.

Изменения:
-------- ------ ----------- --------------------------------------------------- 
 Дата   Версия  Автор               Описание
-------- ------ ----------- --------------------------------------------------- 
18.08.01    Ключев А.О. Доработка всех функций модуля EM. 
19.08.01    Ключев А.О. Доработка функции DIN
20.08.01 1.2.1  Ключев А.О. Исправлена функция InitMR26
20.08.01 1.2.2  Ключев А.О. Убраны лишние переменные
                Исправленая функция CheckParams 
16.09.01 1.2.3  Ключев А.О. Исправлена функция InitMR26

 **************************************************************************** */

#include "mpkdefs.h"
#include "i2c.h"
#include "em.h"
#include "i2c.h"
#include "common.h"

//DEBUG
#include "modem.h"
#include "print.h"


// Функция проверки параметров функций WriteMP, ReadMP,DOUT,DIN
WORD CheckParams(WORD AddrMP,WORD Addr, BYTE din, BYTE dout);
// Функция проверки параметров функций GetRD30 и SetRD30
WORD CheckParamsRD(WORD AddrMP,BYTE nRD30);
// Чтение МР26 без проверки параметров (используется для Init)
WORD RawReadMP(WORD AddrMP,WORD Addr,BPTR Buf,WORD n);

/* ----------------------------------------------------------------------------
                WriteMP
 ----------------------------------------------------------------------------
Запись массива данных в МР-26

Результат   Код ошибки МР26. 

Входные параметры   
AddrMP      адрес МР-26 (1xxx000d)
Addr        адрес памяти внутри MP-26
Buf     ссылка на буфер памяти в ОЗУ
N       количество записываемых байт

Описание
Функция позволяет записать массив данных в один из модулей расширения МР-26.

  ---------------------------------------------------------------------------- */

WORD WriteMP(WORD AddrMP,WORD Addr,BPTR Buf,WORD n)
{
unsigned char i;
WORD res;

    res=CheckParams(AddrMP,Addr,0,0);
    if(res!=ME_OK)  return res;
        

                StartI2C();
                if(SendByteI2C(AddrMP))     { goto Error; }
                if(SendByteI2C(Addr))       { goto Error; }

        for(i=0;i<n;i++)
                if(SendByteI2C(Buf[i])) { goto Error; }

            StopI2C();                                          

        return ME_OK;

Error:
        StopI2C();

        return ME_NODEV;

}


WORD RawWriteMP(WORD AddrMP,WORD Addr,BPTR Buf,WORD n)
{
unsigned char i;
//WORD res;

                StartI2C();
                if(SendByteI2C(AddrMP))     { goto Error; }
                if(SendByteI2C(Addr))       { goto Error; }

        for(i=0;i<n;i++)
                if(SendByteI2C(Buf[i])) { goto Error; }

            StopI2C();                                          

        return ME_OK;

Error:
        StopI2C();

        return ME_NODEV;

}

/* ----------------------------------------------------------------------------
            ReadMP
 ----------------------------------------------------------------------------
Чтение массива данных из МР-26

Результат   Код ошибки МР26.

Входные параметры   
AddrMP  адрес МР-26
Addr    адрес памяти внутри MP-26
Buf ссылка на буфер памяти в ОЗУ
N   количество записываемых байт

Описание
Функция позволяет прочитать массив данных из одного из модулей расширения МР-26.

  ---------------------------------------------------------------------------- */

WORD ReadMP(WORD AddrMP,WORD Addr,BPTR Buf,WORD n)
{
unsigned char i;
WORD r;


    r=CheckParams(AddrMP,Addr,0,0);
    if(r!=ME_OK)  return r;


    StartI2C();
    if(SendByteI2C(AddrMP))
        r = ME_NODEV;
    else
        if(SendByteI2C(Addr))
            r = ME_NODEV;
        else
        {
            StopI2C();
            StartI2C();
            if(SendByteI2C(AddrMP|I2C_READ))
                r = ME_NODEV;
            else
            {
                for(i = 0; i < (n-1); i++)
                {
                    Buf[i] = ReceiveByteI2C();
                }
                Buf[n - 1] = ReceiveByteI2Cna();
            }
        }
    StopI2C();                                          

    return r;
}

// Аналог GetRD30, без проверки параметров

WORD RawReadMP(WORD AddrMP,WORD Addr,BPTR Buf,WORD n)
{
unsigned char i;
//WORD res;


       StartI2C();
       if(SendByteI2C(AddrMP))      { goto Error; }
       if(SendByteI2C(Addr))        { goto Error; }
       StopI2C();
       StartI2C();
       if(SendByteI2C(AddrMP|I2C_READ)) { goto Error; }

        for(i=0;i<(n-1);i++)
        {
        Buf[i]=ReceiveByteI2C();
        }
    Buf[n-1]=ReceiveByteI2Cna(); 


       StopI2C();                                          

        return ME_OK;

Error:
        StopI2C();

        return ME_NODEV;


}

/* ----------------------------------------------------------------------------
                DOUT
 ----------------------------------------------------------------------------
Управление исполнительным устройством (дискретным выходом)

Результат   Код ошибки МР26.

Входные параметры   
AddrMP  Адрес МР-26
D   Номер выхода
C   Значение выхода:  не 0 - вкл., 0 - выкл.

Описание
Функция позволяет управлять исполнительным устройством модуля расширения МР-26.

  ---------------------------------------------------------------------------- */



WORD DOUT(WORD AddrMP,BYTE d,BYTE c)
{
WORD res;

    res=CheckParams(AddrMP,0,0,d);
    if(res!=ME_OK)  return res;


    if(c) d |= 0x80;
    res=WriteMP(AddrMP,0x14,&d,1);

    if(res!=ME_OK) return res; 

    return ME_OK;
}

#if 1

char writeBlock2EM(unsigned char i2caddr, unsigned char addr, char *buf, int count)
{
char r = 0; // return value: 0 - OK, 1 - error
int i;

    StartI2C();
    if(SendByteI2C(i2caddr))
        r = 1;
    else
        if(SendByteI2C(addr))
            r = 1;
        else
            for(i = 0; i < count; ++i)
                if(SendByteI2C(buf[i]))
                {
                    r = 1;
                    break;
                 }
    StopI2C();
    return r;
}

char readBlockFromEM(unsigned char i2caddr, unsigned char addr, char *buf, int count)
{
char r = 0; // return value: 0 - OK, 1 - error
int i;

    StartI2C();
    if(SendByteI2C(i2caddr))
        r = 1;
    else
        if(SendByteI2C(addr))
            r = 1;
        else
        {
            StopI2C();
            StartI2C();
            if(SendByteI2C(i2caddr|I2C_READ))
                r = 1;
            else
            {
                for(i = 0; i < count - 1; ++i)
                {
                    buf[i] = ReceiveByteI2C();
                }
                buf[i] = ReceiveByteI2Cna();
            }
        }
    StopI2C();                                          

    return r;
}


int setDOut(unsigned char em_addr, unsigned char out, int v)
{
    if(v) 
        out |= 0x80;

    return writeBlock2EM(em_addr, 0x14, &out, 1);
}

int getDIn(unsigned char em_addr, unsigned char in, int *v)
{
unsigned char b = 0;
int r;

    
    if( ( r = readBlockFromEM(em_addr, in / 8, &b, 1) ) != 0 )
        return r;

    in %= 8;
    *v = (b & (1 << in)) ? 1 : 0;
    return ME_OK;
}

#endif

/* ----------------------------------------------------------------------------
                DIN
 ----------------------------------------------------------------------------
Чтение  датчика (дискретного входа)

Результат   Код ошибки МР26.
Входные параметры   
AddrMP  адрес МР-26
D   Номер датчика
C   Ссылка на   переменную с состоянием датчика (1 - вкл., 0 - выкл.)

Описание
Функция позволяет  получить состояние датчика одного из модулей расширения МР-26.

  ---------------------------------------------------------------------------- */

static BYTE DIN_Atable[]={  0,0,0,0,0,0,0,0,
                    1,1,1,1,1,1,1,1,
                        2,2,2,2,2,2,2,2,
                    3,3 };


WORD DIN(WORD AddrMP,BYTE d,BPTR c)
{
BYTE din; 
BYTE Addr;
BYTE mask=1;
BYTE n;
WORD res;

    n=DIN_Atable[d];

    

    Addr=MP_DIN+n;



    res=CheckParams(AddrMP,Addr,d,0);
    if(res!=ME_OK)  return res;

    res=ReadMP(AddrMP,Addr,&din,1);

    if(res!=ME_OK) return res;
    
    if(n) d = d-(n<<3);

    mask=mask<<d;       

    if(din & mask) *c = 1; else *c = 0;
    return ME_OK;
}


/* ----------------------------------------------------------------------------
                    GetRD30
 ----------------------------------------------------------------------------
Чтение  RD30

WORD GetRD30(BYTE AddrMP, BYTE d, BPTR Buf)

Результат   Код ошибки МР26считывателя.
Входные параметры   
AddrMP  Адрес МР-26
D   номер RD30
Buf Ссылка на массив прочитанной информации

Описание
Функция позволяет прочитать информацию с одного из считывателей подключенных к плате МР-26Массив содержит 4-х байтовый PIN-код карточки.

  ---------------------------------------------------------------------------- */


WORD GetRD30(WORD AddrMP,BYTE d,BPTR Buf)
{

BYTE rdflag;    // флаги считывателей
WORD res;   // Результат чтения из МР26
BYTE Mask=1;    // Маска, по которой вычисляется готовность PIN кода
//int i;


    res=CheckParamsRD(AddrMP,d);
    if(res!=PIN_OK) return res;


    res=ReadMP(AddrMP,MP_RDFLG,&rdflag,1);
                    // Читаем статус считывателей

    if(res) return PIN_LINE;    // возврат при любой ошибке

                    // Получаем маску кардридера
    Mask <<=d;
    
    if(rdflag & Mask)                // Проверяем наличие PIN кода
        {

        res=ReadMP(AddrMP,MP_RDPIN+(d<<3),Buf,4);
                    // Читаем статус считывателей

        if(res) return PIN_LINE;    // возврат при любой ошибке
        
        res=WriteMP(AddrMP,MP_RDFLG,&Mask,1);
                        // Сбрасываем флаг считывателя

        if(res) return PIN_LINE;        

        return PIN_OK;          
        }
     else
        return PIN_EMPTY;               

    
    
}





/* ----------------------------------------------------------------------------
                    SetRD30
 ----------------------------------------------------------------------------

Управление RD30 (Изменение состояния светодиода)

WORD SetRD30(BYTE AddrMP, BYTE d, BYTE Command, BYTE Time)

Результат   Код ошибки МР26считывателя. 
Входные параметры   
AddrMP  Адрес МР-26
D   номер RD30
Command Команда управления считывателем
    1 - включить зеленый светодиод считывателя
    2 - включить мигающий красный светодиод считывателя (быстро)
    3 - включить мигающий красный светодиод считывателя (медленно)
    4 - включить мигающий зеленый светодиод считывателя(быстро)
    5 - включить мигающий зеленый светодиод считывателя(медленно)
    6 - включить мигающий зеленый/красный светодиод считывателя (быстро)
    7 - включить мигающий зеленый/красный светодиод считывателя (медленно)
Time    Время горения светодиода. Минимальное время горения/мигания (Time=1) - 140 мс, максимальное - (Time=255) - 34 сек. Шаг изменения времени - примерно 140 мс.

Описание

Функция позволяет записать информацию в один из считывателей 
подключенный к плате МР-26
  ---------------------------------------------------------------------------- */ 


WORD SetRD30(WORD AddrMP,BYTE d,BYTE Command,BYTE Time)
{

WORD res;   // Результат записи в МР26
BYTE com[2];


    res=CheckParamsRD(AddrMP,d);
    if(res!=PIN_OK) return res;


    com[0]=Command;
    com[1]=Time;

    res=WriteMP(AddrMP,MP_RDCON+(d<<1),&com,2);

    if(res) return PIN_LINE;        

    return PIN_OK;          

    
}


/* ----------------------------------------------------------------------------
            Функции низкого уровня
 -----------------------------------------------------------------------------*/

// Функция проверки параметров функций WriteMP, ReadMP. Сюда передается всё кроме
// буфера.


/*
typedef struct
{
    BYTE RS;    // Выбор типа вспомогательного интерфейса RS232 или RS485. 
            // 0 - RS232, 1 - RS485
    BYTE MIF;       // Выбор активного сетевого интерфейса по умолчанию 
            // 0 - Ethernet, 1 - RS232|RS485.
    BYTE MODEM; // Выбор режима работы вспомогательного интерфейса. 
            // 0 - последовательный канал работает в режиме прямой 
            // связи, 1 - последовательный канал работает в режиме 
            // модемной связи.
    BYTE LOCK;      // Запрет входа в режим управления (корректировки таблиц)
            // 1 - запрет.
    BYTE PROG;      // Принудительный вход в режим управления(корректировки таблиц))
            // 1 - принудительный вход в режим управления.
    BYTE EXT1;  // Переключение режимов: РАБОЧИЙ/ПРОГРАММАТОР FLASH
            // Перемычка опрашивается начальным
            // загрузчиком. Если EXT1==1 управление передается рабочей
            // Программе, если EXT1==0, то управление передается программатору
            // FLASH. 
    BYTE EXT2;      // Программно опрашиваемая перемычка (для  будущих расширений )
    BYTE N_MR26;    // Количество модулей МР26 обнаруженных на этапе 
            // инициализации контроллера МПК-1. 
    MR26 mr26[MAX_N_MR26];
            // Массив структур данных с описанием конфигурации установленных 
            // МР26 (см. таблицу текущего состояния МР26).
    WORD ERROR_IO;  // Содержит ненулевое значение, если есть хотя бы одна ошибка.
            // Назначение битов:
            // 0 - периферия Am186
            // 1 - Интерфейс RS232/RS485
            // 2 - Интерфейс Ethernet
            // 3- FLASH
            // 4- Часы реального времени/идентификатор
            // 5- Связь с МР26 (I2C)
            // 6..7 - зарезервировано
            // 8..15 - модули МР26
} STAT_MPK;

typedef struct
{
    BYTE AddrMR;    // Сетевой адрес модуля расширения
    BYTE Nin;   // Количество датчиков (дискретных входов)  
    BYTE Nout;      // Количество исполнительных устройств (дискретных выходов)
    BYTE N_RD30;    // Количество считывателей RD30
} MR26;

*/

WORD CheckParamsRD(WORD AddrMP,BYTE nRD30)
{
STAT_MPK s;
int i;
char f=1;
int n;

    GetStatMPK(&s);

// Проверяем корректность AddrMP

    for(i=0;i<s.N_MR26;i++)
        {
        if(AddrMP == s.mr26[i].AddrMR) { f=0; n=i;  break; }
        }
    if(f)  return PIN_ADDR; // неправильный адрес МР26
    
// Проверяем D (номер RD30)
    if(nRD30>=s.mr26[n].AddrMR)
        return PIN_N;   // неправильный адрес считывателя



    return PIN_OK;

}


WORD CheckParams(WORD AddrMP,WORD Addr, BYTE din, BYTE dout)
{
STAT_MPK s;
int i;
char f;
int n;

    GetStatMPK(&s);

// Проверяем корректность AddrMP

    f=1;
    for(i=0;i<s.N_MR26;i++)
        {
        if(AddrMP == s.mr26[i].AddrMR) { f=0; n=i; break; }
        }
    if(f)  return ME_DEV_ADDRESS; // неправильный адрес МР26
    
// Проверяем din (номер дискретного входа)
    if( din >= s.mr26[n].Nin)
        return ME_DIN;  // неправильный номер дискретного входа

// Проверяем dout (номер дискретного выхода)
    if( dout >= s.mr26[n].Nout)
        return ME_DOUT; // неправильный номер дискретного выхода

// Проверяем корректность AddrMP

    f=1;

    if( ( Addr >  MP_DIN )  && ( Addr < (MP_DIN   + 4) ) )  f=0;
    if( ( Addr >= MP_DOUT)  && ( Addr < MP_DOUT  + 4) )     f=0;
    if(   Addr == MP_DOUTCON )              f=0;
    if(   Addr == MP_RDFLG   )              f=0;
    if( ( Addr >= MP_RDPIN)  && (Addr < MP_RDPIN + 48) )    f=0;
    if( ( Addr >= MP_RDCON)  && (Addr < MP_RDCON + 12) )    f=0;

    if(f) return ME_ADDRESS;

    return ME_OK;

}


/*
#define PIN_OK      0   // Все нормально (есть PIN код)
#define PIN_EMPTY   1   // Нет PIN кода.
#define PIN_CRC     2   // Ошибка контрольной суммы при считывани карты
#define PIN_LINE    3   // Нет связи с запрашиваемым МР-26
#define PIN_ADDR    4   // Неверный адрес MP26
#define PIN_N       5   // Неверный номер считывателя

#define ME_OK       0 // Все нормально                              
#define ME_NODEV    1 // Нет связи с запрашиваемым МР-26            
#define ME_ADDRESS  2 // Неверный адрес в пространстве памяти МР26  
#define ME_SIZE     3 // слишком длинный пакет                      
#define ME_DEV_ADDRESS  4 // Неверный адрес MP26                        
#define ME_ACCESS   5 // ошибка доступа (попытка записи в память RO)
#define ME_RD       6 // Неверный номер считывателя                 
#define ME_DIN      7 // Неверный номер датчика                     
#define ME_DOUT     8 // Неверный номер исполнительного устройства  
#define ME_OTHER    9 // Ошибка от устройства MP-26                 

#define MP_DIN     0x00 // Дискретные входы
#define MP_DOUT    0x10 // Дискретные выходы
#define MP_DOUTCON 0x14 // Управление отдельными дискретными выходами
#define MP_RDFLG   0x20 // Флаги готовности считывателей
#define MP_RDPIN   0x30 // PIN коды считывателей
#define MP_RDCON   0x90 // Управление считывателями 
#define MP_CONFIG  0xA0 // Конфигурация модуля расширения

*/

WORD InitMR26(STAT_MPK *s)
{
unsigned char tBuf[5];
unsigned char ta;
int i;
char f=1;


    s->N_MR26=0;
    

    for(i=0;i<MAX_N_MR26;i++)
        {
            ta=0x80 | ( i << 1 );
        
        if(RawReadMP(ta,MP_CONFIG,tBuf,5)==0) 
            { f=0; 
              s->mr26[s->N_MR26].AddrMR = ta;
              s->mr26[s->N_MR26].Nin    = tBuf[2];
              s->mr26[s->N_MR26].Nout   = tBuf[3];
              s->mr26[s->N_MR26].N_RD30 = tBuf[4];
              s->N_MR26++;
            }
        }
    if(f) return ME_NODEV;


    return ME_OK;
}       
