#include <windows.h>
#include <string>
#include "COM.h"

using namespace std;

DWORD WINAPI MyThreadFunction(HANDLE hCom);

void (*g_rd_callback)(char);
HANDLE hCom;

DWORD WritePort(char c) {

	return WritePort_(&c, 1);

}

DWORD WritePort_(void *c, int siz) {
	DWORD bytes;

	OVERLAPPED o2;
	o2.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	o2.Internal = 0;
	o2.InternalHigh = 0;
	o2.Offset = 0;
	o2.OffsetHigh = 0;

	int ok = WriteFile(hCom, c, siz, &bytes, &o2);

	if (!ok) {

		int err = GetLastError();
		if (err == ERROR_IO_PENDING) {

			DWORD ret_ = WaitForSingleObject(o2.hEvent, INFINITE);
			if (ret_ == WAIT_OBJECT_0) {

				DWORD bytes;
				DWORD ok_ = GetOverlappedResult(hCom, &o2, &bytes, false);

				if (!ok_) {
					ExitProcess(GetLastError());
				}

			}
			else {
				ExitProcess(GetLastError());
			}

		}
		else {
			ExitProcess(400);
		}


	}

	return 1;
}

DWORD OpenPort(string com, DWORD baudrate, DWORD bytesize, DWORD parity, DWORD stopbits, void (*rd_callback)(char) ) {
	
	DCB dcb;
	BOOL fSuccess;

	g_rd_callback = rd_callback;

	hCom = CreateFileA(com.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,      //  must be opened with exclusive-access
		NULL,   //  default security attributes
		OPEN_EXISTING, //  must use OPEN_EXISTING
		FILE_FLAG_OVERLAPPED,      //  not overlapped I/O
		NULL); //  hTemplate must be NULL for comm devices

	if(INVALID_HANDLE_VALUE == hCom){
		ExitProcess(GetLastError());
	}

	SecureZeroMemory(&dcb, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);

	dcb.BaudRate = baudrate;     //  baud rate
	dcb.ByteSize = bytesize;             //  data size, xmit and rcv
	dcb.Parity = parity;      //  parity bit
	dcb.StopBits = stopbits;    //  stop bit


	fSuccess = SetCommState(hCom, &dcb);
	if (!fSuccess) {
		ExitProcess(GetLastError());
	}

	DWORD threadID_;
	HANDLE hThread = CreateThread(NULL, 0, MyThreadFunction, hCom, 0, &threadID_);
	if (hThread == NULL) {
		ExitProcess(GetLastError());
	}

}

DWORD WINAPI MyThreadFunction(HANDLE hCom) {

	DWORD dwEvtMask;
	BOOL fSuccess = SetCommMask(hCom, EV_RXCHAR);

	if (!fSuccess)
	{
		// Handle the error. 
		printf("SetCommMask failed with error %d.\n", GetLastError());
		return 0;
	}

	OVERLAPPED o;
	o.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	o.Internal = 0;
	o.InternalHigh = 0;
	o.Offset = 0;
	o.OffsetHigh = 0;

	while (true) {

		int ok = WaitCommEvent(hCom, &dwEvtMask, &o);

		if (ok) {

			if (dwEvtMask & EV_RXCHAR) {

				unsigned char ch;
				DWORD NoBytesRead;

				OVERLAPPED o2;
				o2.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
				o2.Internal = 0;
				o2.InternalHigh = 0;
				o2.Offset = 0;
				o2.OffsetHigh = 0;

				int ok_ = ReadFile(hCom, &ch, sizeof(ch), &NoBytesRead, &o2);
				if (ok_) {
					g_rd_callback(ch);

				}
				else {
					ExitProcess(403);
				}

			}

		}
		else {

			DWORD dwRet = GetLastError();
			if (ERROR_IO_PENDING == dwRet) {

				DWORD ret_ = WaitForSingleObject(o.hEvent, INFINITE);
				if (ret_ == WAIT_OBJECT_0) {

					DWORD bytes;

					DWORD ok_ = GetOverlappedResult(hCom, &o, &bytes, false);
					if (ok_) {

						if (dwEvtMask & EV_RXCHAR) {

							OVERLAPPED o2;
							o2.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
							o2.Internal = 0;
							o2.InternalHigh = 0;
							o2.Offset = 0;
							o2.OffsetHigh = 0;

							unsigned char ch;
							DWORD NoBytesRead;

							int ok_ = ReadFile(hCom, &ch, sizeof(ch), &bytes, &o2);
							if (ok_) {

								g_rd_callback(ch);

							}
							else {
								int error = GetLastError();

								if (ERROR_IO_PENDING == error) {

									DWORD ret__ = WaitForSingleObject(o2.hEvent, INFINITE);
									if (ret__ == WAIT_OBJECT_0) {

										OVERLAPPED o3;
										o3.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
										o3.Internal = 0;
										o3.InternalHigh = 0;
										o3.Offset = 0;
										o3.OffsetHigh = 0;

										int ok_ = ReadFile(hCom, &ch, sizeof(ch), &bytes, &o3);
										if (ok_) {

											g_rd_callback(ch);

										}
										else {
											ExitProcess(GetLastError());
										}


									}
									else {
										ExitProcess(GetLastError());
									}

								}
								else {

									ExitProcess(error);

								}
							}
						}
					}
					else {
						ExitProcess(405);
					}

				}
				else {
					ExitProcess(404);
				}

			}

		}

	}

	return 0;
}