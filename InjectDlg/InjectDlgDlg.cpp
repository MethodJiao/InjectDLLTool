
// InjectDlgDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "InjectDlg.h"
#include "InjectDlgDlg.h"
#include "afxdialogex.h"
#include <string>
#include <TlHelp32.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace std;

// CInjectDlgDlg 对话框
DWORD dwPID;
CString dllName;

CInjectDlgDlg::CInjectDlgDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INJECTDLG_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CInjectDlgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DLLNAME, m_dllname);
	DDX_Control(pDX, IDC_EDIT_PROCESSNAME, m_processname);
	DDX_Control(pDX, IDC_COMBO_PROCESSNAME, m_cbProcessName);
}

BEGIN_MESSAGE_MAP(CInjectDlgDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_INJECT, &CInjectDlgDlg::OnBnClickedButtonInject)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CInjectDlgDlg 消息处理程序

BOOL CInjectDlgDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_dllname.SetWindowText(_T("TestInjectDll.dll"));
	//m_processname.SetWindowText(_T("WXWork.exe"));
	traverseProcesses(m_processNameVec);
	for (size_t i = 0; i < m_processNameVec.size(); i++)
	{
		CString processName;
		processName = m_processNameVec[i].c_str();
		m_cbProcessName.AddString(processName);
	}
	//临时
	m_cbProcessName.SelectString(-1, _T("TestDestInject.exe"));
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CInjectDlgDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CInjectDlgDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//注入按钮
void CInjectDlgDlg::OnBnClickedButtonInject()
{
	UnInjectDll();
	//获取exe路径
	string exePath = GetExePath();

	CString processName;
	m_cbProcessName.GetWindowText(processName);
	m_dllname.GetWindowText(dllName);
	if (processName.IsEmpty() || dllName.IsEmpty())
		return;
	string dllNameA = CT2A(dllName.GetString());
	string szDllPath = exePath + "\\" + dllNameA;
	dwPID = GetPIDForProcess(processName.GetBuffer(processName.GetLength()));
	// 获取进程访问权限
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	if (!hProcess)
	{
		return;
	}
	// 申请内存空间
	LPVOID lpDllMem = VirtualAllocEx(hProcess, NULL, szDllPath.length() + 1, MEM_COMMIT, PAGE_READWRITE);
	if (!lpDllMem)
	{
		CloseHandle(hProcess);
		return;
	}
	// dll路径写入内存
	if (0 == WriteProcessMemory(hProcess, lpDllMem, szDllPath.data(), szDllPath.length() + 1, NULL))
	{
		VirtualFreeEx(hProcess, lpDllMem, szDllPath.length() + 1, MEM_DECOMMIT);
		CloseHandle(hProcess);
		return;
	}
	// 调用远程dll
	HMODULE hK32 = GetModuleHandle(_T("Kernel32.dll"));
	if (!hK32)
	{
		return;
	}
	LPVOID lpLoadLibAddr = GetProcAddress(hK32, "LoadLibraryA");
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lpLoadLibAddr, lpDllMem, 0, 0);
	if (!hThread)
	{
		VirtualFreeEx(hProcess, lpDllMem, szDllPath.length() + 1, MEM_DECOMMIT);
		CloseHandle(hProcess);
		return;
	}
}

void CInjectDlgDlg::UnInjectDll()
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	if (!hProcess)
	{
		return;
	}
	HMODULE hK32 = GetModuleHandle(_T("Kernel32.dll"));
	if (!hK32)
	{
		return;
	}
	LPVOID lpFreeLibAddr = GetProcAddress(hK32, "FreeLibraryAndExitThread");
	HMODULE hDll = GetDLLHandle(dllName.GetBuffer(dllName.GetLength()), dwPID);
	if (!hDll)
	{
		CloseHandle(hProcess);
		return;
	}
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lpFreeLibAddr, hDll, 0, 0);
	if (!hThread)
	{
		CloseHandle(hProcess);
		return;
	}
}


void CInjectDlgDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	UnInjectDll();
	CDialogEx::OnClose();
}


BOOL CInjectDlgDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (VK_RETURN == pMsg->wParam)
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
