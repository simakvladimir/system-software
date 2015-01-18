/**---------------------------------------------------------------------------
            �����
-----------------------------------------------------------------------------*/
#include "mpkdefs.h"
#include "186es.h"
#include "common.h"
#include "modem.h"
#include "sio.h"
#include "serial.h"
#include "low.h"
#include "net.h"
#include "timer.h"
#include "eeprom.h"
#include "time.h"
#include "em.h"
#include "flash.h"
#include "print.h"
#include "vcom.h"
#include "ds2417.h"
#include "i2c.h"

#include <string.h>

#define Print(x)    print(x)

//#define LARGE_TEST

void Blink(long how_much)
{
while(how_much--){
    outpw(P1DATA,(inpw(P1DATA)&~0x1800) ^ 0x0200);
    __asm {
        mov cx,0FFF0h
        del:loop del
        }
    ResetWDT();
    }
}

#define YELLOW_LED  0x0200
#define RED_LED     0x0100

void BlinkM(WORD how_much, WORD mask)
{
while(how_much--){
    outpw(P1DATA,(inpw(P1DATA)&~0x1800) ^ (mask&(YELLOW_LED|RED_LED)));
    __asm {
        mov cx,0FFF0h
        del:loop del
        }
    ResetWDT();
    }
}

#define CLR_LED(x)  \
            outpw(P1DATA,((inpw(P1DATA)&~0x1800) & ~((x)&RED_LED)) | ((x)&YELLOW_LED)); 



#ifdef LARGE_TEST

char string1[] = "abcdefghijklmnopqrstuv";

#define Macro1(qq) \
    puts("This is Macro1. This is Macro1. This is Macro1. This is Macro1. This is Macro1. " \
         "This is Macro1. This is Macro1. This is Macro1. This is Macro1. This is Macro1. " \
         "This is Macro1. This is Macro1. This is Macro1. This is Macro1. This is Macro1. " \
         "This is Macro1. This is Macro1. This is Macro1. This is Macro1. This is Macro1. " \
         "This is Macro1. This is Macro1. This is Macro1. This is Macro1. This is Macro1. " \
         "This is Macro1. This is Macro1. This is Macro1. This is Macro1. This is Macro1. " \
         "This is Macro1. This is Macro1. This is Macro1. This is Macro1. This is Macro1. " \
         "This is Macro1. This is Macro1. This is Macro1. This is Macro1. This is Macro1. " \
         "This is Macro1. This is Macro1. This is Macro1. This is Macro1. This is Macro1. " \
         "This is Macro1. This is Macro1. This is Macro1. This is Macro1. This is Macro1. " \
         "This is Macro1. This is Macro1. This is Macro1. This is Macro1. This is Macro1. " \
         "This is Macro1. This is Macro1. This is Macro1. This is Macro1. This is Macro1. " \
         "This is Macro1. This is Macro1. This is Macro1. This is Macro1. This is Macro1. " \
         "This is Macro1. This is Macro1. This is Macro1. This is Macro1. This is Macro1. " \
         "This is Macro1. This is Macro1. This is Macro1. This is Macro1. This is Macro1. " \
         "This is Macro1. This is Macro1. This is Macro1. This is Macro1. This is Macro1. " \
         "This is Macro1. This is Macro1. This is Macro1. This is Macro1. This is Macro1. " \
         "This is Macro1. This is Macro1. This is Macro1. This is Macro1. This is Macro1. " \
         "This is Macro1. This is Macro1. This is Macro1. This is Macro1. This is Macro1. " \
         "This is Macro1. This is Macro1. This is Macro1. This is Macro1. This is Macro1. " \
         "This is Macro1. This is Macro1. This is Macro1. This is Macro1. This is Macro1. " \
         "This is Macro1. This is Macro1. This is Macro1. This is Macro1. This is Macro1. " \
         "This is Macro1. This is Macro1. This is Macro1. This is Macro1. This is Macro1. " \
         "This is Macro1. This is Macro1. This is Macro1. This is Macro1. This is Macro1. " \
         "This is Macro1. This is Macro1. This is Macro1. This is Macro1. This is Macro1. "); \
    Blink(100000); \
    while(a) \
    { \
        a = a*b/c + b*c/a + ((a*b*c)?a*a/(b*c):b*b/(a*c)); \
        b = b*b/a + 10000/a*b + c*((c-a*b/c+b*b/c*c-10)?b*c/(b*a*c):a/b); \
        c = b*a/c + (a*a*b/a*c)?a*c/b:b*a/c; \
    } \
    Blink(qq);

