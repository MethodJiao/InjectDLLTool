// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <tchar.h>

#include <Windows.h>
#include <Tlhelp32.h>
#include <stdio.h>


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		int a = 0;
		MessageBox(NULL, _T("被注入"), _T("test"), MB_OK);

		HMODULE baseAddr = GetModuleHandle(_T("TestDestInject.exe"));
		DWORD addr1 = 0x001D5220 + (DWORD)baseAddr;
		DWORD* pAddr1 = (DWORD*)addr1;
		DWORD addr2 = *pAddr1 + 0xD4;
		DWORD* pAddr2 = (DWORD*)addr2;
		*pAddr2 = 12345678;//改值

		break;
	}
	case DLL_THREAD_ATTACH:
	{
		int b = 0;
		break;
	}
	case DLL_THREAD_DETACH:
	{
		int c = 0;
		break;
	}
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

