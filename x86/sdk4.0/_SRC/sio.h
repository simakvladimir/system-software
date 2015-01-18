#ifndef SIO

/*------------------------------------------------------------------------------
����⠭�� � �㭪樨 ��� ࠡ��� � ��᫥����⥫�� �������
------------------------------------------------------------------------------*/

#define SIO

// ����⠭�� ᪮��⥩ ������ ��� ��᫥����⥫쭮�� ������ Am186es
#define BAUD_1200   1041
#define BAUD_2400   521
#define BAUD_4800   260
#define BAUD_9600   130
#define BAUD_19200  65
#define BAUD_38400  32
#define BAUD_57600  21
#define BAUD_115200 11

#define RRDY        0x0004  // ��᪠ ��� ��� ��⮢���� �ਥ��
#define RBUSY       0x0008  // 䫠�, ����� ���뢠��� �� ����� ����� � rpack

#define rMAX    256     // ࠧ��� �室���� FIFO
#define tMAX    256     // ࠧ��� ��室���� FIFO
#define rMASK   (rMAX-1)    // ��᪠ �室���� FIFO
#define tMASK   (tMAX-1)    // ��᪠ ��室���� FIFO
#define PACKET_SIZE 4+6+6+2+1500+2+10



extern WORD baud;           // ��६����� ��� �࠭���� baud rate
extern volatile WORD sio_mode;      // ��६����� ��� �࠭���� ����஥�
extern volatile BYTE rBuf[rMAX];    // �室��� FIFO
extern volatile BYTE tBuf[tMAX];    // ��室��� FIFO
extern volatile WORD rStart, rCount, tStart, tCount;    // 㪠��⥫� FIFO
extern volatile BYTE transmitting;  // �ਧ��� ⥪�饩 ��।��
extern volatile WORD roffset, wlen, woffset;
extern volatile BYTE rpack[PACKET_SIZE];
extern BYTE wpack[PACKET_SIZE];

// ��ࠡ��稪� ���뢠���
extern void interrupt far dma1_handle(void);
extern void interrupt far sio_handle(void);

//
extern void InitDMA1(void);

// ���� ������ ��� �㭪権 ReadPack(), WritePack()
#define PACKET_OK   0   // ��� �訡��
#define PACKET_BUSY 1   // ��।��稪 �����
#define PACKET_NO   1   // ��� �����
#define PACKET_ERROR    2   // �訡�� �ਥ��

#endif
