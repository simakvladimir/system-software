//  Функции низкого уровня для работы с I2C

void low_InitI2C(void);     // Инициализация портов ввода-вывода
void low_StartI2C(void);        // Старт I2C
void low_StopI2C(void);         // Стоп I2C
unsigned char low_ReceiveByteI2C(void);      // Прием байта из I2C
unsigned char low_ReceiveByteI2C_NA(void);   // Прием байта без ACK из I2C
int low_SendByteI2C(unsigned char);     // Передача байта по I2C

