//#include <vcl.h>
#include <windows.h>
#pragma hdrstop

#define _WIN64

#include <Imagehlp.h>
#include <Tlhelp32.h>
#include <assert.h>

#include "HookClass.h"

//---------------------------------------------------------------------------
//是否Hook 缺省的5个系统函数的标志
extern bool IsHook;

//---------------------------------------------------------------------------
//在W9X下，当一个程序运行于调试状态时，这个程序所引用的系统函数的地址指向一个STUB
//里边的函数的地址，而不是真正的地址，所以必须进行调整。
PVOID TAPIHook::pvMaxAppAddr = NULL;
const byte PushOpCode = 0x68;       // PUSH 指令代码

//---------------------------------------------------------------------------

TAPIHook* TAPIHook::pHeadHook = NULL;    // Address of first object

//---------------------------------------------------------------------------

TAPIHook TAPIHook::LoadLibraryAHook("Kernel32.Dll","LoadLibraryA",(PROC)TAPIHook::LoadLibraryA,false);
TAPIHook TAPIHook::LoadLibraryWHook("Kernel32.Dll","LoadLibraryW",(PROC)TAPIHook::LoadLibraryW,false);
TAPIHook TAPIHook::LoadLibraryExAHook("Kernel32.Dll","LoadLibraryExA",(PROC)TAPIHook::LoadLibraryExA,false);
TAPIHook TAPIHook::LoadLibraryExWHook("Kernel32.Dll","LoadLibraryExW",(PROC)TAPIHook::LoadLibraryExW,false);
TAPIHook TAPIHook::GetProcAddressHook("Kernel32.Dll","GetProcAddress",(PROC)TAPIHook::GetProcAddress,false);

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TAPIHook::TAPIHook()
{
}
//---------------------------------------------------------------------------
TAPIHook::TAPIHook(PSTR DllName, PSTR DllFuncName, PROC pfnHook_Target,bool IsHookSelfDll)
{
  SetupHook(DllName,DllFuncName,pfnHook_Target,IsHookSelfDll);
}
//---------------------------------------------------------------------------
TAPIHook::~TAPIHook()
{
  ReplaceIATEntryInAllModules(FDllName,pfnHook,pfnOrig,FIsHookSelfDll);

  //删除Hook链里边删除当前Hook
  TAPIHook* p = pHeadHook;

  if(p == this)
  {
    pHeadHook = p->pNextHook;
  }
  else
  {
    bool IsFound = false;

    // 边历Hook链
    for(; !IsFound && (p->pNextHook != NULL); p = p->pNextHook)
    {
      if(p->pNextHook == this)
      {
        // 跳当前Hook
        p->pNextHook = p->pNextHook->pNextHook;
        break;
      }
    }
  }
}
//---------------------------------------------------------------------------
//SetupHook
void TAPIHook::SetupHook(PSTR DllName, PSTR DllFuncName, PROC pfnHook_Target,bool IsHookSelfDll)
{
  pNextHook  = pHeadHook;    // The next node was at the head
  pHeadHook = this;          // This node is now at the head

  if(pvMaxAppAddr == NULL)
  {
    // Functions with address above lpMaximumApplicationAddress require
    // special processing (Windows 98 only)
    SYSTEM_INFO si;

    GetSystemInfo(&si);
    pvMaxAppAddr = si.lpMaximumApplicationAddress;
  }

  // Save information about this hooked function
  FDllName = DllName;
  FDllFuncName = DllFuncName;
  pfnHook = pfnHook_Target;
  pfnOrig = GetProcAddressRaw(GetModuleHandleA(FDllName), FDllFuncName);
  FIsHookSelfDll = IsHookSelfDll;

  assert(pfnOrig != NULL);  // Function doesn't exist

  if(pfnOrig > pvMaxAppAddr)
  {
    // The address is in a shared DLL; the address needs fixing up
    PBYTE pb = (PBYTE)pfnOrig;
    if(pb[0] == PushOpCode)
    {
      // Skip over the PUSH op code and grab the real address
      PVOID pv = * (PVOID*) &pb[1];
      pfnOrig = (PROC) pv;
    }
  }

  ///////

  ReplaceIATEntryInAllModules(FDllName,pfnOrig, pfnHook,FIsHookSelfDll);
}
//---------------------------------------------------------------------------
//由于没有Hook DLL本身的GetProcAddress函数，所以GetProcAddress仍然指向正确的
//Kernel32.Dll中的GetProcAddress的地址，因此不会引起死循环。
FARPROC WINAPI TAPIHook::GetProcAddressRaw(HMODULE hmod, PCSTR pszProcName)
{
  return (::GetProcAddress(hmod, pszProcName));
}
//---------------------------------------------------------------------------
//查询本身DLL的句柄
static HMODULE ModuleFromAddress(PVOID pv)
{
  MEMORY_BASIC_INFORMATION mbi;

  return((VirtualQuery(pv, &mbi, sizeof(mbi)) != 0) ? (HMODULE) mbi.AllocationBase : NULL);
}
//---------------------------------------------------------------------------
//ReplaceIATEntryInAllModules
void WINAPI TAPIHook::ReplaceIATEntryInAllModules(PCSTR DllName,
  PROC pfnCurrent,PROC pfnNew,bool IsHookSelfDll)
{
  // 是否Hook DLL本身的相应函数。对一些系统函数，如GetProcAddress、LoadLibraryA、
  // LoadLibraryW、LoadLibraryEx、LoadLibraryExW这些函数，DLL本身是不能对它们进
  // 行Hook的，否则会引起死循环。

  HMODULE hThisModule = NULL;

  hThisModule = (IsHookSelfDll) ? NULL : ModuleFromAddress(ReplaceIATEntryInAllModules);

  /////
  HANDLE hSnapshot;
  MODULEENTRY32 ModEntry32;
  DWORD dwProcessId;
  BOOL Result;

  dwProcessId = GetCurrentProcessId();
  hSnapshot = CreateToolhelp32Snapshot((DWORD)TH32CS_SNAPMODULE,dwProcessId);

  ModEntry32.dwSize = sizeof(MODULEENTRY32);
  Result = Module32First(hSnapshot,&ModEntry32);
  while(Result)
  {
    if(ModEntry32.hModule != hThisModule)
      ReplaceIATEntryInModule(DllName,pfnCurrent,pfnNew,ModEntry32.hModule);

    Result = Module32Next(hSnapshot,&ModEntry32);
  }

  CloseHandle(hSnapshot);
}
//---------------------------------------------------------------------------
//ReplaceIATEntryInModule
void WINAPI TAPIHook::ReplaceIATEntryInModule(PCSTR DllName,
  PROC pfnCurrent,PROC pfnNew,HMODULE hModCaller)
{
  // 查询模块函数引入表的地址
  DWORD ulSize;
  PIMAGE_IMPORT_DESCRIPTOR pImportDesc;

  pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(hModCaller, true,
					    IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);

  if(pImportDesc == NULL)
    return ;  // 模块没有函数引入表

  // 查询模块是否引入了某个DLL的函数
  for(;pImportDesc->Name; pImportDesc++)
  {
    PSTR pszModName = (PSTR) ((PBYTE)hModCaller + pImportDesc->Name);
    if(lstrcmpiA(pszModName, DllName) == 0)
      break;   // Found
  }

  if(pImportDesc->Name == 0)
    return ;  // 这个模块没有引入指定DLL的函数

  //查询模块是否在DLL中引入了指定的函数
  PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)((PBYTE)hModCaller + pImportDesc->FirstThunk);

  for(; pThunk->u1.Function; pThunk++)
  {
    // 得到这个模块引入的每一个函数的地址
    PROC* ppfn = (PROC*) &pThunk->u1.Function;

    // 比较这个地址是否是我们准备Hook的函数地址
    bool IsFound = (*ppfn == pfnCurrent);

    if(!IsFound && (*ppfn > pvMaxAppAddr))
    {
      // If this is not the function and the address is in a shared DLL,
      // then maybe we're running under a debugger on Windows 98. In this
      // case, this address points to an instruction that may have the
      // correct address.

      PBYTE pbInFunc = (PBYTE) *ppfn;
      if(pbInFunc[0] == PushOpCode)
      {
        // We see the PUSH instruction, the real function address follows
        ppfn = (PROC*) &pbInFunc[1];

        // Is this the function we're looking for?
        IsFound = (*ppfn == pfnCurrent);
      }
    }

    //找到我们要Hook的函数，替换它的地址为我们自定义的函数的地址
    if(IsFound)
    {
      DWORD dwOldProtect;

      //必须修改相应的页面属性，否则有可能无法写入代码
      VirtualProtect(ppfn,sizeof(pfnNew),PAGE_READWRITE,&dwOldProtect);

      WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew,sizeof(pfnNew), NULL);

      VirtualProtect(ppfn,sizeof(pfnNew),dwOldProtect,0);
      return ;  // We did it, get out
    }
  }
}
//---------------------------------------------------------------------------
//当动态装载DLL时，必须Hook那个DLL的相应函数
void WINAPI TAPIHook::FixupNewlyLoadedModule(HMODULE hMod, DWORD dwFlags)
{
  HMODULE hThisModule = NULL;

  //不能Hook DLL本身的函数
  hThisModule = ModuleFromAddress(FixupNewlyLoadedModule);
  if(hMod == hThisModule)
    return;

  // If a new module is loaded, hook the hooked functions
  if((hMod != NULL) && ((dwFlags & LOAD_LIBRARY_AS_DATAFILE) == 0))
  {
    for(TAPIHook* p = pHeadHook; p != NULL; p = p->pNextHook)
    {
      ReplaceIATEntryInModule(p->FDllName,p->pfnOrig, p->pfnHook, hMod);
    }
  }
}
//---------------------------------------------------------------------------
// 由于没有Hook DLL本身的LoadLibraryA函数，所以LoadLibraryA仍然指向正确的
// Kernel32.Dll中的LoadLibraryA的地址，因此不会引起死循环。LoadLibraryW、
// LoadLibraryExA、LoadLibraryExW类似。
HMODULE WINAPI TAPIHook::LoadLibraryA(PCSTR  pszModulePath)
{
  HMODULE hMod;

  hMod = ::LoadLibraryA(pszModulePath);
  if(!IsHook)
    return hMod;

  FixupNewlyLoadedModule(hMod, 0);

  return hMod;
}
//---------------------------------------------------------------------------
HMODULE WINAPI TAPIHook::LoadLibraryW(PCWSTR pszModulePath)
{
  HMODULE hMod;

  hMod = ::LoadLibraryW(pszModulePath);
  if(!IsHook)
    return hMod;

  FixupNewlyLoadedModule(hMod, 0);

  return hMod;
}
//---------------------------------------------------------------------------
HMODULE WINAPI TAPIHook::LoadLibraryExA(PCSTR  pszModulePath,HANDLE hFile, DWORD dwFlags)
{
  HMODULE hMod;

  hMod = ::LoadLibraryExA(pszModulePath,hFile,dwFlags);
  if(!IsHook)
    return hMod;

  FixupNewlyLoadedModule(hMod, dwFlags);

  return hMod;
}
//---------------------------------------------------------------------------
HMODULE WINAPI TAPIHook::LoadLibraryExW(PCWSTR pszModulePath,HANDLE hFile, DWORD dwFlags)
{
  HMODULE hMod;

  hMod = ::LoadLibraryExW(pszModulePath,hFile,dwFlags);
  if(!IsHook)
    return hMod;

  FixupNewlyLoadedModule(hMod, dwFlags);

  return hMod;
}
//---------------------------------------------------------------------------
//动态查询函数的地址。由于已经本身DLL已经Hook了GetProcAddress函数，所以查询一个
//函数的地址时，自己的GetProcAddress函数首先得到控制权，如果要查询的函数属于被
//Hook的函数，则直接返回Hook函数的地址，而不是原来的函数的地址。
FARPROC WINAPI TAPIHook::GetProcAddress(HMODULE hmod, PCSTR pszProcName)
{
  //Get the true address of the function
  FARPROC pfn = GetProcAddressRaw(hmod, pszProcName);

  if(!IsHook) return pfn;

  //Is it one of the functions that we want hooked?
  TAPIHook* p = pHeadHook;

  for(;(pfn != NULL) && (p != NULL); p = p->pNextHook)
  {
    if(pfn == p->pfnOrig)
    {
      // The address to return matches an address we want to hook
      // Return the hook function address instead
      pfn = p->pfnHook;
      break;
    }
  }

  return pfn;
}
//---------------------------------------------------------------------------

