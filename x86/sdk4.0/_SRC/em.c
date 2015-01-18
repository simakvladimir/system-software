/* ****************************************************************************

        24.07.2001 API ���-1 v 1.2.3

    �㭪樨 ��� ࠡ��� � ����ﬨ ���७��
    ----------------------------------------

WORD WriteMP(WORD AddrMP,WORD Addr,BPTR Buf,WORD n);
            // ������ ���ᨢ� ������ � ��-26
WORD ReadMP(WORD AddrMP,WORD Addr,BPTR Buf,WORD n);
            // �⥭�� ���ᨢ� ������ �� ��-26
WORD DOUT(WORD AddrMP,BYTE d,BYTE c);
            // ��ࠢ����� �ᯮ���⥫�� ���ன�⢮� (������ ��室��)
WORD DIN(WORD AddrMP, BYTE d, BPTR c);
            // �⥭��  ���稪� (����⭮�� �室�)
WORD GetRD30(BYTE AddrMP, BYTE d, BPTR Buf);
            // �⥭��  RD30         
WORD SetRD30(WORD AddrMP,BYTE d,BYTE Command,BYTE Time);
            // ��ࠢ����� RD30 (��������� ���ﭨ� ᢥ⮤����)

������஢���� �㭪樨:

WORD InitMR26(STAT_MPK *s);
            // ���������� ⠡���� STAT_MPK ���ଠ樥� � 
            // ������ ���७��

 ------------------------------------------------------------------------------
����: ���祢 �.�.

���������:
-------- ------ ----------- --------------------------------------------------- 
 ���   �����  ����               ���ᠭ��
-------- ------ ----------- --------------------------------------------------- 
18.08.01    ���祢 �.�. ��ࠡ�⪠ ��� �㭪権 ����� EM. 
19.08.01    ���祢 �.�. ��ࠡ�⪠ �㭪樨 DIN
20.08.01 1.2.1  ���祢 �.�. ��ࠢ���� �㭪�� InitMR26
20.08.01 1.2.2  ���祢 �.�. ��࠭� ��譨� ��६����
                ��ࠢ����� �㭪�� CheckParams 
16.09.01 1.2.3  ���祢 �.�. ��ࠢ���� �㭪�� InitMR26

 **************************************************************************** */

#include "mpkdefs.h"
#include "i2c.h"
#include "em.h"
#include "i2c.h"
#include "common.h"

//DEBUG
#include "modem.h"
#include "print.h"


// �㭪�� �஢�ન ��ࠬ��஢ �㭪権 WriteMP, ReadMP,DOUT,DIN
WORD CheckParams(WORD AddrMP,WORD Addr, BYTE din, BYTE dout);
// �㭪�� �஢�ન ��ࠬ��஢ �㭪権 GetRD30 � SetRD30
WORD CheckParamsRD(WORD AddrMP,BYTE nRD30);
// �⥭�� ��26 ��� �஢�ન ��ࠬ��஢ (�ᯮ������ ��� Init)
WORD RawReadMP(WORD AddrMP,WORD Addr,BPTR Buf,WORD n);

