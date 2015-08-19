#include "stdafx.h"
#include "main.h"
#include "LogFile.h"


#define FR_PROTECT_EVENT_NAME		_T("Fr_Protect_Event_Name_By_")
#define FRPOFILENAME			_T("ExeProtect.exe") 

TCHAR m_strAppPath[256]={'\0'}; 
void InitModulePath(HINSTANCE hInst);

//int _tmain(int argc, _TCHAR* argv[])
int APIENTRY WinMain(HINSTANCE hInst,
					 HINSTANCE,
					 LPSTR lpCmdLine, 
					 int)
{
	//命令行参数
	TCHAR pCommandLine[256]  = {0};
	lstrcpy(pCommandLine,lpCmdLine);  

	TCHAR pProtectEventName[256]={0};
	lstrcpy(pProtectEventName,FR_PROTECT_EVENT_NAME); 
	lstrcat(pProtectEventName,lpCmdLine);  

	//启动保护进程
	InitModulePath(hInst); 

	HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,pProtectEventName);
	if(!hEvent)
	{	
		STARTUPINFO StartInfo = {sizeof(STARTUPINFO)};
		PROCESS_INFORMATION ProcessInfo = {0};
		TCHAR cBuffer[256];
		lstrcpy(cBuffer,m_strAppPath); 
		lstrcat(cBuffer,FRPOFILENAME);    //启动保护进程
		memset(pCommandLine,0,256);
		lstrcpy(pCommandLine," "); 
		lstrcat(pCommandLine,lpCmdLine);   

		BOOL bSuccessful = CreateProcess(cBuffer,pCommandLine,NULL,NULL,
			FALSE,0,NULL,NULL,&StartInfo,&ProcessInfo);

		if(bSuccessful)
		{
			CloseHandle(ProcessInfo.hProcess);
			CloseHandle(ProcessInfo.hThread);
		}
	}
	else
		CloseHandle(hEvent); 

	return 0;
}

void InitModulePath(HINSTANCE hInst)
{
	GetModuleFileName(hInst,m_strAppPath,255*sizeof(TCHAR));
	for(int i=lstrlen(m_strAppPath)-1;i>=0;i--)
		if(m_strAppPath[i]=='\\')
		{
			m_strAppPath[i+1] = '\0';
			break;
		}
}

