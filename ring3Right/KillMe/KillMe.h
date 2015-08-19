// KillMe.h : main header file for the KILLME application
//

#if !defined(AFX_KILLME_H__72B4CC5E_FA3B_4EF2_AA6E_8F8D2E1BDF26__INCLUDED_)
#define AFX_KILLME_H__72B4CC5E_FA3B_4EF2_AA6E_8F8D2E1BDF26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CKillMeApp:
// See KillMe.cpp for the implementation of this class
//

class CKillMeApp : public CWinApp
{
public:
	CKillMeApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKillMeApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CKillMeApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KILLME_H__72B4CC5E_FA3B_4EF2_AA6E_8F8D2E1BDF26__INCLUDED_)
