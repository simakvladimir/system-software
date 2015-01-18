/* ****************************************************************************

    24.07.2001 API ���-1 v 1.2.1


    �㭪樨 ��� ࠡ��� � ��᫥����⥫�� ������� (RS232/RS485
    ----------------------------------------------------------

extern WORD GetComError(ERROR_COM far *,BYTE);
                // ���ଠ�� �� �訡��� ��᫥����⥫쭮�� ������
extern void ResetComError(BYTE);
                // ���� �訡�� ��᫥����⥫쭮�� ������

extern WORD SetSioMode(CONFIG_COM far *ConfigCOM,BYTE Interface);
                // ��⠭���� ��ࠬ��஢ � ०���� ࠡ��� ��᫥����⥫쭮�� ������. 
extern WORD GetSioMode(CONFIG_COM far *ConfigCOM,BYTE Interface);
                // ���ଠ�� � ⥪�饬 ०��� ࠡ��� ��᫥����⥫쭮�� ������ 

 ------------------------------------------------------------------------------
����: �㪨祢 �.�.

���������:
-------- ------ ----------- --------------------------------------------------- 
 ���   �����  ����               ���ᠭ��
-------- ------ ----------- --------------------------------------------------- 
18.08.01 1.1    �������� ������ ࠡ��� �������� �㭪権 � ᮮ⢥��⢨� � ����
                ����஬ ०���� ࠡ��� ��᫥����⥫쭮�� ������ (MODEM, RS232,
                RS485)                                                        
20.08.01 1.2.1  ���祢 �.�. ����� ���ᨨ

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
 ���ଠ�� �� �訡��� ��᫥����⥫쭮�� ������

 �������  ��� �訡��.  
        0 - ��� �訡��, 
        1 - ���஡���� �� �訡�� �. � ���ᠭ�� ⠡���� 
        �訡�� ��᫥����⥫쭮�� ������.

 �室�� ��ࠬ����  
         Error_com  ��뫪� �� �����⮢������ �ਪ������ 
                �ணࠬ��� ������ �������� ⨯� ERROR_COM.

         Interface  ����� ����䥩�   
                0 - COM0
                1 - COM1 

 ���ᠭ��
 �㭪�� �������� �������� ERROR_COM 䫠���� �訡�� ��᫥����⥫쭮�� 
 ������ �� ��⥬��� ⠡���� �訡�� ��᫥����⥫쭮�� ������. 
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
���� �訡�� ��᫥����⥫쭮�� ������

�������   ���
�室�� ��ࠬ����   
        Interface   ����� ����䥩�  
                0 - COM0
                1 - COM1 
���ᠭ��

�㭪�� ���뢠�� 䫠�� �訡�� � ������ 
��⥬��� ⠡���� �訡�� ��᫥����⥫쭮�� ������. 

  ---------------------------------------------------------------------------- */

void ResetComError(BYTE Interface)
{
    error_com[Interface&0x01].Error = 0;
    error_com[Interface&0x01].ErrorCount = 0;
    error_com[Interface&0x01].ErrorType = 0;
    // �� ��⠫��� �� ����� �訡�� �� ��᫥����⥫쭮�� ������
    if ( !error_com[COM0].ErrorCount && !error_com[COM1].ErrorCount && !error_net[SER0].ErrorCount )
        stat.ERROR_IO &= ~SM_ERROR_RS;
}

/* ----------------------------------------------------------------------------
            SetSioMode
 ----------------------------------------------------------------------------
��⠭���� ��ࠬ��஢ � ०���� ࠡ��� ��᫥����⥫쭮�� ������. 

�������   0 - ��� �訡��, 
        1 - �訡�� ��⠭���� ०���
    
�室�� ��ࠬ����   

Config_COM  ��뫪� �� �������� ������ ᮤ�ঠ��� ��ࠬ���� ��᫥����⥫쭮�� ������.
Interface   ����� ����䥩�  0 - COM01 - COM1 

���ᠭ��

�㭪�� �������� �������� ��ࠬ���� ��᫥����⥫쭮�� ������ 
  ---------------------------------------------------------------------------- */
