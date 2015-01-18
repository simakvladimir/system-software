/* ****************************************************************************

    24.07.2001 API МПК-1 v 1.2.1


    Функции для работы с FLASH памятью
    ----------------------------------


WORD EraseSector(DWORD);        //Стирание сектора
WORD GetStatusFLASH(void);      //Проверка окончания процесса стирания сектора
WORD WriteFLASH(DWORD,BPTR,WORD);   //Запись массива памяти во FLASH


Примечания для платы прототипа:

Работа с FLASH специфична для МПК-1. FLASH работает в словном режиме, причем
команды FLASH даются как для байтового режима.

 ------------------------------------------------------------------------------
Автор: Лукичев А.Н.

Изменения:
-------- ------ ----------- --------------------------------------------------- 
 Дата   Версия  Автор               Описание
-------- ------ ----------- --------------------------------------------------- 
18.08.01 1.1
20.08.01 1.2.1  Ключев А.О. Смена версии

 **************************************************************************** */

#include "mpkdefs.h"
#include "flash.h"
#include "low.h"
#include "common.h"

WPTR CurrentAddr;       // адрес для опроса готовности стирания/записи

DWORD base = 0x80000;
                // базовое смещение FLASH в памяти

WORD WriteWordFLASH(WPTR Addr,WORD data);
                // Функция записи одного слова во FLASH
WPTR ConvTLA(DWORD l);//Функция преобразования линейного адреса в 
            //выровненный по параграфу указатель


/* ----------------------------------------------------------------------------
                    EraseSector
 ----------------------------------------------------------------------------
Стирание сектора
WORD EraseSector(DWORD Addr)
Результат   Код ошибки: 
        FLASH_OK - стирание сектора успешно завершено, 
        FLASH_BUSY - идет стирание, 
        FLASH_ERROR - ошибка стирания.

Входные параметры   
Addr    адрес из данного сектора FLASH памяти, TLA.

Описание
Функция стирает один сектор FLASH памяти. Функция не блокируется.

  ---------------------------------------------------------------------------- */

WORD EraseSector(DWORD Addr)
{
    //Конвертирование TLA в нормальный
    Addr &= 0xFFFFFFFE;// адрес в словах
    CurrentAddr=ConvTLA(base+Addr);

    if(GetStatusFLASH()==FLASH_BUSY) return FLASH_BUSY;

    if( (Addr<=0x0FFFEl) || (Addr>=0x7C000) )
        return FLASH_ERROR; //Эти секторы стирать запрещено
    __asm {
        push    ax
        push    ds
        push    si

        mov ax,8000h    ;Начало FLASH
        mov ds,ax
        mov ds:[0AAAh], word ptr 0AAh
        mov ds:[554h], word ptr 55h
        mov ds:[0AAAh], word ptr 80h
        mov ds:[0AAAh], word ptr 0AAh
        mov ds:[554h], word ptr 55h
        mov ax, seg CurrentAddr
        mov ds, ax
        lds si, CurrentAddr
        mov ds:[si], word ptr 30h

        pop si
        pop ds
        pop ax
        };

    return GetStatusFLASH();
}

/* ----------------------------------------------------------------------------
                    GetStatusFLASH
 ----------------------------------------------------------------------------
Проверка окончания процесса стирания сектора

Результат   Код ошибки: 
        FLASH_OK - стирание сектора успешно завершено, 
        FLASH_BUSY - идет стирание, 
        FLASH_ERROR - ошибка стирания.

Входные параметры   Нет

Описание
Функция позволяет проверить состояние процесса стирания сектора FLASH памяти. 

  ---------------------------------------------------------------------------- */
