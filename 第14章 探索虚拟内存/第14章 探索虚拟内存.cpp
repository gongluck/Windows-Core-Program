//第14章 探索虚拟内存.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "第14章 探索虚拟内存.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	//系统信息
	SYSTEM_INFO sysinfo = { 0 };
	GetSystemInfo(&sysinfo);

	//获取逻辑处理器的详细信息
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION pBuffer = nullptr;
	DWORD dwSize = 0;
	BOOL bRet = GetLogicalProcessorInformation(pBuffer, &dwSize);
	DWORD lErr = GetLastError(); //ERROR_INSUFFICIENT_BUFFER;
	pBuffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(dwSize);
	bRet = GetLogicalProcessorInformation(pBuffer, &dwSize);
	for (DWORD i = 0; i < dwSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); i++)
	{
		pBuffer[i].Relationship == RelationProcessorCore ? 
			"指定的逻辑处理器共享一个处理器核心。ProcessorCore成员包含额外的信息。" : "";
		pBuffer[i].ProcessorCore.Flags == 1 ?
			"如果这个成员的值是1，那么由ProcessorMask成员的值来识别的逻辑处理器，就像在超线程或SMT中一样。否则，被识别的逻辑处理器不共享功能单元。" : "";
		pBuffer[i].Relationship == RelationCache ? 
			"The specified logical processors share a cache. The Cache member contains additional information. " : "";
	}
	free(pBuffer);
	pBuffer = nullptr;

	//Windows 32-bit On Windows 64-bit (WOW64)
	BOOL iswow = FALSE;
	bRet = IsWow64Process(GetCurrentProcess(), &iswow);//只有32位程序运行在WOW64上时iswow才被置为TRUE。
	
	//获取原来的系统信息(取代GetSystemInfo)
	GetNativeSystemInfo(&sysinfo);

	//虚拟内存状态
	MEMORYSTATUSEX memstatusex = { 0 };//ex增强在支持4GB内存大小长度
	memstatusex.dwLength = sizeof(memstatusex);
	bRet = GlobalMemoryStatusEx(&memstatusex);
	memstatusex.ullTotalPhys;//物理内存总量
	memstatusex.ullAvailPhys;//可分配的内存总量
	memstatusex.ullTotalVirtual;//进程私有空间大小
	memstatusex.ullAvailVirtual;//进程闲置空间大小

	//确定地址空间的状态
	MEMORY_BASIC_INFORMATION info = { 0 };
	lErr = VirtualQueryEx(GetCurrentProcess(), (LPCVOID)0x55, &info, sizeof(info));
	switch (info.State)//State 用于指明所有相邻页面的状态。
	{
	case MEM_FREE://空闲状态。该区域的虚拟地址不受任何内存的支持。该地址空间没有被保留。该状态下AllocationBase、AllocationProtect、Protect和Type等成员均未定义。
		break;
	case MEM_RESERVE://指明页面被保留，但是没有分配任何物理内存。该状态下Protect成员未定。
		break;
	case MEM_COMMIT://指明已分配物理内存或者系统页文件。
		break;
	default:
		break;
	}

	system("pause");
	return 0;
}