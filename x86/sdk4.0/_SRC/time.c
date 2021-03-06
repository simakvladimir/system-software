/* ****************************************************************************

	24.07.2001 API ���-1 v 1.2.1


	�㭪樨 ��� ࠡ��� � ⠩��஬ � �ᠬ� ॠ�쭮�� �६���
	--------------------------------------------------------

extern WORD GetTime(DPTR time);		// ������� �६�
extern WORD SetTime(DWORD time);	// ��⠭����� �६�
extern WORD GetSystemRawTime(void);	// �㭪�� ��� ��।������  
					// ���� �६����� ���ࢠ���.
 ------------------------------------------------------------------------------
����: �㪨祢 �.�.

���������:
-------- ------ ----------- --------------------------------------------------- 
 ���	�����	����				���ᠭ��
-------- ------ ----------- --------------------------------------------------- 
18.08.01 1.1
20.08.01 1.2.1  ���祢 �.�. ����� ���ᨨ

 **************************************************************************** */

#include "186es.h"
#include "mpkdefs.h"
#include "time.h"
#include "low.h"
#include "ds2417.h"

/* ----------------------------------------------------------------------------
        		GetTime
 ----------------------------------------------------------------------------
������� �६�

�������	��� �訡��: 
		0 - ��� �訡��, 
		1 - RTC �� ����㯥�.

time		��६�����, ᮤ�ঠ�� ������⢮ ᥪ㭤, ��襤�� � 1970 ����.

�室�� ��ࠬ����	
time		��뫪� �� 32-� ࠧ�來�� ��६�����

���ᠭ��
�㭪��  �����뢠�� � 32-� ࠧ�來�� ��६����� time ������⢮ ᥪ㭤, 
��襤�� � 12:00 1 ﭢ��� 1970 �.

  ---------------------------------------------------------------------------- */

WORD GetTime(DPTR time)
{
if(GetDS2417(time)){
	stat.ERROR_IO |= SM_ERROR_RTC;
	return 1;
	}
return 0;
}


/* ----------------------------------------------------------------------------
         		SetTime
 ----------------------------------------------------------------------------
��⠭����� �६�


�������	��� �訡��: 
		0 - ��� �訡��, 
		1 - RTC �� ����㯥�.

�室�� ��ࠬ����	
Time	32-� ࠧ�來�� ��६����� � ������⢮� ᥪ㭤, ��襤訬 � 1970 ����

���ᠭ��
�㭪�� ��⠭�������� ��⥬��� �६� ���-1 �१ 32-� 
ࠧ�來�� ��६����� � ������⢮� ᥪ㭤, ��襤訬 � 12:00 1 ﭢ��� 1970 �.

  ---------------------------------------------------------------------------- */
WORD SetTime(DWORD time)
{
if(SetDS2417(time)){
	stat.ERROR_IO |= SM_ERROR_RTC;
	return 1;
	}
return 0;
}




/* ----------------------------------------------------------------------------
                GetSystemRawTime
 ----------------------------------------------------------------------------
�㭪�� ��� ��।������  ���� �६����� ���ࢠ���.

�������		����饥 ���祭�� 16-� ࠧ�來��� �����⭮�� ⠩���.

�室�� ��ࠬ����	���

���ᠭ��
�㭪�� �뤠�� ⥪�饥 ���祭�� �����⭮�� ⠩���. 
�ਬ�砭��.
����� ��� � ��ਮ� ��९������� �����⭮�� ⠩��� �������.

  ---------------------------------------------------------------------------- */

WORD GetSystemRawTime(void)
{
	return inpw(T1CNT);
}






