// KillMeDlg.h : header file
//
//{{AFX_INCLUDES()
#include "webbrowser2.h"
//}}AFX_INCLUDES

#if !defined(AFX_KILLMEDLG_H__4EC643E8_D038_4D1B_BD6B_FB316AFFD886__INCLUDED_)
#define AFX_KILLMEDLG_H__4EC643E8_D038_4D1B_BD6B_FB316AFFD886__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CKillMeDlg dialog

class CKillMeDlg : public CDialog
{
// Construction
public:
	CKillMeDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CKillMeDlg();
// Dialog Data
	//{{AFX_DATA(CKillMeDlg)
	enum { IDD = IDD_KILLME_DIALOG };
	CWebBrowser2	m_ie;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKillMeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CKillMeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonProtect();
	afx_msg void OnButtonReset();
	afx_msg void OnButtonProtect2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KILLMEDLG_H__4EC643E8_D038_4D1B_BD6B_FB316AFFD886__INCLUDED_)
