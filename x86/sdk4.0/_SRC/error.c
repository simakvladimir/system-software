/*-----------------------------------------------------------------------------

		24.07.2001 API МПК-1 v 1.2.1

		Модуль для работы с ошибками
		----------------------------

	Низкоуровневые функции:

	void AddComError(BYTE Interface,BYTE Error,BYTE ErrorType)
	void AddNetError(BYTE Interface,BYTE Error,BYTE ErrorTr,BYTE ErrorRec)

 ------------------------------------------------------------------------------
Автор: Лукичев А.Н.

Изменения:
-------- ------ ----------- --------------------------------------------------- 
 Дата	Версия	Автор				Описание
-------- ------ ----------- --------------------------------------------------- 
18.08.01 1.1
20.08.01 1.2.1  Ключев А.О. Смена версии


-----------------------------------------------------------------------------*/
#include "mpkdefs.h"

ERROR_COM error_com[2];
ERROR_NET error_net[2];


// Добавление ошибки COM 
void AddComError(BYTE Interface,BYTE Error,BYTE ErrorType)
{
	error_com[Interface&1].Error = Error;
	error_com[Interface&1].ErrorType = ErrorType;
	++error_com[Interface&1].ErrorCount;
	stat.ERROR_IO |= SM_ERROR_RS;
}

// Добавление ошибки NET
void AddNetError(BYTE Interface,BYTE Error,BYTE ErrorTr,BYTE ErrorRec)
{
	error_net[Interface&1].Error = Error;
	error_net[Interface&1].ErrorTr = ErrorTr;
	error_net[Interface&1].ErrorRec = ErrorRec;
	++error_net[Interface&1].ErrorCount;
	if ( Interface==SER0 )
		stat.ERROR_IO |= SM_ERROR_RS;
	else
		stat.ERROR_IO |= SM_ERROR_ETH;

}


