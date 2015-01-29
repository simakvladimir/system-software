#include <lcd.h>
#include <board/hw_types.h>
#include <board/hw_sysctl.h>
#include <board/hw_memmap.h>
#include <drivers/gpio.h>
#include <drivers/pin_map.h>

#define PAGE_SIZE    8
#define MAX_ROWS     64
#define MAX_PAGES    MAX_ROWS / PAGE_SIZE
#define MAX_COLUMNS  128

struct PAGE
{
    char data[MAX_COLUMNS];
};

#define FONT_WIDTH   4
#define FONT_HEIGHT  6



#define F_INDEX      12
#define P_INDEX      13
#define S_INDEX      14
#define K_INDEX      10    // index of 'k' char
#define DOT_INDEX    11    // index of '.' char
const char digits_top[] = {0x69, 0x26, 0x69, 0x79, 0x26, 0xE8, 0x68, 0xF1, 0x69, 0x69, 0x89, 0x00, 0x78, 0xE9, 0x78};
const char digits_mid[] = {0x99, 0x22, 0x16, 0x21, 0xAF, 0xE1, 0xE9, 0x12, 0x69, 0x97, 0xAC, 0x00, 0x8E, 0x9E, 0x61};
const char digits_btm[] = {0x96, 0x2F, 0x8F, 0x96, 0x22, 0x1E, 0x96, 0x48, 0x96, 0x16, 0xA9, 0x66, 0x88, 0x88, 0x1E};

/*
symbol map (4x6):
digits_top[7..4]
digits_top[3..0]
digits_mid[7..4]
digits_mid[3..0]
digits_btm[7..4]
digits_btm[3..0]
*/


struct PAGE display_ram[MAX_PAGES];

#define OUTPUT    1
#define INPUT     0

#define READ      1
#define WRITE     0

#define CTX_DATA  1
#define CTX_CMD   0

#define SET_DATA_DIRECTION(v)    if(v == OUTPUT) GPIODirModeSet(GPIO_PORTD_BASE, 0xFF, GPIO_DIR_MODE_OUT); else GPIODirModeSet(GPIO_PORTD_BASE, 0xFF, GPIO_DIR_MODE_IN);
#define SET_E(v)                 GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_0, (v << 0)&0xFF);
#define SET_RW(v)                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_1, (v << 1)&0xFF);
#define SET_A0(v)                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, (v << 2)&0xFF);
#define GET_DATA()               (GPIOPinRead(GPIO_PORTD_BASE, 0xFF) & 0x000000FF);

void SET_DATA(char v)
{
   unsigned long dw = GET_DATA();
   dw = (dw & 0xFFFFFF00) + v;
   GPIOPinWrite(GPIO_PORTD_BASE, 0xFF, dw);
}

void lcd_write(char isdata, char value)
 {
   SET_E(1);
   SET_RW(0);        // write
   SET_A0(isdata);
   SET_DATA(value);
   SET_E(0);
 }

char lcd_read(char isdata)
 {
   SET_E(1);
   SET_DATA_DIRECTION(INPUT);
   SET_RW(WRITE);
   SET_A0(isdata);
   SET_E(0);
   isdata = GET_DATA();
   SET_DATA_DIRECTION(OUTPUT);
   return isdata;
 }

#define CMD_LCDON                0xAF     // LCD display ON
#define CMD_LCDOFF               0xAE     // LCD display OFF
#define CMD_LCDCOLADRINC         0xE0     // Column address increment at write = 1
#define CMD_SETDISPLAYSTARTADR   0x40     // Sets the display RAM display start line address
#define CMD_SETPAGEADR           0xB0     // Sets the display RAM page address
#define CMD_SETCOLUMNADRU        0x10     // Sets the most significant 4 bits of the display RAM column address
#define CMD_SETCOLUMNADRL        0x00     // Sets the least significant 4 bits of the display RAM column address
#define CMD_LCDDIRECTION         0xA0     // Select LCD display direction

#define DIRECTIONNORMAL          0
#define DIRECTIONREVERSE         1

void lcd_set_display_startline_address(char adr)
{
    lcd_write(CTX_CMD, CMD_SETDISPLAYSTARTADR | (adr & 0x3F));
}

void lcd_set_page_address(char adr)
{
    lcd_write(CTX_CMD, CMD_SETPAGEADR | (adr & 0x0F));
}

void lcd_set_column_address(char adr)
{
    lcd_write(CTX_CMD, CMD_SETCOLUMNADRU | ((adr >> 4) & 0x0F));
    lcd_write(CTX_CMD, CMD_SETCOLUMNADRL | (adr & 0x0F));
}

void lcd_init()
{
    char c, p;
    lcd_write(CTX_CMD, CMD_LCDON);
    lcd_write(CTX_CMD, CMD_LCDCOLADRINC);
    lcd_write(CTX_CMD, CMD_LCDDIRECTION + DIRECTIONREVERSE);          // It seems that LCD model mistake REVERSE ADC with NORMAL one
    for (p = 0; p < MAX_PAGES; p++)
    {
      lcd_set_page_address(p);
      lcd_set_column_address(0);
      for (c = 0; c < MAX_COLUMNS; c++)
       { display_ram[p].data[c] = 0;
         lcd_write(CTX_DATA, 0);
       }
    }
}

