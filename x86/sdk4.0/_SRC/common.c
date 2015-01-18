/* ****************************************************************************

    24.07.2001  API ���-1 v 1.2.2

    ���⥬�� �㭪樨 ��饣� �����祭�� 
    -----------------------------------

void Init(void);        // ���樠������ ����� ���᫨⥫�             
void ResetWDT(void);        // ���� ��஦����� ⠩��� (WatchDog-⠩���).
BPTR GetMPKinfo(void);      // ����祭�� ���ଠ�� � ����஫��� ���-1     
WORD CRC16(BPTR,WORD,WORD);                                              
                // ������ CRC                                  
void GetStatMPK(STAT_MPK far *);                                         
                // �⥭�� ⠡���� ⥪�饣� ���ﭨ� ���-1      
void UpdateStatMPK(STAT_MPK far *);                                      
                // ���������� ���ଠ樨 � ��६�窠�           
                // � ⠡��� ⥪�饣� ���ﭨ� ���-1           
void SetLed(BYTE,BYTE);     // ��ࠢ����� ᢥ⮤������ ���-1                
WORD GetID(BPTR);       // ����祭�� 㭨���쭮�� �����䨪��� ���-1   

 ------------------------------------------------------------------------------
����: �㪨祢 �.�.

���������:
-------- ------ ----------- --------------------------------------------------- 
 ���   �����  ����               ���ᠭ��
-------- ------ ----------- --------------------------------------------------- 
18.08.01 1.1
20.08.01 1.2.1  ���祢 �.�. ����� ���ᨨ
22.08.01 1.2.2  �㪨祢 �.�. ���४�஢��

 **************************************************************************** */
#include <string.h>

#include "mpkdefs.h"
#include "186es.h"
#include "low.h"
#include "common.h"
#include "i2c.h"
#include "net.h"
#include "timer.h"
#include "ds2417.h"
#include "serial.h"
#include "sio.h"
#include "em.h"

/* ---------------------------------------------------------------------------
                 ��६����
 ----------------------------------------------------------------------------*/

STAT_MPK stat;  // ������ ⥪�饣� ���ﭨ� ���-1

BYTE MPKinfo[128]="MPC1 24.07.01 v 1.0.2  LMT Ltd.";
        // ASCIIZ ��ப� � ���ଠ樥� � ��⥬�


//BPTR * GetMPKinfo() { return MPKinfo; }

/* ----------------------------------------------------------------------------
                Init
 ----------------------------------------------------------------------------

 ���樠������ ����� ���᫨⥫�

 �������    - ���
 �室�� ��ࠬ���� -    ���

 ���ᠭ��:
 ��ॢ�� ����஫��� � ������᭮� ���ﭨ�, ���樠������ 
 ����਩��� ���ன��, ���������� ⠡���� ⥪�饣� ���ﭨ� ���-1. 
 ---------------------------------------------------------------------------- */
void Init(void)
{
//  sio_mode|=EARLY;//!!!��� ��ࢮ� ���ᨨ ����� ���
//  sio_mode|=SIO_DEBUG;//!!!��. serial.h

    stat.ERROR_IO = 0;
    InitI2C();
    GetMAC((BPTR)mac);
    InitEthernet(); 
    UpdateStatMPK();
    if(InitMR26(&stat)) stat.N_MR26=0;
    InitTime(); 
    if(InitDS2417())
        stat.ERROR_IO |= SM_ERROR_RTC;
}

/* ----------------------------------------------------------------------------
                ResetWDT
----------------------------------------------------------------------------

 ���� ��஦����� ⠩��� (WatchDog-⠩���).

 �������    - ���
 �室�� ��ࠬ���� -    ���

 ���ᠭ��:
 �㭪�� ��� ��� WatchDog ⠩���. ������ ��뢠���� � ���� �����: 
 � �㭪樨 Init() (���� ࠧ, �� ���� (���ᯥ稢����� �ᯮ���⥫��)) � 
 ���� ࠧ � ࠬ��� �ਪ������� 横�� (100 ��) (���ᯥ稢����� �����稪��). 
 ��ਮ� ��९������� WatchDog ⠩��� - 1.6 ᥪ. 

 � ���⨯� ���뢠���� 2 WDT: ����७��� � ���譨�.
 ---------------------------------------------------------------------------- */


