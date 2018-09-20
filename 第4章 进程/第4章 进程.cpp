//第4章 进程.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "第4章 进程.h"

#include <shellapi.h>
#pragma warning(disable:4996)//GetVersionEx

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	HMODULE hModule = GetModuleHandle(nullptr);//获取主调进程的可执行文件的基地址(只检查*主调进程*的地址空间)
	HMODULE hModule2 = nullptr;
	BOOL bRet = GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPTSTR)wWinMain, &hModule2);//获取wWinMain所在模块的基地址,对于dll很有用的

	//获取模块路径
	TCHAR modulename[_MAX_PATH] = { 0 };
	DWORD dRet = GetModuleFileName(hModule2, modulename, sizeof(modulename));
	dRet = GetLongPathName(modulename, modulename, sizeof(modulename));

	TCHAR* cmdline = GetCommandLine();
	int argsnum = 0;
	PWSTR* ppArgv = CommandLineToArgvW(GetCommandLineW(), &argsnum);//切分命令行参数
	for (int i = 0; i < argsnum; ++i)
	{
		ppArgv[i];//第i个参数
	}
	bRet = HeapFree(GetProcessHeap(), 0, ppArgv);
	ppArgv = nullptr;

	LPTSTR envirstr = GetEnvironmentStrings();//获取完整的环境变量字符串
	FreeEnvironmentStrings(envirstr);
	envirstr = nullptr;

	//获取一个环境变量
	DWORD len = GetEnvironmentVariable(TEXT("path"), nullptr, 0);
	PTSTR pszvalue = new TCHAR[len];
	len = GetEnvironmentVariable(TEXT("path"), pszvalue, len);
	//扩展环境变量字符串
	len = ExpandEnvironmentStrings(pszvalue, nullptr, 0);
	PTSTR pszvalue2 = new TCHAR[len];
	len = ExpandEnvironmentStrings(pszvalue, pszvalue2, len);
	delete[] pszvalue2;
	pszvalue2 = nullptr;
	delete[] pszvalue;
	pszvalue = nullptr;

	bRet = SetEnvironmentVariable(TEXT("一个环境变量名"), TEXT("1"));//添加or修改
	bRet = SetEnvironmentVariable(TEXT("一个环境变量名"), nullptr);//删除

	//工作目录
	len = GetCurrentDirectory(0, nullptr);
	PTSTR dir = new TCHAR[len];
	len = GetCurrentDirectory(len, dir);
	delete[] dir;
	dir = nullptr;

	dir = new TCHAR[_MAX_PATH];
	len = GetFullPathName(TEXT("Windows核心编程.sln"), _MAX_PATH, dir, nullptr);//不好用,但这个函数可以获取每个驱动器对应的当前目录~
	delete[] dir;
	dir = nullptr;

	//系统版本
	OSVERSIONINFOEX ver = { 0 };
	ver.dwOSVersionInfoSize = sizeof(ver);
	bRet = GetVersionEx((LPOSVERSIONINFO)&ver);

	//比较版本差异
	//构造一个版本
	OSVERSIONINFOEX osver = { 0 };
	osver.dwOSVersionInfoSize = sizeof(osver);
	osver.dwMajorVersion = 6;
	osver.dwPlatformId = VER_PLATFORM_WIN32_NT;
	//比较标记
	DWORDLONG condition = 0;
	VER_SET_CONDITION(condition, VER_MAJORVERSION, VER_EQUAL);//要求VER_MAJORVERSION相等
	VER_SET_CONDITION(condition, VER_MINORVERSION, VER_EQUAL);
	VER_SET_CONDITION(condition, VER_PLATFORMID, VER_EQUAL);
	//比较版本
	bRet = VerifyVersionInfo(&osver, VER_MAJORVERSION | VER_MINORVERSION | VER_PLATFORMID, condition);//返回FALSE，用GetLastError查看原因

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SECURITY_ATTRIBUTES sap, sat;
	sap.nLength = sizeof(sap);
	sap.lpSecurityDescriptor = nullptr;
	sap.bInheritHandle = TRUE;//进程可继承
	sat.nLength = sizeof(sat);
	sat.lpSecurityDescriptor = nullptr;
	sat.bInheritHandle = FALSE;//线程不可继承
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	TCHAR processcmd[] = TEXT("README.TXT");
	bRet = CreateProcess(TEXT("C:\\WINDOWS\\SYSTEM32\\NOTEPAD.EXE"), processcmd, &sap, &sat, TRUE/*新进程继承句柄*/, CREATE_SUSPENDED | NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi);
	//此时进程内核引用计数==2
	dRet = ResumeThread(pi.hThread);
	bRet = TerminateProcess(pi.hProcess, 10);//(异步)终止进程
	dRet = WaitForSingleObject(pi.hProcess, INFINITE);
	bRet = GetExitCodeProcess(pi.hProcess, &dRet);//==1
	bRet = CloseHandle(pi.hProcess);//==0
	bRet = CloseHandle(pi.hThread);//==0

	//手动提升进程权限
	SHELLEXECUTEINFO sei = { sizeof(sei) };
	sei.lpVerb = TEXT("runas");//特权提升
	sei.lpFile = TEXT("NOTEPAD.EXE");
	sei.lpParameters = TEXT("README.TXT");
	sei.nShow = SW_SHOWNORMAL;
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;//使返回进程句柄
	bRet = ShellExecuteEx(&sei);

	//进程令牌
	HANDLE htoken = nullptr;
	bRet = OpenProcessToken(sei.hProcess, TOKEN_QUERY, &htoken);
	TOKEN_ELEVATION_TYPE elevationtype;//获取启动类型
	bRet = GetTokenInformation(htoken, TokenElevationType, &elevationtype, sizeof(elevationtype), &len);
	CloseHandle(htoken);
	htoken = nullptr;

	TerminateProcess(sei.hProcess, 10);//(异步)终止进程
	bRet = CloseHandle(sei.hProcess);//SEE_MASK_NOCLOSEPROCESS

	system("pause");
	return 0;
}
