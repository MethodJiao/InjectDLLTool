#include "pfunc.h"

//
//  函数: GetPIDForProcess(TCHAR* process)
//
//  目标: 得到进程id。
//
DWORD GetPIDForProcess(TCHAR* process)
{
	BOOL                    working;
	PROCESSENTRY32          lppe = { 0 };
	DWORD                   targetPid = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (!hSnapshot)
	{
		return 0;
	}
	lppe.dwSize = sizeof(lppe);
	working = Process32First(hSnapshot, &lppe);
	while (working)
	{
		if (_tcscmp((TCHAR*)lppe.szExeFile, process) == 0)
		{
			targetPid = lppe.th32ProcessID;
			break;
		}working = Process32Next(hSnapshot, &lppe);
	}
	CloseHandle(hSnapshot);
	return targetPid;
}

//
//  函数: GetDLLHandle(TCHAR* DllName, DWORD tPid)
//
//  目标: 得到进程中dll句柄。
//
HMODULE GetDLLHandle(TCHAR* DllName, DWORD tPid)
{
	HANDLE snapMod;
	MODULEENTRY32 me32;
	if (tPid == 0) return 0;
	snapMod = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, tPid);
	me32.dwSize = sizeof(MODULEENTRY32);
	if (Module32First(snapMod, &me32))
	{
		do
		{
			if (_tcscmp(DllName, me32.szModule) == 0)
			{
				//strcpy(LastDLLPath, me32.szExePath);//dll路径   
				CloseHandle(snapMod);
				return me32.hModule;
			}
		} while (Module32Next(snapMod, &me32));
	}
	CloseHandle(snapMod);
	return NULL;
}

//
//  函数: GetDLLBase(TCHAR* DllName, DWORD tPid)
//
//  目标: 得到进程中dll基址。
//
DWORD GetDLLBase(TCHAR* DllName, DWORD tPid)
{
	HANDLE snapMod;
	MODULEENTRY32 me32;
	if (tPid == 0) return 0;
	snapMod = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, tPid);
	me32.dwSize = sizeof(MODULEENTRY32);
	if (Module32First(snapMod, &me32))
	{
		do
		{
			if (_tcscmp(DllName, me32.szModule) == 0)
			{
				//strcpy(LastDLLPath, me32.szExePath);//dll路径   
				CloseHandle(snapMod);
				return (DWORD)me32.modBaseAddr;
			}
		} while (Module32Next(snapMod, &me32));
	}
	CloseHandle(snapMod);
	return 0;
}

//
//  函数: AdjustPrivileges()
//
//  目标: 进程提权。
//
bool AdjustPrivileges() {
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	TOKEN_PRIVILEGES oldtp;
	DWORD dwSize = sizeof(TOKEN_PRIVILEGES);
	LUID luid;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		if (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED) return true;
		else return false;
	}
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
		CloseHandle(hToken);
		return false;
	}
	ZeroMemory(&tp, sizeof(tp));
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	/* Adjust Token Privileges */
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), &oldtp, &dwSize)) {
		CloseHandle(hToken);
		return false;
	}
	// close handles
	CloseHandle(hToken);
	return true;
}