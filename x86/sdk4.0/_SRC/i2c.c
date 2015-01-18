/* ****************************************************************************

        24.07.2001 API МПК-1 v 1.2.1

        Функции для работы интерфейсом I2C
        ----------------------------------

Низкоуровневые функции:


void InitI2C(void);
void TestI2C(void);
char SendByteI2C(unsigned char x);
void StartI2C(void);
void StopI2C(void);
unsigned char ReceiveByteI2C(void);
unsigned char ReceiveByteI2Cna(void);

 ------------------------------------------------------------------------------
Автор: Ключев А.О.

Изменения:
-------- ------ ----------- --------------------------------------------------- 
 Дата   Версия  Автор               Описание
-------- ------ ----------- --------------------------------------------------- 
18.08.01    Ключев А.О. 
20.08.01 1.2.1  Ключев А.О. Смена версии


 **************************************************************************** */


#include "mpkdefs.h"
#include "common.h"
#include "186es.h"
#include "i2c.h"
#include "i2c_low.h"

// -------------------------------------------------------------------------
//                              I2C.C
// -------------------------------------------------------------------------
extern void StartI2C(void);
void InitI2C(void)
{

    low_InitI2C();
    StartI2C();
    StopI2C();

        
}


char SendByteI2C(unsigned char x)
{
    return (char)low_SendByteI2C(x);
}


void StartI2C(void)
{
    low_StartI2C();
}


void StopI2C(void)
{
    low_StopI2C();
}


unsigned char ReceiveByteI2C(void)
{
    return low_ReceiveByteI2C();
}


unsigned char ReceiveByteI2Cna(void)
{
    return low_ReceiveByteI2C_NA();
}


