#pragma once
#include "Common.h"

#define ROUND(x) (((x) & 1) ? ((x + 1) / 2) : ((x) / 2))

struct RecordHeaderField
{
	long Size;
	long Type;
};

struct TableEntry
{
	vector<string> Content;
};

struct SqliteMasterEntry
{
	string ItemName;
	long RootNum;
	string SqlStatement;
};

class SqlHandler
{
	bool ReadAllBytes(vector<byte>& bytes, string fileName, string path);
	unsigned long long ConvertToULong(unsigned int startIndex, unsigned int size);
	long BitConverter_ToInt64(byte* vec);
	int Gvl(size_t startIdx);
	long Cvl(int startIdx, int endIdx);
	bool IsOdd(long value);
	string GetString(unsigned int index, unsigned int count);
	vector<string> split(string what, char ch);
	bool Compare(string str1, string str2);
	string TrimStart(string str);
	bool ReadMasterTable(unsigned long offset);
	bool ReadTableFromOffset(unsigned long long offset);

	byte _sqlDataTypeSize[10] = { 0, 1, 2, 3, 4, 6, 8, 8, 0, 0 };
	unsigned long _dbEncoding;
	vector<byte> _fileBytes;
	unsigned long _pageSize;
	vector<string> _fieldNames;
	vector<SqliteMasterEntry> _masterTableEntries;
	vector<TableEntry> _tableEntries;
	bool bSuccess;
public:
	SqlHandler(string fileName, bool& success, string path = string())
	{
		bSuccess = true;
		if (ReadAllBytes(_fileBytes, fileName, path))
		{
			success = true;
			_pageSize = ConvertToULong(16, 2);
			_dbEncoding = ConvertToULong(56, 4);
			success = ReadMasterTable(100L);
		}
	}

	bool ReadTable(string tableName);
	string GetValue(unsigned int rowNum, unsigned int field);
	int GetRowCount() { return _tableEntries.size(); }
};