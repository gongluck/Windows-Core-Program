//第1章 错误处理.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "第1章 错误处理.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	HANDLE hFile = CreateFile(TEXT("C:\\gongluck"), 0, 0, nullptr, OPEN_EXISTING, 0, nullptr);

	//获取上一个系统调用的返回状态
	DWORD err = GetLastError();

	HLOCAL hlocal = nullptr;
	if (err != ERROR_SUCCESS)
	{
		//将错误码（可指定自然语言）格式化输出
		DWORD res = FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM/*函数会从系统信息列表中搜索所请求的信息。如果使用FORMAT_MESSAGE_FROM_HMODULE，函数会先在lpSource指定的模块中搜索请求的消息，如果搜索不到再去搜索系统消息表资源。此标志不能与FORMAT_MESSAGE_FROM_STRING同时使用。*/ | 
			FORMAT_MESSAGE_IGNORE_INSERTS/*指定消息定义中的插入序列将被忽略，并将其直接传递给输出缓冲区。 此标志对于获取稍后格式化的消息很有用。 如果设置了此标志，则忽略Arguments参数。*/ | 
			FORMAT_MESSAGE_ALLOCATE_BUFFER/*函数会分配一个足够大的缓冲区保存格式化消息，并且通过lpBuffer指向该地址。当不再使用lpBuffer数据时，需调用LocalFree释放内存。*/,
			nullptr,
			err,/*请求的消息的标识符。当dwFlags标志为FORMAT_MESSAGE_FROM_STRING时会被忽略。*/
			MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),/*请求的消息的语言标识符。*/
			(LPTSTR)&hlocal,/*接收错误信息描述的缓冲区指针。*/
			0,/*如果FORMAT_MESSAGE_ALLOCATE_BUFFER标志没有被指定，这个参数必须指定为输出缓冲区的大小，如果指定，这个参数指定为分配给输出缓冲区的最小数。*/
			nullptr/*保存格式化信息中的插入值的一个数组。*/
		);
		MessageBox(nullptr, (PCTSTR)hlocal, TEXT("err"), MB_OK);
	}
	if (hlocal != nullptr)
	{
		hlocal = LocalFree(hlocal);//释放局部内存对象并使句柄失效
		if (hlocal != nullptr)
		{
			MessageBox(nullptr, TEXT("LocalFree err"), TEXT("err"), MB_OK);
		}
	}

	//FormatMessage的格式化输出功能(好像用处不大)
	int nYear = 2018, nMonth = 5, nDay = 22;
	TCHAR szYear[5], szMonth[3], szDay[3];
	wsprintf(szYear, TEXT("%d"), nYear);
	wsprintf(szMonth, TEXT("%d"), nMonth);
	wsprintf(szDay, TEXT("%d"), nDay);
	LPTSTR lpSource = (LPTSTR)TEXT("今天是:%1年%2月%3日");
	void* pArgs[] = { (void*)szYear, (void*)szMonth, (void*)szDay };
	const DWORD size = 100 + 1;
	WCHAR buffer[size];
	if (FormatMessage(
			FORMAT_MESSAGE_FROM_STRING/*lpSource参数是一个指向以NULL结尾的字符串，字符串包含一个消息定义，这个消息定义可以包含插入序列。此标志不能与FORMAT_MESSAGE_FROM_HMODULE 、FORMAT_MESSAGE_FROM_SYSTEM同时使用*/ | 
			FORMAT_MESSAGE_ARGUMENT_ARRAY/*Arguments参数不是指向va_list结构体，是一个指向保存参数的数组指针。*/,
			lpSource,
			0,
			0,
			buffer,
			size,
			(va_list*)pArgs
			)
		)
	{
		MessageBox(nullptr, buffer, TEXT("Date"), MB_ICONINFORMATION);
	}
	
	system("pause");
	return 0;
}
