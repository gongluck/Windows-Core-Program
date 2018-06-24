//第9章 用内核对象进行线程同步.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "第9章 用内核对象进行线程同步.h"

DWORD WINAPI Thread(PVOID param)
{
	Sleep(2000);
	return 0;
}

HANDLE g_event = nullptr;
DWORD WINAPI Thread2(PVOID param)
{
	WaitForSingleObject(g_event, INFINITE);
	//使事件未触发
	BOOL bres = ResetEvent(g_event);
	//处理事件
	//...
	//触发事件
	bres = SetEvent(g_event);
	return 0;
}

HANDLE g_timer = nullptr;
DWORD WINAPI Thread3(PVOID param)
{
	WaitForSingleObject(g_timer, INFINITE);
	return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	HANDLE hthread = CreateThread(nullptr, 0, Thread, nullptr, 0, nullptr);

	//等待线程被触发
	DWORD dres = WaitForSingleObject(hthread, INFINITE);//INFINITE等待无限长的时间
	switch (dres)
	{
	case WAIT_OBJECT_0:
		//线程被触发(终止)
		CloseHandle(hthread);
		hthread = nullptr;
		break;
	case WAIT_TIMEOUT:
		//超时
		break;
	case WAIT_FAILED:
		//FAILED
		break;
	}

	HANDLE hthreads[3];
	for (int i = 0; i < 3; ++i)
		hthreads[i] = CreateThread(nullptr, 0, Thread, nullptr, 0, nullptr);
	//等待多个线程被触发
	dres = WaitForMultipleObjects(3, hthreads, FALSE, INFINITE);
	switch (dres)
	{
	case WAIT_OBJECT_0+0:
		//线程被触发(终止)
		CloseHandle(hthreads[0]);
		hthreads[0] = nullptr;
		break;
	case WAIT_OBJECT_0+1:
		//线程被触发(终止)
		CloseHandle(hthreads[1]);
		hthreads[1] = nullptr;
		break;
	case WAIT_OBJECT_0+2:
		//线程被触发(终止)
		CloseHandle(hthreads[2]);
		hthreads[2] = nullptr;
		break;
	case WAIT_TIMEOUT:
		//超时
		break;
	case WAIT_FAILED:
		//FAILED
		break;
	}
	dres = WaitForMultipleObjects(3, hthreads, TRUE, INFINITE);
	for (int i = 0; i < 3; ++i)
	{
		if (hthreads[i] == nullptr)
			continue;
		CloseHandle(hthreads[i]);
		hthreads[i] = nullptr;
	}

	//事件
	//OpenEvent打开已创建的事件
	g_event = CreateEvent(nullptr, TRUE, FALSE, nullptr);//如果是自动重置事件，能让等待线程得到事件后马上把事件置为未触发，造成同时只有一个线程得到事件！
	HANDLE hthreads2[2];
	for (int i = 0; i < 2; ++i)
		hthreads2[i] = CreateThread(nullptr, 0, Thread2, nullptr, 0, nullptr);
	SetEvent(g_event);//触发事件
	dres = WaitForMultipleObjects(2, hthreads2, TRUE, INFINITE);
	for (int i = 0; i < 2; ++i)
	{
		if (hthreads2[i] == nullptr)
			continue;
		CloseHandle(hthreads2[i]);
		hthreads2[i] = nullptr;
	}
	CloseHandle(g_event);
	g_event = nullptr;

	//可等待的计时器内核对象
	//OpenWaitableTimer打开已创建的定时器
	g_timer = CreateWaitableTimer(nullptr, TRUE, nullptr);
	LARGE_INTEGER li;
	li.QuadPart = -(2 * 10000000);
	SetWaitableTimer(g_timer, &li, 0, nullptr, nullptr, FALSE);
	HANDLE hthread3 = CreateThread(nullptr, 0, Thread3, nullptr, 0, nullptr);
	WaitForSingleObject(hthread3, INFINITE);
	CloseHandle(hthread3);
	hthread3 = nullptr;
	CancelWaitableTimer(g_timer);
	CloseHandle(g_timer);
	g_timer = nullptr;

	//信号量
	//CreateSemaphore
	//OpenSemaphore
	//WaitForSingleObject
	//ReleaseSemaphore

	//互斥量
	//CreateMutex
	//OpenMutex
	//WaitForSingleObject
	//ReleaseMutex


	system("pause");
	return 0;
}