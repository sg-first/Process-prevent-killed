////////////////////////////////////////////////////////////////
// MSDN Magazine -- September 2002
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual Studio 6.0 and Visual Studio .NET on Windows XP.
//

#include "StdAfx.h"
#include "TaskKeyMgr.h"
//#include "hook.h"
#define HKCU HKEY_CURRENT_USER

// Magic registry key/value for "Remove Task Manager" policy.
//
LPCTSTR KEY_DisableTaskMgr =
	"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System";
LPCTSTR VAL_DisableTaskMgr = "DisableTaskMgr";

//////////////////
// Disable task-key related stuff.
// 
// dwFlags  = what to disable
// bDisable = disable (TRUE) or enable (FALSE)
// bBeep    = whether to beep for illegal keys (TASKKEYS only)
//
void CTaskKeyMgr::Disable(DWORD dwFlags, BOOL bDisable, BOOL bBeep)
{
	//// task manager (Ctrl+Alt+Del)
	//if (dwFlags & TASKMGR) {
	//	HKEY hk;
	//	if (RegOpenKey(HKCU, KEY_DisableTaskMgr,&hk)!=ERROR_SUCCESS)
	//		RegCreateKey(HKCU, KEY_DisableTaskMgr, &hk);

	//	if (bDisable) { // disable TM: set policy = 1
	//		DWORD val=1;
	//		RegSetValueEx(hk, VAL_DisableTaskMgr, NULL,
	//			REG_DWORD, (BYTE*)&val, sizeof(val));

	//	} else { // enable TM: remove policy 
	//		RegDeleteValue(hk,VAL_DisableTaskMgr);
	//	}
	//}
	//// task bar
	//if (dwFlags & TASKBAR) {
	//	HWND hwnd = FindWindow("Shell_traywnd", NULL);
	//	EnableWindow(hwnd, !bDisable);
	//}
	if(Is_Win2000())
	{
		if (dwFlags & TASKLIST) {
			if(bDisable&&!IsProcessListItemDisabled()){
				//only work for windows 2000
				int (_cdecl *pfnHook)(DWORD);
		
				hMyMyNtQuerySystemInformation= LoadLibrary(_T("HookNTQSI.dll"));
				if(hMyMyNtQuerySystemInformation)
				{
					pfnHook = (int(*)(DWORD))GetProcAddress(hMyMyNtQuerySystemInformation,"Hook");
					pfnHook(GetCurrentProcessId());
				}
				else
				{
					MessageBox(NULL,_T("Failed to load HookNTQSI.dll.Err ="),NULL,MB_OK | MB_APPLMODAL | MB_ICONWARNING);
				}
			}
			if(!bDisable&&IsProcessListItemDisabled()){
				int (_cdecl *pfnUnhook)();
				if(hMyMyNtQuerySystemInformation)
				{
					pfnUnhook = (int(*)())GetProcAddress(hMyMyNtQuerySystemInformation,"Unhook");
					pfnUnhook();
					FreeLibrary(hMyMyNtQuerySystemInformation);
					hMyMyNtQuerySystemInformation=NULL;
				}			
			}
		}
	}
}

BOOL CTaskKeyMgr::IsProcessListItemDisabled(){return hMyMyNtQuerySystemInformation!=NULL;}
HINSTANCE	CTaskKeyMgr::hMyMyNtQuerySystemInformation=NULL;



BOOL  Is_Win2000()
{
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;

	// Initialize the OSVERSIONINFOEX structure.

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 5;
	osvi.dwMinorVersion = 0;

	// Initialize the condition mask.
	VER_SET_CONDITION( dwlConditionMask, VER_MAJORVERSION, VER_EQUAL );
	VER_SET_CONDITION( dwlConditionMask, VER_MINORVERSION, VER_EQUAL );

	// Perform the test.
	return VerifyVersionInfo(
		&osvi,
		VER_MAJORVERSION | VER_MINORVERSION,
		dwlConditionMask);
}

BOOL Is_Win2000_SP1_or_Later () 
{
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;

	// Initialize the OSVERSIONINFOEX structure.

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 5;
	osvi.dwMinorVersion = 0;
	osvi.wServicePackMajor = 1;

	// Initialize the condition mask.

	VER_SET_CONDITION( dwlConditionMask, VER_MAJORVERSION, 
		VER_GREATER_EQUAL );
	VER_SET_CONDITION( dwlConditionMask, VER_MINORVERSION, 
		VER_GREATER_EQUAL );
	VER_SET_CONDITION( dwlConditionMask, VER_SERVICEPACKMAJOR, 
		VER_GREATER_EQUAL );

	// Perform the test.

	return VerifyVersionInfo(
		&osvi, 
		VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR,
		dwlConditionMask);
}

BOOL Is_Win2000_Professional () 
{
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;

	// Initialize the OSVERSIONINFOEX structure.

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 5;
	osvi.wProductType = VER_NT_WORKSTATION;

	// Initialize the condition mask.

	VER_SET_CONDITION( dwlConditionMask, VER_MAJORVERSION, 
		VER_GREATER_EQUAL );
	VER_SET_CONDITION( dwlConditionMask, VER_PRODUCT_TYPE, 
		VER_EQUAL );

	// Perform the test.

	return VerifyVersionInfo(
		&osvi, 
		VER_MAJORVERSION | VER_PRODUCT_TYPE,
		dwlConditionMask);
}

bool ISWIN2000_VER = false;
bool ISLoadProcess = false;

BOOL LoadSafeProcess()
{
	ISWIN2000_VER = false;
	ISLoadProcess = false;
	if (Is_Win2000())
	{
		ISWIN2000_VER = true;
		ISLoadProcess = true;
		CTaskKeyMgr::Disable(CTaskKeyMgr::TASKLIST,
			!CTaskKeyMgr::IsProcessListItemDisabled());
	}
	else
	{
		ISLoadProcess = true;
		LoadProcess();
	}
	return ::ISLoadProcess;
}

void UnLoadSafeProcess()
{
	if (ISLoadProcess)
	{
		if (ISWIN2000_VER)
		{
			CTaskKeyMgr::Disable(CTaskKeyMgr::TASKLIST,
				!CTaskKeyMgr::IsProcessListItemDisabled());
		}
		else
		{
			UnLoadProcess();
		}
		ISLoadProcess = false;
		ISWIN2000_VER = false;
	}
}

