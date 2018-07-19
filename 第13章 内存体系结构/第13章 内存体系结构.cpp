//第13章 内存体系结构.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "第13章 内存体系结构.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	WORD w;
	PVOID test = &w;
	char c = *(PBYTE)test;

	//访问错位数据
	test = (PBYTE)&w + 1;
	DWORD d = *(UNALIGNED PDWORD)test;//UNALIGNED在x86平台无效

	if ((int)&d % sizeof(d) == 0)
		OutputDebugString(TEXT("***********对齐***********\n"));
	else
		OutputDebugString(TEXT("***********未对齐***********\n"));

	system("pause");
	return 0;
}