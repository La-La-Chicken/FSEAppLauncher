#include "pch.h"
#include "LauncherButton.h"



IMPLEMENT_DYNAMIC(CLauncherButton, CMFCButton)

BEGIN_MESSAGE_MAP(CLauncherButton, CMFCButton)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()


// 预定义所有按钮（可按需增删）
const ButtonInfo g_ButtonInfos[] = {
	{L"\uE7FC", L"Xbox", LaunchType::Uri, L"msgamingapp:"},
	{L"\uEC25", L"Personal folder", LaunchType::Folder,
	 L"::{59031a47-3f72-44a7-89c5-5595fe6b30ee}"},
	{L"\uE774", L"Browser", LaunchType::Browser, L""},
	{L"\uE945", L"Command Palette", LaunchType::Uri, L"x-cmdpal:"},
	{L"\uE713", L"Settings", LaunchType::Uri, L"ms-settings:"},
	{L"\uE7E8", L"Power", LaunchType::Exe, L"SlideToShutDown.exe"}};

const int NUM_BUTTONS = sizeof(g_ButtonInfos) / sizeof(g_ButtonInfos[0]);


CLauncherButton::CLauncherButton(const ButtonInfo& info, BOOL bDarkMode)
	: m_info(info) {}


CLauncherButton::~CLauncherButton() {}



void CLauncherButton::SetDpi(int iDpi) {
	m_iDpi = iDpi;
}


void CLauncherButton::OnPaint() {
	CPaintDC dc(this);
	DrawButton(&dc, bDarkMode);
}


BOOL CLauncherButton::OnEraseBkgnd(CDC* pDC) {
	return TRUE;  // 禁止擦除背景，避免闪烁
}


void CLauncherButton::OnMouseMove(UINT nFlags, CPoint point) {
	if (!m_bHover) {
		m_bHover = true;
		TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, GetSafeHwnd(), 0};
		TrackMouseEvent(&tme);
		Invalidate();  // 可选：重绘以显示悬停效果
	}

	CMFCButton::OnMouseMove(nFlags, point);
}


void CLauncherButton::OnMouseLeave() {
	m_bHover = false;
	Invalidate();
	CMFCButton::OnMouseLeave();
}



void CLauncherButton::DrawButton(CDC* pDC, BOOL bDarkMode) {
  CRect rect;
  GetClientRect(&rect);

  // 开始缓冲绘制
  BP_PAINTPARAMS bpParams = {sizeof(BP_PAINTPARAMS)};
  HPAINTBUFFER hBufferedPaint = BeginBufferedPaint(
      pDC->GetSafeHdc(), &rect, BPBF_COMPATIBLEBITMAP, &bpParams, NULL);
  if (!hBufferedPaint) return;

  HDC hdcBuffer = GetBufferedPaintDC(hBufferedPaint);
  CDC* pBufferDC = CDC::FromHandle(hdcBuffer);
  if (!pBufferDC) {
    EndBufferedPaint(hBufferedPaint, TRUE);
    return;
  }

  // 获取主题背景色
  COLORREF bgColor = bDarkMode ? RGB(45, 45, 45) : RGB(251, 251, 251);
  COLORREF textColor = bDarkMode ? RGB(255, 255, 255) : RGB(26, 26, 26);

  // 1. 填充背景
  pBufferDC->FillSolidRect(&rect, bgColor);

  // 2. 对整个缓冲区设置 Alpha = 50
  BufferedPaintSetAlpha(hBufferedPaint, &rect, 50);

  // 3. 绘制图标文本
  CFont font;
  LOGFONT lf = {0};
  lf.lfHeight = -MulDiv(20, m_iDpi, USER_DEFAULT_SCREEN_DPI);  // 20磅
  lf.lfWeight = FW_SEMIBOLD;
  wcscpy_s(lf.lfFaceName, L"Segoe Fluent Icons");
  font.CreateFontIndirect(&lf);
  CFont* pOldFont = pBufferDC->SelectObject(&font);

  pBufferDC->SetBkMode(TRANSPARENT);
  pBufferDC->SetTextColor(textColor);
  CRect textRect = rect;
  pBufferDC->DrawText(m_info.iconChar, &textRect,
                      DT_CENTER | DT_VCENTER | DT_SINGLELINE);

  // 4. 计算实际文本占用的矩形（近似：文本区域就是整个按钮，因为图标可能居中）
  //    更精确的做法：通过 GetTextExtent 计算文本边界
  CSize textSize = pBufferDC->GetTextExtent(m_info.iconChar);
  CRect iconRect(CPoint((rect.Width() - textSize.cx) / 2,
                        (rect.Height() - textSize.cy) / 2),
                 textSize);
  iconRect &= rect;  // 确保在客户区内

  // 5. 对文本区域设置 Alpha = 100
  BufferedPaintSetAlpha(hBufferedPaint, &iconRect, 100);

  pBufferDC->SelectObject(pOldFont);

  // 结束缓冲绘制
  EndBufferedPaint(hBufferedPaint, TRUE);
}


CRect CLauncherButton::GetIconRect(CDC* pDC) {
  // 近似文本区域（可被 DrawButton 内部使用）
  return CRect();
}


void CLauncherButton::Launch() {
	CString strTarget(m_info.target);
	SHELLEXECUTEINFO sei = {sizeof(sei)};
	sei.nShow = SW_SHOWNORMAL;
	sei.fMask = SEE_MASK_FLAG_NO_UI;	// 避免系统弹出错误 UI，由我们自己处理

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
		if (!IsProtocolAssociated(strTarget)) {
			AfxMessageBox(
				CString(L"Error: No application is associated with the protocol ") + strTarget);
			return;
		}
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


// 辅助函数：检查打包应用是否安装（需实现）
BOOL CLauncherButton::IsAppInstalled(const CString& aumid) {
	// 可使用 PackageManager 或 IApplicationActivationManager 相关 API
	// 此处简化为始终返回 true（实际项目需实现）
	return true;
}


BOOL CLauncherButton::IsFileExists(const CString& fileName) {
	// 如果文件名不包含路径，搜索系统路径
	if (fileName.Find(L'\\') == -1 && fileName.Find(L'/') == -1) {
		TCHAR szPath[MAX_PATH];
		if (SearchPath(NULL, fileName, NULL, MAX_PATH, szPath, NULL)) return true;
		return false;
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