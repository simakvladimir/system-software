//  �㭪樨 ������� �஢�� ��� ࠡ��� � I2C

void low_InitI2C(void);     // ���樠������ ���⮢ �����-�뢮��
void low_StartI2C(void);        // ���� I2C
void low_StopI2C(void);         // �⮯ I2C
unsigned char low_ReceiveByteI2C(void);      // �ਥ� ���� �� I2C
unsigned char low_ReceiveByteI2C_NA(void);   // �ਥ� ���� ��� ACK �� I2C
int low_SendByteI2C(unsigned char);     // ��।�� ���� �� I2C

