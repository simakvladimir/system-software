#include <dft.h>

short fix_mul(short a, short b)
{
    int c = ((int)a * (int)b) >> 14;
    /* last bit shifted out = rounding-bit */
    b = c & 0x01;
    /* last shift + rounding bit */
    a = (c >> 1) + b;
    return a;
}


void dft(
  short in[],
  short out[]
)
{
  // start dft from 1
  int i, k, arg_index;
  int Re, Im;
  for (i = 1; i < DFT_WINDOW_SIZE / 2; i++)
  {
    Re = 0;
    Im = 0;
    for (k = 0; k < DFT_WINDOW_SIZE; k++)
    {
      arg_index = (k*i) % DFT_WINDOW_SIZE;

      Re += fix_mul(in[k], sin_table[arg_index + DFT_WINDOW_SIZE/4]);// cos
      Im += fix_mul(in[k], sin_table[arg_index]);  // sin
    }
    out[i] = (Re*Re + Im*Im) >> 16;
  }
}

const short sin_table[DFT_WINDOW_SIZE + DFT_WINDOW_SIZE/4] = {
        0 ,
        3212 ,
        6393 ,
        9512 ,
        12539 ,
        15446 ,
        18204 ,
        20787 ,
        23170 ,
        25329 ,
        27245 ,
        28898 ,
        30273 ,
        31356 ,
        32137 ,
        32609 ,

        32767 ,
        32609 ,
        32137 ,
        31356 ,
        30273 ,
        28898 ,
        27245 ,
        25329 ,
        23170 ,
        20787 ,
        18204 ,
        15446 ,
        12539 ,
        9512 ,
        6393 ,
        3212 ,

        0 ,
        -3212 ,
        -6393 ,
        -9512 ,
        -12539 ,
        -15446 ,
        -18204 ,
        -20787 ,
        -23170 ,
        -25329 ,
        -27245 ,
        -28898 ,
        -30273 ,
        -31356 ,
        -32137 ,
        -32609 ,

        -32767 ,
        -32609 ,
        -32137 ,
        -31356 ,
        -30273 ,
        -28898 ,
        -27245 ,
        -25329 ,
        -23170 ,
        -20787 ,
        -18204 ,
        -15446 ,
        -12539 ,
        -9512 ,
        -6393 ,
        -3212 ,

        0 ,
        3212 ,
        6393 ,
        9512 ,
        12539 ,
        15446 ,
        18204 ,
        20787 ,
        23170 ,
        25329 ,
        27245 ,
        28898 ,
        30273 ,
        31356 ,
        32137 ,
        32609
};
