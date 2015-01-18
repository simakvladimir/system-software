#ifndef FLASH

#define FLASH

//Константы для описания результата работы функций FLASH
#define FLASH_OK	0
#define FLASH_BUSY	1
#define FLASH_ERROR	2

extern WORD EraseSector(DWORD);		//Стирание сектора
extern WORD GetStatusFLASH(void);	//Проверка окончания процесса стирания сектора
extern WORD WriteFLASH(DWORD,BPTR,WORD);
					//Запись массива памяти во FLASH

#endif




