#include "pch.h"
#include "LauncherButton.h"



IMPLEMENT_DYNAMIC(CLauncherButton, CMFCButton)

BEGIN_MESSAGE_MAP(CLauncherButton, CMFCButton)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
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


CLauncherButton::CLauncherButton(const ButtonInfo& info, BOOL bDarkMode)
	: m_info(info), m_bDarkMode(bDarkMode) {}


CLauncherButton::~CLauncherButton() {}



void CLauncherButton::SetDarkMode(BOOL bDarkMode) {
	m_bDarkMode = bDarkMode;
}


void CLauncherButton::SetDpi(int iDpi) {
	m_iDpi = iDpi;
}


void CLauncherButton::OnPaint() {
	CPaintDC dc(this);
	DrawButton(&dc);
}


//BOOL CLauncherButton::OnEraseBkgnd(CDC* pDC) {
//	return TRUE;  // 禁止擦除背景，避免闪烁
//}
//
//
//void CLauncherButton::OnMouseMove(UINT nFlags, CPoint point) {
//	if (!m_bHover) {
//		m_bHover = TRUE;
//		TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, GetSafeHwnd(), 0};
//		TrackMouseEvent(&tme);
//		Invalidate();  // 可选：重绘以显示悬停效果
//	}
//
//	CMFCButton::OnMouseMove(nFlags, point);
//}
//
//
//void CLauncherButton::OnMouseLeave() {
//	m_bHover = FALSE;
//	Invalidate();
//	CMFCButton::OnMouseLeave();
//}
//
//
//LRESULT CLauncherButton::WindowProc(UINT message, WPARAM wParam,
//                                    LPARAM lParam) {
//	LRESULT lResult = CMFCButton::WindowProc(message, wParam, lParam);
//	// 这些消息可能导致系统默认绘制覆盖，强制重绘
//	switch (message) {
//	case WM_SETFOCUS:
//	case WM_KILLFOCUS:
//	case WM_LBUTTONDOWN:
//	case WM_LBUTTONUP:
//		Invalidate();
//	}
//
//	return lResult;
//}



void CLauncherButton::DrawButton(CDC* pDC) {
  CRect rect;
  GetClientRect(&rect);
  int cx = rect.Width();
  int cy = rect.Height();

  COLORREF bgColor = m_bDarkMode ? RGB(45, 45, 45) : RGB(251, 251, 251);
  COLORREF textColor = m_bDarkMode ? RGB(255, 255, 255) : RGB(26, 26, 26);

  // 创建内存 DC 和 32 位 DIB
  CDC memDC;
  memDC.CreateCompatibleDC(pDC);
  BITMAPINFO bmi = {0};
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = cx;
  bmi.bmiHeader.biHeight = -cy;  // 自上而下
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32;
  bmi.bmiHeader.biCompression = BI_RGB;

  LPVOID bits;
  HBITMAP hBitmap =
      CreateDIBSection(pDC->GetSafeHdc(), &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
  if (!hBitmap) return;

  HBITMAP hOldBmp = (HBITMAP)memDC.SelectObject(hBitmap);

  // 1. 填充背景色
  memDC.FillSolidRect(rect, bgColor);

  // 2. 绘制图标文本
  CFont font;
  LOGFONT lf = {0};
  lf.lfHeight = -MulDiv(20, m_iDpi, USER_DEFAULT_SCREEN_DPI);
  //lf.lfWeight = FW_SEMIBOLD;
  wcscpy_s(lf.lfFaceName, L"Segoe Fluent Icons");
  font.CreateFontIndirect(&lf);
  CFont* pOldFont = memDC.SelectObject(&font);

  memDC.SetBkMode(TRANSPARENT);
  memDC.SetTextColor(textColor);
  memDC.DrawText(m_info.iconChar, &rect,
                 DT_CENTER | DT_VCENTER | DT_SINGLELINE);

  // 3. 计算文本实际占用矩形
  CSize textSize = memDC.GetTextExtent(m_info.iconChar);
  CRect iconRect(CPoint((cx - textSize.cx) / 2, (cy - textSize.cy) / 2),
                 textSize);
  iconRect &= rect;  // 确保在客户区内

  // 4. 手动设置每个像素的 Alpha 值
  DWORD* pPixel = (DWORD*)bits;
  for (int y = 0; y < cy; ++y) {
    for (int x = 0; x < cx; ++x) {
      int index = y * cx + x;
      pPixel[index] |= 0xFF000000;  // 100%
    }
  }
  // 设置文本区域 Alpha = 100
  for (int y = iconRect.top; y < iconRect.bottom; ++y) {
    for (int x = iconRect.left; x < iconRect.right; ++x) {
      int index = y * cx + x;
      pPixel[index] |= 0xFF000000;  // 100%
    }
  }

  // 5. 将内存 DC 内容复制到屏幕，使用 AlphaBlend 支持透明度
  BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
  AlphaBlend(pDC->GetSafeHdc(), rect.left, rect.top, cx, cy, memDC.GetSafeHdc(),
             0, 0, cx, cy, bf);

  memDC.SelectObject(hOldBmp);
  DeleteObject(hBitmap);
}


//CRect CLauncherButton::GetIconRect(CDC* pDC) {
//  // 近似文本区域（可被 DrawButton 内部使用）
//  return CRect();
//}
//
//
void CLauncherButton::Launch() {
	CString strTarget(m_info.target);
	SHELLEXECUTEINFO sei = {sizeof(sei)};
	sei.nShow = SW_SHOWNORMAL;
	// sei.fMask = SEE_MASK_FLAG_NO_UI;	// 避免系统弹出错误 UI，由我们自己处理

	switch (m_info.type) {
	case LaunchType::Browser:
		sei.lpVerb = L"open";
		sei.lpFile = L"http://";  // 打开默认浏览器空白页
		break;

	case LaunchType::Exe:
		if (!IsFileExists(strTarget)) {
			AfxMessageBox(CString(L"Cannot find ") + strTarget);
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


//// 辅助函数：检查打包应用是否安装（需实现）
//BOOL CLauncherButton::IsAppInstalled(const CString& aumid) {
//	// 可使用 PackageManager 或 IApplicationActivationManager 相关 API
//	// 此处简化为始终返回 true（实际项目需实现）
//	return true;
//}
//
//
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
	                              scheme,
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
//
//
//LRESULT CLauncherButton::WindowProc(UINT message, WPARAM wParam,
//                                    LPARAM lParam) {
//	LRESULT lResult = CMFCButton::WindowProc(message, wParam, lParam);
//	// 当某些消息可能导致默认绘制覆盖时，强制重绘
//	switch (message) {
//	case WM_MOUSEMOVE:
//	case WM_MOUSELEAVE:
//	case WM_SETFOCUS:
//	case WM_KILLFOCUS:
//	case WM_LBUTTONDOWN:
//	case WM_LBUTTONUP:
//		Invalidate();
//	}
//	return lResult;
//}