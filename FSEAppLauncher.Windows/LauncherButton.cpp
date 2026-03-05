#include "pch.h"
#include "LauncherButton.h"



IMPLEMENT_DYNAMIC(CLauncherButton, CMFCButton)

BEGIN_MESSAGE_MAP(CLauncherButton, CMFCButton)
	ON_WM_PAINT()
END_MESSAGE_MAP()


TCHAR keyCombinationForNotifications[] = {VK_LWIN, 'N'};
TCHAR keyCombinationForQuickSettings[] = {VK_LWIN, 'A'};

// Define the buttons (right-to-left).
const ButtonInfo g_ButtonInfos[] = {
	{_T("\uE7E8"), _T("Shut down"), LaunchType::Exe,
	 CString(_T("SlideToShutDown.exe")),
	 CString(_T("Switch to desktop and use the Start menu to perform power operations."))},

	{_T("\uE713"), _T("Settings"), LaunchType::Uri,
	 CString(_T("ms-settings:")),
	 CString(_T("Switch to desktop and use the Start menu to launch Settings."))},

	{_T("\uE91C"), _T("Notifications"), LaunchType::KeyCombination,
	 CString(keyCombinationForNotifications, sizeof(keyCombinationForNotifications) / sizeof(TCHAR)),
	 CString(_T("Switch to desktop to use this feature."))},

	{_T("\uF8A6"), _T("Quick Settings"), LaunchType::KeyCombination,
	 CString(keyCombinationForQuickSettings, sizeof(keyCombinationForQuickSettings) / sizeof(TCHAR)),
	 CString(_T("Switch to desktop to use this feature."))},

	{_T("\uE773"), _T("Command Palette (PowerToys)"), LaunchType::Uri,
	 CString(_T("x-cmdpal:")),
	 CString(_T("The latest version of PowerToys is required. Download link:\n")
	         _T("https://apps.microsoft.com/detail/XP89DCGQ3K6VLD"))},

	{_T("\uE719"), _T("Microsoft Store"), LaunchType::Uri,
	 CString(_T("ms-windows-store:")),
	 CString(_T("The latest version of Microsoft Store is required. Download link:\n")
	         _T("https://apps.microsoft.com/detail/9WZDNCRFJBMP"))},

	{_T("\uED66"), _T("GOG Galaxy"), LaunchType::Uri,
	 CString(_T("goggalaxy:")),
	 CString(_T("The latest version of GOG Galaxy is required. Download link:\n")
	         _T("https://apps.microsoft.com/detail/XPFFXW40W60KCF"))},

	{_T("\uE7FC"), _T("Epic Games Launcher"), LaunchType::Uri,
	 CString(_T("com.epicgames.launcher:")),
	 CString(_T("The latest version of Epic Games Launcher is required. Download link:\n")
	         _T("https://apps.microsoft.com/detail/XP99VR1BPSBQJ2"))},

	{_T("\uE7FC"), _T("Steam"), LaunchType::Uri,
	 CString(_T("steam:")),
	 CString(_T("The latest version of Steam is required. Download link:\n")
	         _T("https://store.steampowered.com/about/"))},

	{_T("\uE7FC"), _T("EA"), LaunchType::Uri,
	 CString(_T("origin2:")),
	 CString(_T("The latest version of EA app is required. Download link:\n")
	         _T("https://www.ea.com/ea-app#downloads"))},

	{_T("\uE7FC"), _T("Xbox"), LaunchType::Uri,
	 CString(_T("msgamingapp:")),
	 CString(_T("The latest version of Xbox app is required. Download link:\n")
	         _T("https://apps.microsoft.com/detail/9MV0B5HZVK9Z"))},

	{_T("\uE774"), _T("Browser"), LaunchType::Uri,
	 CString(_T("https:")),
	 CString(_T("Switch to desktop and use the Start menu to launch a browser."))},

	{_T("\uEC50"), _T("File Explorer"), LaunchType::Exe,
	 CString(_T("explorer.exe")),
	 CString(_T("Switch to desktop and use the Start menu to launch File Explorer."))}};

