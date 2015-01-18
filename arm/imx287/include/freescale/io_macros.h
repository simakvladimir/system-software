/**************************************************
 *
 * Macro declarations used for peripheral I/O
 * declarations for ARM IAR C/C++ Compiler
 *
 * Copyright 1999-2010 IAR Systems. All rights reserved.
 *
 **************************************************/

#ifndef __IO_MACROS_H
#define __IO_MACROS_H

/***********************************************
 *      C specific macros
 ***********************************************/
#define __IAR_SYSTEMS_ICC__

#ifdef __IAR_SYSTEMS_ICC__

//#ifndef _SYSTEM_BUILD
//  #pragma system_include
//#endif

//#pragma language=save
//#pragma language=extended

/* SFR sizes */
#define __REG8 unsigned char
#define __REG16 unsigned short
#define __REG32 volatile unsigned long


/***********************************************
 * I/O reg attributes
 ***********************************************/
#define __READ_WRITE
#ifdef __cplusplus
#define __READ          /* Not supported */
#else
#define __READ          const
#endif
#define __WRITE         /* Not supported */

/***********************************************
 * I/O reg bits (default names)
 ***********************************************/
typedef struct
{
  unsigned char no0:1;
  unsigned char no1:1;
  unsigned char no2:1;
  unsigned char no3:1;
  unsigned char no4:1;
  unsigned char no5:1;
  unsigned char no6:1;
  unsigned char no7:1;
} __BITS8;

typedef struct
{
  unsigned short no0:1;
  unsigned short no1:1;
  unsigned short no2:1;
  unsigned short no3:1;
  unsigned short no4:1;
  unsigned short no5:1;
  unsigned short no6:1;
  unsigned short no7:1;
  unsigned short no8:1;
  unsigned short no9:1;
  unsigned short no10:1;
  unsigned short no11:1;
  unsigned short no12:1;
  unsigned short no13:1;
  unsigned short no14:1;
  unsigned short no15:1;
} __BITS16;

typedef struct
{
  unsigned long no0:1;
  unsigned long no1:1;
  unsigned long no2:1;
  unsigned long no3:1;
  unsigned long no4:1;
  unsigned long no5:1;
  unsigned long no6:1;
  unsigned long no7:1;
  unsigned long no8:1;
  unsigned long no9:1;
  unsigned long no10:1;
  unsigned long no11:1;
  unsigned long no12:1;
  unsigned long no13:1;
  unsigned long no14:1;
  unsigned long no15:1;
  unsigned long no16:1;
  unsigned long no17:1;
  unsigned long no18:1;
  unsigned long no19:1;
  unsigned long no20:1;
  unsigned long no21:1;
  unsigned long no22:1;
  unsigned long no23:1;
  unsigned long no24:1;
  unsigned long no25:1;
  unsigned long no26:1;
  unsigned long no27:1;
  unsigned long no28:1;
  unsigned long no29:1;
  unsigned long no30:1;
  unsigned long no31:1;
} __BITS32;

/***********************************************
 * Define NAME as an I/O reg
 * Access of 8/16/32 bit reg:  NAME
 ***********************************************/

#define __IO_REG8(NAME, ADDRESS, ATTRIBUTE)              \
                   volatile __no_init ATTRIBUTE unsigned char NAME __attribute__((section(".ARM.__at_" #ADDRESS )));\

#define __IO_REG16(NAME, ADDRESS, ATTRIBUTE)             \
                   volatile __no_init ATTRIBUTE unsigned short NAME __attribute__((section(".ARM.__at_" #ADDRESS )));\

#define __IO_REG32(NAME, ADDRESS, ATTRIBUTE)             \
                   volatile  ATTRIBUTE unsigned long NAME __attribute__((section(".ARM.__at_" #ADDRESS )));\

/***********************************************
 * Define NAME as an I/O reg
 * Access of 8/16/32 bit reg:  NAME
 * Access of bit(s):           NAME_bit.noX  (X=1-31)
 ***********************************************/
#define __IO_REG8_BIT(NAME, ADDRESS, ATTRIBUTE, BIT_STRUCT)\
		volatile ATTRIBUTE unsigned char NAME __attribute__((section(".ARM.__at_" #ADDRESS )));            \
		volatile ATTRIBUTE BIT_STRUCT NAME ## _bit __attribute__((section(".ARM.__at_" #ADDRESS )));      \

//                       volatile __no_init ATTRIBUTE union \
//                        {                                 \
//                          unsigned char NAME;             \
//                          BIT_STRUCT NAME ## _bit;      \
//                        } @ ADDRESS

#define __IO_REG16_BIT(NAME, ADDRESS, ATTRIBUTE,BIT_STRUCT)\
		volatile ATTRIBUTE unsigned short NAME __attribute__((section(".ARM.__at_" #ADDRESS )));            \
		volatile ATTRIBUTE BIT_STRUCT NAME ## _bit __attribute__((section(".ARM.__at_" #ADDRESS )));      \

//                        volatile __no_init ATTRIBUTE union \
//                         {                                 \
//                           unsigned short NAME __attribute__((section(".ARM.__at_" #ADDRESS )));            \
//                           BIT_STRUCT NAME ## _bit __attribute__((section(".ARM.__at_" #ADDRESS )));      \
//                         } @ ADDRESS

#define __IO_REG32_BIT(NAME, ADDRESS, ATTRIBUTE, BIT_STRUCT)\
		volatile ATTRIBUTE unsigned long NAME __attribute__((section(".ARM.__at_" #ADDRESS )));       \
		volatile ATTRIBUTE BIT_STRUCT NAME ## _bit __attribute__((section(".ARM.__at_" #ADDRESS )));  \

//                        volatile __no_init ATTRIBUTE union \
//                         {                                 \
                           unsigned long NAME __attribute__((section(".ARM.__at_" #ADDRESS )));    \
                           BIT_STRUCT NAME ## _bit __attribute__((section(".ARM.__at_" #ADDRESS )));       \
//                         }

//                   __IO_REG8(    HW_ICOLL_VECTOR,           0x80000000,__READ_WRITE );
//__IO_REG32(    HW_ICOLL_VECTOR,           0x80000000,__READ_WRITE );
//__IO_REG32_BIT(HW_ICOLL_LEVELACK,         0x80000010,__READ_WRITE ,__hw_icoll_levelack_bits);

//#pragma language=restore

#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __IO_MACROS_H */