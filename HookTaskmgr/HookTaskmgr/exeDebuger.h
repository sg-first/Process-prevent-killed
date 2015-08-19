#if !defined(INCLUDE_exeDEBUGER_H_HYP_2001_10_29)
#define INCLUDE_exeDEBUGER_H_HYP_2001_10_29

//      包含本头文件，用 DPrintf  
//
#define TRACEWND_CLASSNAME _T("TraceWin TRACE Window")
#define OLDTRACEWND_CLASSNAME _T("MfxTraceWindow") // backwards compat

#define ID_COPYDATA_TRACEMSG MAKELONG(MAKEWORD('t','w'),MAKEWORD('i','n'))

#define DEBUGER_MAX_STRING_LENGTH 1024
static char static_DebugerTmpBuffer[DEBUGER_MAX_STRING_LENGTH];//供临时用的缓冲区


static void DPrintf(LPCSTR szText,...)
{
	
	memset(static_DebugerTmpBuffer,0,DEBUGER_MAX_STRING_LENGTH);
	
	va_list pArgList;
	
	va_start(pArgList, szText);
	_vsntprintf(static_DebugerTmpBuffer,DEBUGER_MAX_STRING_LENGTH,szText, pArgList);
	va_end(pArgList);
	

	HWND hTraceWnd = ::FindWindow(TRACEWND_CLASSNAME, NULL);
	if (hTraceWnd==NULL)
		hTraceWnd = ::FindWindow(OLDTRACEWND_CLASSNAME, NULL);
	if (hTraceWnd) 
	{
		COPYDATASTRUCT cds;
		cds.dwData = ID_COPYDATA_TRACEMSG;
		cds.cbData = strlen(static_DebugerTmpBuffer);
		cds.lpData = static_DebugerTmpBuffer;
		CWinApp* pApp = AfxGetApp();
		HWND hWnd = pApp ? pApp->m_pMainWnd->GetSafeHwnd() : NULL;
		::SendMessage(hTraceWnd, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&cds);
	}
		
}



















#endif
