#include "StdAfx.h"
#include <afxmt.h> 
#include "ProProcess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define FASTRUN_APP_EVENT_NAME		_T("Fast_Run_App_Event_Name_By_")
#define FR_PROTECT_EVENT_NAME		_T("Fr_Protect_Event_Name_By_")
#define FASTRUN_NOTIFY_EVENT_NAME	_T("Fast_Run_Notify_Event_Name_")

#define TEMPPROCESSNAME                 _T("TempProcess.exe")

//CRunType g_rtKernel;
CString m_strAppPath;
CEvent g_Event;
CLogFile g_Log;
//extern BOOL bServerRunning;

HANDLE CProProcess::m_hEvent = NULL;
HANDLE CProProcess::m_hMemMap = NULL;
LPVOID CProProcess::m_pMemPointer = NULL;
CWinThread* CProProcess::m_thTimeThread = NULL; 
bool CProProcess::m_bIsProtect = false;  
CString CProProcess::m_sProcess = _T("");

BOOL MyRegister(BOOL bFlag)
{
	// register Server process
	HINSTANCE hKernel32=::LoadLibrary(_T("kernel32"));
	DWORD (WINAPI *fn)(DWORD,DWORD);
	DWORD id=::GetCurrentProcessId();
	if(hKernel32)
	{
		fn=(DWORD(WINAPI *)(DWORD,DWORD))::GetProcAddress ((HMODULE)hKernel32,("RegisterServiceProcess"));
		if(fn)
			(*fn)(id,bFlag);
		::FreeLibrary (hKernel32);
		return TRUE;
	}
	// end
	return FALSE;
}

void InitModulePath()
{
	TCHAR buffer[260]={0};
	GetModuleFileName(AfxGetInstanceHandle(),buffer,260);

	m_strAppPath = buffer;

	int i;
	for(i=lstrlen(buffer)-1;i>=0;i--)
		if(buffer[i]=='\\')
		{
			buffer[i+1] = '\0';
			break;
		}

	m_strAppPath = buffer;
}

UINT TimeCheckingThreadFunc(LPVOID)
{
	BOOL bFirstTime=TRUE;
	//int nCalcu =0;

	while(TRUE)
	{
		if(!bFirstTime)
			Sleep(100);
		else
			bFirstTime=FALSE;

		/*if(nCalcu>=50)
		{
			nCalcu =0;
			g_rtKernel.ThreadChecking ();
		}
		else
			nCalcu ++;*/

		if(CProProcess::IsProtect())
		{
			TCHAR pProtectEventName[256]={0};
			lstrcpy(pProtectEventName,FR_PROTECT_EVENT_NAME); 
			lstrcat(pProtectEventName,CProProcess::GetProcessName());
			HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,pProtectEventName);
			if(!hEvent)
			{
				STARTUPINFO StartInfo = {sizeof(STARTUPINFO)}; 
				PROCESS_INFORMATION ProcessInfo = {0};
				TCHAR cBuffer[512]={0};
				lstrcpy(cBuffer,m_strAppPath); 
				lstrcat(cBuffer,TEMPPROCESSNAME);  //启动中间进程 

				TCHAR pCommandLine[50]  = {0};
				lstrcpy(pCommandLine,_T(" "));  
				lstrcat(pCommandLine,CProProcess::GetProcessName());  
				BOOL bSuccessful = CreateProcess(cBuffer,pCommandLine,NULL,NULL,
					FALSE,0,NULL,NULL,&StartInfo,&ProcessInfo);

				if(bSuccessful)
				{
					g_Log.WriteLine(_T("\nCreate Proprocess success"));  
					CloseHandle(ProcessInfo.hProcess);
					CloseHandle(ProcessInfo.hThread);
					Sleep(500); 
				}
			}
			else
				CloseHandle(hEvent); 
		}
	}
	return 0;
}

void CProProcess::CleanProProcess()
{
	if (!m_bIsProtect) 
	{ 
		g_Log.WriteLine(_T("\nCleanProProcess no Protected"));
		MyRegister(FALSE);  
		::CloseHandle (m_hEvent);
		*((int*)m_pMemPointer) = 1; 
		UnmapViewOfFile (m_pMemPointer); 
		CloseHandle(m_hMemMap);
	}
	else
	{
		g_Log.WriteLine(_T("\nCleanProProcess Protected"));
	}
}

