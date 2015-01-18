#include <types.h>

#define BUF_MAX_SIZE 10

typedef struct {
	Int32U  size;

	/*
	 *  Start/end
	 */
	Int8U *start;
	Int8U *end;

	/*
	 *  Iterators
	 */
	Int8U *first;
	Int8U *last;

	/*
	 *  Data
	 */
	Int8U  data[BUF_MAX_SIZE];

} buffer;

void buffer_construct( buffer * buf );
void buffer_write( buffer * buf, Int8U byte );
Int8U* buffer_read( buffer * buf );