#define Macro2(qq) \
    puts("This is Macro2. This is Macro2. This is Macro2. This is Macro2. This is Macro2. " \
         "This is Macro2. This is Macro2. This is Macro2. This is Macro2. This is Macro2. " \
         "This is Macro2. This is Macro2. This is Macro2. This is Macro2. This is Macro2. " \
         "This is Macro2. This is Macro2. This is Macro2. This is Macro2. This is Macro2. " \
         "This is Macro2. This is Macro2. This is Macro2. This is Macro2. This is Macro2. " \
         "This is Macro2. This is Macro2. This is Macro2. This is Macro2. This is Macro2. " \
         "This is Macro2. This is Macro2. This is Macro2. This is Macro2. This is Macro2. " \
         "This is Macro2. This is Macro2. This is Macro2. This is Macro2. This is Macro2. " \
         "This is Macro2. This is Macro2. This is Macro2. This is Macro2. This is Macro2. " \
         "This is Macro2. This is Macro2. This is Macro2. This is Macro2. This is Macro2. " \
         "This is Macro2. This is Macro2. This is Macro2. This is Macro2. This is Macro2. " \
         "This is Macro2. This is Macro2. This is Macro2. This is Macro2. This is Macro2. " \
         "This is Macro2. This is Macro2. This is Macro2. This is Macro2. This is Macro2. " \
         "This is Macro2. This is Macro2. This is Macro2. This is Macro2. This is Macro2. " \
         "This is Macro2. This is Macro2. This is Macro2. This is Macro2. This is Macro2. " \
         "This is Macro2. This is Macro2. This is Macro2. This is Macro2. This is Macro2. " \
         "This is Macro2. This is Macro2. This is Macro2. This is Macro2. This is Macro2. " \
         "This is Macro2. This is Macro2. This is Macro2. This is Macro2. This is Macro2. " \
         "This is Macro2. This is Macro2. This is Macro2. This is Macro2. This is Macro2. " \
         "This is Macro2. This is Macro2. This is Macro2. This is Macro2. This is Macro2. " \
         "This is Macro2. This is Macro2. This is Macro2. This is Macro2. This is Macro2. " \
         "This is Macro2. This is Macro2. This is Macro2. This is Macro2. This is Macro2. " \
         "This is Macro2. This is Macro2. This is Macro2. This is Macro2. This is Macro2. " \
         "This is Macro2. This is Macro2. This is Macro2. This is Macro2. This is Macro2. " \
         "This is Macro2. This is Macro2. This is Macro2. This is Macro2. This is Macro2. "); \
    Blink(100000); \
    while(a*b) \
    { \
        a = a*b*c + b*c/a + ((a*b*c)?a*a/(b*c):b*b/(a*c)); \
        b = b*b/a + 1000/a*b + c*((c-a*b/c+b*b/c*c-10)?b*c/(b*a*c):a/b); \
        c = b*a/c + (a*a*b/a*c)?a*c/b:b*a/c; \
    } \
    Blink(qq);

#define Macro3(qq) \
    puts("This is Macro3. This is Macro3. This is Macro3. This is Macro3. This is Macro3. " \
         "This is Macro3. This is Macro3. This is Macro3. This is Macro3. This is Macro3. " \
         "This is Macro3. This is Macro3. This is Macro3. This is Macro3. This is Macro3. " \
         "This is Macro3. This is Macro3. This is Macro3. This is Macro3. This is Macro3. " \
         "This is Macro3. This is Macro3. This is Macro3. This is Macro3. This is Macro3. " \
         "This is Macro3. This is Macro3. This is Macro3. This is Macro3. This is Macro3. " \
         "This is Macro3. This is Macro3. This is Macro3. This is Macro3. This is Macro3. " \
         "This is Macro3. This is Macro3. This is Macro3. This is Macro3. This is Macro3. " \
         "This is Macro3. This is Macro3. This is Macro3. This is Macro3. This is Macro3. " \
         "This is Macro3. This is Macro3. This is Macro3. This is Macro3. This is Macro3. " \
         "This is Macro3. This is Macro3. This is Macro3. This is Macro3. This is Macro3. " \
         "This is Macro3. This is Macro3. This is Macro3. This is Macro3. This is Macro3. " \
         "This is Macro3. This is Macro3. This is Macro3. This is Macro3. This is Macro3. " \
         "This is Macro3. This is Macro3. This is Macro3. This is Macro3. This is Macro3. " \
         "This is Macro3. This is Macro3. This is Macro3. This is Macro3. This is Macro3. " \
         "This is Macro3. This is Macro3. This is Macro3. This is Macro3. This is Macro3. " \
         "This is Macro3. This is Macro3. This is Macro3. This is Macro3. This is Macro3. " \
         "This is Macro3. This is Macro3. This is Macro3. This is Macro3. This is Macro3. " \
         "This is Macro3. This is Macro3. This is Macro3. This is Macro3. This is Macro3. " \
         "This is Macro3. This is Macro3. This is Macro3. This is Macro3. This is Macro3. " \
         "This is Macro3. This is Macro3. This is Macro3. This is Macro3. This is Macro3. " \
         "This is Macro3. This is Macro3. This is Macro3. This is Macro3. This is Macro3. " \
         "This is Macro3. This is Macro3. This is Macro3. This is Macro3. This is Macro3. " \
         "This is Macro3. This is Macro3. This is Macro3. This is Macro3. This is Macro3. " \
         "This is Macro3. This is Macro3. This is Macro3. This is Macro3. This is Macro3. "); \
    Blink(100000); \
    while(c/b) \
    { \
        a = a/b*c + b*c/a + ((a*b*c)?a*a/(b*c):b*b/(a*c)); \
        b = b*b/a + 1000/a/b + c*((c-a*b/c+b*b/c*c-10)?b*c/(b*a*c):a/b); \
        c = b*a/b*c + (a*a*b/a*c)?a*c/b:b*a/c; \
    } \
    Blink(qq);

