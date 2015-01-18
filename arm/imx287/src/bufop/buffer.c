#include "buffer.h"
#include <stdbool.h>


bool is_buffer_empty(buffer * buf){
	return buf->last == buf->first ? 1:0;
}

bool is_buffer_full(buffer * buf){
	Int8U *p_temp,val;
	val = buf->last;
	p_temp = (buf->last == buf->start) ? buf->end: val--;
	return p_temp == buf->first;
}



void buffer_construct( buffer * buf )
{
	buf->size = BUF_MAX_SIZE;

	buf->start = (Int8U *)buf->data;
	buf->end   = (Int8U *)&buf->data[BUF_MAX_SIZE - 1];

	buf->first = buf->last = buf->start;
}

/*
 *  TODO make more simple
 */
void buffer_write( buffer * buf, Int8U byte ){
//	Int8U *last_point = buf->last;
//	last_point++;
// if ( buf->last > buf->end ) {
//	 if ( buf->first == buf->last ){
//		 buf->last = buf->start;
//		*(buf->last) = byte;
//		buf->last++;
//	 }
// } else if ( last_point != buf->first ) {//TODO
//
//	*(buf->last) = byte;
//	buf->last++;
// }

	if (!is_buffer_full(buf))
	{	*(buf->last) = byte;
			buf->last++;}
	if ( buf->last > buf->end )
				 buf->last = buf->start;


}

Int8U* buffer_read( buffer * buf ){
	Int8U* result;
//	if (( buf->first > buf->end ) && ( buf->last >= buf->start ))
//		buf->first = buf->start;
//
//	if ( buf->first != buf->last ) {
//
//		result = buf->first;
//		buf->first++;
//	} else {
//		result = NULL;
//	}
//	return result;
	if (is_buffer_empty(buf)) result = NULL;
	else {result = buf->first;
				buf->first++;}
	if (( buf->first > buf->end ))
			buf->first = buf->start;
	return result;
}
