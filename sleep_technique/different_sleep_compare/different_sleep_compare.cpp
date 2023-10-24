#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include "resource.h"

HWND hEditSleep;
HWND hEditNtWaitForSingleObj;
HWND hEditNtWaitForMultipleObjs;
HWND hEditNtDelay;
HWND hEditGetTickCount;
HWND hEditGetTickCount64;
HWND hEditGetLocalTime;
HWND hEditGetSystemTime;
HWND hEditQpc;
HWND hEditRdtsc;

bool g_bStart = false;


DWORD WINAPI ThreadSleep(LPVOID hwnd)
{
	HWND hEdit = (HWND)hwnd;

	while (!g_bStart) SwitchToThread();

	int total_ms = 0;

	while (true)
	{
		Sleep(1000);

		total_ms += 1000;

		char text[64];
		snprintf(text, ARRAYSIZE(text), "%d", total_ms / 1000);
		SetWindowTextA(hEdit, text);
	}

	return 0;
}

DWORD WINAPI ThreadNtWaitForSingleObject(LPVOID hwnd)
{
	typedef NTSTATUS (NTAPI *NtWaitForSingleObjectType)(
			__in HANDLE Handle,
			__in BOOLEAN Alertable,
			__in_opt PLARGE_INTEGER Timeout
		);
	NtWaitForSingleObjectType NtWaitForSingleObject = (NtWaitForSingleObjectType)GetProcAddress(GetModuleHandleA("ntdll"), "NtWaitForSingleObject");

	HWND hEdit = (HWND)hwnd;

	while (!g_bStart) SwitchToThread();

	int total_ms = 0;

	while (true)
	{
		LARGE_INTEGER li = { 0 };
		li.QuadPart = 1000 * -10000;
		NtWaitForSingleObject(GetCurrentProcess(), FALSE, &li);

		total_ms += 1000;

		char text[64];
		snprintf(text, ARRAYSIZE(text), "%d", total_ms / 1000);
		SetWindowTextA(hEdit, text);
	}

	return 0;
}

DWORD WINAPI ThreadNtWaitForMultipleObjects(LPVOID hwnd)
{
	typedef enum _WAIT_TYPE {
		WaitAll,
		WaitAny,
		WaitNotification,
		WaitDequeue
	} WAIT_TYPE;

	typedef NTSTATUS(NTAPI *NtWaitForMultipleObjectsType)(
		__in ULONG Count,
		__in_ecount(Count) HANDLE Handles[],
		__in WAIT_TYPE WaitType,
		__in BOOLEAN Alertable,
		__in_opt PLARGE_INTEGER Timeout
		);
	NtWaitForMultipleObjectsType NtWaitForMultipleObjects = (NtWaitForMultipleObjectsType)GetProcAddress(GetModuleHandleA("ntdll"), "NtWaitForMultipleObjects");

	HWND hEdit = (HWND)hwnd;

	while (!g_bStart) SwitchToThread();

	int total_ms = 0;

	HANDLE hEvent = CreateEventA(NULL, FALSE, FALSE, NULL);

	while (true)
	{
		
		HANDLE h[1] = { hEvent };
		LARGE_INTEGER li = { 0 };
		li.QuadPart = 1000 * -10000;
		NtWaitForMultipleObjects(1, h, WaitAll, FALSE, &li);

		total_ms += 1000;

		char text[64];
		snprintf(text, ARRAYSIZE(text), "%d", total_ms / 1000);
		SetWindowTextA(hEdit, text);
	}

	return 0;
}

DWORD WINAPI ThreadNtDelayExecution(LPVOID hwnd)
{
	typedef NTSTATUS (NTAPI*NtDelayExecutionType)(
			__in BOOLEAN Alertable,
			__in PLARGE_INTEGER DelayInterval
		);
	NtDelayExecutionType NtDelayExecution = (NtDelayExecutionType)GetProcAddress(GetModuleHandleA("ntdll"), "NtDelayExecution");

	HWND hEdit = (HWND)hwnd;

	while (!g_bStart) SwitchToThread();

	int total_ms = 0;

	while (true)
	{
		LARGE_INTEGER li = { 0 };
		li.QuadPart = 1000 * -10000;
		NtDelayExecution(FALSE, &li);

		total_ms += 1000;

		char text[64];
		snprintf(text, ARRAYSIZE(text), "%d", total_ms / 1000);
		SetWindowTextA(hEdit, text);
	}

	return 0;
}