#define Macro4(qq) \
    puts("This is Macro4. This is Macro4. This is Macro4. This is Macro4. This is Macro4. " \
         "This is Macro4. This is Macro4. This is Macro4. This is Macro4. This is Macro4. " \
         "This is Macro4. This is Macro4. This is Macro4. This is Macro4. This is Macro4. " \
         "This is Macro4. This is Macro4. This is Macro4. This is Macro4. This is Macro4. " \
         "This is Macro4. This is Macro4. This is Macro4. This is Macro4. This is Macro4. " \
         "This is Macro4. This is Macro4. This is Macro4. This is Macro4. This is Macro4. " \
         "This is Macro4. This is Macro4. This is Macro4. This is Macro4. This is Macro4. " \
         "This is Macro4. This is Macro4. This is Macro4. This is Macro4. This is Macro4. " \
         "This is Macro4. This is Macro4. This is Macro4. This is Macro4. This is Macro4. " \
         "This is Macro4. This is Macro4. This is Macro4. This is Macro4. This is Macro4. " \
         "This is Macro4. This is Macro4. This is Macro4. This is Macro4. This is Macro4. " \
         "This is Macro4. This is Macro4. This is Macro4. This is Macro4. This is Macro4. " \
         "This is Macro4. This is Macro4. This is Macro4. This is Macro4. This is Macro4. " \
         "This is Macro4. This is Macro4. This is Macro4. This is Macro4. This is Macro4. " \
         "This is Macro4. This is Macro4. This is Macro4. This is Macro4. This is Macro4. " \
         "This is Macro4. This is Macro4. This is Macro4. This is Macro4. This is Macro4. " \
         "This is Macro4. This is Macro4. This is Macro4. This is Macro4. This is Macro4. " \
         "This is Macro4. This is Macro4. This is Macro4. This is Macro4. This is Macro4. " \
         "This is Macro4. This is Macro4. This is Macro4. This is Macro4. This is Macro4. " \
         "This is Macro4. This is Macro4. This is Macro4. This is Macro4. This is Macro4. " \
         "This is Macro4. This is Macro4. This is Macro4. This is Macro4. This is Macro4. " \
         "This is Macro4. This is Macro4. This is Macro4. This is Macro4. This is Macro4. " \
         "This is Macro4. This is Macro4. This is Macro4. This is Macro4. This is Macro4. " \
         "This is Macro4. This is Macro4. This is Macro4. This is Macro4. This is Macro4. " \
         "This is Macro4. This is Macro4. This is Macro4. This is Macro4. This is Macro4. "); \
    Blink(100000); \
    while(b*b) \
    { \
        a = a*b/c + b*c/a + ((a*b*c)?a*a/(b*c):b*b/(a*c)); \
        b = b*b/a + 10000/a*b + c*((c-a*b/c+b*b/c*c-10)?b*c/(b*a*c):a/b); \
        c = b*a/c + (a*a*b/a*c)?a*c/b:b*a/c; \
    } \
    Blink(qq);

#define Macro5(qq) \
    puts("This is Macro5. This is Macro5. This is Macro5. This is Macro5. This is Macro5. " \
         "This is Macro5. This is Macro5. This is Macro5. This is Macro5. This is Macro5. " \
         "This is Macro5. This is Macro5. This is Macro5. This is Macro5. This is Macro5. " \
         "This is Macro5. This is Macro5. This is Macro5. This is Macro5. This is Macro5. " \
         "This is Macro5. This is Macro5. This is Macro5. This is Macro5. This is Macro5. " \
         "This is Macro5. This is Macro5. This is Macro5. This is Macro5. This is Macro5. " \
         "This is Macro5. This is Macro5. This is Macro5. This is Macro5. This is Macro5. " \
         "This is Macro5. This is Macro5. This is Macro5. This is Macro5. This is Macro5. " \
         "This is Macro5. This is Macro5. This is Macro5. This is Macro5. This is Macro5. " \
         "This is Macro5. This is Macro5. This is Macro5. This is Macro5. This is Macro5. " \
         "This is Macro5. This is Macro5. This is Macro5. This is Macro5. This is Macro5. " \
         "This is Macro5. This is Macro5. This is Macro5. This is Macro5. This is Macro5. " \
         "This is Macro5. This is Macro5. This is Macro5. This is Macro5. This is Macro5. " \
         "This is Macro5. This is Macro5. This is Macro5. This is Macro5. This is Macro5. " \
         "This is Macro5. This is Macro5. This is Macro5. This is Macro5. This is Macro5. " \
         "This is Macro5. This is Macro5. This is Macro5. This is Macro5. This is Macro5. " \
         "This is Macro5. This is Macro5. This is Macro5. This is Macro5. This is Macro5. " \
         "This is Macro5. This is Macro5. This is Macro5. This is Macro5. This is Macro5. " \
         "This is Macro5. This is Macro5. This is Macro5. This is Macro5. This is Macro5. " \
         "This is Macro5. This is Macro5. This is Macro5. This is Macro5. This is Macro5. " \
         "This is Macro5. This is Macro5. This is Macro5. This is Macro5. This is Macro5. " \
         "This is Macro5. This is Macro5. This is Macro5. This is Macro5. This is Macro5. " \
         "This is Macro5. This is Macro5. This is Macro5. This is Macro5. This is Macro5. " \
         "This is Macro5. This is Macro5. This is Macro5. This is Macro5. This is Macro5. " \
         "This is Macro5. This is Macro5. This is Macro5. This is Macro5. This is Macro5. "); \
    Blink(100000); \
    while(c/b) \
    { \
        a = a*b*c + b*c/a + ((a*b*c)?a*a/(b*c):b*b/(a*c)); \
        b = b*b/a + 1000/a*b + c*((c-a*b/c+b*b/c*c-10)?b*c/(b*a*c):a/b); \
        c = b*a/c + (a*a*b/a*c)?a*c/b:b*a/c; \
    } \
    Blink(qq);

