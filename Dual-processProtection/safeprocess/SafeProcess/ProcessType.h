// ProcessType.h: interface for the CProcessType class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSTYPE_H__647E2FAE_4F79_440F_8096_3CFE85A69244__INCLUDED_)
#define AFX_PROCESSTYPE_H__647E2FAE_4F79_440F_8096_3CFE85A69244__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "fastrun.h"

typedef struct ProcessType
{
	TCHAR	strName[52];
	HANDLE	hProcess;
	HANDLE	hThread;
	DWORD	dwProcessId;
	DWORD	dwThreadId;
} ProcessType,*pProcessType;

class CProcessType  
{
public:
	CProcessType();
	virtual ~CProcessType();
	int		GetCurrentProcessNumber() const { return m_nCurrentLength;}
	pProcessType GetBufferPoint(int nIndex);
	BOOL TerminateAProcess(int nIndex);
private:
	BOOL IsIndexValid(int nIndex)
	{
		if(nIndex <0 || nIndex>= m_nCurrentLength)
		{
			ASSERT(0);
			return FALSE;
		}
		return TRUE;
	}

private:
	pProcessType	m_pProcessTypeBuffer;
	int				m_nCurrentLength;
	int				m_nBufferLength;
};

#endif // !defined(AFX_PROCESSTYPE_H__647E2FAE_4F79_440F_8096_3CFE85A69244__INCLUDED_)
