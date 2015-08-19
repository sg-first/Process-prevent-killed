#ifndef HOOKAPI_H
#define HOOKAPI_H

#ifdef  _DLL_
#define DLL_EXP_IMP __declspec(dllexport)
#else
#define DLL_EXP_IMP __declspec(dllimport)
#endif

extern "C" DLL_EXP_IMP int _cdecl Hook(DWORD dwSelfProcessID_P);
extern "C" DLL_EXP_IMP int _cdecl Unhook();

#endif

