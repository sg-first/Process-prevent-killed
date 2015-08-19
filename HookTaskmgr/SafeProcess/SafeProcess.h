////////////////////////////////////////////////////////////////
// 2000 VCKBASE Online Journal. 
// 
//
#define DLLIMPORT __declspec(dllimport)


DLLIMPORT BOOL SafeProcessInit();
DLLIMPORT void SafeProcessTerm();
DLLIMPORT int WINAPI SetHandle(HWND HandleofTarget);

