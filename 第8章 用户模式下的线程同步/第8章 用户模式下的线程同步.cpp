//第8章 用户模式下的线程同步.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "第8章 用户模式下的线程同步.h"

LONG g_i = 100;
LONG g_b = FALSE;

CRITICAL_SECTION g_cs; //关键段
SRWLOCK g_rw; //读写锁
CONDITION_VARIABLE g_cv; //条件变量


DWORD WINAPI Thread1(PVOID param)
{
	for (int i = 0; i < 100; ++i)
	{
		EnterCriticalSection(&g_cs);
		g_i += i;
		LeaveCriticalSection(&g_cs);
	}
	return 0;
}

DWORD WINAPI Thread2(PVOID param)
{
	for (int i = 0; i < 100; ++i)
	{
		if (TryEnterCriticalSection(&g_cs))
		{
			g_i += i;
			LeaveCriticalSection(&g_cs);
		}
		else
			SwitchToThread();
	}
	return 0;
}

DWORD WINAPI Thread3(PVOID param)
{
	for (int i = 0; i < 100; ++i)
	{
		AcquireSRWLockExclusive(&g_rw);
		//写
		OutputDebugString(TEXT("------------AcquireSRWLockExclusive succeed.\n"));
		ReleaseSRWLockExclusive(&g_rw);
	}
	return 0;
}

DWORD WINAPI Thread4(PVOID param)
{
	for (int i = 0; i < 100; ++i)
	{
		AcquireSRWLockShared(&g_rw);
		//读
		OutputDebugString(TEXT("------------AcquireSRWLockShared succeed.\n"));
		ReleaseSRWLockShared(&g_rw);
	}
	return 0;
}

DWORD WINAPI Thread5(PVOID param)
{
	int n = 0;
	for (int i = 0; i < 100; ++i)
	{
		EnterCriticalSection(&g_cs);
		++n;
		if (n >= 100)
			break;
		if(g_i <= 0)
			SleepConditionVariableCS(&g_cv, &g_cs, INFINITE); //解锁等待条件变量，返回时再加锁
		g_i--;
		LeaveCriticalSection(&g_cs);
	}
	return 0;
}

DWORD WINAPI Thread6(PVOID param)
{
	for (int i = 0; i < 100; ++i)
	{
		if (TryEnterCriticalSection(&g_cs))
		{
			g_i++;
			WakeConditionVariable(&g_cv); //唤醒等待条件变量的线程
			LeaveCriticalSection(&g_cs);
		}
		else
			SwitchToThread();
	}
	return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	//原子操作Interlocked系列
	LONG newl = InterlockedAdd(&g_i, 1);
	LONG oldl = InterlockedExchange(&g_i, 10);
	oldl = InterlockedCompareExchange(&g_i, 100, 10);
	LONGLONG oldll = InterlockedAnd(&g_i, 0x0001);

	//旋转锁！
	while (InterlockedExchange(&g_b, TRUE) == TRUE)
		Sleep(0);

	//使用关键段
	//InitializeCriticalSection(&g_cs);
	BOOL bret = InitializeCriticalSectionAndSpinCount(&g_cs, 1);//初始化关键段并用上旋转锁
	oldl = SetCriticalSectionSpinCount(&g_cs, 4000);
	g_i = 0;
	HANDLE hthread1 = CreateThread(nullptr, 0, Thread1, nullptr, 0, nullptr);
	HANDLE hthread2 = CreateThread(nullptr, 0, Thread2, nullptr, 0, nullptr);

	WaitForSingleObject(hthread1, INFINITE);
	WaitForSingleObject(hthread2, INFINITE);
	CloseHandle(hthread1);
	hthread1 = nullptr;
	CloseHandle(hthread2);
	hthread2 = nullptr;

	//使用读写锁
	InitializeSRWLock(&g_rw);
	HANDLE hthread3 = CreateThread(nullptr, 0, Thread3, nullptr, 0, nullptr);
	HANDLE hthread4 = CreateThread(nullptr, 0, Thread4, nullptr, 0, nullptr);

	WaitForSingleObject(hthread3, INFINITE);
	WaitForSingleObject(hthread4, INFINITE);
	CloseHandle(hthread3);
	hthread3 = nullptr;
	CloseHandle(hthread4);
	hthread4 = nullptr;

	//条件变量(误入锁？)
	InitializeConditionVariable(&g_cv);
	g_i = 0;
	HANDLE hthread5 = CreateThread(nullptr, 0, Thread5, nullptr, 0, nullptr);
	HANDLE hthread6 = CreateThread(nullptr, 0, Thread6, nullptr, 0, nullptr);

	WaitForSingleObject(hthread5, INFINITE);
	WaitForSingleObject(hthread6, INFINITE);
	CloseHandle(hthread5);
	hthread5 = nullptr;
	CloseHandle(hthread6);
	hthread6 = nullptr;

	DeleteCriticalSection(&g_cs);

	system("pause");
	return 0;
}