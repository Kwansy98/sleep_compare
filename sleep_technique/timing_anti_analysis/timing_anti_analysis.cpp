// timing_anti_analysis.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <stdio.h>
#include <Windows.h>

#define PRINT_FUNCTION() puts("******************" __FUNCTION__" **********************")

#define DELAY_MS 5000

// 由于kusershareddata未及时更新tickcount导致的误差范围，win7虚拟机会出现这种误差
#define DELAY_DEVIATION 50

bool g_phase_1 = true;

// 沙箱探测

void tickcount_check()
{
	PRINT_FUNCTION();
	DWORD tickReference = GetTickCount();
	Sleep(DELAY_MS);
	DWORD currentTick = GetTickCount();
	DWORD elapsedTime = currentTick - tickReference;

	if (elapsedTime < DELAY_MS - DELAY_DEVIATION)
	{
		printf("sleep api hooked expected: %d   actual: %d\n", DELAY_MS, elapsedTime);
		g_phase_1 = false;
	}
	else if (elapsedTime > DELAY_MS + 1000)
	{
		printf("debugger detected\n");
		g_phase_1 = false;
	}
	else
	{
		printf("good\n");
	}
}

void local_time_check()
{
	
	PRINT_FUNCTION();

	SYSTEMTIME sysStart, sysEnd;
	FILETIME fStart, fEnd;
	ULARGE_INTEGER uiStart, uiEnd;

	GetLocalTime(&sysStart);
	Sleep(DELAY_MS);
	GetLocalTime(&sysEnd);

	SystemTimeToFileTime(&sysEnd, &fEnd);
	SystemTimeToFileTime(&sysStart, &fStart);

	uiStart.LowPart = fStart.dwLowDateTime;
	uiStart.HighPart = fStart.dwHighDateTime;
	uiEnd.LowPart = fEnd.dwLowDateTime;
	uiEnd.HighPart = fEnd.dwHighDateTime;


	ULONGLONG d = (uiEnd.QuadPart - uiStart.QuadPart) / 10000;
	if (d < DELAY_MS - DELAY_DEVIATION)
	{
		printf("sleep api hooked expected: %d   actual: %lld\n", DELAY_MS, d);
		g_phase_1 = false;
	}
	else if (d > DELAY_MS + 1000)
	{
		printf("sleep & local time mismatch\n");
		g_phase_1 = false;
	}
	else
	{
		printf("good\n");
	}
}

void qpc_check()
{
	PRINT_FUNCTION();
	LARGE_INTEGER start, end, frequency;
	QueryPerformanceCounter(&start);
	QueryPerformanceFrequency(&frequency);

	Sleep(DELAY_MS);

	QueryPerformanceCounter(&end);

	auto d = (end.QuadPart - start.QuadPart) * 1000 / frequency.QuadPart;
	if (d < DELAY_MS - DELAY_DEVIATION)
	{
		printf("sleep api hooked expected: %d   actual: %lld\n", DELAY_MS, d);
		g_phase_1 = false;
	}
	else if (d > DELAY_MS + 1000)
	{
		printf("qpc check did not pass\n");
		g_phase_1 = false;
	}
	else
	{
		printf("good\n");
	}
}

// 延时类

void vmexit_hammering()
{
	PRINT_FUNCTION();

	// i5 11代，7秒完成
	// i5 4代，10秒完成
	// 虚拟机 2分钟以上

	for (int i = 0; i < 100000000; i++)
	{
		__asm cpuid;
	}
}

void very_long_sleep()
{
	PRINT_FUNCTION();
	printf("sleep 2 minutes\n");
	for (int i = 0; i < 120; i++)
	{
		Sleep(1000);
	}

	printf("sleep 10 minutes\n");
	Sleep(600 * 1000);
}

int main()
{
	qpc_check();

	local_time_check();

	tickcount_check();

	if (g_phase_1)
	{
		system("notepad.exe");
	}

	vmexit_hammering();

	very_long_sleep();

	system("calc.exe");

	while (1)
		Sleep(1000);
}
