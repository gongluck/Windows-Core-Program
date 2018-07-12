//第5章 作业.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "第5章 作业.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	//在调试一个作业(Job)程序时发现，如下代码总是返回TRUE，无论是从VS中启动调试还是从资源管理器中启动。
	//原来，从资源管理器或者VS中启动程序时，系统会自动把该进程放到一个作业(Job)中。知道了这一点，要想让这段代码返回FALSE，只要从CMD中启动该程序即可。
	BOOL bInJob = FALSE;
	BOOL bret = IsProcessInJob(GetCurrentProcess(), nullptr, &bInJob);
	if (bInJob)
		MessageBox(nullptr, TEXT("process already in a job"), TEXT("info"), MB_OK);
	else
		MessageBox(nullptr, TEXT("process not in a job"), TEXT("info"), MB_OK);

	HANDLE hJob = CreateJobObject(nullptr, TEXT("第5章 作业"));
	JOBOBJECT_BASIC_LIMIT_INFORMATION basiclimit = { 0 };
	//作业(进程沙盒)限制
	//JobObjectBasicLimitInformation、JobObjectExtendedLimitInformation、JobObjectBasicUIRestrictions、JobObjectSecurityLimitInformation
	//UserHandleGrantAccess授予或禁止一个Job中的进程访问施加了UI限制的用户对象的句柄的权限.当授予了访问权限，所有相关联的进程都可以在随后识别和使用这个句柄.当访问被拒绝，该进程不能在使用该句柄.
	basiclimit.LimitFlags = JOB_OBJECT_LIMIT_PRIORITY_CLASS | JOB_OBJECT_LIMIT_JOB_TIME;
	basiclimit.PriorityClass = IDLE_PRIORITY_CLASS;//指定关联进程的优先级类
	basiclimit.PerJobUserTimeLimit.QuadPart = 100000;//分配给进程的最大用户模式时间（100纳秒）
	bret = SetInformationJobObject(hJob, JobObjectBasicLimitInformation, &basiclimit, sizeof(basiclimit));
	bret = QueryInformationJobObject(hJob, JobObjectBasicLimitInformation, &basiclimit, sizeof(basiclimit), nullptr);
	JOBOBJECT_END_OF_JOB_TIME_INFORMATION endoftime;
	endoftime.EndOfJobTimeAction = JOB_OBJECT_POST_AT_END_OF_JOB;//到期不终止进程(If no completion port is associated with the job when the time limit has been exceeded, the action taken is the same as for JOB_OBJECT_TERMINATE_AT_END_OF_JOB。)JOB_OBJECT_TERMINATE_AT_END_OF_JOB（到期终止进程）
	bret = SetInformationJobObject(hJob, JobObjectEndOfJobTimeInformation, &endoftime, sizeof(endoftime));
	bret = QueryInformationJobObject(hJob, JobObjectEndOfJobTimeInformation, &endoftime, sizeof(endoftime), nullptr);

	//将进程放入作业中,只能添加未运行的“无业”进程
	STARTUPINFO si = { 0 };
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi = { 0 };
	bret = CreateProcess(TEXT("C:\\WINDOWS\\SYSTEM32\\NOTEPAD.EXE"), nullptr,  nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi);
	bret = AssignProcessToJobObject(hJob, pi.hProcess);
	ResumeThread(pi.hThread);

	//查询作业(进程)统计信息
	JOBOBJECT_BASIC_ACCOUNTING_INFORMATION accountinfo = { 0 };
	bret = QueryInformationJobObject(hJob, JobObjectBasicAccountingInformation, &accountinfo, sizeof(accountinfo), nullptr);
	FILETIME c, e, k, u;
	bret = GetProcessTimes(pi.hProcess, &c, &e, &k, &u);
	JOBOBJECT_BASIC_AND_IO_ACCOUNTING_INFORMATION ioaccountinfo = { 0 };
	bret = QueryInformationJobObject(hJob, JobObjectBasicAndIoAccountingInformation, &ioaccountinfo, sizeof(ioaccountinfo), nullptr);
	IO_COUNTERS ioconters;
	bret = GetProcessIoCounters(pi.hProcess, &ioconters);

	//查询作业内进程
	const int max = 10;
	DWORD cb = sizeof(JOBOBJECT_BASIC_PROCESS_ID_LIST) + (max - 1) * sizeof(ULONG_PTR);
	PJOBOBJECT_BASIC_PROCESS_ID_LIST pidlist = (PJOBOBJECT_BASIC_PROCESS_ID_LIST)malloc(cb);
	pidlist->NumberOfAssignedProcesses = max;
	bret = QueryInformationJobObject(hJob, JobObjectBasicProcessIdList, pidlist, cb, &cb);
	for (int i = 0; i < pidlist->NumberOfProcessIdsInList; ++i)
	{
		pidlist->ProcessIdList[i];
	}
	free(pidlist);
	pidlist = nullptr;

	//已分配的CPU时间到期时，作业状态变成已触发
	WaitForSingleObject(hJob, INFINITE);

	system("pause");

	//终止作业中所有的进程
	bret = TerminateJobObject(hJob, 0);

	CloseHandle(pi.hProcess);
	pi.hProcess = nullptr;
	CloseHandle(pi.hThread);
	pi.hThread = nullptr;
	system("pause");
	return 0;
}