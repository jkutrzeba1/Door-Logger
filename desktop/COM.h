#pragma once
#include<windows.h>
#include<string>

DWORD OpenPort(std::string com, DWORD baudrate, DWORD bytesize, DWORD parity, DWORD stopbits, void (*rd_callback)(char));
DWORD WritePort_(void* c, int siz);
DWORD WritePort(char c);