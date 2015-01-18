/* ****************************************************************************

        24.07.2001 API ���-1 v 1.2.1

        �㭪樨 ��� ࠡ��� c EEPROM
        ---------------------------

������஢���� �㭪樨:

char WriteByteEEPROM(unsigned char v, unsigned char Adr);
char ReadByteEEPROM(unsigned char *c, unsigned short Adr);

 ------------------------------------------------------------------------------
����: ���祢 �.�.

���������:
-------- ------ ----------- --------------------------------------------------- 
 ���   �����  ����               ���ᠭ��
-------- ------ ----------- --------------------------------------------------- 
18.08.01 1.1
20.08.01 1.2.1  ���祢 �.�. ����� ���ᨨ

 **************************************************************************** */
#include "mpkdefs.h"
#include "common.h"
#include "186es.h"
#include "i2c.h"
#include "i2c_low.h"
#include "modem.h"
#include "low.h"

char WriteByteEEPROM(unsigned char v, unsigned char Adr)
{
char r;

    StartI2C();
    if(SendByteI2C(SLAVE_ADDR))
        r = 1;
    else
        if(SendByteI2C(Adr))
            r = 1;
        else
            if(SendByteI2C(v))
                r = 1;
            else
                r = 0;
    StopI2C();
    return r;
}

char ReadByteEEPROM(unsigned char *c, unsigned short Adr)
{
char r;

    StartI2C();
    if(SendByteI2C(SLAVE_ADDR))
        r = 1;
    else
        if(SendByteI2C(Adr))
            r = 1;
        else
        {
            StartI2C();
            if(SendByteI2C(SLAVE_ADDR|I2C_READ))
                r = 1;
            else
            {
                *c = ReceiveByteI2Cna();
                r = 0;
            }
        }
    StopI2C();
    return r;
}




