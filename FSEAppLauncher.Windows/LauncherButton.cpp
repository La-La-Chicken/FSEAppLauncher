#include "pch.h"
#include "LauncherButton.h"



IMPLEMENT_DYNAMIC(CLauncherButton, CMFCButton)

BEGIN_MESSAGE_MAP(CLauncherButton, CMFCButton)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// 预定义所有按钮（可按需增删） (Right-to-left)
const ButtonInfo g_ButtonInfos[] = {
	{L"\uE7E8", L"Power", LaunchType::Exe, L"SlideToShutDown.exe"},
	{L"\uE713", L"Settings", LaunchType::Uri, L"ms-settings:"},
	{L"\uE945", L"Command Palette", LaunchType::Uri, L"x-cmdpal:"},
	{L"\uE774", L"Browser", LaunchType::Browser, L""},
	{L"\uEC25", L"Personal folder", LaunchType::Folder,
	 L"::{59031a47-3f72-44a7-89c5-5595fe6b30ee}"},
	{L"\uE7FC", L"Xbox", LaunchType::Uri, L"msgamingapp:"}};

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
	CString strTarget(m_info.target);
	SHELLEXECUTEINFO sei = { sizeof(sei) };
	sei.nShow = SW_SHOWNORMAL;
	// sei.fMask = SEE_MASK_FLAG_NO_UI;	// 避免系统弹出错误 UI，由我们自己处理

	switch (m_info.type) {
	case LaunchType::Browser:
		sei.lpVerb = L"open";
		sei.lpFile = L"https://";  // 打开默认浏览器
		break;

	case LaunchType::Exe:
		if (!IsFileExists(strTarget)) {
			AfxMessageBox(CString(L"Error: Cannot find ") + strTarget);
			return;
		}
		sei.lpVerb = L"open";
		sei.lpFile = strTarget;
		break;

	case LaunchType::Folder:
		sei.lpVerb = L"open";
		sei.lpFile = L"explorer.exe";
		sei.lpParameters = strTarget;
		break;

	case LaunchType::Uri:
		// 检查 URI 协议是否有关联应用
		//if (!IsProtocolAssociated(strTarget)) {
		//	AfxMessageBox(
		//		CString(L"Error: No application is associated with the protocol ") + strTarget);
		//	return;
		//}
		sei.lpVerb = L"open";
		sei.lpFile = strTarget;  // 直接使用 URI，如 "msgamingapp:"
		break;
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


// 检查 URI 协议是否有默认处理程序
BOOL CLauncherButton::IsProtocolAssociated(const CString& protocol) {
	// 移除末尾的冒号，因为 AssocQueryString 期望如 "http" 这样的协议名
	CString scheme = protocol;
	if (scheme.Right(1) == L":") {
		scheme = scheme.Left(scheme.GetLength() - 1);
	}

	DWORD dwLen = 0;
	// 查询关联的可执行文件
	HRESULT hr = AssocQueryString(ASSOCF_INIT_IGNOREUNKNOWN,
	                              ASSOCSTR_EXECUTABLE,
	                              protocol,
	                              L"open",
	                              NULL,
	                              &dwLen);
	// 如果返回 ERROR_NO_ASSOCIATION，表示无关联
	if (hr == HRESULT_FROM_WIN32(ERROR_NO_ASSOCIATION)) {
		return FALSE;
	}

	// S_FALSE 且 dwLen > 0 表示存在默认程序
	if (hr == S_FALSE && dwLen > 0) {
		return TRUE;
	}

	// 其他情况（成功找到可执行文件或未知错误），保守返回 false
	return SUCCEEDED(hr);
}