#define Macro6(qq) \
    puts("This is Macro6. This is Macro6. This is Macro6. This is Macro6. This is Macro6. " \
         "This is Macro6. This is Macro6. This is Macro6. This is Macro6. This is Macro6. " \
         "This is Macro6. This is Macro6. This is Macro6. This is Macro6. This is Macro6. " \
         "This is Macro6. This is Macro6. This is Macro6. This is Macro6. This is Macro6. " \
         "This is Macro6. This is Macro6. This is Macro6. This is Macro6. This is Macro6. " \
         "This is Macro6. This is Macro6. This is Macro6. This is Macro6. This is Macro6. " \
         "This is Macro6. This is Macro6. This is Macro6. This is Macro6. This is Macro6. " \
         "This is Macro6. This is Macro6. This is Macro6. This is Macro6. This is Macro6. " \
         "This is Macro6. This is Macro6. This is Macro6. This is Macro6. This is Macro6. " \
         "This is Macro6. This is Macro6. This is Macro6. This is Macro6. This is Macro6. " \
         "This is Macro6. This is Macro6. This is Macro6. This is Macro6. This is Macro6. " \
         "This is Macro6. This is Macro6. This is Macro6. This is Macro6. This is Macro6. " \
         "This is Macro6. This is Macro6. This is Macro6. This is Macro6. This is Macro6. " \
         "This is Macro6. This is Macro6. This is Macro6. This is Macro6. This is Macro6. " \
         "This is Macro6. This is Macro6. This is Macro6. This is Macro6. This is Macro6. " \
         "This is Macro6. This is Macro6. This is Macro6. This is Macro6. This is Macro6. " \
         "This is Macro6. This is Macro6. This is Macro6. This is Macro6. This is Macro6. " \
         "This is Macro6. This is Macro6. This is Macro6. This is Macro6. This is Macro6. " \
         "This is Macro6. This is Macro6. This is Macro6. This is Macro6. This is Macro6. " \
         "This is Macro6. This is Macro6. This is Macro6. This is Macro6. This is Macro6. " \
         "This is Macro6. This is Macro6. This is Macro6. This is Macro6. This is Macro6. " \
         "This is Macro6. This is Macro6. This is Macro6. This is Macro6. This is Macro6. " \
         "This is Macro6. This is Macro6. This is Macro6. This is Macro6. This is Macro6. " \
         "This is Macro6. This is Macro6. This is Macro6. This is Macro6. This is Macro6. " \
         "This is Macro6. This is Macro6. This is Macro6. This is Macro6. This is Macro6. "); \
    Blink(100000); \
    while(b/a) \
    { \
        a = a/b*c + b*c/a + ((a*b*c)?a*a/(b*c):b*b/(a*c)); \
        b = b*b/a + 1000/a/b + c*((c-a*b/c+b*b/c*c-10)?b*c/(b*a*c):a/b); \
        c = b*a/b*c + (a*a*b/a*c)?a*c/b:b*a/c; \
    } \
    Blink(qq);

#define Macro7(qq) \
    puts("This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. " \
         "This is Macro7. This is Macro7. This is Macro7. This is Macro7. This is Macro7. "); \
    Blink(100000); \
    while(a*8/b) \
    { \
        a = a*b*c + b*c/a + ((a*b*c)?b*a/(b*c):b*b/(a*c)); \
        b = b*b/a + 1000/a*b + c*((c-a*a/c+b*b/c*c-10)?b*c/(b*a*c):a/b); \
        c = b*a/c + (a*a*b/a*c)?a*c/b:b*a/b; \
    } \
    Blink(qq);

#define Macro8(qq) \
    puts("This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. " \
         "This is Macro8. This is Macro8. This is Macro8. This is Macro8. This is Macro8. "); \
    Blink(100000); \
    while(b*b/c) \
    { \
        a = a/b*c + b*c/a + ((a*b*c)?a*c/(b*c):b*b/(a*c)); \
        b = b*b/a + 1000/a/b + c*((c-a*b/a+b*b/c*c-10)?b*c/(b*a*c):a/b); \
        c = b*a/b*c + (a*a*b/a*c)?a*c/b:a*a/c; \
    } \
    Blink(qq);


void BigFunc(void)
{
DWORD a,b,c;
    Macro6(1);
    Macro3(2);
    Macro6(3);
    Macro5(4);
    Macro6(11);
    Macro1(12);
    Macro1(13);
    Macro7(14);
    Macro4(15);
    Macro2(16);
    Macro6(17);
    Macro1(18);
    Macro2(19);
    Macro8(20);
    Macro6(21);
    Macro3(22);

    Macro2(19);
    Macro8(20);
    Macro6(21);
    Macro3(22);
    Macro1(12);
    Macro1(13);
    Macro7(14);
    Macro4(15);
    Macro2(16);

    Macro7(14);
    Macro4(15);
    Macro2(16);
    Macro6(17);
    Macro1(18);

    Macro5(4);
    Macro6(11);
    Macro1(12);
    Macro1(13);
    Macro7(14);
    Macro4(15);

}
#endif //LARGE_TEST



char sEthernetMenu[] = "       ����� Ethernet\r\n" \
    "1. InitEthernet()\r\n" \
    "2. WritePack() -> 000111111000\r\n" \
    "3. ReadPack() <- 000111111000\r\n" \
    "4. ������� �������������� ReadPack() (��� \"�������������\" ��������)\r\n" \
    "q. ����� � ������� ����\r\n" \
    "h. This Help\r\n";
char sEthernetPrompt[] = "[���� Ethernet] ";

