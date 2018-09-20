//第10章 同步设备IO与异步设备IO.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "第10章 同步设备IO与异步设备IO.h"

//可提醒IO回调
VOID WINAPI funComplete(
	_In_    DWORD dwErrorCode,
	_In_    DWORD dwNumberOfBytesTransfered,
	_Inout_ LPOVERLAPPED lpOverlapped
)
{
	DWORD id = GetCurrentProcessId();
}

//IO完成端口工作线程
DWORD WINAPI workthread(LPVOID lpThreadParameter)
{
	DWORD NumberOfBytesTransferred;
	DWORD CompletionKey;
	OVERLAPPED* pOverlapped;
	BOOL bres = GetQueuedCompletionStatus(lpThreadParameter, &NumberOfBytesTransferred, &CompletionKey, &pOverlapped, INFINITE);
	//GetQueuedCompletionStatusEx可获取多个IO请求结果
	DWORD dres = GetLastError();

	return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	//打开(创建)文件
	HANDLE hFile = CreateFile(TEXT("第10章 同步设备IO与异步设备IO.cpp"), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(nullptr, TEXT("打开文件失败!"), TEXT("error"), MB_OK);
		return 0;
	}

	//文件类型
	DWORD dres = GetFileType(hFile);
	//dres == FILE_TYPE_DISK

	//文件大小
	LARGE_INTEGER li;
	BOOL bres = GetFileSizeEx(hFile, &li);//逻辑大小
	li.LowPart = GetCompressedFileSize(TEXT("第10章 同步设备IO与异步设备IO.cpp"), (DWORD*)&li.HighPart);//物理大小

	//文件指针
	li.QuadPart = 0;
	bres = SetFilePointerEx(hFile, li, nullptr, FILE_END);
	bres = SetFilePointerEx(hFile, li, &li, FILE_CURRENT);
	bres = SetEndOfFile(hFile);
	li.QuadPart = 0;
	bres = SetFilePointerEx(hFile, li, nullptr, FILE_BEGIN);

	//同步IO
	char* buf = new char[100];
	bres = ReadFile(hFile, buf, 100, &dres, nullptr);
	delete []buf;
	buf = nullptr;

	//刷新缓冲区
	bres = FlushFileBuffers(hFile);

	//CancelSynchronousIo
	//取消线程未完成的同步IO请求

	//关闭文件
	CloseHandle(hFile);
	hFile = nullptr;

	//异步IO
	HANDLE hFile2 = CreateFile(TEXT("第10章 同步设备IO与异步设备IO.cpp"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);
	OVERLAPPED ol;
	ol.Offset = 1024;
	ol.OffsetHigh = 0;
	ol.hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	char* buf2 = new char[1024];
	bres = ReadFile(hFile2, buf2, 1024, nullptr, &ol);//异步方式总是返回FALSE
	dres = GetLastError();
	switch (dres)
	{
	case ERROR_IO_PENDING:
		//IO请求添加成功
		//WaitForSingleObject(hFile2, INFINITE);
		WaitForSingleObject(ol.hEvent, INFINITE);
		break;
	case ERROR_INVALID_USER_BUFFER:
	case ERROR_NOT_ENOUGH_MEMORY:
		break;
	case ERROR_NOT_ENOUGH_QUOTA:
		break;
	default:
		break;
	}

	//取消队列中的IO请求
	//bres = CancelIo(hFile2);
	bres = CancelIoEx(hFile2, &ol);
	//bres = CancelIoEx(hFile2, nullptr);
	//bres = CloseHandle(hFile2);

	if (ol.hEvent != nullptr)
	{
		CloseHandle(ol.hEvent);
		ol.hEvent = nullptr;
	}

	//可提醒IO
	OVERLAPPED ol2;
	ol2.Offset = 1024;
	ol2.OffsetHigh = 0;
	ol2.hEvent = nullptr;
	dres = ReadFileEx(hFile2, buf2, 1024, &ol2, funComplete);//回调函数在同一个线程空间
	DWORD id = GetCurrentProcessId();
	//置为可提醒状态
	//APC队列中只要有一个，线程就不会进入睡眠
	dres = SleepEx(INFINITE, TRUE);
	//WaitForSingleObjectEx
	//WaitForMultipleObjectsEx
	//SignalObjectAndWait
	//GetQueuedCompletionStatusEx
	//MsgWaitForMultipleObjectsEx
	dres = GetLastError();//WAIT_IO_COMPLETION

	//手动添加一项到APC队列
	//可以让目标线程结束睡眠
	dres = QueueUserAPC((PAPCFUNC)funComplete, GetCurrentThread(), NULL);

	//IO完成端口
	HANDLE iocp = CreateIoCompletionPort(hFile2, nullptr, 1, 2);
	HANDLE hts[2];
	for (int i = 0; i < 2; ++i)
		hts[i] = CreateThread(nullptr, 0, workthread, iocp, 0, nullptr);
	Sleep(5000);
	bres = PostQueuedCompletionStatus(iocp, 100, 50, &ol); //模拟发送IO请求完成
	bres = ReadFile(hFile2, buf2, 1024, nullptr, &ol2);
	dres = GetLastError();

	WaitForMultipleObjects(2, hts, TRUE, INFINITE);
	for (int i = 0; i < 2; ++i)
	{
		CloseHandle(hts[i]);
		hts[i] = nullptr;
	}
	CloseHandle(iocp);
	iocp = nullptr;

	CloseHandle(hFile2);
	hFile2 = nullptr;
	delete[]buf2;
	buf2 = nullptr;

	system("pause");
	return 0;
}