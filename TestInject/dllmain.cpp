// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <tchar.h>

#include <Windows.h>
#include <Tlhelp32.h>
#include <stdio.h>
#include <vector>
using namespace std;
DWORD* getAddrPtr(DWORD firstAddr, DWORD secAddr)
{
	DWORD addr = firstAddr + secAddr;
	DWORD* pAddr = (DWORD*)addr;
	return pAddr;
}

DWORD* getFinalAddrPtr(vector<DWORD> addrVec)
{
	DWORD* pAddr = nullptr;
	if (addrVec.size() < 2)
		return pAddr;
	for (size_t i = 0; i < addrVec.size() - 1; i++)
	{
		if (i == 0)
		{
			pAddr = getAddrPtr(addrVec[i], addrVec[i + 1]);
		}
		else
		{
			pAddr = getAddrPtr(*pAddr, addrVec[i + 1]);
		}
	}
	return pAddr;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		MessageBeep(MB_OK);
		//获取目标进程基址
		HMODULE baseAddr = GetModuleHandle(_T("TestDestInject.exe"));
		//寻址
		vector<DWORD> addrVec = { (DWORD)baseAddr, 0x001D5220, 0xD4 };
		DWORD* finalAddr = getFinalAddrPtr(addrVec);
		if (finalAddr == nullptr)
			break;
		//改值
		*finalAddr = 12345678;

		break;
	}
	case DLL_THREAD_ATTACH:
	{
		break;
	}
	case DLL_THREAD_DETACH:
	{
		break;
	}
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

