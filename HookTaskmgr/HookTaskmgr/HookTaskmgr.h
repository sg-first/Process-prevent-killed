// HookTaskmgr.h : main header file for the HookTaskmgr application
//

#if !defined(AFX_HookTaskmgr_H__3EDB105C_0D8B_42B8_A00E_F31814C5420E__INCLUDED_)
#define AFX_HookTaskmgr_H__3EDB105C_0D8B_42B8_A00E_F31814C5420E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CHookTaskmgrApp:
// See HookTaskmgr.cpp for the implementation of this class
//

class CHookTaskmgrApp : public CWinApp
{
public:
	CHookTaskmgrApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHookTaskmgrApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL ExistInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CHookTaskmgrApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HookTaskmgr_H__3EDB105C_0D8B_42B8_A00E_F31814C5420E__INCLUDED_)
