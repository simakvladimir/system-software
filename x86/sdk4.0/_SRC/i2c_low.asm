;***************************************************************************
;
;       12.08.2001 API МПК-1 v 1.2.1
;
;   Функции низкого уровня для работы с I2C
;   ---------------------------------------
;
;   low_StartI2C        Старт I2C
;   low_StopI2C     Стоп I2C
;   low_InitI2C_        Инициализация портов ввода-вывода
;   low_ReceiveByteI2C_ Прием байта из I2C
;   low_ReceiveByteI2C_NA_  Прием байта без ACK из I2C
;   low_SendByteI2C_    Передача байта по I2C
;
; Использование: 
;       функции рассчитаны на использование внутри ассемблернных
;       вставок вызывающих программ написанных на С. 
;       Регистры в стеке не сохраняются.
;
; ------------------------------------------------------------------------------
; Автор: Ключев А.О.
;
; Изменения:
;-------- ------ ----------- --------------------------------------------------- 
;  Дата   Версия    Автор               Описание
;-------- ------ ----------- --------------------------------------------------- 
; 15.08.01 1.1   Изменение функции low_InitI2C
;        Изменение способа работы с PIO
;        Поддержка "затягивания" CLK от Slave I2C (для PIC16F87x)
; 20.08.01 1.2.1 Замена версии
;
;****************************************************************************

.186
    name i2c_low

.XLIST
.LIST
.MODEL SMALL
.CODE
; -----------------------------------------------------------------------------
; Описание портов ввода-вывода Am186es (МПК-1)
; -----------------------------------------------------------------------------

IOBASE  EQU     0FF00H
P1MODE  EQU (IOBASE+76H)    
P1DIR   EQU     (IOBASE+78H)
P1DATA  EQU     (IOBASE+7AH)

; -----------------------------------------------------------------------------
; Маски для работы с I2C в МПК-1
; -----------------------------------------------------------------------------

MASK_SDA    EQU 1000H   ;PIO28
MASK_SCL    EQU 800H    ;PIO27

MASK_SDA1   EQU 0EFFFH  ;PIO28
MASK_SCL1   EQU 0F7FFH  ;PIO27

; -----------------------------------------------------------------------------
; Макросы для манипуляции портами I2C
; -----------------------------------------------------------------------------

; SDA переключить на выход

SDA_OUT MACRO   

    mov DX,P1DIR    
    in  AX,DX
    and AX,MASK_SDA1
    out DX,AX
ENDM

; SDA переключить на вход

SDA_IN  MACRO

    mov DX,P1DIR    
    in  AX,DX
    or  AX,MASK_SDA
    out DX,AX

ENDM

; SCL переключить на выход

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


;Прочитать регистр данных

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


; Описание глобальных функций

    public  low_StartI2C_, low_StopI2C_,low_InitI2C_
    public  low_ReceiveByteI2C_,low_ReceiveByteI2C_NA_,low_SendByteI2C_


; -----------------------------------------------------------------------------
; Локальные функции 
; -----------------------------------------------------------------------------

; ----------------------------------------------------------------------------
; Формирование CLK
;
; Вход:  нет
; Выход: нет
; ----------------------------------------------------------------------------


ClockI2C_   PROC
    SCL1            ; SCL=1

    push    CX
    mov CX,200      ; Таймаут на затягивание CLK
    mov DX,P1DATA
ScanCLK:
    in  ax,dx
    test    ax,MASK_SCL
    jnz IsUp        ; Зафиксировали "1" на SDA
    loop    ScanCLK
IsUp:
    SCL0

    pop     CX  

    ret
ClockI2C_   ENDP


; ----------------------------------------------------------------------------
; Чтение бита данных
;
; Вход:  нет
;
; Выход: 
;   SI=1 если прочитана логическая 1 
; ----------------------------------------------------------------------------

GetBitI2C_  PROC
    SCL1

    push    CX
    mov CX,200      ; Таймаут на затягивание CLK
    mov DX,P1DATA
ScanCLK1:
    in  ax,dx
    test    ax,MASK_SCL
    jnz IsUp1       ; Зафиксировали "1" на SCL
    loop    ScanCLK1
IsUp1:

    pop     CX
    
    in  AX,DX
    test    ax,MASK_SDA
    mov SI,1        ;SI=1
    jnz Continue    ;если 1 то обходим обнуление SI
    xor SI,SI       ;если 0 то обнуляем SI
Continue:
    SCL0
    ret
GetBitI2C_  ENDP


; -----------------------------------------------------------------------------
; Глобальные функции 
; -----------------------------------------------------------------------------

; ----------------------------------------------------------------------------
; Инициализация портов для работы с I2C
;
; Вход:  нет
; Выход: нет
; ----------------------------------------------------------------------------

low_InitI2C_    PROC
    push    ax
    push    bx
    push    cx
    push    dx

    mov dx,P1MODE   ;Установка MODE=1 для PIO
    in  ax,dx
    or  ax,MASK_SDA
    or  ax,MASK_SCL
    out dx,ax

    mov dx,P1DATA
    in  ax,dx
    and ax,MASK_SDA1
    and ax,MASK_SCL1
    out dx,ax

    mov dx,P1DIR    ;Установка DIR=1 для PIO
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
; Формирование стартового сигнала
;
; Вход:  нет
; Выход: нет
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
; Формирование  сигнала СТОП
;
; Вход:  нет
; Выход: нет
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
; Прием байта из I2C
;
; Вход:
;   нет
; Выход: 
;   Принятый байт в al
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
; Прием байта из I2C без ACK
;
; Вход:
;   нет
; Выход: 
;   Принятый байт в al
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
; Передача байта через I2C
;
; Вход:
;
;   Передаваемый байт в AX
; Выход:
;
;   Если AX=1 ошибка ( нет ACK)
;   Если AX=0 все нормально 
; ----------------------------------------------------------------------------

low_SendByteI2C_    PROC
    push    bx
    push    cx
    push    dx
    push    si

    mov     bh,al
    mov     bl,80h      ; Маска 
    mov     cx,8            ; Счетчик повторений
    mov     dx,P1DIR
    in      ax,dx
cycle:
    test    BH,BL
    jne     Set1
;Посылаем 0
    and     AX, MASK_SDA1
    jmp     short SendBit   ; Маска "0"
Set1:
;Посылаем 1
    
    or  AX, MASK_SDA    ; Маска "1"
SendBit:
    out     DX,AX       ; Передаем очередной бит
    ror     BL,1
    call    ClockI2C_
    loop    cycle

    SDA1
    call    GetBitI2C_      ; Проверяем ACK

    mov     ax,si
    pop     si
    pop     dx
    pop     cx
    pop     bx
    ret
low_SendByteI2C_    ENDP

end 