void TestEthernet(void)
{
BYTE ch;
rTable RTable = {{0,1,2},{{0x00,0x01,0x11,0x11,0x10,0x00}}};
ERROR_NET net_err;
BPTR Buf = (BPTR)0x20000000l;
WORD i,len,result;
WORD start_time;

    Width=2;
    puts("MAC-�����: ");
    GetMAC((BPTR)Buf);
    for(i=0; i<6; i++)
    {
        print(Buf[i]);
        putc(' ');
    }
    puts("\r\n");
    

    puts(sEthernetMenu);
    
    Width=4;

    for(i=0; i<1500; i++)
        Buf[i] = (BYTE)i;

    while(1)
    {
        puts(sEthernetPrompt);
        ch=getc();
        putc(ch);
        puts("\r\n");
        switch(ch)
        {
        case '1': 
            puts("��������� InitEthernet(): ");
            print(InitEthernet());
            puts("\r\n");
            break;
        case '2':
            puts("��������� WritePack(): ");
            print(WritePack(ETH0,(VPTR)&RTable,(BPTR)Buf,1500));
            puts("\r\n");
            break;
        case '3':
            puts("��������� ReadPack(): ");
            result = ReadPack(ETH0,(VPTR)&RTable,(BPTR)Buf,(WPTR)&len);
            print(result);
            puts("\r\n");
            switch(result)
            {
            case PACKET_OK:
                puts("������ �����. �����: ");
                print(len);
                puts(" ����, MAC-���. �����������:");
                Width=2;
                for(i=0; i<6; i++)
                {
                    putc(' ');
                    print(RTable.MACaddress[Buf[0]][i]);
                }
                puts(".������:");
                for(i=0; i<len; i++)
                {
                    if(!(i%25))puts("\r\n");
                    print(Buf[i+1]);
                    putc(' ');
                }
                Width=4;
                puts("\r\n");
                break;
            case PACKET_ERROR:
                GetNetError((ERROR_NET far*)&net_err,ETH0);
                puts("������ ������. ErrorRec = 0x");
                print(net_err.ErrorRec);
                ResetNetError(ETH0);
                puts("\r\n");
                break;
            case PACKET_NO:
                puts("��� ������\r\n");
                break;
            }
            break;
        case '4': 
             {
            puts("������� ���������� ������� ReadPack(), ������������ ������. ������������\r\n" \
               "�� ������� ���������������, �� ������� ����������������� ������.\r\n" \
               "����� ��������������� �������� (���� �����) ��������� ����������������\r\n" \
               "(��� ������) � ����������� �� ����� 100�� (���������� ������� ����� ���������\r\n�������). q - �����\r\n");
            while(ReadPack(ETH0,(VPTR)&RTable,(BPTR)Buf,(WPTR)&len)!=PACKET_OK)
            {
                ResetWDT();
                if(RsioStat()) if(getc()=='q') goto lbBrk;
            }

            i=1;
            start_time=SystemTime;
            while(1)
            {
                if(ReadPack(ETH0,(VPTR)&RTable,(BPTR)Buf,(WPTR)&len)==PACKET_OK)
                {
                    i++;
                    start_time=SystemTime;
                }
                else
                {
                    if((SystemTime-start_time) >= 10) break;
                }

                ResetWDT();
                if(RsioStat()) if(getc()=='q') goto lbBrk;
            }

            Radix=10;
            Width=6;
            puts("���������� ��������� ���� �� ������ �������: ");
            print(i);
            puts("\r\n");
             lbBrk: ;
             }
             Radix=16;
             break;
            
        case 'h': puts(sEthernetMenu); break;
        case 'q': return;
        }
    }
}

#define EEPROM_TIMEOUT  40

void TestEEPROM(void)
{
BYTE Buf[6];
BYTE ch=1,check;
WORD i;
WORD start_time=SystemTime;

    puts("������������ EEPROM ������� ��������\r\n��������� MAC-������...");
    if(GetMAC((BPTR)Buf))
    {
        puts("������!\r\n");
        return;
    }
    
    puts("\r\n������");
    for(i=0; i<256; i++)
    {
        while(WriteByteEEPROM(ch,(BYTE)i))
        {
            if((SystemTime-start_time)>EEPROM_TIMEOUT) 
            {
                puts("EEPROM �� ��������!\r\n");
                return;
            }
            ResetWDT();
        }
        ch<<=1;
        if(!ch)ch=1;
        if(!(i%0x10)) putc('.');
    }

    puts("\r\n������");
    start_time=SystemTime;
    while((SystemTime-start_time)<EEPROM_TIMEOUT);
    check=1;
    Width=2;
    for(i=0; i<256; i++)
    {
        if(ReadByteEEPROM(&ch,(BYTE)i))
        {
            puts("������ ������ ����� �� ������ ");
            print((BYTE)i);
            puts("!\r\n");
            return;
        }

        if(ch!=check)
        {
            puts("������������ �� ������ ");
            print((BYTE)i);
            puts("! ������ ");
            print((BYTE)ch);
            puts(" ������ ");
            print((BYTE)check);
            puts("!\r\n");
        }

        if(!(i%0x10)) putc('.');
        
        check<<=1;
        if(!check) check=1;
    }

    puts("\r\n���� ��������. ���������� MAC-������...\r\n");
    if(SetMAC((BPTR)Buf))
    {
        puts("������!\r\n");
    }
}
        

void ShowDS2417(void)
{
WORD start_time=SystemTime;
DWORD time;

    puts("���� RTC DS2417. ��������� �������� �������� ����� ����������. q - �����\r\n");
    Width=4;
    while(1)
    {
        if((SystemTime-start_time)==50)
        {
            start_time=SystemTime;
            if(GetTime((DPTR)&time))
            {
                puts("RTC �� ��������!\r\n");
                return;
            }
            print((WORD)(time>>16));
            print((WORD)time);
            puts("\r\n");
        }
    
        if(RsioStat()) if(Rsio()=='q') break;

        ResetWDT();
    }
}

extern BYTE CheckPresence(void);
extern void TransmitByte(BYTE);
extern void ReceiveByte(BPTR);

BYTE WriteDevC(BYTE c)
{
    if(CheckPresence()) return 1; //���� ���� �� �������� ��� ��������� �� �����
    TransmitByte(0xCC);//�������� ����� ������� SkipROM
    TransmitByte(0x99);//�������� ����� ������� Write Clock
    TransmitByte(c);//�������� Device Control Byte
    if(CheckPresence()) return 1; //���� ���� �� �������� ��� ��������� �� �����
    return 0;
}

char sDS2417Menu[] = \
    "1. ����� ���� MicroLAN\r\n" \
    "2. �������� �������������\r\n" \
    "3. �������� ���������� �������� DS2417\r\n" \
    "4. ���������� ������� �� 0xFFFFFFFF\r\n" \
    "5. ��������� ����\r\n" \
    "6. ���������� ����\r\n" \
    "q. ����� � ������� ����\r\n" \
    "h. This help\r\n";

char sDS2417Prompt[] = "[DS2417] ";

