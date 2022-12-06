#pragma once
//#define IS_DEBUG //закоменти этот макрос, если вывод дебаг-инфы не нужен

#ifdef IS_DEBUG
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

static char source_data[512];
static char debug_data[512];

inline LPCSTR basefilename(LPCSTR fname)
{
	LPCSTR ptr = strrchr(fname, '\\');
	return (ptr) ? (ptr + 1) : fname;
}

static void DebugToFile() 
{
	static char info[1024];
	lstrcatA(info, source_data);
	lstrcatA(info, debug_data);
	OutputDebugStringA(info);
	lstrcpyA(info, "");
}

#define DEBUG(format, ...) \
wsprintfA(source_data, "%s:%s:%d: ", basefilename(__FILE__),  __FUNCTION__, __LINE__); \
wsprintfA(debug_data, format, __VA_ARGS__); \
DebugToFile();
#else
#define DEBUG(...)
#endif
