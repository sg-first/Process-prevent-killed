#ifndef HOOKCLASS_H
#define HOOKCLASS_H
//---------------------------------------------------------------------------

class TAPIHook
{
private:
       static PVOID pvMaxAppAddr;     // 在这个地址之上的地址是共享DLL所在的地址
                                      //（仅对W9X适用）

       static TAPIHook* pHeadHook;    // 第一个TAPIHook引用
       TAPIHook* pNextHook;           // 下一个TAPIHook引用

public:
       PCSTR FDllName;                // 将Hook哪个DLL里边的函数
       PCSTR FDllFuncName;            // Hook DLL里边哪个函数
       PROC  pfnOrig;                 // 那个函数的真正地址
       PROC  pfnHook;                 // 自定义函数的地址
       bool  FIsHookSelfDll;          // 是否Hook DLL本身的相应函数，对一些系统函
       				      // 数，如GetProcAddress...等，DLL本身是不能
                                      // 对它们进行Hook的，否则会引起死循环。

       TAPIHook();
       TAPIHook(PSTR DllName, PSTR DllFuncName, PROC pfnHook_Target,bool IsHookSelfDll);
       ~TAPIHook();

       // 在当前进程的所有模块中Hook指定的函数
       void SetupHook(PSTR DllName, PSTR DllFuncName, PROC pfnHook_Target,bool IsHookSelfDll);

       // 返回函数的真正地址
       operator PROC() { return(pfnOrig); }

public:
       // 得到函数的真正地址
       static FARPROC WINAPI GetProcAddressRaw(HMODULE hmod, PCSTR pszProcName);

private:

       // 查找进程所有的模块，然后一一进行Hook
       static void WINAPI ReplaceIATEntryInAllModules(PCSTR DllName,
            PROC pfnCurrent,PROC pfnNew,bool IsHookSelfDll);

       // Hook指定模块中的指定函数
       static void WINAPI ReplaceIATEntryInModule(PCSTR DllName,
            PROC pfnCurrent,PROC pfnNew,HMODULE hModCaller);

private:
       // 当动态装载DLL时，必须Hook那个DLL的相应函数
       static void WINAPI FixupNewlyLoadedModule(HMODULE hmod, DWORD dwFlags);

       // 截获这4个系统函数，以便Hook装载之后的DLL
       static HMODULE WINAPI LoadLibraryA(PCSTR  pszModulePath);
       static HMODULE WINAPI LoadLibraryW(PCWSTR pszModulePath);
       static HMODULE WINAPI LoadLibraryExA(PCSTR  pszModulePath,HANDLE hFile, DWORD dwFlags);
       static HMODULE WINAPI LoadLibraryExW(PCWSTR pszModulePath,HANDLE hFile, DWORD dwFlags);

       // 动态得到函数的地址时，也必须进行Hook
       static FARPROC WINAPI GetProcAddress(HMODULE hmod, PCSTR pszProcName);

private:

       //缺省的API Hook
       static TAPIHook LoadLibraryAHook;
       static TAPIHook LoadLibraryWHook;
       static TAPIHook LoadLibraryExAHook;
       static TAPIHook LoadLibraryExWHook;
       static TAPIHook GetProcAddressHook;
};


#endif