void TestDS2417(void)
{
BYTE ch;
BYTE st[10];

    Radix = 16;
    
    puts(sDS2417Menu);
    while(1){
        puts(sDS2417Prompt);
        ch=getc();
        putc(ch);
        puts("\r\n"); 
        switch(ch){
        case '1': puts("����� ����: ");
                if( !CheckPresence() )
                    puts("Ok\r\n");
                else
                    puts("������\r\n");
                break;
        case '2':
                puts("��������� ID...");
                if( GetDS2417ID((BPTR)st) ){
                    puts("������!\r\n");
                    }
                else{
                    puts("Ok. ID: ");
                    Width = 2;
                    for(ch=0; ch<8; ch++){
                        Print(st[ch]);
                        Wsio(' ');
                        }
                    puts("\r\n");
                    }
                break;
        case '3':
                ShowDS2417();
                break;
        case '4':
                puts("��������� �������� �� 0xFFFFFFFF...");
                if( SetDS2417( 0xFFFFFFFFl ) )
                    puts("������!\r\n");
                else
                    puts("Ok\r\n");
                break;
        case '5':
                puts("������ �����: ");
                if( WriteDevC(0x0C) )
                    puts("������!\r\n");
                else
                    puts("Ok\r\n");
                break;
        case '6':
                puts("��������� �����: ");
                if( WriteDevC(0x00) )
                    puts("������!\r\n");
                else
                    puts("Ok\r\n");
                break;
                    
        case 'q': return;
        case 'h': puts(sDS2417Menu);
                break;
            }
        }
}
                
int setDOut(unsigned char em_addr, unsigned char out, int v);
int getDIn(unsigned char em_addr, unsigned char in, int *v);

#define MR_ADDRESS  0x80

void TestMR(void)
{
BYTE Dnum=0;
WORD start_time=SystemTime;
WORD result;
long pdins, dins = 0;
//int v, i;

    puts("������� ������� �� ���������� ������� 0..6. q - �����\r\n");
    while(1)
    {
        pdins = dins;

        Width=4;
        if((SystemTime-start_time)>=10)
        {
            start_time=SystemTime;
//            result=DOUT(MR_ADDRESS,Dnum,1);
            result=setDOut(MR_ADDRESS,Dnum,1);
            if(result)
            {
                puts("������ 0x");
                print(result);
                puts("\r\n");
            }
    
//            result=DOUT(MR_ADDRESS,(Dnum)?(Dnum-1):12,0);
            result = setDOut(MR_ADDRESS,(Dnum)?(Dnum-1):12,0);
            if(result)
            {
                puts("������ 0x");
                print(result);
                puts("\r\n");
            }

            Dnum++;
            Dnum%=13;
        }
//        Width = 1;
//        print(Dnum); puts(" ");

        if( result = ReadMP(MR_ADDRESS, MP_DIN, (BPTR)&dins, 4) )
        {
            puts("�� ��������� �����. ������: ");
            print(result);
            puts("\r\n");
        }

//        dins &= 0x1FFF;
        if( dins ^ pdins )
        {
        int i;

            for(i = 0; i < 13; ++i)
            {
                putc('I'); putc('0' + i); putc(':'); 
                putc('0' + ((dins & (1<<i)) ? 1 : 0));
                putc(' ');
            }

            print((unsigned short)(dins>>16));
            print((unsigned short)dins);

            puts("\r\n");
        }

        if(RsioStat()) 
            if(Rsio()=='q') 
                break;
        ResetWDT();
    }
}

#define FLASH_BASE      0x80000l
#define SECTORS_COUNT   6

typedef struct {
    DWORD offs;
    WORD size_w;
    } Sector;

extern WPTR CurrentAddr; //flash.c
    
void TestFLASH(void)
{
        //����������� ������� 29f400bb
Sector sectors[SECTORS_COUNT] =   //Size in words
    {{0x90000l-FLASH_BASE,32768},
    {0xA0000l-FLASH_BASE,32768},
    {0xB0000l-FLASH_BASE,32768},
    {0xC0000l-FLASH_BASE,32768},
    {0xD0000l-FLASH_BASE,32768},
    {0xE0000l-FLASH_BASE,32768}};

WORD value,i,j,progress,result;
WORD huge * flash;
WPTR buffer = (WPTR)0x20000000l;

    puts("������������ �������� FLASH:\r\n");
    for(i=0; i<SECTORS_COUNT; i++)
    {
        Radix=16;
        Width=4;
        puts("0x");
        print((WORD)((sectors[i].offs+FLASH_BASE)>>16));
        print((WORD)(sectors[i].offs+FLASH_BASE));
        puts(" (");
        Radix=10;
        Width=5;
        print(sectors[i].size_w);
        puts(" ����)\r\n");
    }

    Radix=16;
    Width=4;
    puts("�������� ��������");
    for(i=0; i<SECTORS_COUNT; i++)
    {
        if(EraseSector(sectors[i].offs)==FLASH_ERROR)
        {
            puts("������ �������� ������� 0x");
            print((WORD)(sectors[i].offs>>16));
            print((WORD)sectors[i].offs);
            puts("! ����������\r\n");
            return;
        }

        while((result=GetStatusFLASH())==FLASH_BUSY)ResetWDT();
        if(result==FLASH_ERROR)
        {
            puts("������ �������� ������� 0x");
            print((WORD)(sectors[i].offs>>16));
            print((WORD)sectors[i].offs);
            puts("! ����������\r\n");
            return;
        }
        else putc('.');
    }

    puts("\r\n������ (\'.\' = 32768 ����)");
    value=0; progress=0;
    for(i=0; i<SECTORS_COUNT; i++)
    {
        value = sectors[i].offs;
        for(j=0; j<(sectors[i].size_w>>1); j++)
        {
            buffer[j] = value;
            if( j&0x1 )
                value = ~value + 1;
            else
                value = ~value; 
        }

        if(WriteFLASH(sectors[i].offs,(BPTR)buffer,sectors[i].size_w)==FLASH_ERROR)
        {
            puts("\r\n������ ������ �� ������ ");
            print((WORD)( ((DWORD)CurrentAddr) >> 16));
            putc(':');
            print((WORD)((DWORD)CurrentAddr));
            puts("! ����������\r\n");
            return;
        }
        
        progress += sectors[i].size_w;
        if(!(progress%0x8000)) putc('.');

        for(j=0; j<(sectors[i].size_w>>1); j++)
        {
            buffer[j] = value;
            if( j&0x1 )
                value = ~value + 1;
            else
                value = ~value; 
        }

        if(WriteFLASH(sectors[i].offs+sectors[i].size_w,(BPTR)buffer,sectors[i].size_w)==FLASH_ERROR)
        {
            puts("\r\n������ ������! ����������\r\n");
            return;
        }

        progress += sectors[i].size_w;
        if(!(progress%0x8000)) putc('.');

        ResetWDT();
    }

    puts("\r\n������ (\'.\' = 32768 ����)");
    progress=0;
    flash = (WORD huge*)((sectors[0].offs + FLASH_BASE)<<12);
    for(i=0; i<SECTORS_COUNT; i++)
    {
        value = sectors[i].offs;
        for(j=0; j<sectors[i].size_w; j++, flash++)
        {
            if(*flash != value)
            {
                puts("\r\n������������ �� ������ ");
                Width=4;
                print((WORD)(((DWORD)flash)>>16));
                putc(':');
                print((WORD)((DWORD)flash));
                puts(". ������ ");
                print(value);
                puts(" ����������� ");
                print(*flash);
                puts(" (j=");
                print(j);
                putc(')');
            }

            if( j&0x1 )
                value = ~value + 1;
            else
                value = ~value; 

            progress+=2;
            if(!(progress%0x8000)) putc('.');
        }

        ResetWDT();
    }
    
    puts("\r\n���� ��������\r\n");
}

