#include "StdAfx.h"
#include "WriteMemory.h"
#include "kwindef.h"

DWORD CWriteMemory::m_dwFixData = 0;

BOOL  CWriteMemory::WriteMemory(const DWORD  dwFixData)
{
	ZWQUERYSYSTEMINFORMATION		ZwQuerySystemInformation = NULL;
	NTSYSTEMDEBUGCONTROL			NtSystemDebugControl = NULL;
	HANDLE hThread = NULL;
	DWORD	dwPID = GetCurrentProcessId();
	DWORD	dwTID = GetCurrentThreadId();
	PSYSTEM_HANDLE_INFORMATION		pHandleInfo = NULL;
	ULONG	uObjCnt = 0;
	NTSTATUS	status;
	DWORD	dwBufLen = 1024*800;
	DWORD	dwRetLen = 1024*800;
	DWORD	dwETHREAD = 0;
	BOOL	bRet = FALSE;
	CWriteMemory::m_dwFixData = dwFixData;

	CWriteMemory::EnableDebugPrivilege(TRUE);    

	ZwQuerySystemInformation = (ZWQUERYSYSTEMINFORMATION)GetProcAddress(LoadLibrary("ntdll.dll"),"ZwQuerySystemInformation");
	NtSystemDebugControl = (NTSYSTEMDEBUGCONTROL)GetProcAddress(LoadLibrary("ntdll.dll"),"NtSystemDebugControl");

	//先创建一个线程对象
	GetProcAddress(LoadLibrary("kernel32.dll"),"OpenThread");

	__asm
	{
		push dwTID
			push 0
			push THREAD_ALL_ACCESS
			call eax
			mov hThread,eax
	}

	//获得进程对象的地址
	BYTE * pBuf = new BYTE[dwBufLen];
	ZeroMemory(pBuf,dwBufLen);
	status = ZwQuerySystemInformation(SystemHandleInformation,pBuf,dwBufLen,&dwRetLen);

	uObjCnt = (ULONG)*(ULONG*)pBuf;

	pHandleInfo = (PSYSTEM_HANDLE_INFORMATION)(pBuf+sizeof(ULONG));

	if(NT_SUCCESS(status))
	{

		for(int i=0;i<uObjCnt;i++)
		{
			if(pHandleInfo->ProcessId==dwPID
				&&pHandleInfo->Handle==(USHORT)hThread)
			{
				dwETHREAD = (DWORD)pHandleInfo->Object;
				break;
			}
			pHandleInfo++;
		}

		//patch 内核
		MEMORY_CHUNKS datas;
		datas.Address = dwETHREAD+0x248;
		datas.Data = &CWriteMemory::m_dwFixData;
		datas.Length = 4;

		status = NtSystemDebugControl(0x9,&datas,sizeof(MEMORY_CHUNKS),NULL,0,&dwRetLen);
		//status = NtSystemDebugControl(0x8,&datas,sizeof(MEMORY_CHUNKS),NULL,0,&dwRetLen);
		if(NT_SUCCESS(status))
			bRet = TRUE;

	}
	delete [] pBuf;
	CloseHandle(hThread);
	return bRet;
}

BOOL CWriteMemory::EnableDebugPrivilege(BOOL bEnable)
{
	BOOL bOk = FALSE; 
	HANDLE hToken;

	if(::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) 
	{
		LUID uID;
		::LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &uID);

		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		tp.Privileges[0].Luid = uID;
		tp.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;
		::AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
		bOk = (::GetLastError() == ERROR_SUCCESS);
		::CloseHandle(hToken);
	}
	return bOk;
}