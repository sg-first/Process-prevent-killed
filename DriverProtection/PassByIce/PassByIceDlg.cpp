// PassByIceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PassByIce.h"
#include "PassByIceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
HWND hwndOfIceSword=NULL,hwndOfTarget=NULL;
HANDLE   hProcess;
HANDLE handle=NULL;
#define IOCTL_MT_HIDE  \
CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 0x201, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_MT_HIDEME  \
CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 0x202, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
BOOL InstallDriver( IN SC_HANDLE SchSCManager, IN LPCTSTR DriverName, IN LPCTSTR ServiceExe )
{
    SC_HANDLE  schService;
	
    //
    // NOTE: This creates an entry for a standalone driver. If this
    //       is modified for use with a driver that requires a Tag,
    //       Group, and/or Dependencies, it may be necessary to
    //       query the registry for existing driver information
    //       (in order to determine a unique Tag, etc.).
    //
	
    schService = CreateService( SchSCManager,          // SCManager database
		DriverName,           // name of service
		DriverName,           // name to display
		SERVICE_ALL_ACCESS,    // desired access
		SERVICE_KERNEL_DRIVER, // service type
		SERVICE_DEMAND_START,  // start type
		SERVICE_ERROR_NORMAL,  // error control type
		ServiceExe,            // service's binary
		NULL,                  // no load ordering group
		NULL,                  // no tag identifier
		NULL,                  // no dependencies
		NULL,                  // LocalSystem account
		NULL                   // no password
		);
    if ( schService == NULL )
    {
		return FALSE;
	}
	
    CloseServiceHandle( schService );
	
    return TRUE;
}


/****************************************************************************
*
*    FUNCTION: StartDriver( IN SC_HANDLE, IN LPCTSTR)
*
*    PURPOSE: Starts the driver service.
*
****************************************************************************/
BOOL StartDriver( IN SC_HANDLE SchSCManager, IN LPCTSTR DriverName )
{
    SC_HANDLE  schService;
    BOOL       ret;
	
    schService = OpenService( SchSCManager,
		DriverName,
		SERVICE_ALL_ACCESS
		);
    if ( schService == NULL )
    {
		return FALSE;
	}
	
    ret = StartService( schService, 0, NULL )
		|| GetLastError() == ERROR_SERVICE_ALREADY_RUNNING 
		|| GetLastError() == ERROR_SERVICE_DISABLED;
	
	CloseServiceHandle( schService );
	
    return ret;
}
/****************************************************************************
*
*    FUNCTION: StopDriver( IN SC_HANDLE, IN LPCTSTR)
*
*    PURPOSE: Has the configuration manager stop the driver (unload it)
*
****************************************************************************/
BOOL StopDriver( IN SC_HANDLE SchSCManager, IN LPCTSTR DriverName )
{
    SC_HANDLE       schService;
    BOOL            ret;
    SERVICE_STATUS  serviceStatus;
	
    schService = OpenService( SchSCManager, DriverName, SERVICE_ALL_ACCESS );
    if ( schService == NULL )
        return FALSE;
	
    ret = ControlService( schService, SERVICE_CONTROL_STOP, &serviceStatus );
	
    CloseServiceHandle( schService );
	
    return ret;
}


/****************************************************************************
*
*    FUNCTION: RemoveDriver( IN SC_HANDLE, IN LPCTSTR)
*
*    PURPOSE: Deletes the driver service.
*
****************************************************************************/
BOOL RemoveDriver( IN SC_HANDLE SchSCManager, IN LPCTSTR DriverName )
{
    SC_HANDLE  schService;
    BOOL       ret;
	
    schService = OpenService( SchSCManager,
		DriverName,
		SERVICE_ALL_ACCESS
		);
	
    if ( schService == NULL )
        return FALSE;
	
    ret = DeleteService( schService );
	
    CloseServiceHandle( schService );
	
    return ret;
}


/****************************************************************************
*
*    FUNCTION: UnloadDeviceDriver( const TCHAR *)
*
*    PURPOSE: Stops the driver and has the configuration manager unload it.
*
****************************************************************************/
BOOL UnloadDeviceDriver( const TCHAR * Name )
{
	SC_HANDLE	schSCManager;
	
	schSCManager = OpenSCManager(	NULL,                 // machine (NULL == local)
		NULL,                 // database (NULL == default)
		SC_MANAGER_ALL_ACCESS // access required
								);
	
	StopDriver( schSCManager, Name );
	RemoveDriver( schSCManager, Name );
	
	CloseServiceHandle( schSCManager );
	
	return TRUE;
}



