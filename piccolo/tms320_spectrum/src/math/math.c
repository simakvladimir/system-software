/*
 * math.c
 *
 *  Created on: 01.11.2014
 *      Author: BoBaHouD
 */
#include <math/math.h>

/*
  FIX_MPY() - fixed-point multiplication & scaling.
  Substitute inline assembly for hardware-specific
  optimization suited to a particluar DSP processor.
  Scaling ensures that result remains 16-bit.
*/
int16 FIX_MPY(int16 a, int16 b)
{
    /* shift right one less bit (i.e. 15-1) */
    int c = ((int32)a * (int32)b) >> 14;
    /* last bit shifted out = rounding-bit */
    b = c & 0x01;
    /* last shift + rounding bit */
    a = (c >> 1) + b;
    return a;
}


#define INDEX_MASK          0x0000003F
static const int16 armRecipTableQ15[64] = {
 0x7F03, 0x7D13, 0x7B31, 0x795E, 0x7798, 0x75E0,
 0x7434, 0x7294, 0x70FF, 0x6F76, 0x6DF6, 0x6C82,
 0x6B16, 0x69B5, 0x685C, 0x670C, 0x65C4, 0x6484,
 0x634C, 0x621C, 0x60F3, 0x5FD0, 0x5EB5, 0x5DA0,
 0x5C91, 0x5B88, 0x5A85, 0x5988, 0x5890, 0x579E,
 0x56B0, 0x55C8, 0x54E4, 0x5405, 0x532B, 0x5255,
 0x5183, 0x50B6, 0x4FEC, 0x4F26, 0x4E64, 0x4DA6,
 0x4CEC, 0x4C34, 0x4B81, 0x4AD0, 0x4A23, 0x4978,
 0x48D1, 0x482D, 0x478C, 0x46ED, 0x4651, 0x45B8,
 0x4521, 0x448D, 0x43FC, 0x436C, 0x42DF, 0x4255,
 0x41CC, 0x4146, 0x40C2, 0x4040
};


static Uint32 clz(Uint32 x)
{
    Uint32 n = 0;
    if (x <= 0x0000ffff) n += 16, x <<= 16;
    if (x <= 0x00ffffff) n +=  8, x <<= 8;
    if (x <= 0x0fffffff) n +=  4, x <<= 4;
    if (x <= 0x3fffffff) n +=  2, x <<= 2;
    if (x <= 0x7fffffff) n ++;
    return n;
}


static inline Uint32 arm_recip_q15(
  int16 in,
  int16 * dst,
  int16 * pRecipTable)
  {

    Uint32 out = 0, tempVal = 0;
    Uint32 index = 0, i = 0;
    Uint32 signBits = 0;

    if(in > 0)
    {
      signBits = clz(in) - 17;
    }
    else
    {
      signBits = clz(-in) - 17;
    }

    /* Convert input sample to 1.15 format */
    in = in << signBits;

    /* calculation of index for initial approximated Val */
    index = in >> 8;
    index = (index & INDEX_MASK);

    /*      1.15 with exp 1  */
    out = pRecipTable[index];

    /* calculation of reciprocal value */
    /* running approximation for two iterations */
    for (i = 0; i < 2; i++)
    {
      tempVal = (int16) (((int32) in * out) >> 15);
      tempVal = 0x7FFF - tempVal;
      /*      1.15 with exp 1 */
      out = (int16) (((int32) out * tempVal) >> 14);
    }

    /* write output */
    *dst = out;

    /* return num of signbits of out = 1/in value */
    return (signBits + 1);

  }

int sqrt_q15(
  int16 in,
  int16 * pOut)
{
  int32 prevOut;
  int16 oneByOut;
  Uint32 sign_bits;

  /* Run the below code for Cortex-M4 and Cortex-M3 */

  int32 out;

  if(in > 0)
  {
    /* run for ten iterations */

    /* Take initial guess as half of the input and first iteration */
    out = ((int32) in >> 1u) + 0x3FFF;

    /* Calculation of reciprocal of out */
    /* oneByOut contains reciprocal of out which is in 2.14 format
       and oneByOut should be upscaled by signBits */
    sign_bits = arm_recip_q15((int16) out, &oneByOut, (int16*)armRecipTableQ15);

    /* 0.5 * (out) */
    out = out >> 1u;
    /* prevOut = 0.5 * out + (in * (oneByOut << signBits))) */
    prevOut = out + (((int16) (((int32) in * oneByOut) >> 16)) << sign_bits);

    /* Third iteration */
    sign_bits = arm_recip_q15((int16) prevOut, &oneByOut, (int16*)armRecipTableQ15);
    prevOut = prevOut >> 1u;
    out = prevOut + (((int16) (((int32) in * oneByOut) >> 16)) << sign_bits);

    sign_bits = arm_recip_q15((int16) out, &oneByOut, (int16*)armRecipTableQ15);
    out = out >> 1u;
    prevOut = out + (((int16) (((int32) in * oneByOut) >> 16)) << sign_bits);

    /* Fifth iteration */
    sign_bits = arm_recip_q15((int16) prevOut, &oneByOut, (int16*)armRecipTableQ15);
    prevOut = prevOut >> 1u;
    out = prevOut + (((int16) (((int32) in * oneByOut) >> 16)) << sign_bits);

    sign_bits = arm_recip_q15((int16) out, &oneByOut, (int16*)armRecipTableQ15);
    out = out >> 1u;
    prevOut = out + (((int16) (((int32) in * oneByOut) >> 16)) << sign_bits);

    /* Seventh iteration */
    sign_bits = arm_recip_q15((int16) prevOut, &oneByOut, (int16*)armRecipTableQ15);
    prevOut = prevOut >> 1u;
    out = prevOut + (((int16) (((int32) in * oneByOut) >> 16)) << sign_bits);

    sign_bits = arm_recip_q15((int16) out, &oneByOut, (int16*)armRecipTableQ15);
    out = out >> 1u;
    prevOut = out + (((int16) (((int32) in * oneByOut) >> 16)) << sign_bits);

    sign_bits = arm_recip_q15((int16) prevOut, &oneByOut, (int16*)armRecipTableQ15);
    prevOut = prevOut >> 1u;
    out = prevOut + (((int16) (((int32) in * oneByOut) >> 16)) << sign_bits);

    /* tenth iteration */
    sign_bits = arm_recip_q15((int16) out, &oneByOut, (int16*)armRecipTableQ15);
    out = out >> 1u;
    *pOut = out + (((int16) (((int32) in * oneByOut) >> 16)) << sign_bits);

    return 0;
  }
  else
    {

      *pOut = 0;
      return (-1);
    }

  }

