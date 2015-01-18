/* ****************************************************************************

    24.07.2001 API ���-1 v 1.2.1


    �㭪樨 ��� ࠡ��� � FLASH �������
    ----------------------------------


WORD EraseSector(DWORD);        //��࠭�� ᥪ��
WORD GetStatusFLASH(void);      //�஢�ઠ ����砭�� ����� ��࠭�� ᥪ��
WORD WriteFLASH(DWORD,BPTR,WORD);   //������ ���ᨢ� ����� �� FLASH


�ਬ�砭�� ��� ����� ���⨯�:

����� � FLASH ᯥ��筠 ��� ���-1. FLASH ࠡ�⠥� � ᫮���� ०���, ��祬
������� FLASH ������ ��� ��� ���⮢��� ०���.

 ------------------------------------------------------------------------------
����: �㪨祢 �.�.

���������:
-------- ------ ----------- --------------------------------------------------- 
 ���   �����  ����               ���ᠭ��
-------- ------ ----------- --------------------------------------------------- 
18.08.01 1.1
20.08.01 1.2.1  ���祢 �.�. ����� ���ᨨ

 **************************************************************************** */

#include "mpkdefs.h"
#include "flash.h"
#include "low.h"
#include "common.h"

WPTR CurrentAddr;       // ���� ��� ���� ��⮢���� ��࠭��/�����

DWORD base = 0x80000;
                // ������� ᬥ饭�� FLASH � �����

WORD WriteWordFLASH(WPTR Addr,WORD data);
                // �㭪�� ����� ������ ᫮�� �� FLASH
WPTR ConvTLA(DWORD l);//�㭪�� �८�ࠧ������ ��������� ���� � 
            //��஢����� �� ��ࠣ��� 㪠��⥫�


/* ----------------------------------------------------------------------------
                    EraseSector
 ----------------------------------------------------------------------------
��࠭�� ᥪ��
WORD EraseSector(DWORD Addr)
�������   ��� �訡��: 
        FLASH_OK - ��࠭�� ᥪ�� �ᯥ譮 �����襭�, 
        FLASH_BUSY - ���� ��࠭��, 
        FLASH_ERROR - �訡�� ��࠭��.

�室�� ��ࠬ����   
Addr    ���� �� ������� ᥪ�� FLASH �����, TLA.

���ᠭ��
�㭪�� ��ࠥ� ���� ᥪ�� FLASH �����. �㭪�� �� ����������.

  ---------------------------------------------------------------------------- */

WORD EraseSector(DWORD Addr)
{
    //�������஢���� TLA � ��ଠ���
    Addr &= 0xFFFFFFFE;// ���� � ᫮���
    CurrentAddr=ConvTLA(base+Addr);

    if(GetStatusFLASH()==FLASH_BUSY) return FLASH_BUSY;

    if( (Addr<=0x0FFFEl) || (Addr>=0x7C000) )
        return FLASH_ERROR; //�� ᥪ��� ����� ����饭�
    __asm {
        push    ax
        push    ds
        push    si

        mov ax,8000h    ;��砫� FLASH
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
�஢�ઠ ����砭�� ����� ��࠭�� ᥪ��

�������   ��� �訡��: 
        FLASH_OK - ��࠭�� ᥪ�� �ᯥ譮 �����襭�, 
        FLASH_BUSY - ���� ��࠭��, 
        FLASH_ERROR - �訡�� ��࠭��.

�室�� ��ࠬ����   ���

���ᠭ��
�㭪�� �������� �஢���� ���ﭨ� ����� ��࠭�� ᥪ�� FLASH �����. 

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

    if((w1&0x40)^(w2&0x40)) {//��� 6 �����������?
        if(w2&0x20){ //��� 5 ��⠭�����?
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
            if((w1&0x40)^(w2&0x40)) {//��� 6 �����������?
                w1 = (WORD)(base>>4);
                __asm {
                    push    ds
                    push    si
                    push    ax
        
                    mov ax,w1
                    mov ds,ax
                    xor si,si
                    mov ax,0F0h //�����饭�� 稯� �
                    mov ds:[si],ax//०��� �⥭�� ������
        
                    pop ax
                    pop si
                    pop ds
                    };
                stat.ERROR_IO |= SM_ERROR_FLASH;
                return FLASH_ERROR;//�訡��
                }
            else //��� - ������ �����稫��� �ᯥ譮
                return FLASH_OK;
            }
        else //��� - ������ �믮������
            return FLASH_BUSY;
        }
    else    
        return FLASH_OK;//��� - ������ �믮�����
}


/* ----------------------------------------------------------------------------
                WriteFLASH
 ----------------------------------------------------------------------------
������ ���ᨢ� ����� �� FLASH

�������   ��� �訡��: 
        FLASH_OK - ��� �訡��,
        FLASH_BUSY - ���� ��楤�� ��࠭��/�����
        FLASH_ERROR - �訡�� �����

�室�� ��ࠬ����   
Addr        ���� FLASH �����
Buf     ��뫪� �� ���� ����� � ��� 
N       ������⢮ �����뢠���� ���� (�� 65535 ����)

���ᠭ��
�㭪�� �������� ������� ���ᨢ �� FLASH ������. �㭪�� ���������� �� �६� �����.

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
    if ( Addr&1 ){  //�᫨ ����� ����, � ���� ���� �����뢠��
            //�� �⭮�� ����� ��� ᫮��, ��. ���� �-ண� 0xFF
        a = Buf[0];
        a<<=8;
        a |= 0xFF;
        if(WriteWordFLASH( ConvTLA(Addr & (~1)),a ))
            return FLASH_ERROR;
        Buf++, n--, Addr++;
        }
    adr = ConvTLA(Addr);
    for ( i=0; i<n; i+=2 ){ // �᭮���� 横�
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
            �㭪樨 ������� �஢��
 ---------------------------------------------------------------------------- */


// �㭪�� ��� ����� ᫮�� �� FLASH
// Addr - ���� �����뢠����� ᫮��. ���� ���.
// data - �����뢠���� ᫮��
// �㭪�� �����頥� 0 - ������� ���諠 �ᯥ譮
//                    1 - �訡�� �����

WORD WriteWordFLASH(WPTR Addr,WORD data)
{
    WORD a;
    if(GetStatusFLASH())return 1;//� ����� ������ ������ � FLASH �믮������
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
//�८�ࠧ������ TLA �� ���� �᫠ � ��஢����� 㪠��⥫�
TLA.dword=l;
ch=TLA.a[0]&0xF;//���饭�� <= 16
TLA.seg_ofs[1]=(WORD)(TLA.dword>>4);
TLA.seg_ofs[0]=(WORD)ch;
return TLA.ptr;
}