const int NUM_BUTTONS = sizeof(g_ButtonInfos) / sizeof(g_ButtonInfos[0]);


CLauncherButton::CLauncherButton(const ButtonInfo& info)
	: m_info(info) {}



void CLauncherButton::OnPaint() {
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	int cx = rect.Width();
	int cy = rect.Height();

	// DC 32 bit DIB
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = cx;
	bmi.bmiHeader.biHeight = -cy;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	VOID* bits;
	CBitmap* pBm = CBitmap::FromHandle(CreateDIBSection(dc.GetSafeHdc(),
	                                                    &bmi,
	                                                    DIB_RGB_COLORS,
	                                                    &bits,
	                                                    NULL,
	                                                    0));
	if (!pBm) {
		return;
	}

	CBitmap* pOldBmp = memDC.SelectObject(pBm);

	SendMessage(WM_PRINTCLIENT, (WPARAM)memDC.GetSafeHdc(),
	            PRF_CLIENT | PRF_ERASEBKGND | PRF_NONCLIENT);

	DWORD* pPixel = (DWORD*)bits;
	for (int i = 0; i < cx * cy; ++i) {
		pPixel[i] |= 0xFF000000;
	}

	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

	AlphaBlend(dc.GetSafeHdc(),
	           rect.left,
	           rect.top,
	           cx,
	           cy,
	           memDC.GetSafeHdc(),
	           0,
	           0,
	           cx,
	           cy,
	           bf);

	memDC.SelectObject(pOldBmp);
	DeleteObject(HBITMAP(pBm));
}



void CLauncherButton::Launch() {
	SHELLEXECUTEINFO sei = { sizeof(sei) };
	sei.nShow = SW_SHOWNORMAL;

	switch (m_info.type) {
	case LaunchType::Exe:
		if (!IsFileExists(m_info.target)) {
			AfxMessageBox(m_info.errorMessage);
			return;
		}
		sei.lpVerb = L"open";
		sei.lpFile = m_info.target;
		break;

	case LaunchType::KeyCombination:
		SendKeyCombination(m_info.target);
		return;

	case LaunchType::Uri:
		sei.lpVerb = L"open";
		sei.lpFile = m_info.target;
	}

	if (!ShellExecuteEx(&sei)) {
		DWORD dwErr = GetLastError();
		CString strMsg;
		strMsg.Format(L"Error: Failed to launch. Error code: %u", dwErr);
		AfxMessageBox(strMsg);
	}
}


BOOL CLauncherButton::IsFileExists(const CString& fileName) {
	// 如果文件名不包含路径, 搜索系统路径.
	if (fileName.Find(L'\\') == -1 && fileName.Find(L'/') == -1) {
		TCHAR szPath[MAX_PATH];
		if (SearchPath(NULL, fileName, NULL, MAX_PATH, szPath, NULL)) {
			return TRUE;
		}
		return FALSE;
	}

	return GetFileAttributes(fileName) != INVALID_FILE_ATTRIBUTES;
}


void CLauncherButton::SendKeyCombination(const CString& keyCombination) {
	INT length = keyCombination.GetLength(), lengthDouble = length * 2, i;
	INPUT* inputs = new INPUT[lengthDouble]();

	for (i=0;i<length;++i) {
		// Press the key.
		(inputs + i)->type = INPUT_KEYBOARD;
		(inputs + i)->ki.wVk = static_cast<WORD>(keyCombination.GetAt(i));
	}

	for (;i<lengthDouble;++i) {
		// Release the key.
		(inputs + i)->type = INPUT_KEYBOARD;
		(inputs + i)->ki.wVk = static_cast<WORD>(keyCombination.GetAt(lengthDouble - 1 - i));
		(inputs + i)->ki.dwFlags = KEYEVENTF_KEYUP;
	}

	SendInput(lengthDouble, inputs, sizeof(INPUT));

	delete[] inputs;
}
