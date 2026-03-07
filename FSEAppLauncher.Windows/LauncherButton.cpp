#include "pch.h"
#include "LauncherButton.h"



IMPLEMENT_DYNAMIC(CLauncherButton, CMFCButton)

BEGIN_MESSAGE_MAP(CLauncherButton, CMFCButton)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// Do not add '\0'.
TCHAR keyCombinationForNotifications[] = {VK_LWIN, 'N'};
TCHAR keyCombinationForQuickSettings[] = {VK_LWIN, 'A'};

// Define the buttons (right-to-left).
CONST ButtonInfo g_ButtonInfos[] = {
	{nullptr, _T("\uE7E8"),
	 _T("Shut down"),
	 LaunchType::Exe,
	 CString(_T("SlideToShutDown.exe")),
	 _T("Switch to desktop and use the Start menu to perform power operations.")},

	{nullptr, _T("\uE713"),
	 _T("Settings"),
	 LaunchType::Uri,
	 CString(_T("ms-settings:")),
	 _T("Switch to desktop and use the Start menu to launch Settings.")},

	{nullptr, _T("\uE91C"),
	 _T("Notifications"),
	 LaunchType::KeyCombination,
	 CString(keyCombinationForNotifications,
	         sizeof(keyCombinationForNotifications) / sizeof(TCHAR)),
	 _T("Switch to desktop to use this feature.")},

	{nullptr, _T("\uF8A6"),
	 _T("Quick Settings"),
	 LaunchType::KeyCombination,
	 CString(keyCombinationForQuickSettings,
	         sizeof(keyCombinationForQuickSettings) / sizeof(TCHAR)),
	 _T("Switch to desktop to use this feature.")},

	{nullptr, _T("\uE773"),
	 _T("Command Palette (PowerToys)"),
	 LaunchType::Uri,
	 CString(_T("x-cmdpal:")),
	 _T("The latest version of PowerToys is required. Download link:\n")
	 _T("https://apps.microsoft.com/detail/XP89DCGQ3K6VLD")},

	{nullptr, _T("\uE719"),
	 _T("Microsoft Store"),
	 LaunchType::Uri,
	 CString(_T("ms-windows-store:")),
	 _T("The latest version of Microsoft Store is required. Download link:\n")
	 _T("https://apps.microsoft.com/detail/9WZDNCRFJBMP")},

	{nullptr, _T("\uED66"),
	 _T("GOG Galaxy"),
	 LaunchType::Uri,
	 CString(_T("goggalaxy:")),
	 _T("The latest version of GOG Galaxy is required. Download link:\n")
	 _T("https://apps.microsoft.com/detail/XPFFXW40W60KCF")},

	{nullptr, _T("\uE7FC"),
	 _T("Epic Games Launcher"),
	 LaunchType::Uri,
	 CString(_T("com.epicgames.launcher:")),
	 _T("The latest version of Epic Games Launcher is required. Download link:\n")
	 _T("https://apps.microsoft.com/detail/XP99VR1BPSBQJ2")},

	{nullptr, _T("\uE7FC"),
	 _T("EA"),
	 LaunchType::Uri,
	 CString(_T("origin2:")),
	 _T("The latest version of EA app is required. Download link:\n")
	 _T("https://www.ea.com/ea-app#downloads")},

	{MAKEINTRESOURCE(IDB_PNG_STEAM), _T(""),
	 _T("Steam Big Picture Mode"),
	 LaunchType::Uri,
	 CString(_T("steam://open/bigpicture")),
	 _T("The latest version of Steam is required. Download link:\n")
	 _T("https://store.steampowered.com/about/")},

	{nullptr, _T("\uE7FC"),
	 _T("Xbox"),
	 LaunchType::Uri,
	 CString(_T("msgamingapp:")),
	 _T("The latest version of Xbox app is required. Download link:\n")
	 _T("https://apps.microsoft.com/detail/9MV0B5HZVK9Z")},

	{nullptr, _T("\uE774"),
	 _T("Browser"),
	 LaunchType::Uri,
	 CString(_T("https:")),
	 _T("Switch to desktop and use the Start menu to launch a browser.")},

	{nullptr, _T("\uEC50"),
	 _T("File Explorer"),
	 LaunchType::Exe,
	 CString(_T("explorer.exe")),
	 _T("Switch to desktop and use the Start menu to launch File Explorer.")}};

CONST INT NUM_BUTTONS = sizeof(g_ButtonInfos) / sizeof(g_ButtonInfos[0]);


CLauncherButton::CLauncherButton(const ButtonInfo& info)
	: m_info(info) {
	if (m_info.iconPngResource) {
		LoadIconPng();
	}
}



