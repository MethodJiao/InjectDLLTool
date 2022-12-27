#include "pch.h"
#include "func.h"
#include <algorithm>
std::string GetExePath()
{
	char szFilePath[MAX_PATH + 1] = { 0 };
	GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
	/*
	strrchr:函数功能：查找一个字符c在另一个字符串str中末次出现的位置（也就是从str的右侧开始查找字符c首次出现的位置），
	并返回这个位置的地址。如果未能找到指定字符，那么函数将返回NULL。
	使用这个地址返回从最后一个字符c到str末尾的字符串。
	*/
	(strrchr(szFilePath, '\\'))[0] = 0; // 删除文件名，只获得路径字串//
	std::string path = szFilePath;
	return path;
}
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

bool traverseProcesses(std::vector<std::string>& nameVec)
{
	nameVec.clear();
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//获取进程快照
	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		//cout << "CreateToolhelp32Snapshot Error!" << endl;;
		return false;
	}

	BOOL bResult = Process32First(hProcessSnap, &pe32);

	int num(0);

	while (bResult)
	{
		//string name = string(pe32.szExeFile);
		char temp[300];
		WideCharToMultiByte(CP_ACP, 0, pe32.szExeFile, -1, temp, sizeof(temp), NULL, NULL);
		std::string name = std::string(temp);
		int id = pe32.th32ProcessID;

		//cout << "[" << ++num << "] : " << "Process Name:"
		//	<< name << "  " << "ProcessID:" << id << endl;

		nameVec.push_back(name); //存储
		bResult = Process32Next(hProcessSnap, &pe32);
	}
	//去重
	std::sort(nameVec.begin(), nameVec.end());
	nameVec.erase(std::unique(nameVec.begin(), nameVec.end()), nameVec.end());

	CloseHandle(hProcessSnap);

	return true;
}