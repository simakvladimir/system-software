/*****************************************
 *      AMD AM186ES Register MAP
 *****************************************/
#define     CPU_FREQ    20000000 /* 20 MHz */

#define     IOBASE      0xFF00

//DMA Registers
#define     D0CON	(IOBASE+0x00CA)
#define     D0TC	(IOBASE+0x00C8)
#define     D0DSTH	(IOBASE+0x00C6)
#define     D0DSTL	(IOBASE+0x00C4)
#define     D0SRCH	(IOBASE+0x00C2)
#define     D0SRCL	(IOBASE+0x00C0)
#define     D1CON	(IOBASE+0x00DA)
#define     D1TC	(IOBASE+0x00D8)
#define     D1DSTH	(IOBASE+0x00D6)
#define     D1DSTL	(IOBASE+0x00D4)
#define     D1SRCH	(IOBASE+0x00D2)
#define     D1SRCL	(IOBASE+0x00D0)

//Asynchronous Serial Port (UART) Registers
#define     SP0CT	(IOBASE+0x0080)
#define     SP0STS	(IOBASE+0x0082)
#define     SP0TD	(IOBASE+0x0084)
#define     SP0RD	(IOBASE+0x0086)
#define     SP0BAUD	(IOBASE+0x0088)

//Interrupt controller registers
#define     I0CON       (IOBASE+0x0038)
#define     I1CON       (IOBASE+0x003A)
#define     I2CON       (IOBASE+0x003C)
#define     I3CON       (IOBASE+0x003E)
#define     I4CON       (IOBASE+0x0040)
#define     SP0CON	(IOBASE+0x0044)
#define     SP1CON	(IOBASE+0x0042)
#define     DMA0CON	(IOBASE+0x0034)
#define     DMA1CON	(IOBASE+0x0036)
#define     TCUCON	(IOBASE+0x0032)

#define     INTSTS      (IOBASE+0x0030)
#define     REQST       (IOBASE+0x002E)
#define     INSERV      (IOBASE+0x002C)
#define     PRIMSK      (IOBASE+0x002A)
#define     IMASK       (IOBASE+0x0028)
#define     POLLST      (IOBASE+0x0026)
#define     POLL        (IOBASE+0x0024)
#define     EOI         (IOBASE+0x0022)
#define     INTVECT     (IOBASE+0x0020)

//Timer Registers
#define     T0CON       (IOBASE+0x0056)
#define     T0CNT       (IOBASE+0x0050)
#define     T0CMPA      (IOBASE+0x0052)
#define     T0CMPB      (IOBASE+0x0054)
#define     T1CON       (IOBASE+0x005E)
#define     T1CNT       (IOBASE+0x0058)
#define     T1CMPA      (IOBASE+0x005A)
#define     T1CMPB      (IOBASE+0x005B)
#define     T2CON       (IOBASE+0x0066)
#define     T2CNT       (IOBASE+0x0060)
#define     T2CMPA      (IOBASE+0x0062)

//Chip Selects Registers
#define     UMCS        (IOBASE+0x00A0)
#define     LMCS        (IOBASE+0x00A2)
#define     PACS        (IOBASE+0x00A4)
#define     MMCS        (IOBASE+0x00A6)
#define     MPCS        (IOBASE+0x00A8)

//Programmable I/O (PIO) Registers
#define     P0MODE	(IOBASE+0x0070)
#define     P0DIR	(IOBASE+0x0072)
#define     P0DATA	(IOBASE+0x0074)
#define     P1MODE	(IOBASE+0x0076)
#define     P1DIR	(IOBASE+0x0078)
#define     P1DATA	(IOBASE+0x007A)

//Reset Configuration Register
#define     RESCON      (IOBASE+0x00F6)

//Watchdog Timer Register
#define     WDTCON      (IOBASE+0x00E6)

//Miscellaneous Registers
#define     SYSCON      (IOBASE+0x00F0)
#define     PRL         (IOBASE+0x00F4)
#define     RELOC       (IOBASE+0x00FE)
#define     AUXCON	(IOBASE+0x00F2)
#define     EDRAM	(IOBASE+0x00E4)
#define     CLCPSC	(IOBASE+0x00E2)
#define     MDRAM	(IOBASE+0x00E0)
