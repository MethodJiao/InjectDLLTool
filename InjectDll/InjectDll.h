#ifndef _INJECT_H
#define _INJECT_H

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

//#include "resource.h"
#include "framework.h"
#include "pfunc.h"

#include <commctrl.h>
#include <commdlg.h>
#include <algorithm>
//#include <string>

// ȫ�ֱ���
HINSTANCE hInst;
HICON hIcon;
char szDllPath[MAX_PATH];
char szDName[_MAX_FNAME];
DWORD dwPID = 0;
TCHAR szPName[_MAX_FNAME];

// ǰ������
BOOL InjectDll();
BOOL UnInjectDll();
BOOL OpenFileDlg(HWND);
LRESULT CALLBACK    AboutProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    MainDlgProc(HWND, UINT, WPARAM, LPARAM);

#endif