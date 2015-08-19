/********************************************************************
* 描    述：  进程防杀dll
* 作    者：  孙涛
* 版 本 号：  1.0
* 完成日期：  20090313
********************************************************************/


#ifndef __WRITEMEMEORYC_H
#define __WRITEMEMEORYC_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#ifdef  _ANTIKILL_EXPORT
#define   ANTIKILL_PORT  __declspec(dllexport)
#else
#define   ANTIKILL_PORT  __declspec(dllimport)
#endif

#include "WriteMemory.h"
#endif // __WRITEMEMEORYC_H
