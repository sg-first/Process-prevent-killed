// HookTaskmgrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HookTaskmgr.h"
#include "HookTaskmgrDlg.h"
#include "exeDebuger.h"
#include "../SafeProcess/SafeProcess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT m_timerid = NULL;
#define  TASKMGR "Windows 任务管理器"

/*LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam);
HINSTANCE g_hInstance = NULL;
HHOOK  g_cbtHook = NULL;
HWND g_hWnd = NULL;
BOOL bSetWindowLong = FALSE;
WNDPROC g_orgProc =  NULL;

BOOL SafeProcessInit()
{
	if (g_cbtHook == NULL)
	{	
		g_cbtHook = SetWindowsHookEx(WH_CALLWNDPROC,CBTProc,g_hInstance, 0);
	}
	if (!g_cbtHook)
		return FALSE;

	_ASSERT(g_cbtHook);
	return TRUE;
}

void SafeProcessTerm()
{
	if (g_cbtHook != NULL) 
	{
		if( UnhookWindowsHookEx(g_cbtHook))
		{
			UnhookWindowsHookEx(g_cbtHook); 
			g_cbtHook = NULL;
		}
	}
}*/

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHookTaskmgrDlg dialog

CHookTaskmgrDlg::CHookTaskmgrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHookTaskmgrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHookTaskmgrDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hHookedWindow = NULL;
}

CHookTaskmgrDlg::~CHookTaskmgrDlg()
{
	if(m_timerid!=NULL)
	{
		KillTimer(m_timerid);
		m_timerid=NULL;
	}
	SafeProcessTerm();
}

void CHookTaskmgrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHookTaskmgrDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHookTaskmgrDlg, CDialog)
	//{{AFX_MSG_MAP(CHookTaskmgrDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CHookTaskmgrDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CHookTaskmgrDlg::OnBnClickedButton2)
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHookTaskmgrDlg message handlers

BOOL CHookTaskmgrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHookTaskmgrDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHookTaskmgrDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHookTaskmgrDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
//开始保护
void CHookTaskmgrDlg::OnBnClickedButton1()
{
	//启动定时器
	/*if (!m_timerid) 
	{
		m_timerid = SetTimer(1, 1000, NULL); // 1000 msec = 1 second
	}*/
	if (IsExistTaskmgr())
	{
		SetHandle(m_hHookedWindow);
	}
}

void CHookTaskmgrDlg::OnBnClickedButton2()
{ 
	//关闭定时器
	if(m_timerid!=NULL)
	{
		KillTimer(m_timerid);
		m_timerid=NULL;
	}
	//卸载钩子
	SafeProcessTerm();
}

bool CHookTaskmgrDlg::IsExistTaskmgr()
{
	m_hHookedWindow = NULL;
		
	return (m_hHookedWindow=::FindWindow(NULL, TASKMGR)) != NULL? true:false;
}

void CHookTaskmgrDlg::OnTimer(UINT nIDEvent)
{
	//检测任务管理器是否存在
	if (IsExistTaskmgr())
	{
		SetHandle(m_hHookedWindow);
	}
}
