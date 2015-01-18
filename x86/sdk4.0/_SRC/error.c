/*-----------------------------------------------------------------------------

		24.07.2001 API ���-1 v 1.2.1

		����� ��� ࠡ��� � �訡����
		----------------------------

	������஢���� �㭪樨:

	void AddComError(BYTE Interface,BYTE Error,BYTE ErrorType)
	void AddNetError(BYTE Interface,BYTE Error,BYTE ErrorTr,BYTE ErrorRec)

 ------------------------------------------------------------------------------
����: �㪨祢 �.�.

���������:
-------- ------ ----------- --------------------------------------------------- 
 ���	�����	����				���ᠭ��
-------- ------ ----------- --------------------------------------------------- 
18.08.01 1.1
20.08.01 1.2.1  ���祢 �.�. ����� ���ᨨ


-----------------------------------------------------------------------------*/
#include "mpkdefs.h"

ERROR_COM error_com[2];
ERROR_NET error_net[2];


// ���������� �訡�� COM 
void AddComError(BYTE Interface,BYTE Error,BYTE ErrorType)
{
	error_com[Interface&1].Error = Error;
	error_com[Interface&1].ErrorType = ErrorType;
	++error_com[Interface&1].ErrorCount;
	stat.ERROR_IO |= SM_ERROR_RS;
}

// ���������� �訡�� NET
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