/****************************************************************************
*
*    FUNCTION: LoadDeviceDriver( const TCHAR, const TCHAR, HANDLE *)
*
*    PURPOSE: Registers a driver with the system configuration manager 
*	 and then loads it.
*
****************************************************************************/
BOOL LoadDeviceDriver( const TCHAR * Name, const TCHAR * Path, PDWORD Error )
{
	SC_HANDLE	schSCManager;
	BOOL		okay;
	
	schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	
	if(schSCManager)
	{
		// Remove previous instance
		RemoveDriver( schSCManager, Name );
		
		// Ignore success of installation: it may already be installed.
		InstallDriver( schSCManager, Name, Path );
		
		// Ignore success of start: it may already be started.
		okay = StartDriver( schSCManager, Name );
		
		*Error = GetLastError();
		CloseServiceHandle( schSCManager );
	}
	return okay;
}
BOOL CALLBACK EnumChildProc(HWND hwnd,LPARAM lParam)
{
	int isv;
	long style;	
	DWORD PID,bytesReturned;
	isv=1;//::IsWindowVisible(hwnd); 
	if(isv==1)
	{
		style=::GetWindowLong(hwnd,GWL_STYLE);
		if(style==349159424||style==365936640)     //为什么要两个呢？因为最大化和正常是不一样的
		{
			hwndOfIceSword=hwnd;
			GetWindowThreadProcessId(hwndOfIceSword, &PID);
			DeviceIoControl(handle,(DWORD)IOCTL_MT_HIDE,NULL,0,NULL,0,&bytesReturned,NULL);
			hProcess=OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
			if (!hProcess)
			{
                return FALSE;
			}
			hwnd=FindWindowEx(hwndOfIceSword,0,"AfxMDIFrame42s",0);
			hwnd=FindWindowEx(hwnd,0,"AfxMDIFrame42s",0);
			hwnd=FindWindowEx(hwnd,0,"SysListView32",0);
			hwndOfTarget=hwnd;
		}
	}
    return TRUE;                
}
void FindIceSword()
{
	::EnumChildWindows(::GetDesktopWindow(),EnumChildProc,0) ;
}
void Filter(int ItemCount)
{
	LVITEM lvitem, *plvitem;
	char ItemBuf[512],*pItem;
    for(int i=0;i<ItemCount;i++)
	{
        plvitem=(LVITEM*)VirtualAllocEx(hProcess, NULL, sizeof(LVITEM), MEM_COMMIT, PAGE_READWRITE);
		pItem=(char*)VirtualAllocEx(hProcess, NULL, 512, MEM_COMMIT, PAGE_READWRITE);
		if ((!plvitem)||(!pItem))
			
			return;
		
		else
		{
			lvitem.cchTextMax=512;
			//lvitem.iSubItem=1;//PID
			lvitem.iSubItem=0;  //ProcessName
			lvitem.pszText=pItem;
			WriteProcessMemory(hProcess, plvitem, &lvitem, sizeof(LVITEM), NULL);
			SendMessage(hwndOfTarget, LVM_GETITEMTEXT, (WPARAM)i, (LPARAM)plvitem);
			ReadProcessMemory(hProcess, pItem, ItemBuf, 512, NULL);
			//MessageBox(NULL,ItemBuf,ItemBuf,NULL);
			if (!strcmp(ItemBuf,"PassByIce.exe"))
			{
				SendMessage(hwndOfTarget, LVM_DELETEITEM, i, 0);
				break;
				//MessageBox(NULL,ItemBuf,ItemBuf,NULL);
			}
		}
		VirtualFreeEx(hProcess, plvitem, 0, MEM_RELEASE);
		VirtualFreeEx(hProcess, pItem, 0, MEM_RELEASE);
		//CloseHandle(hProcess);
	}
	CloseHandle(hProcess);
}
DWORD WINAPI ThreadProc( LPVOID lpParam ) 
{ 
	int iItem=0;
	while (TRUE)
	{	
		FindIceSword();	
		iItem=SendMessage(hwndOfTarget,LVM_GETITEMCOUNT,0,0);
		Filter(iItem);
		Sleep(50);	
	}
	return 0;
}
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
// CPassByIceDlg dialog

CPassByIceDlg::CPassByIceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPassByIceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPassByIceDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPassByIceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPassByIceDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPassByIceDlg, CDialog)
	//{{AFX_MSG_MAP(CPassByIceDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPassByIceDlg message handlers

BOOL CPassByIceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	char path[MAX_PATH];
	DWORD dwError,bytesReturned;
	GetSystemDirectory(path,MAX_PATH);
	sprintf(path+strlen(path),"\\%s", "drivers\\PassByIce.sys");
	HRSRC hr=FindResource(0,MAKEINTRESOURCE(IDR_SYS),"SYS");
	if(hr==NULL)
	{
		return false;
	}
	DWORD dwWritten,dwSize=SizeofResource(NULL,hr);
	HGLOBAL hg=LoadResource(NULL,hr);
    if(hg==NULL)
	{
		return false;
	}
	LPSTR lp=(LPSTR)LockResource(hg);
	if(lp==NULL)
	{
		return false;
	}
	HANDLE hFile;
	hFile=CreateFile(path,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
	if(hFile==NULL)
	{
		return false;
	}
	WriteFile(hFile,(LPCVOID)lp,dwSize,&dwWritten,NULL);
	CloseHandle(hFile);
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	//DeleteFile(path);
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
	LoadDeviceDriver( "PassByIce", path, &dwError);
	handle=CreateFile( "\\\\.\\PassByIce",
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
			);
	//DeleteFile(path);
	if (handle==INVALID_HANDLE_VALUE)
	{
		UnloadDeviceDriver("PassByIce");
		OnCancel();
	}
	DeviceIoControl(handle,(DWORD)IOCTL_MT_HIDE,NULL,0,NULL,0,&bytesReturned,NULL);
	DeviceIoControl(handle,(DWORD)IOCTL_MT_HIDEME,NULL,0,NULL,0,&bytesReturned,NULL);
	CreateThread(NULL,0,ThreadProc, NULL,0,NULL); 
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPassByIceDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CPassByIceDlg::OnPaint() 
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
HCURSOR CPassByIceDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

LRESULT CPassByIceDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch ( message )
	case WM_CLOSE:
	{
		UnloadDeviceDriver("PassByIce");
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CPassByIceDlg::OnOK() 
{
	CDialog::OnOK();
}

void CPassByIceDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	CDialog::OnCancel();
}
