//第3章 内核对象.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "第3章 内核对象.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	SECURITY_ATTRIBUTES sa = { 0 };
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = nullptr;
	sa.bInheritHandle = TRUE;//可被继承
	HANDLE hMutex = CreateMutex(&sa, FALSE, nullptr);
	BOOL bRet = SetHandleInformation(hMutex, HANDLE_FLAG_INHERIT, 0);//关闭可继承
	bRet = SetHandleInformation(hMutex, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);//打开可继承

	HANDLE hMutex2 = CreateMutex(nullptr, FALSE, TEXT("有名的Mutex"));
	DWORD err = GetLastError();
	if (err == ERROR_ALREADY_EXISTS)
		MessageBox(NULL, TEXT("打开了已经创建的Mutex"), TEXT("CreateMutex"), MB_OK);
	else
		MessageBox(NULL, TEXT("创建了新的的Mutex"), TEXT("CreateMutex"), MB_OK);

	HANDLE hMutex3 = OpenMutex(MUTEX_ALL_ACCESS, TRUE, TEXT("有名的Mutex"));

	//灵活的(跨进程)句柄复制
	HANDLE hMutex4 = nullptr;
	bRet = DuplicateHandle(GetCurrentProcess(), hMutex, GetCurrentProcess(), &hMutex4, 0, FALSE, DUPLICATE_SAME_ACCESS);

	bRet = CloseHandle(hMutex);//调用CloseHandle后，句柄表中的对应值就被重置了，hMutex不会指向原来的内核对象(无论此时引用计数是否为0)
	hMutex = nullptr;//良好的习惯
	bRet = CloseHandle(hMutex2);
	hMutex2 = nullptr;
	bRet = CloseHandle(hMutex3);
	hMutex3 = nullptr;
	bRet = CloseHandle(hMutex4);
	hMutex4 = nullptr;
	system("pause");
	return 0;
}