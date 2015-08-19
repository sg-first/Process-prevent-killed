// HookTaskmgrDlg.h : header file
//

#if !defined(AFX_HookTaskmgrDLG_H__D5108457_09EB_4AD9_9EA7_7C40673585B3__INCLUDED_)
#define AFX_HookTaskmgrDLG_H__D5108457_09EB_4AD9_9EA7_7C40673585B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CHookTaskmgrDlg dialog

class CHookTaskmgrDlg : public CDialog
{
// Construction
public:
	CHookTaskmgrDlg(CWnd* pParent = NULL);	// standard constructor
	~CHookTaskmgrDlg();

// Dialog Data
	//{{AFX_DATA(CHookTaskmgrDlg)
	enum { IDD = IDD_HookTaskmgr_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHookTaskmgrDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	afx_msg void OnTimer(UINT nIDEvent);
	
	// Generated message map functions
	//{{AFX_MSG(CHookTaskmgrDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
public:
	afx_msg void OnBnClickedButton2();

	bool IsExistTaskmgr();
private:
	HWND m_hHookedWindow;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HookTaskmgrDLG_H__D5108457_09EB_4AD9_9EA7_7C40673585B3__INCLUDED_)
