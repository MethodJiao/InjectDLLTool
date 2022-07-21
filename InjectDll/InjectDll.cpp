// InjectDll.cpp : 定义应用程序的入口点。
//
#include "InjectDll.h"
#include "resource.h"
#pragma comment(lib, "ComCtl32.lib")

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	hInst = hInstance;
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DLG_MAIN), NULL, (DLGPROC)MainDlgProc);
    return 0;
}

//
//  函数: MainProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//
LRESULT CALLBACK MainDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int iWmId, iWmEvent;
	switch (uMsg)
	{
	case WM_CLOSE:
		//if (MessageBox(NULL, _T("是否退出？"), _T("退出"), MB_YESNO) == IDYES)
		//{
		EndDialog(hWnd, 0);
		//}
		break;
	case WM_INITDIALOG:
		// 设置图标
		hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_SMALL));
		SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		// 初始化控件
		InitCommonControls();

		// 提权
		if (!AdjustPrivileges())
		{
			MessageBox(NULL, _T("提权失败，请以管理员权限运行本程序！"), _T("错误"), MB_OK | MB_ICONERROR);
			EndDialog(hWnd, 0);
			return FALSE;
		}
		return TRUE;
	case WM_COMMAND:
		iWmId = LOWORD(wParam);               // 触发消息的控件ID
		iWmEvent = HIWORD(wParam);			  // 消息码
		
		if (iWmId == IDC_BTN_ABOUT)
		{
			DialogBox(NULL, MAKEINTRESOURCE(IDD_DLG_ABOUT), hWnd, (DLGPROC)AboutProc);
			break;
		}
		if (iWmId == IDC_BTN_OPEN)
		{
			if (OpenFileDlg(hWnd))
			{
				//MessageBox(hWnd, _T("打开文件失败！"), _T("错误"), MB_OK | MB_ICONERROR);
				SendDlgItemMessage(hWnd, IDC_EDIT_DLL_PATH, WM_SETTEXT, 0, (LPARAM)szDllPath);
				break;
			}
		}
		if (iWmId == IDC_BTN_INJECT)
		{
			SendDlgItemMessage(hWnd, IDC_EDIT_PROCESS_NAME, WM_GETTEXT, _MAX_FNAME, (LPARAM)szPName);
			if (0 == szPName[0])
			{
				MessageBox(NULL, _T("请指定进程名！"), _T("警告"), MB_OK | MB_ICONWARNING);
				break;
			}
			// 得到pid
			dwPID = GetPIDForProcess(szPName);
			if (!dwPID)
			{
				MessageBox(NULL, _T("获取进程PID失败！"), _T("失败"), MB_OK);
				break;
			}

			// 注入dll
			if (!InjectDll())
			{
				MessageBox(NULL, _T("注入DLL失败！"), _T("注入失败"), MB_OK);
				break;
			}
			break;
		}
		if (iWmId == IDC_BTN_UNINJECT)
		{
			SendDlgItemMessage(hWnd, IDC_EDIT_PROCESS_NAME, WM_GETTEXT, _MAX_FNAME, (LPARAM)szPName);
			if (0 == szPName[0])
			{
				MessageBox(NULL, _T("请指定进程名！"), _T("警告"), MB_OK | MB_ICONWARNING);
				break;
			}
			// 得到pid
			dwPID = GetPIDForProcess(szPName);
			if (!dwPID)
			{
				MessageBox(NULL, _T("获取进程PID失败！"), _T("失败"), MB_OK);
				break;
			}
			// 卸载Dll
			if (dwPID)
			{
				if (!UnInjectDll())
				{
					MessageBox(NULL, _T("卸载DLL失败！"), _T("卸载失败"), MB_OK);
					break;
				}
				else
				{
					MessageBox(NULL, _T("卸载DLL成功！"), _T("卸载成功"), MB_OK);
				}
			}
			break;
		}
	// 下面代码引起栈溢出错误，尝试更改为DefWindowProc后“关于”对话框无法响应消息
	//default:
	//	return DefDlgProc(hWnd, uMsg, wParam, lParam;
	}
    return 0;
}

