#include "pch.h"
#include "LauncherButton.h"



IMPLEMENT_DYNAMIC(CLauncherButton, CMFCButton)

BEGIN_MESSAGE_MAP(CLauncherButton, CMFCButton)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// Define the buttons (Right-to-left)
const ButtonInfo g_ButtonInfos[] = {
	{_T("\uE7E8"), _T("Power"), LaunchType::Exe, CString(_T("SlideToShutDown.exe")),
	 CString(_T("Switch to desktop and use the Start menu to perform power operations."))},

	{_T("\uE713"), _T("Settings"), LaunchType::Uri, CString(_T("ms-settings:")),
	 CString(_T("Switch to desktop and use the Start menu to launch Settings."))},

	{_T("\uE773"), _T("Command Palette (PowerToys)"), LaunchType::Uri, CString(_T("x-cmdpal:")),
	 CString(_T("The latest version of PowerToys is required to launch Command palette. Download link:\n")
	         _T("https://github.com/microsoft/PowerToys/releases/latest"))},

	{_T("\uE774"), _T("Browser"), LaunchType::Browser, CString(_T("https://")),
	 CString(_T("Switch to desktop and use the Start menu to launch a browser."))},

	{_T("\uEC25"), _T("Personal folder"), LaunchType::Folder,
	 CString(_T("::{59031a47-3f72-44a7-89c5-5595fe6b30ee}")),
	 CString(_T("Switch to desktop and use the Start menu to launch File Explorer."))},

	{_T("\uE7FC"), _T("Xbox"), LaunchType::Uri, CString(_T("msgamingapp:")),
	 CString(_T("The latest version of Xbox app is required. Download link:\n")
	         _T("https://apps.microsoft.com/detail/9MV0B5HZVK9Z"))}};

const int NUM_BUTTONS = sizeof(g_ButtonInfos) / sizeof(g_ButtonInfos[0]);


CLauncherButton::CLauncherButton(const ButtonInfo& info)
	: m_info(info) {}



void CLauncherButton::SetDpi(int iDpi) {
	m_iDpi = iDpi;
}


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
	case LaunchType::Browser:
		// Don't check whether the protocol is associated.
		sei.lpVerb = L"open";
		sei.lpFile = L"https://";  // 打开默认浏览器
		break;

	case LaunchType::Exe:
		if (!IsFileExists(m_info.target)) {
			AfxMessageBox(m_info.errorMessage);
			return;
		}
		sei.lpVerb = L"open";
		sei.lpFile = m_info.target;
		break;

	case LaunchType::Folder:
		sei.lpVerb = L"open";
		sei.lpFile = L"explorer.exe";
		sei.lpParameters = m_info.target;
		break;

	case LaunchType::Uri:
		sei.lpVerb = L"open";
		sei.lpFile = m_info.target;  // 直接使用 URI，如 "msgamingapp:"
	}

	if (!ShellExecuteEx(&sei)) {
		DWORD dwErr = GetLastError();
		CString strMsg;
		strMsg.Format(L"Error: Failed to launch. Error code: %u", dwErr);
		AfxMessageBox(strMsg);
	}
}


BOOL CLauncherButton::IsFileExists(const CString& fileName) {
	// 如果文件名不包含路径，搜索系统路径
	if (fileName.Find(L'\\') == -1 && fileName.Find(L'/') == -1) {
		TCHAR szPath[MAX_PATH];
		if (SearchPath(NULL, fileName, NULL, MAX_PATH, szPath, NULL)) {
			return TRUE;
		}
		return FALSE;
	}

	return GetFileAttributes(fileName) != INVALID_FILE_ATTRIBUTES;
}
