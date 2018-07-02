//第12章 纤程.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "第12章 纤程.h"

LPVOID g_Covert = nullptr;
DWORD g_index = 0;

VOID WINAPI FiberFun(LPVOID lpFiberParameter)
{
	//纤程参数
	LPVOID pFiberCurrent = GetCurrentFiber();
	BOOL bres = IsThreadAFiber();
	LPVOID pCurrentData = GetFiberData();

	//纤程局部存储区
	bres = FlsSetValue(g_index, (PVOID)200);
	PVOID flsvalue = FlsGetValue(g_index);

	SwitchToFiber(g_Covert);
}

VOID NTAPI FlsFun(IN PVOID lpFlsData)
{
	//FlsSetValue多少次，系统调用FlsFun就多少次
	//可以主动调用FlsFree删除Fls槽
	//纤程销毁时也会调用FlsFun
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	//将线程转换为纤程
	LPVOID pFiberCovert = ConvertThreadToFiberEx(nullptr, FIBER_FLAG_FLOAT_SWITCH);
	g_Covert = pFiberCovert;

	//纤程局部存储区
	g_index = FlsAlloc(FlsFun);
	BOOL bres = FlsSetValue(g_index, (PVOID)100);
	PVOID flsvalue = FlsGetValue(g_index);

	//创建纤程
	LPVOID pFiberCreate = CreateFiberEx(0, 0, FIBER_FLAG_FLOAT_SWITCH, FiberFun, nullptr);

	//切换纤程
	SwitchToFiber(pFiberCreate);

	bres = FlsFree(g_index);
	//bres = FlsFree(g_index);
	
	//销毁纤程
	DeleteFiber(pFiberCreate);

	//解除线程的纤程状态
	bres = ConvertFiberToThread();

	system("pause");
	return 0;
}