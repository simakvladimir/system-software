#include "compilers_4.h"
#include "lcd_lib_2.h"
#include "lookup.h"
#include <avr/pgmspace.h>
#include <stdio.h>

#define ADC_VREF_TYPE 0x00
#define VS 5//напряжение питания

#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

const uint8_t one[] PROGMEM   = {0,0,0,0,0,0,0,31};
//const uint8_t one[] PROGMEM   = {0,0,0,0,0,0,0,31};
const uint8_t two[] PROGMEM   = {0,0,0,0,0,0,31,31};
const uint8_t three[] PROGMEM   = {0,0,0,0,0,31,31,31};
const uint8_t four[] PROGMEM   = {0,0,0,0,31,31,31,31};
const uint8_t five[] PROGMEM   = {0,0,0,31,31,31,31,31};
const uint8_t six[] PROGMEM   = {0,0,31,31,31,31,31,31};
const uint8_t seven[] PROGMEM   = {0,31,31,31,31,31,31,31};
//const uint8_t eight[]  PROGMEM   = {31,31,31,31,31,31,31,31};
const uint8_t eight[]  PROGMEM   = {31,31,31,31,31,31,31,31,31,31,31};

int16_t read_adc(void);
void send_Uart_str(unsigned char *s);
void init_UART(void);
void send_Uart(unsigned char c);
void USART_SendByte(uint8_t u8Data);
void FurieTrans();
void timer1_init();
unsigned short adc_res;
int8_t  fx[WINDOW_SIZE];
int16_t Fu[WINDOW_SIZE/2][2];
int16_t mag = 0;
int16_t c = 0;
uint8_t val = 0x41;

int main( void )
{
	int i = 0;
	//инициализируем дисплей
	LCD_Init();
	init_UART();
	
	USART_SendByte(val);
	_delay_ms(250);
	ADMUX=ADC_VREF_TYPE & 0xff;
	ADCSRA=0x84;

	//загружаем новые символы
	LCD_SetUserChar(one,   0);
	LCD_SetUserChar(two,   1);
	LCD_SetUserChar(three, 2);
	LCD_SetUserChar(four,  3);
	LCD_SetUserChar(five,  4);
	LCD_SetUserChar(six,   5);
	LCD_SetUserChar(seven, 6);
	LCD_SetUserChar(eight, 7);

	//позиционируем курсор и выводим строки
	LCD_Goto(0,1);

	//выводим новый символ
	for (i=0; i<WINDOW_SIZE/2; i++)
	{
		LCD_WriteData(0);
		Fu[i][0]=0;
		Fu[i][1]=0;		
	}
	
	timer1_init();

	// uart conf	
	init_UART();                //  инициализация UART
	_delay_ms(1000);            //  задержка 1c
	send_Uart(0x01);// отправка строки
	send_Uart(0x01);// отправка строки
	
	ADMUX = (ADC_VREF_TYPE & 0xff);
	_delay_us(10);
	// main loop
	while(1)
	{
		//считываем 32 значения
		TCNT1 = 0;
		TIFR |= 1<<OCF1A;
		
		for(i=0; i<WINDOW_SIZE; i++)
		{
			while((TIFR & (1<<OCF1A)) == 0){
				// nop
			}
			fx[i] = read_adc() >> 3;
	
			//fx[i]/=64;
			TIFR |= 1<<OCF1A;
		}
		
		FurieTrans();
		
		// draw graph from 1kHz to 10 kHz		
		for(i=1; i<10; i++)
		{
			int8_t Re = (Fu[i][0] & 0xFF00) >> 8;
			int8_t Im = (Fu[i][1] & 0xFF00) >> 8;
			mag = (Re*Re + Im*Im);
			
			if((mag)>7)
			{
				mag -= 8;
				LCD_Goto(i-1,0);
				LCD_WriteData(mag);
				LCD_Goto(i-1,1);
				LCD_WriteData(7);
			}
			else
			{
				LCD_Goto(i-1,1);
				LCD_WriteData(mag);
			}
		}
		
		
	}
	return 0;
}

