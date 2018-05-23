//第2章 字符和字符串处理.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "第2章 字符和字符串处理.h"

#include "StrSafe.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	TCHAR str[] = TEXT("第2章 字符和字符串处理");
	size_t len = _tcslen(str);

	TCHAR str2[5] = { 0 };
	//errno_t eno = _tcscpy_s(str2, _countof(str2), str);//缓冲区过小程序会终止

	PTCHAR pend = nullptr;//字符串结尾指针
	size_t left = 0;//目标缓冲区剩余(加上字符串结尾，所以>=1)
	//截断拷贝
	HRESULT hres = StringCchCatEx(str2, _countof(str2), str, &pend, &left, STRSAFE_FILL_BEHIND_NULL);

	//字符串(码位)比较
	//0-调用失败，
	//CSTR_LESS_THAN            1           // string 1 less than string 2
	//CSTR_EQUAL                2           // string 1 equal to string 2
	//CSTR_GREATER_THAN         3           // string 1 greater than string 2
	int ires = CompareStringOrdinal(str, _countof(str), str2, _countof(str2), FALSE);

	//字符串转换
	//https://github.com/gongluck/Tools/tree/master/TransCode
	char ansic[] = "第2章 字符和字符串处理";
	int ilen = MultiByteToWideChar(CP_ACP, 0, ansic, -1, NULL, 0);
	wchar_t* unicode = new wchar_t[ilen];
	MultiByteToWideChar(CP_ACP, 0, ansic, -1, unicode, ilen);//我觉得书中对最后一个参数的解释错了，应该是“字符数”。
	delete[] unicode;

	system("pause");
	return 0;
}