#define M_RS        0x1
#define M_MODEM     0x2
#define M_MIF       0x4
#define M_LOCK      0x8
#define M_EXT1      0x10
#define M_PROG      0x20
#define M_EXT2      0x40

void TestJumpers(void)
{
WORD start_time = SystemTime;
WORD j; 
//� �������� ������ ������ ������ x ������������� 0 ��� 1
char st[] = "MODEM: x; RS: x; LOCK: x; MIF: x; PROG: x; EXT1: x; EXT2: x\r\n";
//                  7      14       23      31       40       49       58
    puts("������������ ���������. ����� �� \'q\'...\r\n");
    while(1)
    {
        if((SystemTime-start_time)==50)
        {
            start_time=SystemTime;
            j = inpw(P0DATA);
            st[7] = (j&M_MODEM)?'1':'0';
            st[14] = (j&M_RS)?'1':'0';
            st[23] = (j&M_LOCK)?'1':'0';
            st[31] = (j&M_MIF)?'1':'0';
            st[40] = (j&M_PROG)?'1':'0';
            st[49] = (j&M_EXT1)?'1':'0';
            st[58] = (j&M_EXT2)?'1':'0';
            puts(st);
        }
    
        if(RsioStat()) if(Rsio()=='q') break;

        ResetWDT();
    }
}
        
BYTE B_uf[1600];
BYTE B_uf2[1600];
rTable RTable = {{0,1},{{0x00,0x01,0x11,0x11,0x10,0x00},{0x00,0x08,0x09,0x0A,0x0E,0x00}}};

ERROR_COM err_com;
ERROR_NET err_net;


BYTE SearchInComStream(BPTR string)
{ //0-Ok, 1-Not found, 2-Error
ERROR_COM err;
BYTE c;
BPTR st;

    st = string;
    //puts(string);
    ResetComError(COM0);
    while( 1 ){
        if( !(*st) )return 0;

        c=Rsio();
        putc(c);

        if( *st==c ){
            st++;
            }
        else {
            st = string;
            }

        if( GetComError((ERROR_COM far*)&err,COM0) ){
            if( err.ErrorType!=1 ){
                return 2;
                }
            else{
                ResetComError( COM0 );
                return 1;
                }
            }

        ResetWDT();
        }
}

void TestMODEM(void)
{
CONFIG_COM com   ={0,0,500};
WORD com_speeds[]={BAUD_1200,BAUD_2400,BAUD_4800,BAUD_9600,BAUD_19200,BAUD_57600,BAUD_38400,BAUD_115200};
BYTE *scom_speeds[] = {"1200","2400","4800","9600","19200","57600","34800","115200"};
WORD i,found=0;

    if( stat.N_MR26==0 )
    {
        puts("�� ������ ��-26 �� ������� ��� �������������. ���������� ��-26 ���\r\n" \
             "����������� �������\r\n");
        return;
    }


    UpdateStatMPK();
    if( (stat.RS==SM_RS485) || (stat.MODEM!=SM_MODEM) ){
        puts("�������� ��������� RS � ���������� MODEM ��� �������� � ����� RS232 (MODEM)\r\n");
        do {
            UpdateStatMPK();
            Blink(2);
            } while( (stat.RS==SM_RS485) || (stat.MODEM!=SM_MODEM) );
        }

    puts("���� �������, �������� �� ��������, ������������ � VCOM\r\n");
    while( transmitting )ResetWDT();

    Direction = PRINT_I2C;
    Radix = 16;
    Width = 4;
    
    InitVCOM();

    puts("Hello! ��� ������ ������������� �������� � ����� ������� Enter...\r\n");
    //SetDTR(1);
    while( getc()!=0xD );

    for( i=0; i<8; i++ ){
    //  print((BYTE)i);
        puts(scom_speeds[i]);
        puts(": ");
        com.Baud=com_speeds[i];
        SetSioMode((CONFIG_COM far *)&com,COM0);
        SendModemString("AT&H1&R2\r");
        switch( SearchInComStream("OK") ){
        case 0: puts(" Ok(y/n)? "); 
                if(getc()=='y')found=1;
                else found=0;
                puts("\r\n");
                break;
        case 1: puts(" No answer!\n\r"); break;
        case 2: puts(" Error!!!\n\r"); break;
            }
        if( found )break;
        }

    Width = 2;
    puts("��������� �������� ��������: ");
    print((BYTE)GetModemSignal());
    puts(", ������: ");
    print((BYTE)GetModemSignal());
    puts("\r\n������� � ����� ���������. ��� �������� � ������� ���� �������� ��������� EXT2\r\n");

    while( 1 ){
        UpdateStatMPK();
        if( stat.EXT2 == 0 ) //����� ��� ��������� ��������� EXT2
            break;

        while( RsioStat() ){
            ResetWDT();
            putc( Rsio() );
            }
        while( kbhit() ){
            ResetWDT();
            Wsio( getc() );
            }
        ResetWDT();
        }

    puts("\r\nEXT2 �������� - ���������� �����. ��������� � �������� RS232 ������, ������ ��\r\n" \
         "������������������� �� 115200 ����� PC, � ������� � ��������������� ��������� Enter\r\n");

    com.Baud = BAUD_115200;
    com.Timeout = 60000;
    UpdateStatMPK();
    if( (stat.RS==SM_RS485) || (stat.MODEM!=SM_MODEM) ){
        puts("�������� ��������� RS � ���������� MODEM ��� �������� � ����� RS232 (MODEM)\r\n");
        do {
            UpdateStatMPK();
            Blink(2);
            } while( stat.RS!=SM_RS485 );
        }

    SetSioMode((CONFIG_COM far *)&com,COM0);
    Direction = PRINT_COM;

    puts("�������� ������� Enter...");
    while( Rsio()!=0xD );
    puts("Ok\r\n");

}


