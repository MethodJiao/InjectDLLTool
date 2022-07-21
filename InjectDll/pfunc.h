#ifndef _PFUNC_H
#define _PFUNC_H

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include "framework.h"
#include <TlHelp32.h>

bool AdjustPrivileges();
DWORD GetDLLBase(TCHAR* DllName, DWORD tPid);
HMODULE GetDLLHandle(TCHAR* DllName, DWORD tPid);
DWORD GetPIDForProcess(TCHAR* process);

#endif