void ResetWDT(void)
{
    outpw(WDTCON,0xAAAA);   
    outpw(WDTCON,0x5555);
}

/* ---------------------------------------------------------------------------
                GetMPKinfo
 -----------------------------------------------------------------------------
����祭�� ���ଠ�� � ����஫��� ���-1

�������       - ��ப� � 㪠������ ⨯� ���ன�⢠, 
            ���ᨨ ��⥬���� ��, ���ᨨ 
            ��������� ���-1, ࠧࠡ��稪� � �ந�����⥫�.
    
�室�� ��ࠬ����   - ���
���ᠭ��

�㭪�� �����頥� ���� ⥪�⮢�� ASCIIZ ��ப� � ���ଠ樥� � 
����஫���.���ᨬ��쭠� ����� ��ப� - 128 ����. 
��ଠ� ��ப�:<��������><��� ᮧ�����><�����><��ଠ ����⮢�⥫� >
�ਬ�� ��ப�:MPC1 16.08.01 18:42 v 1.2.1  LMT Ltd.


 ----------------------------------------------------------------------------*/

// �㭪�� ॠ�������� � ���� ����� (�. common.h)

/* ---------------------------------------------------------------------------
                    CRC16
 -----------------------------------------------------------------------------
������ CRC

�������       16-� ࠧ�來� CRC (x16+x15+x2+1)
�室�� ��ࠬ����   Buf     ���� ���ᨢ� � �����.
            N       ������⢮ ���� � ���ᨢ�
            start_crc   ��砫쭮� ���祭�� CRC
���ᠭ��
�㭪�� ������뢠�� 16-� ࠧ�來� CRC ��� ���ᨢ� � �����. �᫨ ������뢠���� CRC ������ ���ᨢ�, � start_crc ������ ���� ࠢ�� 0. �� ������ CRC ��᪮�쪨� ���ᨢ�� ��室����� � ࠧ��� ����� � ��६����� start_crc ����室��� ����⠢���� CRC ��᫥����� ���ᨢ�. �㭪�� �� ��᫥������ ���室 �१ �࠭��� ᥣ����.

 ----------------------------------------------------------------------------*/


// ⠡��� ��� ����� CRC16 �� �������� x^16+x^15+x^2+1
WORD CRCTBL[256] = {
#include "A001.tbl"
};

WORD CRC16(BPTR Buf,WORD n,WORD start_crc)
{
    while ( n-- )
        start_crc = CRCTBL[((BYTE)start_crc)^(*Buf++)]^(start_crc>>8);
    return start_crc;
}


/* ---------------------------------------------------------------------------
                    GetStatMPK
 -----------------------------------------------------------------------------
�⥭�� ⠡���� ⥪�饣� ���ﭨ� ���-1

�������       ��।����� �१ 㪠��⥫�
            StatMPK ��뫪� �� �������� ⨯� STAT_MPK, ��� 
            ��࠭���� ⠡���� ⥪�饣� ���ﭨ� ���-1 
            (�. ���ᠭ�� ������� ������).
�室�� ��ࠬ����   StatMPK     ��뫪� �� ��������  ⨯� STAT_MPK, 
            � ������ ������ �㭪�� ������� �����.
���ᠭ��
�㭪�� �������� ��������, �।��⠢������ �ਪ������ 
�ணࠬ���, ����묨 �� ��⥬��� ⠡���� ⥪�饣� ���ﭨ� MPK-1. 

 ----------------------------------------------------------------------------*/

void GetStatMPK(STAT_MPK far *StatMPK)
{
    _fmemcpy((void far*)StatMPK,(void far *)&stat,sizeof(STAT_MPK));
}