char sMainMenu[] = "      ������� ����\r\n" \
    "2. ����� Ethernet\r\n" \
    "3. ���� EEPROM\r\n" \
    "4. ���� FLASH\r\n" \
    "5. ���� DS2417\r\n" \
    "6. ������������ ��26\r\n" \
    "7. ������������ ���������\r\n" \
/*    "9. ����� � ������� (��������� ��-26 � ��������� VCOM � ������ ��� ������)\r\n" \ */
    "d. �������� ����� ������������\r\n" \
    "m. �������� MAC-����� 00 08 09 0A 0E 00\r\n" \
    "h. This Help\r\n";
char sMainPrompt[] = "[������� ����] ";

#ifdef LARGE_TEST
char string3[] = "01234567899876543210";
#endif //LARGE_TEST

#define SV      0x0300 //POW, RS

void test(void)
{
CONFIG_COM com = {0,BAUD_57600,60000};
BYTE Mac[] = {0x00,0x08,0x09,0x0A,0x0E,0x00};
BYTE ch;
char st[] = "��������� ������������ ����� ���\r\n";


    UpdateStatMPK();
    
    SetSioMode((CONFIG_COM far *)&com,COM0);
    puts("RS232 � ���������� Hardware flow control �� ����� ��������\r\n��� �������� ����������������� �� ���� ������ ������� ���� �������...");
    do{
        ch = Rsio();
        Blink(1);
        } while(ch!=Rsio());
    CLR_LED(YELLOW_LED);
    puts("Ok\r\n");

    Radix=16;

    //EEPROM (MAC)
    InitI2C();
    puts("������ MAC...");
    
    if( GetMAC((BPTR)st) )
        puts("\a������!\r\n");
    else
        if( _fmemcmp( (const void far*)Mac, (void far*)st, 6) )
            puts("\a����������� MAC �� ��������� � ����������!\r\n����������� �������� MAC-�����, ������ m �� ����\r\n");
        else
            puts("Ok\r\n");
    
    //DS2417
    puts("DS2417. ��������� ID...");
    if( InitDS2417() ){
        puts("\a������ �������������!\r\n");
        }
    else{
        if( GetDS2417ID((BPTR)st) ){
            puts("\a�� ������� �������� (init ok)!\r\n");
            }
        else{
            puts("Ok. ID: ");
            Width = 2;
            for(ch=0; ch<8; ch++){
                Print(st[ch]);
                Wsio(' ');
                }

            if( WriteDevC(0xC) ){
                puts("\a������ ������� �����. ����� ����, ������ �������...\r\n");
                }
            else{
                puts("���� ��������...\r\n");
                }
            }
        }

    //Ethernet
    puts("������������� CS8900a (Ethernet)... ");
    InitEthernet(); 
    if( stat.ERROR_IO&SM_ERROR_ETH )
        puts(" ������!\r\n");
    else
        puts(" Ok\r\n");


    InitTime();
    if(InitMR26(&stat)) {
        puts("��-26 �� ������\r\n");
        stat.N_MR26=0;
        }
    else
        puts("��-26 ���������\r\n");
    
#ifdef LARGE_TEST
    puts(string1);
    puts("\r\n");
    puts(string2);
    puts("\r\n");
    puts(string3);
    puts("\r\n");
#endif //LARGE_TEST


    puts(sMainMenu);
    while(1){
        puts(sMainPrompt);
        ch=getc();
        putc(ch);
        puts("\r\n"); 
        switch(ch){
//        case '1': TestSIO(); break;
        case '2': TestEthernet(); break;
        case '3': TestEEPROM(); break;
        case '4': TestFLASH(); break;
        case '5': TestDS2417(); break;
        case '6': TestMR(); break;
        case '7': TestJumpers(); break;
//        case '8': TestRS485(); break;
//        case '9': TestMODEM(); break;
        case 'd': 
            for(ch=0; ch<20; ch++){
                outpw(P1DATA,(inpw(P1DATA)&~0x1800) ^ SV);
                SetLED(0, !(ch&0x1));
                __asm {
                    mov cx,0FFF0h
                    del:loop del
                    }
                ResetWDT();
                }
            break;
        case 'm': 
            if(SetMAC(Mac))
                puts("MAC-����� �� ��������!\r\n");
            else
                puts("Ok\r\n");
            break;
        case 'h': puts(sMainMenu); break;
            }
        }
    
    while(1)ResetWDT();
}