int16_t read_adc()
{
	int16_t x = 0;
	ADCSRA|=0x40;
	while ((ADCSRA & 0x10)==0);
	ADCSRA|=0x10;
	x = (int16_t)ADCL;
	x |= (int16_t)(ADCH<<8);
	return x;
}


int8_t fix_mul(int8_t a, int8_t b)
{
	int16_t c = (a * b) >> 6;
	b = c & 0x01;
	a = (c >> 1) + b;
	return a;
}

void FurieTrans()
{
	uint8_t i;
	uint8_t k;
	int16_t Re;
	int16_t Im;

	// zero array	
	for (i=0; i<WINDOW_SIZE/2;i++)
	{
		Fu[i][0]=0;
		Fu[i][1]=0;
	}
	
	/*
	fx[0] = 63;
	fx[1] = 75;
	fx[2] = 87;
	fx[3] = 98;
	fx[4] = 107;
	fx[5] = 115;
	fx[6] = 121;
	fx[7] = 124;
	fx[8] = 126;
	fx[9] = 124;
	fx[10] = 121;
	fx[11] = 115;
	fx[12] = 107;
	fx[13] = 98;
	fx[14] = 87;
	fx[15] = 75;
	fx[16] = 63;
	fx[17] = 50;
	fx[18] = 38;
	fx[19] = 27;
	fx[20] = 18;
	fx[21] = 10;
	fx[22] = 4;
	fx[23] = 1;
	fx[24] = 0;
	fx[25] = 1;
	fx[26] = 4;
	fx[27] = 10;
	fx[28] = 18;
	fx[29] = 27;
	fx[30] = 38;
	fx[31] = 50;*/ 
	
	
	// start dft from 1
	uint16_t arg_index;
	for (i = 1; i < WINDOW_SIZE/2; i++)
	{
		for (k=0; k<WINDOW_SIZE; k++)
		{
			arg_index = (k*i) % WINDOW_SIZE;

			Re = fix_mul(fx[k], Sinewave[arg_index+8]);// cos
			Im = fix_mul(fx[k], Sinewave[arg_index]);  // sin
			 
			Fu[i][0] +=  Re;
			Fu[i][1] +=  Im;
			
		}
	//	USART_SendByte(Fu[i][0] & 0xFF00);
//		USART_SendByte(Fu[i][0] & 0xff);
		
	//	USART_SendByte(Fu[i][1] & 0xFF00);
		//USART_SendByte(Fu[i][1] & 0xff);
			
		
		//_delay_ms(1000); 
	}

}

void timer1_init()
{
	TCCR1B = (1<<WGM12)|(1<<CS10);
	OCR1A = 250;
}

void init_UART(void)
{
   // Set baud rate
   UBRRL = BAUD_PRESCALE;// Load lower 8-bits into the low byte of the UBRR register
   UBRRH = (BAUD_PRESCALE >> 8); 
	 /* Load upper 8-bits into the high byte of the UBRR register
    Default frame format is 8 data bits, no parity, 1 stop bit
   to change use UCSRC, see AVR datasheet*/ 

   // Enable receiver and transmitter and receive complete interrupt 
   UCSRB = ((1<<TXEN)|(1<<RXEN) | (1<<RXCIE));
}

void USART_SendByte(uint8_t u8Data){

	// Wait until last byte has been transmitted
	while((UCSRA &(1<<UDRE)) == 0);

	// Transmit data
	UDR = u8Data;
}

void send_Uart(unsigned char c)//   Отправка байта
{
	while(!(UCSRA&(1<<UDRE)))    //  Устанавливается, когда регистр свободен
	UDR = c;
}

void send_Uart_str(unsigned char *s)//  Отправка строки
{
	while (*s != 0) send_Uart(*s++);
}

