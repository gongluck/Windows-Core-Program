//第7章 线程调度、优先级和关联性.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "第7章 线程调度、优先级和关联性.h"

//线程函数
DWORD WINAPI ThreadProc(PVOID param)
{
	return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	//线程的挂起和恢复
	HANDLE hthread = CreateThread(nullptr, 0, ThreadProc, nullptr, CREATE_SUSPENDED, nullptr);
	DWORD suspendcount = SuspendThread(hthread);//返回线程的前一个挂起计数
	suspendcount = ResumeThread(hthread);//返回线程的前一个挂起计数

	BOOL bret = SwitchToThread();//当前线程放弃调度，返回是否有其他可调度线程(低优先级的饥饿线程也会调度)
	Sleep(0);//当前线程放弃调度，但是低优先级的饥饿线程不会调度

	FILETIME c, e, k, u;
	bret = GetThreadTimes(hthread, &c, &e, &k, &u);//获取线程时间

	//(更)精确的时间计算
	LARGE_INTEGER start, end;
	bret = QueryPerformanceCounter(&start);//时钟次数
	LARGE_INTEGER frequency;
	bret = QueryPerformanceFrequency(&frequency);//时钟频率
	bret = QueryPerformanceCounter(&end);
	double time = (double)(end.QuadPart - start.QuadPart) / (double)frequency.QuadPart;

	//黑科技GetThreadContext、SetThreadContext

	//进程优先级
	STARTUPINFO si = { 0 };
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi = { 0 };
	bret = CreateProcess(TEXT("C:\\WINDOWS\\SYSTEM32\\NOTEPAD.EXE"), nullptr, nullptr, nullptr, FALSE, CREATE_SUSPENDED|NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi);
	bret = SetPriorityClass(pi.hProcess, BELOW_NORMAL_PRIORITY_CLASS);//修改进程优先级
	DWORD priority = GetPriorityClass(pi.hProcess);

	//线程优先级
	bret = SetThreadPriority(pi.hThread, THREAD_PRIORITY_BELOW_NORMAL);
	priority = GetThreadPriority(pi.hThread);

	//禁止系统自动的优先级提升
	BOOL bPriority;
	bret = SetProcessPriorityBoost(pi.hProcess, FALSE);
	bret = GetProcessPriorityBoost(pi.hProcess, &bPriority);
	bret = SetThreadPriorityBoost(pi.hThread, FALSE);
	bret = GetThreadPriorityBoost(pi.hThread, &bPriority);

	CloseHandle(pi.hProcess);
	pi.hProcess = nullptr;
	CloseHandle(pi.hThread);
	pi.hThread = nullptr;

	//调度IO请求优先级
	bret = SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN);//降低线程调度优先级，不允许改变另一个线程的IO优先级
	/*IO操作*/
	bret = SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_END);//让线程进程normal级别的调度级别，不允许改变另一个线程的IO优先级
	//设置进程下所有进程的IO请求优先级
	bret = SetPriorityClass(GetCurrentProcess(), PROCESS_MODE_BACKGROUND_BEGIN);
	bret = SetPriorityClass(GetCurrentProcess(), PROCESS_MODE_BACKGROUND_END);

	//单独设置文件IO优先级
	HANDLE hFile = CreateFile(TEXT("C:\\gongluck"), 0, 0, NULL, CREATE_NEW, 0, NULL);
	DWORD err = GetLastError();
	FILE_IO_PRIORITY_HINT_INFO phi;
	phi.PriorityHint = IoPriorityHintLow;
	bret = SetFileInformationByHandle(hFile, FileIoPriorityHintInfo, &phi, sizeof(phi));
	err = GetLastError();
	CloseHandle(hFile);
	hFile = nullptr;

	//进程和CPU关联性
	DWORD processmask = 0;
	DWORD systemmask = 0;
	bret = GetProcessAffinityMask(GetCurrentProcess(), &processmask, &systemmask);
	bret = SetProcessAffinityMask(GetCurrentProcess(), processmask);
	bret = GetProcessAffinityMask(GetCurrentProcess(), &processmask, &systemmask);

	//线程和CPU关联性
	DWORD oldmask = SetThreadAffinityMask(hthread, 0x00000001);//限制只能在CPU0上运行
	DWORD oldidea = SetThreadIdealProcessor(hthread, MAXIMUM_PROCESSORS);//设置理想CPU

	CloseHandle(hthread);
	hthread = nullptr;

	system("pause");
	return 0;
}