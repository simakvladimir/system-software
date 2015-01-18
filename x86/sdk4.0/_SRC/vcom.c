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

#include "186es.h"
#include "mpkdefs.h"
#include "low.h"
#include "sio.h"
#include "modem.h"
#include "serial.h"
#include "error.h"
#include "net.h"
#include "i2c.h"
#include "eeprom.h"
#include "em.h"
#include "print.h"
#include "common.h"

unsigned char AddrMR26;

WORD InitVCOM(void)
{
STAT_MPK s;
    GetStatMPK(&s);


    if(s.N_MR26)
        {
            AddrMR26=s.mr26[s.N_MR26-1].AddrMR;
            return 0;
        }

    return 1;
}

void Wsio2(unsigned char x)
{

unsigned char cTmp;

    while(1)
        {
        ResetWDT();
        if(RawReadMP(AddrMR26,0xA6,&cTmp,1)) return; 
        if(cTmp&2) break;
        }
    
    if(RawWriteMP(AddrMR26,0xA5,&x,1)) return; 
        
}

unsigned char Rsio2(void)
{
unsigned char cTmp;

    while(1)
        {
        ResetWDT();
        if(RawReadMP(AddrMR26,0xA6,&cTmp,1)) return 0; 
        if(cTmp&1) break;
        }
    
    RawReadMP(AddrMR26,0xA5,&cTmp,1); 

    return cTmp;

        
}

unsigned char RsioStat2(void)
{
unsigned char cTmp;

    if(RawReadMP(AddrMR26,0xA6,&cTmp,1)) return 0; 
    if(cTmp&1) return 1;
     else return 0;

}