void CLauncherButton::OnPaint() {
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	int cx = rect.Width();
	int cy = rect.Height();

	// Create the memory DC and 32-bit DIB.
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = cx;
	bmi.bmiHeader.biHeight = -cy;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	PVOID bits;
	CBitmap* pBm = CBitmap::FromHandle(CreateDIBSection(dc.GetSafeHdc(),
	                                                    &bmi,
	                                                    DIB_RGB_COLORS,
	                                                    &bits,
	                                                    nullptr,
	                                                    0));
	if (!pBm) {
		return;
	}

	CBitmap* pOldBmp = memDC.SelectObject(pBm);

	// Let the button draw its own background and border (including hover and hold).
	SendMessage(WM_PRINTCLIENT,
	            reinterpret_cast<WPARAM>(memDC.GetSafeHdc()),
	            PRF_CLIENT | PRF_ERASEBKGND | PRF_NONCLIENT);

	// If this button uses a custom icon, draw the icon.
	if (m_info.iconPngResource) {
		DrawIconPng(&memDC, rect);
	}

	PDWORD pPixel = (PDWORD)bits;
	for (int i = 0; i < cx * cy; ++i) {
		pPixel[i] |= 0xFF000000;
	}

	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

	dc.AlphaBlend(rect.left,
	              rect.top,
	              cx,
	              cy,
	              &memDC,
	              0,
	              0,
	              cx,
	              cy,
	              bf);

	memDC.SelectObject(pOldBmp);
	DeleteObject(HBITMAP(pBm));
}



VOID CLauncherButton::Launch() {
	SHELLEXECUTEINFO sei = { sizeof(sei) };
	sei.nShow = SW_SHOWNORMAL;

	switch (m_info.type) {
	case LaunchType::Exe:
		if (!IsFileExists(m_info.target)) {
			AfxMessageBox(m_info.errorMessage);
			return;
		}
		sei.lpVerb = _T("open");
		sei.lpFile = m_info.target;
		break;

	case LaunchType::KeyCombination:
		SendKeyCombination(m_info.target);
		return;

	case LaunchType::Uri:
		sei.lpVerb = _T("open");
		sei.lpFile = m_info.target;
		break;

	default:
		AfxMessageBox(_T("Error: Invalid parameter."));
		return;
	}

	if (!ShellExecuteEx(&sei)) {
		DWORD dwErr = GetLastError();
		CString strMsg;
		strMsg.Format(_T("Error: Failed to launch. Error code: %u"), dwErr);
		AfxMessageBox(strMsg);
	}
}



VOID CLauncherButton::LoadIconPng() {
	if (!m_iconPng.Load(m_info.iconPngResource, AfxGetResourceHandle())) {
		AfxMessageBox(_T("Error: Cannot load the icon file from resource."));
	}
}


// Draws the icon that scale and invert color.
VOID CLauncherButton::DrawIconPng(CDC* pDC, const CRect& rect) {
	int iDpi = GetDpiForWindow(GetSafeHwnd());

	// target size
	const int destW = MulDiv(18, iDpi, USER_DEFAULT_SCREEN_DPI),
	          destH = MulDiv(18, iDpi, USER_DEFAULT_SCREEN_DPI);
	int x = rect.left + (rect.Width() - destW) / 2;  // horizontally centered
	int y = rect.top + (rect.Height() - destH) / 2;  // vertically centered

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap = memDC.SelectObject(&m_iconPng);

	BITMAP bm;
	m_iconPng.GetBitmap(&bm);

	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	pDC->AlphaBlend(x, y, destW, destH, &memDC, 0, 0, bm.bmWidth,
	                bm.bmHeight, bf);

	memDC.SelectObject(pOldBitmap);
}


BOOL CLauncherButton::IsFileExists(const CString& fileName) {
	// If the filename does not contain a path, search for the system path.
	if (fileName.Find(_T('\\')) == -1 && fileName.Find(_T('/')) == -1) {
		TCHAR szPath[MAX_PATH];
		if (SearchPath(nullptr, fileName, nullptr, MAX_PATH, szPath, nullptr)) {
			return TRUE;
		}
		return FALSE;
	}

	return GetFileAttributes(fileName) != INVALID_FILE_ATTRIBUTES;
}


VOID CLauncherButton::SendKeyCombination(const CString& keyCombination) {
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
		(inputs + i)->ki.wVk = static_cast<WORD>(
			keyCombination.GetAt(lengthDouble - 1 - i));
		(inputs + i)->ki.dwFlags = KEYEVENTF_KEYUP;
	}

	SendInput(lengthDouble, inputs, sizeof(INPUT));

	delete[] inputs;
}
