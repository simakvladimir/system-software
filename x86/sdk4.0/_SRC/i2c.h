//#define SA_RTC		0xA0
//#define SA_EEPROM_RW	0xA2
//#define SA_EEPROM_RO	0xA4
//#define SA_DS1621	0xA6

#define SLAVE_ADDR	0xA0	
#define SLAVE_ADDR_MR26	0x80	
#define I2C_READ	0x1


extern void InitI2C(void);
extern void TestI2C(void);
extern char SendByteI2C(unsigned char x);
extern void StartI2C(void);
extern void StopI2C(void);
extern unsigned char ReceiveByteI2C(void);
extern unsigned char ReceiveByteI2Cna(void);
