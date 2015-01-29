#ifndef __DFT_H__
#define __DFT_H__

#define DFT_WINDOW_SIZE 64

extern const short sin_table[DFT_WINDOW_SIZE + DFT_WINDOW_SIZE/4];

short fix_mul(short a, short b);

void dft(
  short in[],
  short out[]
);

#endif // __ADC_H__