LRESULT CALLBACK    AboutProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int iWmId, iWmEvent;
	HICON hIcon;
	switch (uMsg)
	{
	case WM_INITDIALOG:
		// 显示图标
		hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_INJECTDLL));
		SendDlgItemMessage(hWnd, IDC_PIC_ICON, STM_SETICON, (WPARAM)hIcon, 0);
		SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		break;
	case WM_COMMAND:
		iWmId = LOWORD(wParam);               // 触发消息的控件ID
		iWmEvent = HIWORD(wParam);			  // 消息码

		if (IDC_BTN_OK == iWmId)
		{
			EndDialog(hWnd, 0);
		}
		break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;
	//default:
	//	return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

BOOL OpenFileDlg(HWND hWnd)
{
	OPENFILENAME ofn;
	memset(szDllPath, 0, MAX_PATH);
	memset(szDName, 0, _MAX_FNAME);
	memset(&ofn, 0, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.hInstance = hInst;
	ofn.lpstrFilter = _T("DLL文件\0*.dll\0\0");
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrFile = szDllPath;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFileTitle = szDName;
	ofn.nMaxFileTitle = _MAX_FNAME;
	ofn.lpstrInitialDir = TEXT(".");
	ofn.lpstrTitle = TEXT("Open...");

	if (!GetOpenFileName(&ofn))
	{
		return FALSE;
	}
	return TRUE;
}

//
//  函数: InjectDll()
//
//  目标: 注入dll。
//
//
BOOL InjectDll()
{
	// 获取进程访问权限
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	if (!hProcess)
	{
		return FALSE;
	}
	// 申请内存空间
	LPVOID lpDllMem = VirtualAllocEx(hProcess, NULL, sizeof(szDllPath), MEM_COMMIT, PAGE_READWRITE);
	if (!lpDllMem)
	{
		CloseHandle(hProcess);
		return FALSE;
	}
	// dll路径写入内存
	if (0 == WriteProcessMemory(hProcess, lpDllMem, szDllPath, sizeof(szDllPath), NULL))
	{
		VirtualFreeEx(hProcess, lpDllMem, sizeof(szDllPath), MEM_DECOMMIT);
		CloseHandle(hProcess);
		return FALSE;
	}
	// 调用远程dll
	HMODULE hK32 = GetModuleHandle(_T("Kernel32.dll"));
	LPVOID lpLoadLibAddr = GetProcAddress(hK32, "LoadLibraryA");
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lpLoadLibAddr, lpDllMem, 0, 0);
	if (!hThread)
	{
		VirtualFreeEx(hProcess, lpDllMem, sizeof(szDllPath), MEM_DECOMMIT);
		CloseHandle(hProcess);
		return FALSE;
	}
	//::WaitForSingleObject(hThread, INFINITE);

	/*WaitForSingleObject(hThread, INFINITE);*/
	/*VirtualFreeEx(hProcess, lpDllMem, sizeof(szDllPath), MEM_DECOMMIT);
	CloseHandle(hThread);
	CloseHandle(hProcess);*/
	/*TCHAR test[100] = { 0 };
	_sntprintf_s(test, sizeof(test)/sizeof(TCHAR), _T("写入的地址： %p"), lpDllMem);
	OutputDebugString(test);*/
	return TRUE;
}

//
//  函数: UnInjectDll()
//
//  目标: 卸载dll。
//
//
BOOL UnInjectDll()
{
	//// 从Dll路径中获取名称
	//std::string szDllName(szDllPath);
	//std::replace(szDllName.begin(), szDllName.end(), '\\', '/');
	//int pos = szDllName.find_last_of('/');
	//szDllName = szDllName.substr(pos + 1);
	//char szDName[0x100] = { 0 };
	//strcpy_s(szDName, sizeof(szDName), szDllName.c_str());
	//char szDName[0x100] = "WeChatHelperDll.dll";
	// 获取进程访问权限
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	if (!hProcess)
	{
		return FALSE;
	}
	HMODULE hK32 = GetModuleHandle(_T("Kernel32.dll"));
	LPVOID lpFreeLibAddr = GetProcAddress(hK32, "FreeLibraryAndExitThread");
	HMODULE hDll = GetDLLHandle(szDName, dwPID);
	if (!hDll)
	{
		CloseHandle(hProcess);
		return FALSE;
	}
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lpFreeLibAddr, hDll, 0, 0);
	if (!hThread)
	{
		CloseHandle(hProcess);
		return FALSE;
	}

	//WaitForSingleObject(hThread, INFINITE);
	//CloseHandle(hThread);
	//CloseHandle(hProcess);
	return TRUE;
}