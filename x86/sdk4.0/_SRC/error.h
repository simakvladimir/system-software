#ifndef ERROR_

/*------------------------------------------------------------------------------
����� � �訡���� ��᫥����⥫쭮�� ������ � ��
------------------------------------------------------------------------------*/

#define ERROR_


extern ERROR_COM error_com[2];
extern ERROR_NET error_net[2];


extern void AddComError(BYTE,BYTE,BYTE);
extern void AddNetError(BYTE,BYTE,BYTE,BYTE);

// ���� ��� ���� ERROR_COM.ErrorType
#define ERROR_COM_OK        0 // ��� �訡��.                                                   
#define ERROR_COM_TIMEOUT   1 // �������.                                                      
#define ERROR_COM_PARITY    2 // �訡�� �⭮��.                                              
#define ERROR_COM_FRAME     3 // �訡�� �३��.                                                
#define ERROR_COM_MODE      4 // �訡�� ०���: �ᯮ�짮����� �������� �㭪権 � ०��� MCNF=0 
                                  // ��� ������� �㭪権 � ०��� MCNF=1                          
#define ERROR_COM_INTERFACE 5 // ����� �������⨬��� ����䥩� (COM1)                        


// ���� ��� ���� ERROR_NET.ErrorTr (ERROR_NET.ErrorRec)
#define ERROR_NET_OK        0 // ��� �訡��                                                                  
#define ERROR_NET_COLLISION 1 // �������� (��� IEEE 802.3 (Ethernet))                                        
#define ERROR_NET_JABBER    2 // ��䨪�஢�� Jabber (��� IEEE 802.3 (Ethernet) �������� ������ ����� 26 ��)
#define ERROR_NET_FULL      3 // ���� ��।�� �����                                                        
#define ERROR_NET_FIREWALL  4 // ����饭�� �� ⠡���� ࠧ�襭��� ���ᮢ                                   
#define ERROR_NET_LEN       5 // �訡�筠� ����� �����                                                      


#define ERROR_NET_TIMEOUT   1 // �������.                                                    
#define ERROR_NET_CRC       2 // �訡�� CRC.                                                 
#define ERROR_NET_MODE      3 // �訡�� ०���. �ᯮ�짮����� �⥢�� �㭪権 � ०��� MCNF=1
//#define ERROR_NET_FIREWALL    4 // ����饭�� �� ⠡���� ࠧ�襭��� ���ᮢ                   
#define ERROR_NET_LACK      5 // �ਭ�� ����� ���� 64 ���� (Ethernet)                      
#define ERROR_NET_SIZE      6 // �ਭ�� ����� ������� 1518 ����                              

#endif
