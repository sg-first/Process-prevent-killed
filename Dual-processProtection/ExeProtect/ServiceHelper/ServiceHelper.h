//////////////////////////////////////////////////////////////////////
// Author :- Nish 
// ServiceHelper.h: interface for the CServiceHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVICEHELPER_H__7489A32A_B469_42E8_A720_A337CC3B0940__INCLUDED_)
#define AFX_SERVICEHELPER_H__7489A32A_B469_42E8_A720_A337CC3B0940__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsvc.h>

struct SERVICEINFO
{
public:
	LPCTSTR lpServiceName;
	LPCTSTR lpDisplayName;
	LPCTSTR lpBinaryPathName;
	BOOL bAutoStart;

	SERVICEINFO();
};

class CServiceHelper  
{
public:
	BOOL Continue();
	BOOL Pause();
	BOOL Stop();
	BOOL Start();
	BOOL Delete();
	BOOL Create();
	
	CServiceHelper();
	virtual ~CServiceHelper();

private:
	SC_HANDLE m_scm;
	SERVICEINFO m_serviceinfo;

public:
	void SetServiceName(LPCTSTR str);
	void SetServiceDisplayName(LPCTSTR str);
	void SetServicePath(LPCTSTR str);
	void SetAutoStart(BOOL b);
};


#endif // !defined(AFX_SERVICEHELPER_H__7489A32A_B469_42E8_A720_A337CC3B0940__INCLUDED_)
