#ifndef SERIAL_
#define SERIAL_

#define MODE_MASK   0x0018  // �������㥬� ���� � ॣ���� SP0CT

#define MCNF        0x0001  // ��᪠ ��� ०��� MODEM � sio_mode
#define RS485       0x4000  //��᪠ ��� ०��� RS(485)
#define EARLY       0x8000  //��᪠ ��� ��।������ ���ᨨ ����஫���
                //(ࠡ�� � ०��� RS485 �� ࠭��� ����� ���
                //�⫨砥��� �� ������� ���� - �ᯮ������
                //�� �஢�� �ࠩ���, 1 - ࠭��� �����)
#define SER     0x2000  //���� ��� ��।������ ࠡ��� �ࠩ��� � 
                //०��� MODEM (���뫪� ����⮢/�⤥����
                //���⮢, 1 - �����) 
#define SIO_DEBUG   0x1000  //�᫨ ��� 䫠� ��⠭�����, � �� 
                //��᫥����⥫�� ����� ����ࠨ������,
                //������ᨬ� �� ���祭�� ���譨� ��६�祪,
                //� ०��� No HS, 8, N, 232, Modem AND Net
extern WORD GetComError(ERROR_COM far *,BYTE);
                // ���ଠ�� �� �訡��� ��᫥����⥫쭮�� ������
extern void ResetComError(BYTE);
                // ���� �訡�� ��᫥����⥫쭮�� ������

extern WORD SetSioMode(CONFIG_COM far *ConfigCOM,BYTE Interface);
                // ��⠭���� ��ࠬ��஢ � ०���� ࠡ��� ��᫥����⥫쭮�� ������. 
extern WORD GetSioMode(CONFIG_COM far *ConfigCOM,BYTE Interface);
                // ���ଠ�� � ⥪�饬 ०��� ࠡ��� ��᫥����⥫쭮�� ������ 

#endif //SERIAL_
