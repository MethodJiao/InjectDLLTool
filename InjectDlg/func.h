#pragma once
#include <TlHelp32.h>
#include <string>
#include <vector>
std::string GetExePath();
HMODULE GetDLLHandle(TCHAR* DllName, DWORD tPid);
DWORD GetPIDForProcess(TCHAR* process);
bool traverseProcesses(std::vector<std::string>& nameVec);