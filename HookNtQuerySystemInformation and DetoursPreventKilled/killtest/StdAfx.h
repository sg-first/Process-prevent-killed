// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__8C6E7ABC_0F64_4331_8006_744A1C68D242__INCLUDED_)
#define AFX_STDAFX_H__8C6E7ABC_0F64_4331_8006_744A1C68D242__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <stdio.h>
#include <Windows.h>
#include <tchar.h>




#ifdef _DEBUG
	#pragma comment(lib,"../lib/debug/NoKillDll.lib")
#else
	#pragma comment(lib,"../lib/release/NoKillDll.lib")
#endif


// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__8C6E7ABC_0F64_4331_8006_744A1C68D242__INCLUDED_)
