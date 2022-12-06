#include "Common.h"

UINT crc32_run(const char* str)
{
	unsigned int c = 0xFFFFFFFF;
	const char* pStr = str;
	while (*pStr)
	{
		c = table[(c ^ pStr[0]) & 0xFF] ^ (c >> 8);
		pStr++;
	}

	return c;
}

UINT crc32_run(const wchar_t* str)
{
	unsigned int c = 0xFFFFFFFF;
	const wchar_t* pStr = str;
	while (*pStr)
	{
		c = table[(c ^ pStr[0]) & 0xFF] ^ (c >> 8);
		pStr++;
	}

	return c;
}