/* ----------------------------------------------------------------------------
                WriteMP
 ----------------------------------------------------------------------------
������ ���ᨢ� ������ � ��-26

�������   ��� �訡�� ��26. 

�室�� ��ࠬ����   
AddrMP      ���� ��-26 (1xxx000d)
Addr        ���� ����� ����� MP-26
Buf     ��뫪� �� ���� ����� � ���
N       ������⢮ �����뢠���� ����

���ᠭ��
�㭪�� �������� ������� ���ᨢ ������ � ���� �� ���㫥� ���७�� ��-26.

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
�⥭�� ���ᨢ� ������ �� ��-26

�������   ��� �訡�� ��26.

�室�� ��ࠬ����   
AddrMP  ���� ��-26
Addr    ���� ����� ����� MP-26
Buf ��뫪� �� ���� ����� � ���
N   ������⢮ �����뢠���� ����

���ᠭ��
�㭪�� �������� ������ ���ᨢ ������ �� ������ �� ���㫥� ���७�� ��-26.

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

// ������ GetRD30, ��� �஢�ન ��ࠬ��஢

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
��ࠢ����� �ᯮ���⥫�� ���ன�⢮� (������ ��室��)

�������   ��� �訡�� ��26.

�室�� ��ࠬ����   
AddrMP  ���� ��-26
D   ����� ��室�
C   ���祭�� ��室�:  �� 0 - ���., 0 - �몫.

���ᠭ��
�㭪�� �������� �ࠢ���� �ᯮ���⥫�� ���ன�⢮� ����� ���७�� ��-26.

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
�⥭��  ���稪� (����⭮�� �室�)

�������   ��� �訡�� ��26.
�室�� ��ࠬ����   
AddrMP  ���� ��-26
D   ����� ���稪�
C   ��뫪� ��   ��६����� � ���ﭨ�� ���稪� (1 - ���., 0 - �몫.)

���ᠭ��
�㭪�� ��������  ������� ���ﭨ� ���稪� ������ �� ���㫥� ���७�� ��-26.

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
�⥭��  RD30

WORD GetRD30(BYTE AddrMP, BYTE d, BPTR Buf)

�������   ��� �訡�� ��26���뢠⥫�.
�室�� ��ࠬ����   
AddrMP  ���� ��-26
D   ����� RD30
Buf ��뫪� �� ���ᨢ ���⠭��� ���ଠ樨

���ᠭ��
�㭪�� �������� ������ ���ଠ�� � ������ �� ���뢠⥫�� ������祭��� � ���� ��-26���ᨢ ᮤ�ন� 4-� ���⮢� PIN-��� ����窨.

  ---------------------------------------------------------------------------- */


WORD GetRD30(WORD AddrMP,BYTE d,BPTR Buf)
{

BYTE rdflag;    // 䫠�� ���뢠⥫��
WORD res;   // ������� �⥭�� �� ��26
BYTE Mask=1;    // ��᪠, �� ���ன �������� ��⮢����� PIN ����
//int i;


    res=CheckParamsRD(AddrMP,d);
    if(res!=PIN_OK) return res;


    res=ReadMP(AddrMP,MP_RDFLG,&rdflag,1);
                    // ��⠥� ����� ���뢠⥫��

    if(res) return PIN_LINE;    // ������ �� �� �訡��

                    // ����砥� ���� ���ਤ��
    Mask <<=d;
    
    if(rdflag & Mask)                // �஢��塞 ����稥 PIN ����
        {

        res=ReadMP(AddrMP,MP_RDPIN+(d<<3),Buf,4);
                    // ��⠥� ����� ���뢠⥫��

        if(res) return PIN_LINE;    // ������ �� �� �訡��
        
        res=WriteMP(AddrMP,MP_RDFLG,&Mask,1);
                        // ����뢠�� 䫠� ���뢠⥫�

        if(res) return PIN_LINE;        

        return PIN_OK;          
        }
     else
        return PIN_EMPTY;               

    
    
}





