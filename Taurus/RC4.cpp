#include "Common.h"

#define swap(type,a,b,c) do{type t=a[b];a[b]=a[c];a[c]=t;}while(0)

void RC4::Encrypt(string encKey, string& data)
{
	size_t keySize = encKey.size();
	size_t dataSize = data.size();
	int s[256];
	int i, j, c;

	for (i = 0; i < 256; i++)
		s[i] = i;

	for (i = j = 0; i < 256; ++i) {
		j = (j + s[i] + encKey[i % keySize]) % 256;
		swap(BYTE, s, i, j);
	}

	i = j = c = 0;

	while (dataSize--) {
		j = (j + s[i = (i + 1) % 256]) % 256;
		swap(BYTE, s, i, j);

		data[c++] ^= s[(s[i] + s[j]) % 256];
	}
}

void RC4::Decrypt(string encKey, string& data)
{
	size_t keySize = encKey.size();
	size_t dataSize = data.size();
	int i, j, c;
	int gg = 256;
	int s[256];

	for (i = 0; i < 256; i++)
		s[i] = i;

	for (i = j = 0; i < gg; ++i)
	{
		j = (j + s[i] + encKey[i % keySize]) % gg;
		swap(BYTE, s, i, j);
	}

	i = j = c = 0;

	while (dataSize--)
	{
		j = (j + s[i = (i + 1) % gg]) % gg;
		swap(BYTE, s, i, j);
		data[c++] ^= s[(s[i] + s[j]) % gg];
	}
}