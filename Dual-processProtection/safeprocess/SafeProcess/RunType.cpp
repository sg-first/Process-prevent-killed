// RunType.cpp: implementation of the CRunType class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RunType.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define WM_MYTIME_MESSAGE		(WM_USER+724)
#define WM_MYTIME_RUN_MESSAGE	(WM_USER+734)

CRunType::CRunType()
{
	m_nBufferLength = m_nCurrentLength =0 ;
	m_pRunTypeBuffer = NULL;
	for(int i=0;i<7;i++)
	{
		m_stSuperTypeBuffer[i].bEnable =TRUE;
		m_stSuperTypeBuffer[i].bPrompt =TRUE;
//		m_stSuperTypeBuffer[i].dwReserve =0;
		m_stSuperTypeBuffer[i].nHour = 0;
		m_stSuperTypeBuffer[i].nMinute =0;
		m_stSuperTypeBuffer[i].nAheadMinutes =5;
		m_stSuperTypeBuffer[i].nTask =2;
	}
}

CRunType::~CRunType()
{
	if(m_pRunTypeBuffer)
		delete [] m_pRunTypeBuffer;
}

///////////////////////////////////////////////////////////////////////////////
// 函数：BOOL CRunType::RegisterAllItemsHotKey()
// 注册所有的热键
// 成功的话就返回1
////////////////////////////////////////////////////////////////////////////////
BOOL CRunType::RegisterAllItemsHotKey()
{
	for(int i=0;i<m_nCurrentLength;i++)
	{
		if(m_pRunTypeBuffer[i].uHotKeyVk)
		{
			if(!::RegisterHotKey (m_hMainWnd,m_pRunTypeBuffer[i].uHotKeyID ,
								  m_pRunTypeBuffer[i].uHotKeyFlag ,
								  m_pRunTypeBuffer[i].uHotKeyVk ))
			{
				m_pRunTypeBuffer[i].uHotKeyVk =0;
				m_pRunTypeBuffer[i].uHotKeyFlag =0;
			}
		}
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 函数：BOOL CRunType::UnRegisterAllItemsHotKey()
// 注销所有的热键
////////////////////////////////////////////////////////////////////////////////
void CRunType::UnRegisterAllItemsHotKey()
{
	for(int i=0;i<m_nCurrentLength;i++)
	{
		if(m_pRunTypeBuffer[i].uHotKeyVk)			
			::UnregisterHotKey (m_hMainWnd ,m_pRunTypeBuffer[i].uHotKeyID );
	}
}

///////////////////////////////////////////////////////////////////////////////
// 函数：int CRunType::SearchTheID(UINT uID)
// UINT uID:代表需要在缓冲区中查找的id
// 如果成功找到则返回索引
// 否则返回－1
////////////////////////////////////////////////////////////////////////////////
int CRunType::SearchTheID(UINT uID)
{
	for(int i=0;i<m_nCurrentLength;i++)
		if(m_pRunTypeBuffer[i].uHotKeyID == uID)
			return i;
	return -1;
}

void CRunType::SetHotKeyName(LPTSTR source, WORD vk, WORD flag)
{
	static TCHAR ctrl[]=_T("Ctrl+");
	static TCHAR alt[]=_T("Alt+");
	static TCHAR shift[]=_T("Shift+");
	static TCHAR tab[]=_T("\t");
	TCHAR sVk[20];

	lstrcat(source,tab);
	if((flag & HOTKEYF_CONTROL)==HOTKEYF_CONTROL)
		lstrcat(source,ctrl);
	if((flag & HOTKEYF_ALT)==HOTKEYF_ALT)
		lstrcat(source,alt);
	if((flag & HOTKEYF_SHIFT)==HOTKEYF_SHIFT)
		lstrcat(source,shift);

	if(vk>=48 && vk<=57)
		_stprintf(sVk,_T("%d"),vk-48);
	else if(vk>=65 && vk<=90)
		_stprintf(sVk,_T("%c"),vk);
	else
		sVk[0]=0;
	lstrcat(source,sVk);
}

void CRunType::ThreadChecking()
{
	CTime tTime=CTime::GetCurrentTime ();
	int index=tTime.GetDayOfWeek () -1;
	int hour=tTime.GetHour ();
	int minute=tTime.GetMinute ();

	if(!(m_stSuperTypeBuffer[index].bEnable))
	{
		if(!((hour>m_stSuperTypeBuffer[index].nHour) || 
			((hour==m_stSuperTypeBuffer[index].nHour) && 
			(minute>m_stSuperTypeBuffer[index].nMinute ))))
		{
			if((hour == m_stSuperTypeBuffer[index].nHour) && (minute == m_stSuperTypeBuffer[index].nMinute))
			{
				SendMessage (m_hMainWnd,WM_MYTIME_MESSAGE,FALSE,(LPARAM)index);
			}
			else
			{
				if(m_stSuperTypeBuffer[index].bPrompt)
					tTime+=CTimeSpan(0,0,m_stSuperTypeBuffer[index].nAheadMinutes ,0);
				hour=tTime.GetHour ();
				minute=tTime.GetMinute ();
				if((hour>m_stSuperTypeBuffer[index].nHour) || 
					((hour==m_stSuperTypeBuffer[index].nHour) && 
					(minute>=m_stSuperTypeBuffer[index].nMinute )))
				{
					SendMessage (m_hMainWnd,WM_MYTIME_MESSAGE,TRUE,0);
				}
			}
		}
	}
	//////////////////end task 1 /////////////////////////////
	tTime=CTime::GetCurrentTime ();
	index=tTime.GetDayOfWeek () -1;
	hour=tTime.GetHour ();
	minute=tTime.GetMinute ();
	for(int i=0;i<m_nCurrentLength;i++)
	{
		if((!m_pRunTypeBuffer[i].bRuned) && (m_pRunTypeBuffer[i].bRunWithTime))
		{
			CTime tTime1(tTime.GetYear (),tTime.GetMonth(),tTime.GetDay (),
						m_pRunTypeBuffer[i].nHour ,m_pRunTypeBuffer[i].nMinute ,tTime.GetSecond ());
			if(tTime1<=tTime)
			{
				SendMessage (m_hMainWnd,WM_MYTIME_RUN_MESSAGE,(WPARAM)i,0);
				m_pRunTypeBuffer[i].bRuned = TRUE;
			}
		}
	}
}

int CRunType::GetSuperTypeTask(int nIndex)
{
	if(nIndex <0 || nIndex >= 7)
		return -1;

	return m_stSuperTypeBuffer[nIndex].nTask ;
}
