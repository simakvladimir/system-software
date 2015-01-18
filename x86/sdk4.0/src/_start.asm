;***************************************************************************
;
;		12.08.2001 API МПК-1 v 1.2.1
;
; 			Стартовый модуль
;			----------------
;
;
; ------------------------------------------------------------------------------
; Автор: Ключев А.О.
;
; Изменения:
;-------- ------ ----------- --------------------------------------------------- 
;  Дата	  Версия	Автор				Описание
;-------- ------ ----------- --------------------------------------------------- 
; 20.08.01 1.2.1 Замена версии
;
;****************************************************************************

;.186

.XLIST
.LIST
	name	cstart

	assume	nothing

	extrn	main_			: near
;	extrn	__InitRtns		: near
;	extrn	__FiniRtns		: near
;	extrn	__fatal_runtime_error_	: near
;	extrn	_edata			: byte	; end of DATA (start of BSS)
;	extrn	_end			: byte	; end of BSS (start of STACK)

;	extrn	"C",_curbrk		: word
;	extrn	"C",_psp		: word
;	extrn	"C",_osmajor		: byte
;	extrn	"C",_osminor		: byte
;	extrn	__osmode		: byte
;	extrn	__HShift		: byte
;	extrn	"C",_STACKLOW		: word
;	extrn	"C",_STACKTOP		: word
;	extrn	"C",_cbyte		: word
;	extrn	"C",_child		: word
;	extrn	__no87			: word
;	extrn	__FPE_handler		: word
;	extrn  ___FPE_handler		: word
;	extrn	"C",_LpCmdLine		: word
;	extrn	"C",_LpPgmName		: word
;	extrn	__get_ovl_stack		: word
;	extrn	__restore_ovl_stack	: word
;	extrn	__close_ovl_file	: word
;	extrn	__DOSseg__		: byte
 
 DGROUP group _NULL,_AFTERNULL,CONST,STRINGS,_DATA,DATA,XIB,XI,XIE,YIB,YI,YIE,_BSS,STACK




BEGTEXT  segment word public 'CODE'
	assume	cs:BEGTEXT
;forever	label	near
;	int	3h
;	jmp	short forever
;___begtext label byte
;	nop
;	nop
;	nop
;	nop
;	public ___begtext
	assume	cs:nothing
BEGTEXT  ends




_TEXT	segment word public 'CODE'


FAR_DATA segment byte public 'FAR_DATA'
FAR_DATA ends


	assume	ds:DGROUP


	INIT_VAL	equ 0101h
	NUM_VAL 	equ 16

_NULL	segment para public 'BEGDATA'
__nullarea label word
	dw	NUM_VAL dup(INIT_VAL)
	public	__nullarea
_NULL	ends

_AFTERNULL segment word public 'BEGDATA'
	dw	0			; nullchar for string at address 0
_AFTERNULL ends



CONST	segment word public 'DATA'
CONST	ends

STRINGS segment word public 'DATA'
STRINGS ends

XIB	segment word public 'DATA'
XIB	ends
XI	segment word public 'DATA'
XI	ends
XIE	segment word public 'DATA'
XIE	ends

YIB	segment word public 'DATA'
YIB	ends
YI	segment word public 'DATA'
YI	ends
YIE	segment word public 'DATA'
YIE	ends

_DATA	segment word public 'DATA'

_DATA	ends

DATA	segment word public 'DATA'
DATA	ends

_BSS	segment word public 'BSS'
_BSS	ends


STACK_SIZE	equ	800h

STACK	segment para stack 'STACK'
	db	(STACK_SIZE) dup('s')
STACK	ends


	assume	nothing
	public	_cstart_
;	public	_Not_Enough_Memory_

	assume	cs:_TEXT

 _cstart_ proc near
	jmp	around

;
; copyright message
;
        db      "WATCOM C/C++16 Run-Time system. "
        db      "(c) Copyright by WATCOM International Corp. 1988-1995."
	db	' All rights reserved.'
;
; miscellaneous code-segment messages
;


;NullAssign	db	0dh,0ah,'*** NULL assignment detected',0dh,0ah,0



;NoMemory	db	'Not enough memory',0dh,0ah,0

;ConsoleName	db	'con',00h



;		dw	___begtext	; make sure dead code elimination



around: sti				; enable interrupts
	mov	ax,DGROUP		; get proper stack segment
	mov	ds,ax
	mov	es,ax

;	mov	ax, STACK   ;; Initialization may change depending on
	mov	ss, ax			;; compiler & model of segmentation selected
	mov	sp, 0FFF0h ;-10h
	xor 	bp,bp
    	jmp     main_	        ;; Go To "C" code.
        jmp     $   			        ;; The "C" main should never return

_cstart_ endp

_TEXT	ends

	end	_cstart_