DWORD WINAPI ThreadGetTickCount(LPVOID hwnd)
{
	HWND hEdit = (HWND)hwnd;

	while (!g_bStart) SwitchToThread();

	DWORD start = GetTickCount();

	while (true)
	{
		DWORD now = GetTickCount();

		int dur = int(now - start);

		char text[64];
		snprintf(text, ARRAYSIZE(text), "%d", dur / 1000);
		SetWindowTextA(hEdit, text);

		Sleep(200);
	}

	return 0;
}

DWORD WINAPI ThreadGetTickCount64(LPVOID hwnd)
{
	HWND hEdit = (HWND)hwnd;

	while (!g_bStart) SwitchToThread();

	ULONGLONG start = GetTickCount64();

	while (true)
	{
		ULONGLONG now = GetTickCount();

		int dur = int(now - start);

		char text[64];
		snprintf(text, ARRAYSIZE(text), "%d", dur / 1000);
		SetWindowTextA(hEdit, text);

		Sleep(200);
	}

	return 0;
}

DWORD WINAPI ThreadGetLocalTime(LPVOID hwnd)
{
	HWND hEdit = (HWND)hwnd;

	while (!g_bStart) SwitchToThread();

	SYSTEMTIME start;
	GetLocalTime(&start);
	FILETIME ftStart;
	SystemTimeToFileTime(&start, &ftStart);

	while (true)
	{
		SYSTEMTIME now;
		GetLocalTime(&now);
		FILETIME ftNow;
		SystemTimeToFileTime(&now, &ftNow);

		ULONGLONG dur = ((ULONGLONG(ftNow.dwHighDateTime) << 32) | ULONGLONG(ftNow.dwLowDateTime)) - 
			((ULONGLONG(ftStart.dwHighDateTime) << 32) | ULONGLONG(ftStart.dwLowDateTime));

		char text[64];
		snprintf(text, ARRAYSIZE(text), "%d", (int)(dur / 10000000));
		SetWindowTextA(hEdit, text);

		Sleep(200);
	}

	return 0;
}

DWORD WINAPI ThreadGetSystemTime(LPVOID hwnd)
{
	HWND hEdit = (HWND)hwnd;

	while (!g_bStart) SwitchToThread();

	SYSTEMTIME start;
	GetSystemTime(&start);
	FILETIME ftStart;
	SystemTimeToFileTime(&start, &ftStart);

	while (true)
	{
		SYSTEMTIME now;
		GetSystemTime(&now);
		FILETIME ftNow;
		SystemTimeToFileTime(&now, &ftNow);

		ULONGLONG dur = ((ULONGLONG(ftNow.dwHighDateTime) << 32) | ULONGLONG(ftNow.dwLowDateTime)) -
			((ULONGLONG(ftStart.dwHighDateTime) << 32) | ULONGLONG(ftStart.dwLowDateTime));

		char text[64];
		snprintf(text, ARRAYSIZE(text), "%d", int(dur / 10000000));
		SetWindowTextA(hEdit, text);

		Sleep(200);
	}

	return 0;
}

DWORD WINAPI ThreadQpc(LPVOID hwnd)
{
	HWND hEdit = (HWND)hwnd;

	while (!g_bStart) SwitchToThread();

	LARGE_INTEGER start, end, frequency;
	QueryPerformanceCounter(&start);
	QueryPerformanceFrequency(&frequency);

	while (true)
	{
		QueryPerformanceCounter(&end);

		LONGLONG dur = (end.QuadPart - start.QuadPart) * 1000 / frequency.QuadPart;

		char text[64];
		snprintf(text, ARRAYSIZE(text), "%d", int(dur / 1000));
		SetWindowTextA(hEdit, text);

		Sleep(200);
	}

	return 0;
}