void CProProcess::InitProProcess(CString sProcess)
{
	InitLogFile();
	g_Log.WriteLine(_T("\nInitProProcess"));
	m_bIsProtect = false;
	m_hEvent = NULL;
	m_hMemMap = NULL;
	m_pMemPointer = NULL;
	m_thTimeThread = NULL;
	m_sProcess = sProcess;

	if (!m_hEvent)
	{
		TCHAR pAppEventName[256]={0};
		lstrcpy(pAppEventName,FASTRUN_APP_EVENT_NAME);
		lstrcat(pAppEventName,sProcess);
		m_hEvent = ::CreateEvent (NULL,FALSE,FALSE,pAppEventName);
	}
	if (!m_hMemMap)
	{
		TCHAR pAppNotifyName[256]={0};
		lstrcpy(pAppNotifyName,FASTRUN_NOTIFY_EVENT_NAME);
		lstrcat(pAppNotifyName,sProcess);
		m_hMemMap = ::CreateFileMapping (INVALID_HANDLE_VALUE,NULL,
			PAGE_READWRITE,0,sizeof(int),pAppNotifyName);
	}
	m_pMemPointer = ::MapViewOfFile (m_hMemMap,FILE_MAP_WRITE,0,0,0);
	*((int*)m_pMemPointer) = 0;

	MyRegister(TRUE);
	InitModulePath();

	if (!m_thTimeThread)
	{
		m_thTimeThread= ::AfxBeginThread (TimeCheckingThreadFunc,
			NULL,
			THREAD_PRIORITY_BELOW_NORMAL);
	}
	
	//默认不是保护 就必须通过配置文件记录保护属性
	//得到配置文件的属性
	if (GetProfileSetting())
		StartProtect();
	else
		StopProtect();
}

bool CProProcess::StartProtect()
{		
	g_Log.WriteLine(_T("\nStartProtect"));   
	m_bIsProtect = true;  
	SetProfileSetting(m_bIsProtect);

	if (m_hMemMap)
	{
		m_pMemPointer = ::MapViewOfFile (m_hMemMap,FILE_MAP_WRITE,0,0,0); 
		*((int*)m_pMemPointer) = 0;
	}
	return true;
}

bool CProProcess::StopProtect()
{
	g_Log.WriteLine(_T("\nStopProtect"));
	m_bIsProtect = false;

	SetProfileSetting(m_bIsProtect);

	if (m_hMemMap)
	{
		m_pMemPointer = ::MapViewOfFile (m_hMemMap,FILE_MAP_WRITE,0,0,0); 
		*((int*)m_pMemPointer) = 1;
	}
	return true;
}

bool CProProcess::IsProtect()
{
	if (m_bIsProtect)
		g_Log.WriteLine(_T("\nget protected"));
	else
		g_Log.WriteLine(_T("\nget no protected")); 
	return m_bIsProtect ;
}

CString CProProcess::GetProcessName()
{
	return m_sProcess;
}

void CProProcess::InitLogFile()
{
	CString strAppPath = m_strAppPath;
	strAppPath += _T("proprocesslog.txt");
	g_Log.SetFileName(strAppPath);    
	g_Log.Init();  
	g_Log.WriteLine(_T("\nInitLogFile")); 
}

void CProProcess::SetProfileSetting(bool bProtect) 
{
	TCHAR pInitFile[256]={0};
	lstrcpy(pInitFile,m_strAppPath);
	lstrcat(pInitFile,_T("ProcessSet.ini"));

	TCHAR pValue[255]={0};
	char *sValue = new char[2] ;
	_itoa_s(int(bProtect),sValue,2,10) ;
	CString strValue = CString(sValue);
	lstrcpy(pValue,strValue);   
	WritePrivateProfileString(_T("Setting"),_T("Protect"),pValue,pInitFile);
	delete[] sValue;
	sValue = NULL;
}

bool CProProcess::GetProfileSetting() 
{
	TCHAR pInitFile[512]={0};
	lstrcpy(pInitFile,m_strAppPath);
	lstrcat(pInitFile,_T("ProcessSet.ini"));

	TCHAR pValue[255]={0};
	GetPrivateProfileString(_T("Setting"),_T("Protect"),_T(""),pValue,255,pInitFile); 
	return atoi(LPSTR(pValue)); 
}