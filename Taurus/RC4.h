#pragma once
#include "Common.h"

class RC4 {
public:
	void Encrypt(string encKey, string& data);
	void Decrypt(string encKey, string& data);
};
