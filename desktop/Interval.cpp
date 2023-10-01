
#include "Interval.h"

void (*g_callback)();
int g_interval;

DWORD WINAPI MyThreadFunction2(HANDLE p) {

	while (true) {

		Sleep(g_interval);
		g_callback();

	}

}

void setInterval(void (*callback)(), int interval) {

	g_callback = callback;
	g_interval = interval;

	DWORD threadID_;
	HANDLE hThread = CreateThread(NULL, 0, MyThreadFunction2, NULL, 0, &threadID_);
	if (hThread == NULL) {
		ExitProcess(GetLastError());
	}

}