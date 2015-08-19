// ProcessType.cpp: implementation of the CProcessType class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "fastrun.h"
#include "ProcessType.h"
#include <afxmt.h> 
#include "RunType.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define  DEFAULT_BUFFER_LENGTH			10
#define  DEFAULT_BUFFER_INCREASE_STEP	3

//extern CRunType g_rtKernel;
extern CEvent g_Event;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CALLBACK EnumThreadWndProc(HWND hwnd,LPARAM);

CProcessType::CProcessType()
{
	m_nBufferLength = DEFAULT_BUFFER_LENGTH;
	m_nCurrentLength = 0;

	m_pProcessTypeBuffer = new ProcessType[m_nBufferLength]; 
}

CProcessType::~CProcessType()
{
	if(m_pProcessTypeBuffer)
		delete [] m_pProcessTypeBuffer;
}

pProcessType CProcessType::GetBufferPoint(int nIndex)
{
	if(!IsIndexValid(nIndex))
		return NULL;

	return m_pProcessTypeBuffer + nIndex;
}

BOOL CProcessType::TerminateAProcess(int nIndex)
{
	if(!IsIndexValid(nIndex))
		return FALSE;

	//::PostThreadMessage (m_pProcessTypeBuffer[nIndex].dwThreadId,WM_CLOSE,0,0);
	EnumThreadWindows(m_pProcessTypeBuffer[nIndex].dwThreadId,(WNDENUMPROC)EnumThreadWndProc,0);

	DWORD dwResult = ::WaitForSingleObject (m_pProcessTypeBuffer[nIndex].hThread,1000);

	if(dwResult == WAIT_TIMEOUT)
		::TerminateProcess (m_pProcessTypeBuffer[nIndex].hProcess,0);
	return TRUE;
}

BOOL CALLBACK EnumThreadWndProc(HWND hwnd,LPARAM)
{
	SendMessage(hwnd,WM_CLOSE,0,0);

	return TRUE;
}