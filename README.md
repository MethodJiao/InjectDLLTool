# InjectDLLTool
利用Windows API开发的DLL注入工具

![image](https://user-images.githubusercontent.com/45934872/209620069-3e07ce87-e06d-402b-b15d-bbf67115a7be.png)

1.运行TestDestInject为被注入方程序

2.运行InjectDlg为注入方程序

3.运行点击InjectDlg对话框的注入按钮，然后再点击TestDestInject的刷新按钮发现值已经被注入dll（TestInjectDll）修改


## 为什么要DLL注入？
有一些操作放在目标程序中比远程执行更方便，可定制性强。注入DLL可以帮助目标程序实现更多功能。

## 工具使用
### 流程
- 选择DLL
- 输入进程名，大小写敏感（推荐使用火绒剑查看）
- 点击“注入”按钮
- （此时转到DLL操作）
- 点击“卸载”按钮（如果不卸载就不能再次注入）
- 退出本程序
### 问题
1. 如果点击“注入”按钮之后，DLL没有注入或者无反应，则是DLL的问题，与本程序无关。
2. 如果多次点击“注入”按钮没有反应，先尝试点击“卸载”按钮，知道弹出“卸载失败”后再次注入。

## 原理解释
### DLL注入步骤
大概流程如下：
1. 打开进程，获取进程句柄
2. 在目标进程中申请内存空间
3. 写入DLL路径
4. 创建远程调用线程通过LoadLibrary载入dll

涉及的API主要如下：
- OpenProcess
- VirtualAllocEx

- WriteProcessMemory

- VirtualFreeEx

- CreateRemoteThread

- WaitForSingleObject

示例代码：
```c++
BOOL InjectDll(char * szDllPath, DWORD dwPID)

{

       // 获取进程访问权限

       HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);

       if (!hProcess)

       {

              return FALSE;

       }

       // 申请内存空间

       LPVOID lpDllMem = VirtualAllocEx(hProcess, NULL, sizeof(szDllPath), 
MEM_COMMIT, PAGE_READWRITE);

       if (!lpDllMem)

       {

              CloseHandle(hProcess);

              return FALSE;

       }

       // dll路径写入内存

       if (0 == WriteProcessMemory(hProcess, lpDllMem, szDllPath, 
sizeof(szDllPath), NULL))

       {

              VirtualFreeEx(hProcess, lpDllMem, sizeof(szDllPath), MEM_DECOMMIT);

              CloseHandle(hProcess);

              return FALSE;

       }

       // 调用远程dll

       HMODULE hK32 = GetModuleHandle(_T("Kernel32.dll"));

       LPVOID lpLoadLibAddr = GetProcAddress(hK32, "LoadLibraryA");

       HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, 
(LPTHREAD_START_ROUTINE)lpLoadLibAddr, lpDllMem, 0, 0);

       if (!hThread)

       {

              VirtualFreeEx(hProcess, lpDllMem, sizeof(szDllPath), MEM_DECOMMIT);

              CloseHandle(hProcess);

              return FALSE;

       }



       WaitForSingleObject(hThread, INFINITE);

       VirtualFreeEx(hProcess, lpDllMem, sizeof(szDllPath), MEM_DECOMMIT);

       CloseHandle(hThread);

       CloseHandle(hProcess);

       /*TCHAR test[100] = { 0 };

       _sntprintf_s(test, sizeof(test)/sizeof(TCHAR), _T("写入的地址： %p"), 
lpDllMem);

       OutputDebugString(test);*/

       return TRUE;

}

```

### DLL卸载

卸载与注入步骤类似，注入用的LoadLibrary，卸载用FreeLibrary。示例如下：
```c++
BOOL UnInjectDll(char * szDllPath, DWORD dwPID)

{
       char szDName[0x100] = "WeChatHelperDll.dll";

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

       HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, 
(LPTHREAD_START_ROUTINE)lpFreeLibAddr, hDll, 0, 0);

       if (!hThread)

       {

              CloseHandle(hProcess);

              return FALSE;

       }



       WaitForSingleObject(hThread, INFINITE);

       CloseHandle(hThread);

       CloseHandle(hProcess);

       return TRUE;

}
```
注意这里没有使用的`FreeLibrary`，在Win10系统中测试注入微信，发生了异常崩溃，改用`FreeLibraryAndExitThread`后正常卸载，原因尚不明确。
