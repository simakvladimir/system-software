//LOOKUP TABLE//
#include <avr/pgmspace.h>
 
 #define WINDOW_SIZE 32
 
int8_t Sinewave[WINDOW_SIZE+WINDOW_SIZE/4] = {
	0,
	24,
	48,
	71,
	90,
	106,
	118,
	125,

	127,
	125,
	118,
	106,
	90,
	71,
	48,
	24,

	0,
	-24,
	-48,
	-71,
	-90,
	-106,
	-118,
	-125,

	//-32137,
	-127,
	-125,
	-118,
	-106,
	-90,
	-71,
	-48,
	-24,

	0,
	24,
	48,
	71,
	90,
	106,
	118,
	125
};