/* ----------------------------------------------------------------------------
                    SetRD30
 ----------------------------------------------------------------------------

��ࠢ����� RD30 (��������� ���ﭨ� ᢥ⮤����)

WORD SetRD30(BYTE AddrMP, BYTE d, BYTE Command, BYTE Time)

�������   ��� �訡�� ��26���뢠⥫�. 
�室�� ��ࠬ����   
AddrMP  ���� ��-26
D   ����� RD30
Command ������� �ࠢ����� ���뢠⥫��
    1 - ������� ������ ᢥ⮤��� ���뢠⥫�
    2 - ������� �����騩 ���� ᢥ⮤��� ���뢠⥫� (�����)
    3 - ������� �����騩 ���� ᢥ⮤��� ���뢠⥫� (��������)
    4 - ������� �����騩 ������ ᢥ⮤��� ���뢠⥫�(�����)
    5 - ������� �����騩 ������ ᢥ⮤��� ���뢠⥫�(��������)
    6 - ������� �����騩 ������/���� ᢥ⮤��� ���뢠⥫� (�����)
    7 - ������� �����騩 ������/���� ᢥ⮤��� ���뢠⥫� (��������)
Time    �६� ��७�� ᢥ⮤����. �������쭮� �६� ��७��/������� (Time=1) - 140 ��, ���ᨬ��쭮� - (Time=255) - 34 ᥪ. ��� ��������� �६��� - �ਬ�୮ 140 ��.

���ᠭ��

�㭪�� �������� ������� ���ଠ�� � ���� �� ���뢠⥫�� 
������祭�� � ���� ��-26
  ---------------------------------------------------------------------------- */ 


