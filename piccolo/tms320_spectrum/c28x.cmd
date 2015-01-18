/****************************************************************************/
/* SINEWAVE.CMD - COMMAND FILE FOR LINKING TMS27xx/TMS28xx C PROGRAMS       */
/*                                                                          */
/*   Description: This file is a sample command file that can be used       */
/*                for linking programs built with the TMS320C28xx C         */
/*                Compiler.   Use it as a guideline; you may want to change */
/*                the allocation scheme according to the size of your       */
/*                program and the memory layout of your target system.      */
/****************************************************************************/

MEMORY
{
PAGE 0 : BEGIN       : origin = 0x000000, length = 0x0002
/*PAGE 0 : PROG(R)     : origin = 0x3F0000, length = 0x4000*/
PAGE 0 : BOOT(R)     : origin = 0x3FF000, length = 0xFC0
PAGE 0 : RESET(R)    : origin = 0x3FFFC0, length = 0x2
PAGE 0 : VECS(R)  	: origin = 0x3FFFC2, length = 0x3E
PAGE 0 : PIEVECS(R)  : origin = 0x000D00, length = 0xFF
PAGE 0 : H0RAM(RW)   : origin = 0x008000, length = 0x1000
PAGE 0 : FLASHB      : origin = 0x3F4000, length = 0x002000     /* on-chip FLASH */
//PAGE 0 : FLASHC      : origin = 0x3F2000, length = 0x002000     /* on-chip FLASH */
//PAGE 0 : FLASHD      : origin = 0x3F0000, length = 0x002000     /* on-chip FLASH */

PAGE 1 : M0RAM(RW)   : origin = 0x000000, length = 0x400
PAGE 1 : M1RAM(RW)   : origin = 0x000400, length = 0x400
PAGE 1 : L0L1RAM(RW) : origin = 0x008000, length = 0x1000
/*PAGE 1 : OTP         : origin = 0x3D7800, length = 0x000400 */

PAGE 1 : EXPORT(W)   : origin = 0x100000, length = 0x0002
}

SECTIONS
{
   /* 22-bit program sections */
   codestart: > BEGIN,   PAGE = 0
   ramfuncs : > M1RAM, 	 PAGE = 1
   .reset   : > RESET,   PAGE = 0, TYPE = DSECT
//    vectors : > VECS, 	 PAGE = 0
   .pinit   : > M1RAM, 	 PAGE = 1
   .cinit   : > M1RAM, 	 PAGE = 1
   .text    : > H0RAM,   PAGE = 0
//   SINTBL	: > FLASHD,  PAGE = 0
   DisplayRAM: > M0RAM,	 PAGE = 1

   /* 16-Bit data sections */
   .const   : > FLASHB,  PAGE = 0
   .bss     : > M1RAM, 	 PAGE = 1
   .stack   : > M1RAM, 	 PAGE = 1
//   .sysmem  : > M1RAM, 	 PAGE = 1

//   IQmath   : > H0RAM,   PAGE = 0

   FFT_Q       : > H0RAM, 	PAGE = 0
   FFT_I	   : > H0RAM,   PAGE = 0

   /* 32-bit data sections */
   .ebss    : > M1RAM, 	 PAGE = 1
   .econst  : > FLASHB,  PAGE = 0
   .esysmem : > M1RAM, 	 PAGE = 1

//   DbgFileReg : > EXPORT,PAGE = 1, TYPE = NOLOAD
}
