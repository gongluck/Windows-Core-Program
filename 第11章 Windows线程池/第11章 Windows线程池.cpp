//第11章 Windows线程池.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "第11章 Windows线程池.h"

VOID NTAPI SimpleCB(
	_Inout_     PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID                 Context
)
{

}

VOID NTAPI WorkCB(
	_Inout_     PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID                 Context,
	_Inout_     PTP_WORK              Work
)
{

}

VOID NTAPI TimerCB(
	_Inout_     PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID                 Context,
	_Inout_     PTP_TIMER             Timer
)
{
	static DWORD i = 0;
	i += 1;
	static LARGE_INTEGER li;
	li.QuadPart = -10000000ll * i;
	static FILETIME duetime = { 0 };
	duetime.dwLowDateTime = li.LowPart;
	duetime.dwHighDateTime = li.HighPart;

	SetThreadpoolTimer(Timer, &duetime, 0, 0); //设置定时器
}

VOID NTAPI WaitCB(
	_Inout_     PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID                 Context,
	_Inout_     PTP_WAIT              Wait,
	_In_        TP_WAIT_RESULT        WaitResult
	)
{
	switch (WaitResult)
	{
	case WAIT_OBJECT_0:
		break;
	case WAIT_TIMEOUT:
		break;
	case WAIT_ABANDONED:
		break;
	default:
		break;
	}
}

VOID WINAPI IoCB(
	_Inout_     PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID                 Context,
	_Inout_opt_ PVOID                 Overlapped,
	_In_        ULONG                 IoResult,
	_In_        ULONG_PTR             NumberOfBytesTransferred,
	_Inout_     PTP_IO                Io
)
{
	switch (IoResult)
	{
	case NO_ERROR:
		break;
	default:
		break;
	}
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	//以异步方式调用函数
	BOOL bres = TrySubmitThreadpoolCallback(SimpleCB, nullptr, nullptr); //将工作项添加到线程池队列
	PTP_WORK pwork = CreateThreadpoolWork(WorkCB, nullptr, nullptr); //创建一个工作项
	SubmitThreadpoolWork(pwork); //向线程池提交一个请求
	WaitForThreadpoolWorkCallbacks(pwork, FALSE/*是否先尝试取消提交的工作项*/); //取消工作项或等待完成
	CloseThreadpoolWork(pwork); // 释放工作项内存
	pwork = nullptr;

	//每隔一段时间调用一个函数
	PTP_TIMER ptimer = CreateThreadpoolTimer(TimerCB, nullptr, nullptr); //创建定时器
	LARGE_INTEGER li;
	li.QuadPart = -1ll;//立即开始
	FILETIME duetime = { 0 };
	duetime.dwLowDateTime = li.LowPart; 
	duetime.dwHighDateTime = li.HighPart;
	SetThreadpoolTimer(ptimer, &duetime, 1/*再次调用的时间间隔*/, 0/*用来给回调函数的执行时间增加一些随机性*/); //设置定时器
	WaitForThreadpoolTimerCallbacks(ptimer, FALSE); //调试发现，TimerCB没机会执行，也没有阻塞主线程啊!?
	bres = IsThreadpoolTimerSet(ptimer); //检查定时器状态

	//在内核对象触发时调用一个函数
	PTP_WAIT pwait = CreateThreadpoolWait(WaitCB, NULL, nullptr); //创建线程池等待对象
	HANDLE hevent = CreateEvent(nullptr, FALSE, TRUE, nullptr);
	SetThreadpoolWait(pwait, hevent, nullptr); //绑定到线程池
	WaitForThreadpoolWaitCallbacks(pwait, FALSE);

	//在异步IO请求完成时调用一个函数
	HANDLE hFile = CreateFile(TEXT("第11章 Windows线程池.cpp"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);
	PTP_IO pio = CreateThreadpoolIo(hFile, IoCB, nullptr, nullptr); //创建线程池IO对象
	char buf[_MAX_PATH] = { 0 };
	OVERLAPPED ol = { 0 };
	StartThreadpoolIo(pio); //每次IO调用之前，都要调用StartThreadpoolIo启用线程池IO对象
	bres = ReadFile(hFile, buf, _MAX_PATH, nullptr, &ol);
	StartThreadpoolIo(pio);
	bres = ReadFile(hFile, buf, _MAX_PATH, nullptr, &ol);
	WaitForThreadpoolIoCallbacks(pio, FALSE);
	StartThreadpoolIo(pio);
	CancelThreadpoolIo(pio); //撤销线程池IO对象
	bres = ReadFile(hFile, buf, _MAX_PATH, nullptr, &ol);

	system("pause");

	SetThreadpoolTimer(ptimer, nullptr, 0, 0); //取消定时器
	CloseThreadpoolTimer(ptimer);
	ptimer = nullptr;

	SetThreadpoolWait(pwait, nullptr, nullptr);
	CloseThreadpoolWait(pwait);
	pwait = nullptr;
	CloseHandle(hevent);
	hevent = nullptr;

	//CloseThreadpoolIo(pio);  //调用CancelThreadpoolIo之后不用CloseThreadpoolIo了
	pio = nullptr;

	CloseHandle(hFile);
	hFile = nullptr;

	return 0;
}
