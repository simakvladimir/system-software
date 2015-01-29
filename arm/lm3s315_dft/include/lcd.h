/*
 * lcd.h
 *
 *  Created on: 25.10.2014
 *      Author: BoBaHouD
 */

#ifndef LCD_H_
#define LCD_H_

void lcd_init();

void lcd_set_point(int x, int y);

void lcd_line(int x1, int y1, int x2, int y2);

void lcd_rect(int x, int y, int width, int height);

void lcd_paintbegin();

void lcd_paintend();

void print_char(int char_idx, int x_left, int y_bottom);

void print_str(const char* s, int x_left, int y_bottom);

void lcd_print_num(
  int num,
  short base,
  char   sign,
  int    x,
  int    y
);

void lcd_configure(
  void
);


#endif /* LCD_H_ */