WORD SetRD30(WORD AddrMP,BYTE d,BYTE Command,BYTE Time)
{

WORD res;   // ������� ����� � ��26
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
            �㭪樨 ������� �஢��
 -----------------------------------------------------------------------------*/

// �㭪�� �஢�ન ��ࠬ��஢ �㭪権 WriteMP, ReadMP. � ��।����� ��� �஬�
// ����.


/*
typedef struct
{
    BYTE RS;    // �롮� ⨯� �ᯮ����⥫쭮�� ����䥩� RS232 ��� RS485. 
            // 0 - RS232, 1 - RS485
    BYTE MIF;       // �롮� ��⨢���� �⥢��� ����䥩� �� 㬮�砭�� 
            // 0 - Ethernet, 1 - RS232|RS485.
    BYTE MODEM; // �롮� ०��� ࠡ��� �ᯮ����⥫쭮�� ����䥩�. 
            // 0 - ��᫥����⥫�� ����� ࠡ�⠥� � ०��� ��אַ� 
            // �裡, 1 - ��᫥����⥫�� ����� ࠡ�⠥� � ०��� 
            // �������� �裡.
    BYTE LOCK;      // ����� �室� � ०�� �ࠢ����� (���४�஢�� ⠡���)
            // 1 - �����.
    BYTE PROG;      // �ਭ㤨⥫�� �室 � ०�� �ࠢ�����(���४�஢�� ⠡���))
            // 1 - �ਭ㤨⥫�� �室 � ०�� �ࠢ�����.
    BYTE EXT1;  // ��४��祭�� ०����: �������/������������ FLASH
            // ��६�窠 ���訢����� ��砫��
            // �����稪��. �᫨ EXT1==1 �ࠢ����� ��।����� ࠡ�祩
            // �ணࠬ��, �᫨ EXT1==0, � �ࠢ����� ��।����� �ணࠬ�����
            // FLASH. 
    BYTE EXT2;      // �ணࠬ��� ���訢����� ��६�窠 (���  ����� ���७�� )
    BYTE N_MR26;    // ������⢮ ���㫥� ��26 �����㦥���� �� �⠯� 
            // ���樠����樨 ����஫��� ���-1. 
    MR26 mr26[MAX_N_MR26];
            // ���ᨢ ������� ������ � ���ᠭ��� ���䨣��樨 ��⠭�������� 
            // ��26 (�. ⠡���� ⥪�饣� ���ﭨ� ��26).
    WORD ERROR_IO;  // ����ন� ���㫥��� ���祭��, �᫨ ���� ��� �� ���� �訡��.
            // �����祭�� ��⮢:
            // 0 - ������ Am186
            // 1 - ����䥩� RS232/RS485
            // 2 - ����䥩� Ethernet
            // 3- FLASH
            // 4- ���� ॠ�쭮�� �६���/�����䨪���
            // 5- ���� � ��26 (I2C)
            // 6..7 - ��१�ࢨ஢���
            // 8..15 - ���㫨 ��26
} STAT_MPK;

typedef struct
{
    BYTE AddrMR;    // ��⥢�� ���� ����� ���७��
    BYTE Nin;   // ������⢮ ���稪�� (������� �室��)  
    BYTE Nout;      // ������⢮ �ᯮ���⥫��� ���ன�� (������� ��室��)
    BYTE N_RD30;    // ������⢮ ���뢠⥫�� RD30
} MR26;

*/

WORD CheckParamsRD(WORD AddrMP,BYTE nRD30)
{
STAT_MPK s;
int i;
char f=1;
int n;

    GetStatMPK(&s);

// �஢��塞 ���४⭮��� AddrMP

    for(i=0;i<s.N_MR26;i++)
        {
        if(AddrMP == s.mr26[i].AddrMR) { f=0; n=i;  break; }
        }
    if(f)  return PIN_ADDR; // ���ࠢ���� ���� ��26
    
// �஢��塞 D (����� RD30)
    if(nRD30>=s.mr26[n].AddrMR)
        return PIN_N;   // ���ࠢ���� ���� ���뢠⥫�



    return PIN_OK;

}


WORD CheckParams(WORD AddrMP,WORD Addr, BYTE din, BYTE dout)
{
STAT_MPK s;
int i;
char f;
int n;

    GetStatMPK(&s);

// �஢��塞 ���४⭮��� AddrMP

    f=1;
    for(i=0;i<s.N_MR26;i++)
        {
        if(AddrMP == s.mr26[i].AddrMR) { f=0; n=i; break; }
        }
    if(f)  return ME_DEV_ADDRESS; // ���ࠢ���� ���� ��26
    
// �஢��塞 din (����� ����⭮�� �室�)
    if( din >= s.mr26[n].Nin)
        return ME_DIN;  // ���ࠢ���� ����� ����⭮�� �室�

// �஢��塞 dout (����� ����⭮�� ��室�)
    if( dout >= s.mr26[n].Nout)
        return ME_DOUT; // ���ࠢ���� ����� ����⭮�� ��室�

// �஢��塞 ���४⭮��� AddrMP

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
#define PIN_OK      0   // �� ��ଠ�쭮 (���� PIN ���)
#define PIN_EMPTY   1   // ��� PIN ����.
#define PIN_CRC     2   // �訡�� ����஫쭮� �㬬� �� ���뢠�� �����
#define PIN_LINE    3   // ��� �裡 � ����訢���� ��-26
#define PIN_ADDR    4   // ������ ���� MP26
#define PIN_N       5   // ������ ����� ���뢠⥫�

#define ME_OK       0 // �� ��ଠ�쭮                              
#define ME_NODEV    1 // ��� �裡 � ����訢���� ��-26            
#define ME_ADDRESS  2 // ������ ���� � ����࠭�⢥ ����� ��26  
#define ME_SIZE     3 // ᫨誮� ������ �����                      
#define ME_DEV_ADDRESS  4 // ������ ���� MP26                        
#define ME_ACCESS   5 // �訡�� ����㯠 (����⪠ ����� � ������ RO)
#define ME_RD       6 // ������ ����� ���뢠⥫�                 
#define ME_DIN      7 // ������ ����� ���稪�                     
#define ME_DOUT     8 // ������ ����� �ᯮ���⥫쭮�� ���ன�⢠  
#define ME_OTHER    9 // �訡�� �� ���ன�⢠ MP-26                 

#define MP_DIN     0x00 // ������ �室�
#define MP_DOUT    0x10 // ������ ��室�
#define MP_DOUTCON 0x14 // ��ࠢ����� �⤥��묨 �����묨 ��室���
#define MP_RDFLG   0x20 // ����� ��⮢���� ���뢠⥫��
#define MP_RDPIN   0x30 // PIN ���� ���뢠⥫��
#define MP_RDCON   0x90 // ��ࠢ����� ���뢠⥫ﬨ 
#define MP_CONFIG  0xA0 // ���䨣���� ����� ���७��

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
