#include "Common.h"

string Enc::Encrypt(string data)
{
	if (data.empty()) return string();
	const string encKey = Utils::random_string(16);
	RC4().Encrypt(encKey, data);
	return encKey + base64_encode(data);
}

string Enc::Decrypt(string data)
{
	if (data.size() < 16) return string();

	string encKey = data.substr(0, 16);
	data = data.substr(16, data.size());
	string response_decrypted = base64_decode(data);
	RC4().Decrypt(encKey, response_decrypted);
	return response_decrypted;
}