DWORD WINAPI ThreadRdtsc(LPVOID hwnd)
{
	HWND hEdit = (HWND)hwnd;

	while (!g_bStart) SwitchToThread();

	DWORD64 start = __rdtsc();

	while (true)
	{
		DWORD64 now = __rdtsc();

		DWORD64 dur = now - start;

		char text[64];
		snprintf(text, ARRAYSIZE(text), "%I64d", dur);
		SetWindowTextA(hEdit, text);

		Sleep(200);
	}

	return 0;
}


VOID CenterDialog(HWND hDlg)
{
	HWND hwndOwner = NULL;
	RECT rcOwner, rcDlg, rc;
	// Get the owner window and dialog box rectangles. 			
	if ((hwndOwner = GetParent(hDlg)) == NULL)
	{
		hwndOwner = GetDesktopWindow();
	}
	GetWindowRect(hwndOwner, &rcOwner);
	GetWindowRect(hDlg, &rcDlg);
	CopyRect(&rc, &rcOwner);

	// Offset the owner and dialog box rectangles so that right and bottom 
	// values represent the width and height, and then offset the owner again 
	// to discard space taken up by the dialog box. 

	OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
	OffsetRect(&rc, -rc.left, -rc.top);
	OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

	// The new position is the sum of half the remaining space and the owner's 
	// original position. 

	SetWindowPos(hDlg,
		HWND_TOP,
		rcOwner.left + (rc.right / 2),
		rcOwner.top + (rc.bottom / 2),
		0, 0,          // Ignores size arguments. 
		SWP_NOSIZE);
}





INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_INITDIALOG)
	{
		hEditSleep = GetDlgItem(hwndDlg, IDC_EDIT_SLEEP);
		CloseHandle(CreateThread(0, 0, ThreadSleep, hEditSleep, 0, 0));

		hEditNtWaitForSingleObj = GetDlgItem(hwndDlg, IDC_EDIT_NTWAITFORSINGLE);
		CloseHandle(CreateThread(0, 0, ThreadNtWaitForSingleObject, hEditNtWaitForSingleObj, 0, 0));

		hEditNtWaitForMultipleObjs = GetDlgItem(hwndDlg, IDC_EDIT_NTWAITFORMULTI);
		CloseHandle(CreateThread(0, 0, ThreadNtWaitForMultipleObjects, hEditNtWaitForMultipleObjs, 0, 0));

		hEditNtDelay = GetDlgItem(hwndDlg, IDC_EDIT_NTDELAY);
		CloseHandle(CreateThread(0, 0, ThreadNtDelayExecution, hEditNtDelay, 0, 0));

		hEditGetTickCount = GetDlgItem(hwndDlg, IDC_EDIT_GETTICKCOUNT);
		CloseHandle(CreateThread(0, 0, ThreadGetTickCount, hEditGetTickCount, 0, 0));

		hEditGetTickCount64 = GetDlgItem(hwndDlg, IDC_EDIT_GETTICKCOUNT64);
		CloseHandle(CreateThread(0, 0, ThreadGetTickCount64, hEditGetTickCount64, 0, 0));

		hEditGetLocalTime = GetDlgItem(hwndDlg, IDC_EDIT_GETLOCALTIME);
		CloseHandle(CreateThread(0, 0, ThreadGetLocalTime, hEditGetLocalTime, 0, 0));

		hEditGetSystemTime = GetDlgItem(hwndDlg, IDC_EDIT_GETSYSTEMTIME);
		CloseHandle(CreateThread(0, 0, ThreadGetSystemTime, hEditGetSystemTime, 0, 0));

		hEditQpc = GetDlgItem(hwndDlg, IDC_EDIT_QPC);
		CloseHandle(CreateThread(0, 0, ThreadQpc, hEditQpc, 0, 0));

		hEditRdtsc = GetDlgItem(hwndDlg, IDC_EDIT_RDTSC);
		CloseHandle(CreateThread(0, 0, ThreadRdtsc, hEditRdtsc, 0, 0));


		CenterDialog(hwndDlg);

		g_bStart = true;

		return TRUE;
	}
	else if (uMsg == WM_CLOSE)
	{
		DestroyWindow(hwndDlg);
		return TRUE;
	}
	return FALSE;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	srand((unsigned int)time(NULL));
	DialogBoxA(hInstance, MAKEINTRESOURCEA(IDD_DIALOG1), 0, DialogProc);

	return 0;
}