WORD SetSioMode(CONFIG_COM far *ConfigCOM,BYTE Interface)
{
    WORD n;
    if ( Interface!=COM0 )          // �����ন������ ⮫쪮 COM0
    {
        AddComError(Interface,0,ERROR_COM_INTERFACE);
        return 1;
    }

    __asm {cli};
    outpw(SP0CT,inpw(SP0CT)&~0x0060);   // �� ������ ����ன�� �⪫�砥� TMODE, RMODE

    outpw(SP0STS,0);

    // ����ன�� ����� TX, RX, CTS, RTR
        outpw(P1MODE,inpw(P1MODE)&~0x0030);
        outpw(P1DIR,inpw(P1DIR)&~0x0030);

    // ��⠭���� ���뢠��� UART
    setvect(0x14,(void (__interrupt __far *)())&sio_handle);
    EnInterrupt(0x0400);        // ࠧ�襭�� ���뢠���

    // ��⠭���� ���뢠��� DMA
    setvect(0xB,(void (__interrupt __far *)())&dma1_handle);
    EnInterrupt(0x08);          // ��⨥ ��᪨

    //��⠭���� ���뢠��� RI
    setvect(0xF,(void (__interrupt __far *)())&RI_handle);
    EnInterrupt(0x80);      // ��⨥ ��᪨

    // ����ன�� SIO    
    UpdateStatMPK();

    if(!(sio_mode&SIO_DEBUG)){
        sio_mode &= ~(MCNF|RS485);
//        sio_mode |= (stat.RS==SM_RS485)?RS485:0;
//        sio_mode |= (stat.MODEM==SM_MODEM)?MCNF:0;
        sio_mode |= MCNF;
        }
    else sio_mode&=(SIO_DEBUG|EARLY);

    rStart = rCount = tStart = tCount = 0;  // ��� ���஢ FIFO
    transmitting = 0;
    sio_mode &= ~(RRDY|RBUSY);
    wlen = woffset = roffset = 0;

    n = 0x10E0;         // RSIE, RXIE, TMODE, RMODE
    if(!(sio_mode&SIO_DEBUG)){
        n |= ConfigCOM->Mode&MODE_MASK; //��।������ �⭮��
        if(ConfigCOM->Mode&0x0002)  //�᫨ 7 ��� ������
            if(ConfigCOM->Mode&0x8) //�᫨ ����஫� �⭮�� ����祭
                n |= 0x1;   //SP0 mode 1
            else
                n |= 0x4;   //SP0 mode 4
        else                            //�᫨ 8 ��� ������
            if(ConfigCOM->Mode&0x8) //� ����஫� �⭮�� ����祭
                n |= 0x3;
            else 
                n |= 0x1;
    

        //��⠭���� HS 
        if(sio_mode&MCNF){//�᫨ ०�� MODEM
            n|=0x200;//���. HS 
            outpw(AUXCON,inpw(AUXCON) & ~0x18);//CTS/RTR
//            outpw(AUXCON,(inpw(AUXCON)&~0x10)|0x8);//DTE
            }
        outpw(P1MODE,inpw(P1MODE)&~0x30);//CTS/RTR normal
        outpw(P1DIR,inpw(P1DIR)&~0x30);
        outpw(P1DATA,inpw(P1DATA)&~0x1C00);//��⠭���� #RS232+��� 2� ����� i2c
        }
    else n|=0x1; //SioMode 1
    
    outpw(SP0CT,n);
    outpw(SP0BAUD,ConfigCOM->Baud);

    Timeout = ConfigCOM->Timeout;   //��⠭���� ⠩�-��� �⢥� �� ������

    ResetComError(Interface);       // ���⪠ �訡��

    InitDMA1();
    __asm {sti};

    return 0;
}

/* ----------------------------------------------------------------------------
                     GetSioMode
 ----------------------------------------------------------------------------
���ଠ�� � ⥪�饬 ०��� ࠡ��� ��᫥����⥫쭮�� ������ 


�������   0 - ��� �訡��, 1 - �訡��

Config_COM  ��뫪� �� �������� ������, � ������ ����ᠭ� ��ࠬ���� ��᫥����⥫쭮�� ������.

�室�� ��ࠬ����   

Config_COM  ��뫪� �� �������� ������ �����⮢������ �ਪ������ �ணࠬ��� ��� ����� ��ࠬ��஢ ��᫥����⥫쭮�� ������.
Interface   ����� ����䥩�  0 - COM01 - COM1  

���ᠭ��
�㭪�� �������� ������� ��ࠬ���� ��᫥����⥫쭮�� ������. 
���ଠ�� � ��ࠬ���� ��������� � �������� �।��⠢�塞�� 
���짮��⥫�᪮� �ணࠬ���. 
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

