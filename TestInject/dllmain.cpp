// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <tchar.h>

BOOL APIENTRY DllMain( HMODULE hModule,
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

