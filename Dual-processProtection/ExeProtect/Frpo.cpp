// Frpo.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Frpo.h"
#include "ServiceHelper/ServiceHelper.h"
#include "MyLog.h"
/////////////////////////////////////////////////////////////////////////////
#define FASTRUN_APP_EVENT_NAME		_T("Fast_Run_App_Event_Name_By_")
#define FR_PROTECT_EVENT_NAME		_T("Fr_Protect_Event_Name_By_")
#define FASTRUN_NOTIFY_EVENT_NAME	_T("Fast_Run_Notify_Event_Name_")


#define MAINFILENAME                 _T("SafeProcess.exe")
#define SERVICENAME                 _T("SafeProcess")

/////////////////////////////////////////////////////////////////////////////
// Global Variables:
BOOL RegisterService(BOOL bFlag);
void InitMainAppPathName(HINSTANCE hInst);
void InitServiceName();

TCHAR g_strMainAppPathName[256]={'\0'};
// Foward declarations of functions included in this code module:


int APIENTRY WinMain(HINSTANCE hInst,
                     HINSTANCE,
                     LPSTR lpCmdLine,
                     int)
{
	int nRetry = 10;

	TCHAR pAppEventName[256]={0};
	lstrcpy(pAppEventName,FASTRUN_APP_EVENT_NAME);
	lstrcat(pAppEventName,lpCmdLine);

	TCHAR pProtectEventName[256]={0};
	lstrcpy(pProtectEventName,FR_PROTECT_EVENT_NAME); 
	lstrcat(pProtectEventName,lpCmdLine);  

	TCHAR pAppNotifyName[256]={0};
	lstrcpy(pAppNotifyName,FASTRUN_NOTIFY_EVENT_NAME);
	lstrcat(pAppNotifyName,lpCmdLine);

	::SetThreadPriority (::GetCurrentThread (),THREAD_PRIORITY_BELOW_NORMAL);

	RegisterService(TRUE);

	#ifdef _NTSERVICE_
		InitServiceName();
	#else
		InitMainAppPathName(hInst);
	#endif

	HANDLE hEvent = ::CreateEvent (NULL,FALSE,FALSE,pProtectEventName);  

//	HANDLE hNotifyEvent = CreateEvent(NULL,TRUE,FALSE,pAppNotifyName);
	HANDLE hMemMap = ::CreateFileMapping (INVALID_HANDLE_VALUE,NULL,
										  PAGE_READONLY,0,sizeof(int),pAppNotifyName);
	while(1) 
	{
//		DWORD dwResult = WaitForSingleObject(hNotifyEvent,1);
//		if(dwResult == WAIT_OBJECT_0)
//			break;
		LPVOID pMemPointer = ::MapViewOfFile (hMemMap,FILE_MAP_READ,0,0,0);
		if(*((int*)pMemPointer) !=0) 
		{
			UnmapViewOfFile (pMemPointer);
			break;
		}
		else
			UnmapViewOfFile (pMemPointer);

		HANDLE hFrEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,pAppEventName);
		if(!hFrEvent)
		{	
		#ifdef _NTSERVICE_
			{
				CServiceHelper service;
				service.SetServiceName(g_strMainAppPathName);
				BOOL bSuccessful = service.Start();
				//判断是否注册服务
				if(!bSuccessful) 
				{
					DWORD dwErr = GetLastError();
					CHAR str[255]="";
					wsprintf(str,"\n启动服务'%s'[err:%d]失败!",g_strMainAppPathName,dwErr);
					OutputDebugString(str);
					
					//重试
					if ( dwErr == 1056 && nRetry-- > 0 )
					{
						memset(str,0,255);
						wsprintf(str,"\n重试启动服务'%s'[%d]!",g_strMainAppPathName,nRetry);
						OutputDebugString(str);
						Sleep(50);
						continue;
					}
				}
				/*else
				{
					CHAR str[255]="";
					wsprintf(str,"\n启动服务'%s'成功!",g_strMainAppPathName);
				}*/
			} 
		#else
			{
				STARTUPINFO StartInfo = {sizeof(STARTUPINFO)};
				PROCESS_INFORMATION ProcessInfo = {0};
				BOOL bSuccessful = CreateProcess(g_strMainAppPathName,NULL,NULL,NULL,
					FALSE,0,NULL,NULL,&StartInfo,&ProcessInfo);

				if(bSuccessful)
				{
					CloseHandle(ProcessInfo.hProcess);
					CloseHandle(ProcessInfo.hThread);
				}
				else
				{	
					CHAR str[255]="";
					wsprintf(str,"创建进程'%s'失败!",g_strMainAppPathName);
					OutputDebugString(str);
				}
			}
		#endif

			break; 
		}
		else
		{
			CloseHandle(hFrEvent);
		}
		Sleep(100);
	}

	::CloseHandle(hEvent);
	CloseHandle(hMemMap);
//	CloseHandle(hNotifyEvent);
	RegisterService(FALSE);

	return 0;
}

void InitMainAppPathName(HINSTANCE hInst)
{
	GetModuleFileName(hInst,g_strMainAppPathName,255*sizeof(TCHAR));

	for(int i=lstrlen(g_strMainAppPathName)-1;i>=0;i--)
		if(g_strMainAppPathName[i]=='\\')
		{
			g_strMainAppPathName[i+1] = '\0';
			break;
		}
	lstrcat(g_strMainAppPathName,MAINFILENAME);
}

void InitServiceName()
{
	lstrcpy(g_strMainAppPathName,SERVICENAME); 
}

BOOL RegisterService(BOOL bFlag)
{
	// register Server process
	HINSTANCE hKernel32=::LoadLibrary(_T("kernel32"));
	DWORD (WINAPI *fn)(DWORD,DWORD);
	DWORD id=GetCurrentProcessId();
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