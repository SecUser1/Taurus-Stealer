#pragma once
class HTTP
{
private:
	string useragent;

	string SendReq(string type, string domain, DWORD port, string path, string data);
public:
	HTTP()
	{
		useragent = Utils::GenUseragent();
	}
	string POST(string domain, DWORD port, string path, string data);
	string GET(string domain, DWORD port, string path, string data);
};

