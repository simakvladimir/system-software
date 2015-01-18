/*
 * fir.c
 *
 *  Created on: 29.10.2014
 *      Author: Simakov Vladimir
 */

#include <math/fir.h>
#include <math/fix_fft.h>
#include <math/math.h>
#include <st/service.h>


//static int16 filter_context[FIR_LENGTH];

//const int16_T B[11] = {
//    -3354, -14798,     40,  11597,  26657,  32767,  26657,  11597,     40,
//   -14798,  -3354
//};
static const  int16 filter_coef[FIR_LENGTH] = {
        -42,   -308,   -321,   -176,    131,    313,    153,   -241,   -456,
       -172,    426,   707,    192,  -821,  -1279,   -205,   2334,   5020,
      6171,
      5020,   2334,   -205,  -1279,  -821,    192,   707,    426,
       -172,   -456,   -241,    153,    313,    131,   -176,   -321,   -308,
        -42
};

//static void fir_save_context(
//	int16  input[],
//	int16 length
//)
//{
//	int16 i;
//	for (i = length - FIR_LENGTH; i < length; ++i){
//		filter_context[i - (length - FIR_LENGTH)] = input[i];
//	}
//}
//
//static void fir_shift(
//  int16 input[],
//  int16 length,
//  int16 shift
//)
//{
//	int16 i;
//	for (i = length - shift - 1; i > 0; i--)
//	{
//		input[i+shift] = input[i];
//	}
//}
//
//static void fir_restore(
//	int16 input[],
//	int16 restore[],
//	int16 restore_length,
//	int16 start_index
//)
//{
//	int16 i;
//	for (i = start_index; i < start_index + restore_length; i++)
//	{
//		input[i] = restore[i-start_index];
//	}
//}

void fir_init(
  void
)
{
//	service_clear_array(filter_context, FIR_LENGTH);
}

void fir_process_real(
  int16  input[],
  int16  length
)
{
//	int16 i;
//	int16 j;
//
////	int16 first_out[FIR_LENGTH];
//
//	for (i = 0; i < (length - FIR_LENGTH); ++i){
//		int32 acc = 0;
////		int16 in;
////		int32 in32;
//		for (j = 0; j < FIR_LENGTH; ++j){
////			in   = (i + j) < 0 ? filter_context[i+FIR_LENGTH+j] : input[i+j];
////			in32 = in;
////			acc += FIX_MPY(in, filter_coef[j]);
////			if (acc & 0xFFFF0000){
////				acc >>= 15;
////			}
//			acc += FIX_MPY(input[i+j], filter_coef[j]);
//		}
//
////		if (i < 0){
////			first_out[i+FIR_LENGTH] = acc;
////		} else {
//		input[i] = acc;
////		}
//	}
////	fir_save_context(input, length);
////	fir_shift(input,length, FIR_LENGTH);
////	fir_restore(input, first_out, FIR_LENGTH, 0);

	int16 n;
	int16 m;
	int32 tmpval;
	for (n = 0; n < (length - FIR_LENGTH); n++){
		tmpval = 0;
		for (m = 0; m < FIR_LENGTH; m++){
			tmpval += FIX_MPY(input[n + m], filter_coef[FIR_LENGTH - m - 1]);
		}
		if (tmpval > 32767)
		    tmpval++;
		input[n] = (int16)tmpval;
	}
}
