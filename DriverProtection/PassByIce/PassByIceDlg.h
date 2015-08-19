// PassByIceDlg.h : header file
//

#if !defined(AFX_PASSBYICEDLG_H__B63B375C_1AD8_4F86_871C_F80E925A592D__INCLUDED_)
#define AFX_PASSBYICEDLG_H__B63B375C_1AD8_4F86_871C_F80E925A592D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPassByIceDlg dialog

class CPassByIceDlg : public CDialog
{
// Construction
public:
	
	CPassByIceDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPassByIceDlg)
	enum { IDD = IDD_PASSBYICE_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPassByIceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPassByIceDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSBYICEDLG_H__B63B375C_1AD8_4F86_871C_F80E925A592D__INCLUDED_)