/* ---------------------------------------------------------------------------
                    UpdateStatMPK
 -----------------------------------------------------------------------------
���������� ���ଠ樨 � ��६�窠� � ⠡��� ⥪�饣� ���ﭨ� ���-1

�������       ��।����� �१ 㪠��⥫�
            StatMPK ��뫪� �� �������� ⨯� STAT_MPK, 
            ��� ��࠭���� ⠡���� ⥪�饣� ���ﭨ� ���-1 
            (�. ���ᠭ�� ������� ������).
�室�� ��ࠬ����   StatMPK ��뫪� �� ��������  ⨯� STAT_MPK, 
            � ������ ������ �㭪�� ������� �����.
���ᠭ��
�㭪�� ���뢠�� ⥪�騥 ���ﭨ� ��६�祪 � �������� 
��������, �।��⠢������ �ਪ������ �ணࠬ���, 
����묨 �� ��⥬��� ⠡���� ⥪�饣� ���ﭨ� MPK-1. 
 ----------------------------------------------------------------------------*/
void UpdateStatMPK(void)
{
WORD w;
    w=inpw(P0DATA);
    stat.RS    =  (w&0x1)?SM_RS485:SM_RS232;
    stat.MIF   =  (w&0x4)?SM_SIO:SM_ETH;
    stat.MODEM =  (w&0x2)?SM_MODEM:SM_NOMODEM;
    stat.LOCK  =  (w&0x8)?SM_LOCK:(!SM_LOCK);
    stat.PROG  =  (w&0x20)?SM_PROG:(!SM_PROG);
    stat.EXT1  =  (w&0x10)?1:0;
    stat.EXT2  =  (w&0x40)?1:0;

}


/* ---------------------------------------------------------------------------
                    SetLed
 -----------------------------------------------------------------------------
��ࠢ����� ᢥ⮤������ ���-1

�������       ���
�室�� ��ࠬ����   
            N       ����� ᢥ⮤����
                    0 - LNK
                    1 - RS
            StatusLED   ��⠭���������� ���ﭨ� 
                        ᢥ⮤���� (0 - �몫., 1 - ���.)
���ᠭ��
�㭪�� �������� �������� � ����� ᢥ⮤����, 
�ᯮ������� �� ���� ���-1
 ----------------------------------------------------------------------------*/
#include "eth.h"

void SetLED(BYTE n,BYTE StatusLED)
{
WORD p;
    if(n) { //RS 
        __asm {cli};
        p = inpw(P1DATA);
        if(StatusLED) //������
            outpw(P1DATA,p&0xE5FF);//+��� 2� ����� i2c
        else  //�������
            outpw(P1DATA,(p&~0x1800)|0x0200);//+��� 2� ����� i2c
        __asm {sti};
        }
    else {
        outpw(ETHPPPTR,0x114);//������� SelfCTL CS8900A
        p = inpw(ETHPPDATAL);
        if(StatusLED)
            outpw(ETHPPDATAL, p|0x4000);
        else
            outpw(ETHPPDATAL, p&0xBFFF);
        }
}

/* ---------------------------------------------------------------------------
                    GetID memc
 -----------------------------------------------------------------------------
����祭�� 㭨���쭮�� �����䨪��� ���-1

�������   ��� �訡��.  
        0 - ��� �訡��. 
        1 - ��� �裡 � �����஭�� �����䨪��஬.

        Buf ���� ���ᨢ� � �����䨪��஬ (8 ����)

�室�� ��ࠬ����   
        Buf ��뫪� �� �����⮢����� �ਪ������ 
        �ணࠬ��� ���ᨢ BYTE Buf[8];
���ᠭ��
�㭪�� �������� ������� 㭨����� 8-�� ����� �����䨪��� ���-1.
 ----------------------------------------------------------------------------*/
/*
WORD GetID(BPTR Buf)
{
    if(GetDS2417ID(Buf)){
        stat.ERROR_IO |= SM_ERROR_RTC;
        return 1;
        }
    else return 0;
}
*/