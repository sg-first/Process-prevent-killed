// killtest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include "TaskKeyMgr.h"

//HOOKPROC hkprcMsgProc=NULL;		//the pointer of MsgProc()
//static HINSTANCE hinstDLL=NULL;		//the handle of dll!
//static HHOOK hhookMsg=NULL;		//the handle of hook!
//
//typedef void (*SETPID)(DWORD nPID);

int main(int argc, char* argv[])
{
	//hinstDLL = LoadLibrary((LPCTSTR)"NoKillDll.dll");	//hookdll.dll is in the same path

	//SETPID fnSetPID=(SETPID)GetProcAddress(hinstDLL,"SetPID");
	//if (NULL == fnSetPID)
	//{
	//	return 0;
	//}
	//(fnSetPID)(GetCurrentProcessId());

	//hkprcMsgProc = (HOOKPROC)GetProcAddress(hinstDLL, "MsgProc");	//get pointer of msg proc

	//hhookMsg = SetWindowsHookEx(WH_GETMESSAGE,hkprcMsgProc,hinstDLL,0);	//0 mean hook all process! you can specify a process id,hook one process!
	//unsigned long id = GetCurrentProcessId();
	//printf("%l",GetCurrentProcessId());

	//Init();
	
	//CTaskKeyMgr::Disable(CTaskKeyMgr::TASKLIST,
		//!CTaskKeyMgr::IsProcessListItemDisabled());

	LoadSafeProcess();
	//LoadProcess();
	while (getchar() != 'q')
	{
	}
	//UnLoadProcess();
	UnLoadSafeProcess();

	while (getchar() != 'w')
	{
	}

	return 0;
}