WORD GetStatusFLASH(void)
{
WORD w1,w2;

    w1 = 0;
    w2 = 0;

    __asm {
        push    ds
        push    si
        push    ax

        lds si,CurrentAddr
        mov ax,ds:[si]
        mov w1,ax
        mov ax,ds:[si]
        mov w2,ax

        pop ax
        pop si
        pop ds
        };

    if((w1&0x40)^(w2&0x40)) {//Бит 6 инвертируется?
        if(w2&0x20){ //Бит 5 установлен?
            __asm {
                push    ds
                push    si
                push    ax
        
                lds si,CurrentAddr
                mov ax,ds:[si]
                mov w1,ax
                mov ax,ds:[si]
                mov w2,ax
        
                pop ax
                pop si
                pop ds
                };
            if((w1&0x40)^(w2&0x40)) {//Бит 6 инвертируется?
                w1 = (WORD)(base>>4);
                __asm {
                    push    ds
                    push    si
                    push    ax
        
                    mov ax,w1
                    mov ds,ax
                    xor si,si
                    mov ax,0F0h //Возвращение чипа к
                    mov ds:[si],ax//режиму чтения данных
        
                    pop ax
                    pop si
                    pop ds
                    };
                stat.ERROR_IO |= SM_ERROR_FLASH;
                return FLASH_ERROR;//Ошибка
                }
            else //Нет - операция закончилась успешно
                return FLASH_OK;
            }
        else //Нет - операция выполняется
            return FLASH_BUSY;
        }
    else    
        return FLASH_OK;//Нет - операция выполнена
}


/* ----------------------------------------------------------------------------
                WriteFLASH
 ----------------------------------------------------------------------------
Запись массива памяти во FLASH

Результат   Код ошибки: 
        FLASH_OK - нет ошибки,
        FLASH_BUSY - идет процедура стирания/записи
        FLASH_ERROR - ошибка записи

Входные параметры   
Addr        адрес FLASH памяти
Buf     ссылка на буфер памяти в ОЗУ 
N       количество записываемых байт (до 65535 байт)

Описание
Функция позволяет записать массив во FLASH память. Функция блокируется на время записи.

  ---------------------------------------------------------------------------- */

WORD WriteFLASH(DWORD Addr,BPTR Buf,WORD n)
{
    WORD a, i;
    WPTR adr;
    if ( !n )
        return FLASH_OK;
    a=GetStatusFLASH();
    if(a) return a;
    Addr+=base;
    if ( Addr&1 ){  //Если нечетный адрес, то первый байт записываем
            //по четному адресу как слово, мл. часть к-рого 0xFF
        a = Buf[0];
        a<<=8;
        a |= 0xFF;
        if(WriteWordFLASH( ConvTLA(Addr & (~1)),a ))
            return FLASH_ERROR;
        Buf++, n--, Addr++;
        }
    adr = ConvTLA(Addr);
    for ( i=0; i<n; i+=2 ){ // основной цикл
        if((i+1)==n)
            a = Buf[i] | 0xFF00;
        else
            a = *((WPTR)(Buf+i));
        if(WriteWordFLASH(adr, a))
            return FLASH_ERROR;
        if(!(((DWORD)(adr+1))&0xF))
            adr = ConvTLA(Addr+i+2);
        else 
            adr++;
        }
    return FLASH_OK;
}

/* ----------------------------------------------------------------------------
            Функции низкого уровня
 ---------------------------------------------------------------------------- */


// Функция для записи слова во FLASH
// Addr - адрес записываемого слова. Адрес четный.
// data - записываемое слово
// Функция возвращает 0 - записть поршла успешно
//                    1 - ошибка записи

WORD WriteWordFLASH(WPTR Addr,WORD data)
{
    WORD a;
    if(GetStatusFLASH())return 1;//В данный момент операция с FLASH выполняется
    CurrentAddr=Addr;
    __asm {
        push    ax
        push    ds
        push    si

        mov ax,8000h
        mov ds,ax
        mov ds:[0AAAh], word ptr 0AAh
        mov ds:[554h], word ptr 55h
        mov ds:[0AAAh], word ptr 0A0h
        mov ax,seg CurrentAddr
        mov ds,ax
        lds si,CurrentAddr
        mov ax,data
        mov ds:[si], ax

        pop si
        pop ds
        pop di
        };
    while ( 1 ){
        a = GetStatusFLASH();
        if (!a)
            return 0;
        if (a==1)
            continue;
        return 1;
        }
}

WPTR ConvTLA(DWORD l)
{
BYTE ch;
union {
    DWORD dword;
    VPTR ptr;
    WORD seg_ofs[2];
    BYTE a[4];
    } TLA;
//Преобразование TLA из просто числа в выровненный указатель
TLA.dword=l;
ch=TLA.a[0]&0xF;//Смещение <= 16
TLA.seg_ofs[1]=(WORD)(TLA.dword>>4);
TLA.seg_ofs[0]=(WORD)ch;
return TLA.ptr;
}

