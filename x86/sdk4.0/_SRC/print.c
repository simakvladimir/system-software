/* ****************************************************************************

        24.07.2001 API МПК-1 v 1.2.1

            Тестовый ввод-вывод
            -------------------

 ------------------------------------------------------------------------------
Автор: Ключев А.О.

Изменения:
-------- ------ ----------- --------------------------------------------------- 
 Дата   Версия  Автор               Описание
-------- ------ ----------- --------------------------------------------------- 
20.08.01 1.2.1  Ключев А.О. Смена версии
    
***************************************************************************** */


//#include "global.def"
#include "186es.h"
#include "mpkdefs.h"
#include "sio.h"
#include "low.h"
#include "error.h"
#include "serial.h"
#include "modem.h"
#include "print.h"
#include "vcom.h"

char unsigned Buf[9];
char * Str1;
char Radix=10;
char Width=5;
char iWidth;
char WidthFloat=2; 
char Direction=PRINT_COM;
#define SPRINTFMAX  8

void sprintf (unsigned short x)
{
char i,index,cTmp;

    if(iWidth>SPRINTFMAX) Width=8;  

     index=iWidth;
     Buf[index]=0;  
     for(i=0;i<iWidth;i++)
     {
        if(x) cTmp = x % Radix; else cTmp=0;
        if(cTmp>9) cTmp += 0x37; else cTmp += 0x30;
        Buf[--index]=cTmp;
        if(x) x/=Radix;
     }  
}


void print(unsigned short Val)
{
//char * Str;

    iWidth=Width;   

    sprintf(Val); 
    Str1=Buf;

        while(*Str1)
                { 
        if(Direction & PRINT_COM) Wsio( *Str1);
        if(Direction & PRINT_I2C) Wsio2(*Str1);
        Str1++;
        }

    

}

void puts(const char * Str)
{
        while(*Str)
                {
        if(Direction & PRINT_COM) Wsio( *Str);
        if(Direction & PRINT_I2C) Wsio2(*Str);
        Str++;
        }
}

void putc(char sym)
{
    if(Direction & PRINT_COM) Wsio( sym);
    if(Direction & PRINT_I2C) Wsio2(sym);
        
}

unsigned char getc(void)
{
    if(Direction & PRINT_COM) return Rsio();
    if(Direction & PRINT_I2C) return Rsio2();
    return 0;
        
}

unsigned char kbhit(void)
{
    if(Direction & PRINT_COM) return RsioStat();
    if(Direction & PRINT_I2C) return RsioStat2();
    return 0;
        
}

