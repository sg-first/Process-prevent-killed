// DetourDll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "stdio.h"

#include "DetourDll.h"

#include "detours.h"	//the main API header of detours

#pragma comment(lib,"detours.lib")


//////////////////////////////////////////////////////////////////////////
//custom API hook code here
//////////////////////////////////////////////////////////////////////////

//hook  
//HANDLE WINAPI OpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId )
HANDLE WINAPI CopyOpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId );
DETOUR_TRAMPOLINE(HANDLE WINAPI CopyOpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId ), OpenProcess);
HANDLE WINAPI MyOpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId )
{
	if (dwProcessId == GetPID())  
	{
		return NULL;
	}
	HANDLE nResult=CopyOpenProcess(dwDesiredAccess,bInheritHandle,dwProcessId);	//call origin function
	return nResult;
}

BOOL WINAPI CopyTerminateProcess(HANDLE hProcess,DWORD uExitCode);
DETOUR_TRAMPOLINE(BOOL WINAPI CopyTerminateProcess(HANDLE hProcess,DWORD uExitCode), TerminateProcess);
BOOL WINAPI MyTerminateProcess(HANDLE hProcess,DWORD uExitCode)
{
	HANDLE handle=::OpenProcess(PROCESS_ALL_ACCESS,true,GetPID());     
	if (hProcess == handle)  
	{
		return FALSE;
	}
	return CopyTerminateProcess(hProcess,uExitCode);	//call origin function
}

//////////////////////////////////////////////////////////////////////////


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			OutputDebugString("Detour dll Load!");
			DetourFunctionWithTrampoline((PBYTE)CopyOpenProcess, (PBYTE)MyOpenProcess);
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			OutputDebugString("Detour dll Exit!");
			DetourRemove((PBYTE)CopyOpenProcess, (PBYTE)MyOpenProcess);
			break;
    }
    return TRUE;
}


LRESULT WINAPI MsgProc(int code, WPARAM wParam, LPARAM lParam)
{
	//note :on windows 2k ,the 1st paramter to CallNextHookEx can be NULL
	//On win 98 ,it must be the hook handle
	return(CallNextHookEx(NULL,code,wParam,lParam));
}


void SetPID(DWORD dwPID)
{
// 	CHAR str[1024]="";
// 	wsprintf(str,"%d",dwPID);
// 	OutputDebugString(str);

	HKEY hKey=NULL;
	RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Peter\\DetourHook",&hKey);
	RegSetValueEx(hKey,NULL,0,REG_DWORD,(const LPBYTE)&dwPID,4);	//set value!!
	RegCloseKey(hKey);
}


DWORD GetPID()
{
	DWORD dwPID=0;
	

	DWORD dwType=REG_DWORD;
	DWORD dwSize=4;

	HKEY	hKey=NULL;
	RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Peter\\DetourHook",&hKey);
	RegQueryValueEx(hKey,NULL,0,&dwType,(LPBYTE)&dwPID,&dwSize);	//get value!!
	RegCloseKey(hKey);

	return dwPID;
}

//yacki added
HOOKPROC hkprcMsgProc=NULL;		//the pointer of MsgProc()
static HINSTANCE hinstDLL=NULL;		//the handle of dll!
static HHOOK hhookMsg=NULL;		//the handle of hook!

typedef void (*SETPID)(DWORD nPID);
void LoadProcess()
{
	hinstDLL = LoadLibrary((LPCTSTR)"NoKillDll.dll");	//hookdll.dll is in the same path

	SETPID fnSetPID=(SETPID)GetProcAddress(hinstDLL,"SetPID");
	if (NULL == fnSetPID)
	{
		return;
	}
	(fnSetPID)(GetCurrentProcessId());

	hkprcMsgProc = (HOOKPROC)GetProcAddress(hinstDLL, "MsgProc");	//get pointer of msg proc
	hhookMsg = SetWindowsHookEx(WH_GETMESSAGE,hkprcMsgProc,hinstDLL,0);	//0 mean hook all process! you can specify a process id,hook one process!
	//unsigned long id = GetCurrentProcessId();
	//printf("%l",GetCurrentProcessId());
}

void UnLoadProcess()
{
	if (hhookMsg)
	{
		UnhookWindowsHookEx(hhookMsg);	//uninstall hook!! 
	}
}

