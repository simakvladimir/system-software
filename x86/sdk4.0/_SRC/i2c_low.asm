;***************************************************************************
;
;       12.08.2001 API ���-1 v 1.2.1
;
;   �㭪樨 ������� �஢�� ��� ࠡ��� � I2C
;   ---------------------------------------
;
;   low_StartI2C        ���� I2C
;   low_StopI2C     �⮯ I2C
;   low_InitI2C_        ���樠������ ���⮢ �����-�뢮��
;   low_ReceiveByteI2C_ �ਥ� ���� �� I2C
;   low_ReceiveByteI2C_NA_  �ਥ� ���� ��� ACK �� I2C
;   low_SendByteI2C_    ��।�� ���� �� I2C
;
; �ᯮ�짮�����: 
;       �㭪樨 ����⠭� �� �ᯮ�짮����� ����� ��ᥬ���୭��
;       ��⠢�� ��뢠��� �ணࠬ� ����ᠭ��� �� �. 
;       �������� � �⥪� �� ��࠭�����.
;
; ------------------------------------------------------------------------------
; ����: ���祢 �.�.
;
; ���������:
;-------- ------ ----------- --------------------------------------------------- 
;  ���   �����    ����               ���ᠭ��
;-------- ------ ----------- --------------------------------------------------- 
; 15.08.01 1.1   ��������� �㭪樨 low_InitI2C
;        ��������� ᯮᮡ� ࠡ��� � PIO
;        �����প� "��������" CLK �� Slave I2C (��� PIC16F87x)
; 20.08.01 1.2.1 ������ ���ᨨ
;
;****************************************************************************

.186
    name i2c_low

.XLIST
.LIST
.MODEL SMALL
.CODE
; -----------------------------------------------------------------------------
; ���ᠭ�� ���⮢ �����-�뢮�� Am186es (���-1)
; -----------------------------------------------------------------------------

IOBASE  EQU     0FF00H
P1MODE  EQU (IOBASE+76H)    
P1DIR   EQU     (IOBASE+78H)
P1DATA  EQU     (IOBASE+7AH)

; -----------------------------------------------------------------------------
; ��᪨ ��� ࠡ��� � I2C � ���-1
; -----------------------------------------------------------------------------

MASK_SDA    EQU 1000H   ;PIO28
MASK_SCL    EQU 800H    ;PIO27

MASK_SDA1   EQU 0EFFFH  ;PIO28
MASK_SCL1   EQU 0F7FFH  ;PIO27

; -----------------------------------------------------------------------------
; ������ ��� �������樨 ���⠬� I2C
; -----------------------------------------------------------------------------

; SDA ��४����� �� ��室

SDA_OUT MACRO   

    mov DX,P1DIR    
    in  AX,DX
    and AX,MASK_SDA1
    out DX,AX
ENDM

; SDA ��४����� �� �室

SDA_IN  MACRO

    mov DX,P1DIR    
    in  AX,DX
    or  AX,MASK_SDA
    out DX,AX

ENDM

; SCL ��४����� �� ��室

SCL_OUT MACRO

    mov DX,P1DIR    
    in  AX,DX
    and AX,MASK_SCL1
    out DX,AX

ENDM


SCL_IN  MACRO

    mov DX,P1DIR    
    in  AX,DX
    or  AX,MASK_SCL
    out DX,AX

ENDM


;������ ॣ���� ������

IN_PIO  MACRO
    mov DX,P1DATA   
    in  AX,DX

ENDM

;SDA=1

SDA1    MACRO
    mov DX,P1DIR
    in  ax,dx
    OR  AX,MASK_SDA
    out DX,AX

ENDM

;SDA=0

SDA0    MACRO
    mov DX,P1DIR
    in  ax,dx
    AND AX,MASK_SDA1
    out DX,AX

ENDM

;SCL=1

SCL1    MACRO

    mov DX,P1DIR
    in  ax,dx
    OR  AX,MASK_SCL
    out DX,AX

ENDM

;SCL=0

SCL0    MACRO
    mov DX,P1DIR
    in  ax,dx
    AND AX,MASK_SCL1
    out DX,AX

ENDM


; ���ᠭ�� ��������� �㭪権

    public  low_StartI2C_, low_StopI2C_,low_InitI2C_
    public  low_ReceiveByteI2C_,low_ReceiveByteI2C_NA_,low_SendByteI2C_


; -----------------------------------------------------------------------------
; ������� �㭪樨 
; -----------------------------------------------------------------------------

; ----------------------------------------------------------------------------
; ��ନ஢���� CLK
;
; �室:  ���
; ��室: ���
; ----------------------------------------------------------------------------


ClockI2C_   PROC
    SCL1            ; SCL=1

    push    CX
    mov CX,200      ; ������� �� �������� CLK
    mov DX,P1DATA
ScanCLK:
    in  ax,dx
    test    ax,MASK_SCL
    jnz IsUp        ; ��䨪�஢��� "1" �� SDA
    loop    ScanCLK
IsUp:
    SCL0

    pop     CX  

    ret
ClockI2C_   ENDP


; ----------------------------------------------------------------------------
; �⥭�� ��� ������
;
; �室:  ���
;
; ��室: 
;   SI=1 �᫨ ���⠭� �����᪠� 1 
; ----------------------------------------------------------------------------