void lcd_set_point(int x, int y)
{
   int  p, idx, v;
   x &= (MAX_COLUMNS-1);
   y &= (MAX_ROWS-1);
   p = y / PAGE_SIZE;
   idx = y - p*PAGE_SIZE;
   v = 1 << idx;
   display_ram[p].data[x] |= v;
 }

void lcd_line(int x1, int y1, int x2, int y2)
{
   int dx, dy, sx, sy, i, check, e, x, y;
   dx = x2 - x1;
   if (dx < 0)
    { sx = -1;
      dx = -dx;
    }
   else
      sx = 1;
   dy = y2 - y1;
   if (dy < 0)
    { sy = -1;
      dy = -dy;
    }
   else
      sy = 1;
   x = x1;
   y = y1;
   check = 0;
   if (dy > dx)
    { dx += dy;
      dy = dx - dy;
      dx = dx - dy;
      check = 1;
    }
   e = 2 * dy - dx;
   for(i=0; i<dx; i++)
    { lcd_set_point(x, y);
      if (e >= 0)
       { if (check)
            x += sx;
         else
            y += sy;
         e -= 2*dx;
       }
      if (check)
         y += sy;
      else
         x += sx;
      e += 2*dy;
    }
 }

void lcd_rect(int x, int y, int width, int height)
{
	int i;
	for (i = x; i < x + width; i++){
		lcd_line(i,y,i,y+height);
	}
}

void lcd_paintbegin()
 { int p, c;
   for (p = 0; p < MAX_PAGES; p++)
    { for (c = 0; c < MAX_COLUMNS; c++)
       { display_ram[p].data[c] = 0;
       }
    }
 }

void lcd_paintend()
 {
   int p, c;
   for (p = 0; p < MAX_PAGES; p++)
    {
      lcd_set_page_address(p);
      lcd_set_column_address(0);
      for (c = 0; c < MAX_COLUMNS; c++)
       {
          lcd_write(CTX_DATA, display_ram[p].data[c]);
       }
    }
 }

void print_char(int char_idx, int x_left, int y_bottom)
 { char i, msk, y;
   msk = 0x88;
   for (i = 0; i < FONT_WIDTH; i++)
    { y = y_bottom;
      if (digits_btm[char_idx] & msk & 0x0F)
       { lcd_set_point(x_left + i, y);
       }
      y--;
      if (digits_btm[char_idx] & msk & 0xF0)
       { lcd_set_point(x_left + i, y);
       }
      y--;

      if (digits_mid[char_idx] & msk & 0x0F)
       { lcd_set_point(x_left + i, y);
       }
      y--;
      if (digits_mid[char_idx] & msk & 0xF0)
       { lcd_set_point(x_left + i, y);
       }
      y--;

      if (digits_top[char_idx] & msk & 0x0F)
       { lcd_set_point(x_left + i, y);
       }
      y--;
      if (digits_top[char_idx] & msk & 0xF0)
       { lcd_set_point(x_left + i, y);
       }
      y--;
      msk >>= 1;
    }
 }

void print_str(const char* s, int x_left, int y_bottom)
// s should contain valid symbols
 {
   char char_idx;
   while(*s)
    { if (*s == 'k' || *s == 'K')
      {
    	char_idx = K_INDEX;
      } else if (*s >= '0' && *s <= '9')
      {
    	char_idx = *s - '0';
      } else if (*s == 'f' || *s == 'F'){
	    char_idx = F_INDEX;
      } else if (*s == 'p' || *s == 'P'){
  	    char_idx = P_INDEX;
      } else if (*s == 's' || *s == 'S'){
		char_idx = S_INDEX;
	  } else {
		char_idx = DOT_INDEX;
      }
      print_char(char_idx, x_left, y_bottom);
      x_left += FONT_WIDTH + 1;
      s++;
    }
 }

void lcd_print_num(
  int num,
  short base,
  char   sign,
  int    x,
  int    y
)
{
    unsigned int unsigned_num;
    int n;
    int count;
  char toPrint[20];

  if ( sign ) {
    print_char('-', x,y);
    unsigned_num = (unsigned int) -num;
  } else {
    unsigned_num = (unsigned int) num;
  }

  count = 0;
  while ((n = unsigned_num / base) > 0) {
    toPrint[count++] = (char) (unsigned_num - (n * base));
    unsigned_num = n;
  }
  toPrint[count++] = (char) unsigned_num;

  for (n = 0; n < count; n++, x+=FONT_WIDTH+1) {
      print_char("0123456789ABCDEF"[(int)(toPrint[count-(n+1)])] - '0', x, y);
  }
}


void lcd_configure(
  void
)
{
    HWREG(SYSCTL_RCGC2) |= SYSCTL_RCGC2_GPIOA;   // enable GPIOA clocking for changing pin functions
    HWREG(SYSCTL_RCGC2) |= SYSCTL_RCGC2_GPIOD;   // enable GPIOD clocking for changing pin functions

    GPIODirModeSet(GPIO_PORTA_BASE, 0xFF, GPIO_DIR_MODE_OUT);
    GPIODirModeSet(GPIO_PORTD_BASE, 0xFF, GPIO_DIR_MODE_OUT);
    SET_DATA(0xAA); // test
    SET_A0(CTX_CMD);
    SET_A0(CTX_DATA);
    SET_RW(0);
    SET_RW(1);
    SET_E(0);
    SET_E(1);
    //    GPIODirModeSet(GPIO_PORTC_BASE, 0x01, GPIO_DIR_MODE_OUT);
}
