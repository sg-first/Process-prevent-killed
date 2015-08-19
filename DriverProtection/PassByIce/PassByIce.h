// PassByIce.h : main header file for the PASSBYICE application
//

#if !defined(AFX_PASSBYICE_H__7F7F00E8_F9BB_4787_8E9B_F374464504F3__INCLUDED_)
#define AFX_PASSBYICE_H__7F7F00E8_F9BB_4787_8E9B_F374464504F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPassByIceApp:
// See PassByIce.cpp for the implementation of this class
//

class CPassByIceApp : public CWinApp
{
public:
	CPassByIceApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPassByIceApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPassByIceApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSBYICE_H__7F7F00E8_F9BB_4787_8E9B_F374464504F3__INCLUDED_)
