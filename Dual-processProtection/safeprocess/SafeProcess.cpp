#include <stdio.h>
#include <afx.h>
#include <afxwin.h>
#include <conio.h>
#include <afxdisp.h>
#include <iostream>
#include "./SafeProcess/ProProcess.h"
using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		_tprintf(_T("¥ÌŒÛ: MFC ≥ı ºªØ ß∞‹\n"));
		return 1;
	}
	if (!AfxOleInit())
	{
		return 1;
	}

	CProProcess::InitProProcess(_T("SafeProcess"));

	cout<<"Enter 'q' to exit:"<<endl;
	while ((getch() != 'q') && (getch() != 'Q'))
	{
	}
	return 0;
}