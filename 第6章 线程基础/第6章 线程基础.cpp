//第6章 线程基础.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "第6章 线程基础.h"
#include <process.h>

//线程函数
DWORD WINAPI ThreadProc(PVOID param)
{
	return 0;
}
unsigned __stdcall ThreadProc2(void* param)
{
	return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	//CreateThread和_beginthreadex的本质区别
	//https://blog.csdn.net/morewindows/article/details/7421759

	//创建Windows线程
	HANDLE hthread = CreateThread(nullptr, 0, ThreadProc, nullptr, CREATE_SUSPENDED, nullptr);
	ResumeThread(hthread);
	system("pause");
	BOOL bRet = TerminateThread(hthread, 0);//终止线程（异步）,可能会导致资源没有释放（例如没有调用线程里类实例的析构函数！）
	DWORD exitcode;
	bRet = GetExitCodeThread(hthread, &exitcode);//可能是STILL_ACTIVE或者退出代码
	WaitForSingleObject(hthread, INFINITE);
	CloseHandle(hthread);
	hthread = nullptr;

	//创建C++线程
	hthread = (HANDLE)_beginthreadex(nullptr, 0, ThreadProc2, nullptr, 0, nullptr);
	CloseHandle(hthread);
	hthread = nullptr;

	//将GetCurrentThread()得到的伪句柄转换成可用的句柄.(记住：句柄表属于进程，线程共享句柄表)
	bRet = DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &hthread, 0, FALSE, DUPLICATE_SAME_ACCESS);
	CloseHandle(hthread);

	system("pause");
	return 0;
}