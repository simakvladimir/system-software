#ifndef FLASH

#define FLASH

//����⠭�� ��� ���ᠭ�� १���� ࠡ��� �㭪権 FLASH
#define FLASH_OK	0
#define FLASH_BUSY	1
#define FLASH_ERROR	2

extern WORD EraseSector(DWORD);		//��࠭�� ᥪ��
extern WORD GetStatusFLASH(void);	//�஢�ઠ ����砭�� ����� ��࠭�� ᥪ��
extern WORD WriteFLASH(DWORD,BPTR,WORD);
					//������ ���ᨢ� ����� �� FLASH

#endif