GetBitI2C_  PROC
    SCL1

    push    CX
    mov CX,200      ; ������� �� �������� CLK
    mov DX,P1DATA
ScanCLK1:
    in  ax,dx
    test    ax,MASK_SCL
    jnz IsUp1       ; ��䨪�஢��� "1" �� SCL
    loop    ScanCLK1
IsUp1:

    pop     CX
    
    in  AX,DX
    test    ax,MASK_SDA
    mov SI,1        ;SI=1
    jnz Continue    ;�᫨ 1 � ��室�� ���㫥��� SI
    xor SI,SI       ;�᫨ 0 � ����塞 SI
Continue:
    SCL0
    ret
GetBitI2C_  ENDP


; -----------------------------------------------------------------------------
; �������� �㭪樨 
; -----------------------------------------------------------------------------

; ----------------------------------------------------------------------------
; ���樠������ ���⮢ ��� ࠡ��� � I2C
;
; �室:  ���
; ��室: ���
; ----------------------------------------------------------------------------

low_InitI2C_    PROC
    push    ax
    push    bx
    push    cx
    push    dx

    mov dx,P1MODE   ;��⠭���� MODE=1 ��� PIO
    in  ax,dx
    or  ax,MASK_SDA
    or  ax,MASK_SCL
    out dx,ax

    mov dx,P1DATA
    in  ax,dx
    and ax,MASK_SDA1
    and ax,MASK_SCL1
    out dx,ax

    mov dx,P1DIR    ;��⠭���� DIR=1 ��� PIO
    in  ax,dx
    or  ax,MASK_SDA
    or  ax,MASK_SCL
    out dx,ax

    pop     dx
    pop     cx
    pop     bx
    pop     ax
    ret
low_InitI2C_    ENDP


; ----------------------------------------------------------------------------
; ��ନ஢���� ���⮢��� ᨣ����
;
; �室:  ���
; ��室: ���
; ----------------------------------------------------------------------------

low_StartI2C_   PROC
    push    ax
    push    dx

    SDA1
    SCL1
    SDA0
    SCL0

    pop     dx
    pop     ax
    ret
low_StartI2C_   ENDP

; ----------------------------------------------------------------------------
; ��ନ஢����  ᨣ���� ����
;
; �室:  ���
; ��室: ���
; ----------------------------------------------------------------------------

low_StopI2C_ PROC
    push    ax
    push    dx

    SDA0
    SCL1
    SDA1

    pop     dx
    pop     ax
    ret
low_StopI2C_    ENDP

; ----------------------------------------------------------------------------
; �ਥ� ���� �� I2C
;
; �室:
;   ���
; ��室: 
;   �ਭ��� ���� � al
; ----------------------------------------------------------------------------
low_ReceiveByteI2C_ PROC
    push    bx
    push    cx
    push    dx
    push    ax
    push    SI

    SDA1
    mov     CX,8
    mov     BL,80h
    xor     BH,BH
Again3:
    call    GetBitI2C_
    
    cmp     SI,1
    jnz     Next3
    or      BH,BL
Next3:  
    shr     BL,1
    loop    Again3

    SDA0
    call    ClockI2C_   ;ACK
    SDA1

    pop     si
    pop     ax
    mov     al,bh
    pop     dx
    pop     cx
    pop     bx
    ret
low_ReceiveByteI2C_ ENDP


; ----------------------------------------------------------------------------
; �ਥ� ���� �� I2C ��� ACK
;
; �室:
;   ���
; ��室: 
;   �ਭ��� ���� � al
; ----------------------------------------------------------------------------
low_ReceiveByteI2C_NA_  PROC
    push    bx
    push    cx
    push    dx
    push    ax
    push    SI

    SDA1
    mov     CX,8
    mov     BL,80h
    xor     BH,BH
Again2:
    call    GetBitI2C_
    
    cmp     SI,1
    jnz     Next2
    or      BH,BL
Next2:  
    shr     BL,1
    loop    Again2

    call    ClockI2C_

    pop     SI
    pop     ax
    mov     al,bh
    pop     dx
    pop     cx
    pop     bx
    ret
low_ReceiveByteI2C_NA_  ENDP


; ----------------------------------------------------------------------------
; ��।�� ���� �१ I2C
;
; �室:
;
;   ��।������ ���� � AX
; ��室:
;
;   �᫨ AX=1 �訡�� ( ��� ACK)
;   �᫨ AX=0 �� ��ଠ�쭮 
; ----------------------------------------------------------------------------

low_SendByteI2C_    PROC
    push    bx
    push    cx
    push    dx
    push    si

    mov     bh,al
    mov     bl,80h      ; ��᪠ 
    mov     cx,8            ; ���稪 ����७��
    mov     dx,P1DIR
    in      ax,dx
cycle:
    test    BH,BL
    jne     Set1
;���뫠�� 0
    and     AX, MASK_SDA1
    jmp     short SendBit   ; ��᪠ "0"
Set1:
;���뫠�� 1
    
    or  AX, MASK_SDA    ; ��᪠ "1"
SendBit:
    out     DX,AX       ; ��।��� ��।��� ���
    ror     BL,1
    call    ClockI2C_
    loop    cycle

    SDA1
    call    GetBitI2C_      ; �஢��塞 ACK

    mov     ax,si
    pop     si
    pop     dx
    pop     cx
    pop     bx
    ret
low_SendByteI2C_    ENDP

end 

