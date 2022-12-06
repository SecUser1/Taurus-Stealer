#pragma once
#include "Common.h"
#include "PasswordCheck.h"
#include "Asn1Der.h"
#include "MozillaPBE.h"
#include "BerkeleyDB.h"

const static byte MagicNumber1[16] =
{
	248, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1
};

class FireFoxBase
{
private:
	void ArrayCopy(const vector<byte>& src, int sourceIndex, vector<byte>& dest, int destIndex, int length);
	int StringToInt(string str);
	vector<byte> ConvertHexStringToByteArray(string hexString);
public:
	vector<byte> ExtractPrivateKey3(string file);
	vector<byte> ExtractPrivateKey4(string file);
	string TripleDesDecrypt(const vector<byte>& key, const vector<byte>& iv, const vector<byte>& input);
};