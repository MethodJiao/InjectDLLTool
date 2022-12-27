#include "pch.h"
#include "func.h"
#include <algorithm>
std::string GetExePath()
{
	char szFilePath[MAX_PATH + 1] = { 0 };
	GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
	/*
	strrchr:�������ܣ�����һ���ַ�c����һ���ַ���str��ĩ�γ��ֵ�λ�ã�Ҳ���Ǵ�str���Ҳ࿪ʼ�����ַ�c�״γ��ֵ�λ�ã���
	���������λ�õĵ�ַ�����δ���ҵ�ָ���ַ�����ô����������NULL��
	ʹ�������ַ���ش����һ���ַ�c��strĩβ���ַ�����
	*/
	(strrchr(szFilePath, '\\'))[0] = 0; // ɾ���ļ�����ֻ���·���ִ�//
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
				//strcpy(LastDLLPath, me32.szExePath);//dll·��   
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

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//��ȡ���̿���
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

		nameVec.push_back(name); //�洢
		bResult = Process32Next(hProcessSnap, &pe32);
	}
	//ȥ��
	std::sort(nameVec.begin(), nameVec.end());
	nameVec.erase(std::unique(nameVec.begin(), nameVec.end()), nameVec.end());

	CloseHandle(hProcessSnap);

	return true;
}