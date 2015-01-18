#ifndef SIO

/*------------------------------------------------------------------------------
Константы и функции для работы с последовательным каналом
------------------------------------------------------------------------------*/

#define SIO

// Константы скоростей обмена для последовательного канала Am186es
#define BAUD_1200   1041
#define BAUD_2400   521
#define BAUD_4800   260
#define BAUD_9600   130
#define BAUD_19200  65
#define BAUD_38400  32
#define BAUD_57600  21
#define BAUD_115200 11

#define RRDY        0x0004  // маска для бита готовности приема
#define RBUSY       0x0008  // флаг, когда прерывание не может писать в rpack

#define rMAX    256     // размер входного FIFO
#define tMAX    256     // размер выходного FIFO
#define rMASK   (rMAX-1)    // маска входного FIFO
#define tMASK   (tMAX-1)    // маска выходного FIFO
#define PACKET_SIZE 4+6+6+2+1500+2+10



extern WORD baud;           // переменная для хранения baud rate
extern volatile WORD sio_mode;      // переменная для хранения настроек
extern volatile BYTE rBuf[rMAX];    // входной FIFO
extern volatile BYTE tBuf[tMAX];    // выходной FIFO
extern volatile WORD rStart, rCount, tStart, tCount;    // указатели FIFO
extern volatile BYTE transmitting;  // признак текущей передачи
extern volatile WORD roffset, wlen, woffset;
extern volatile BYTE rpack[PACKET_SIZE];
extern BYTE wpack[PACKET_SIZE];

// Обработчики прерываний
extern void interrupt far dma1_handle(void);
extern void interrupt far sio_handle(void);

//
extern void InitDMA1(void);

// Коды возврата для функций ReadPack(), WritePack()
#define PACKET_OK   0   // нет ошибки
#define PACKET_BUSY 1   // передатчик занят
#define PACKET_NO   1   // нет пакета
#define PACKET_ERROR    2   // ошибка приема

#endif
