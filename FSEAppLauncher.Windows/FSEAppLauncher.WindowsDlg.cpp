
// FSEAppLauncher.WindowsDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "FSEAppLauncher.Windows.h"
#include "FSEAppLauncher.WindowsDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CFSEAppLauncherWindowsDlg 对话框



CFSEAppLauncherWindowsDlg::CFSEAppLauncherWindowsDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FSEAPPLAUNCHERWINDOWS_DIALOG, pParent) {
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


CFSEAppLauncherWindowsDlg::~CFSEAppLauncherWindowsDlg() {
	// 确保资源被释放
	if (m_pExplorerBrowser) {
		m_pExplorerBrowser->Destroy();
		m_pExplorerBrowser->Release();
		m_pExplorerBrowser = NULL;
	}
}


void CFSEAppLauncherWindowsDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFSEAppLauncherWindowsDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_ACTIVATE()
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_DPICHANGED, &CFSEAppLauncherWindowsDlg::OnDpiChangedMessage)
	ON_WM_MOVE()
	ON_MESSAGE(WM_NCCALCSIZE, &CFSEAppLauncherWindowsDlg::OnNcCalcSizeMessage)
	ON_WM_NCCREATE()
	ON_WM_SETTINGCHANGE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CFSEAppLauncherWindowsDlg 消息处理程序

BOOL CFSEAppLauncherWindowsDlg::OnInitDialog() {
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_MAXIMIZE);

	// TODO: Add extra initialization here
	CMenu* pMenu = GetSystemMenu(FALSE);
	pMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
	pMenu->EnableMenuItem(SC_MOVE, MF_BYCOMMAND | MF_GRAYED);

	if (!CreateExplorerBrowser()) {
		return FALSE;
	}

	CreateButtons();  // 创建按钮

	return TRUE;  // return TRUE unless you set the focus to a control
}


// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFSEAppLauncherWindowsDlg::OnPaint() {
	CPaintDC dc(this); // 用于绘制的设备上下文

	if (IsIconic()) {
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int iDpi = GetDpiForWindow(GetSafeHwnd());

		// 使图标在工作区矩形中居中
		// Replaced with Per-Monitor version
		int cxIcon = GetSystemMetricsForDpi(SM_CXICON, iDpi);
		int cyIcon = GetSystemMetricsForDpi(SM_CYICON, iDpi);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	} else {
		CDialogEx::OnPaint();
	}

	PaintTitle(&dc);
}


//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFSEAppLauncherWindowsDlg::OnQueryDragIcon() {
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL CFSEAppLauncherWindowsDlg::OnCommand(WPARAM wParam, LPARAM lParam) {
	if (HIWORD(wParam) == BN_CLICKED) {
		int nID = LOWORD(wParam);
		int index = nID - IDC_LAUNCHER_BTN_BASE;
		if (index >= 0 && index < (int)m_buttons.size()) {
			m_buttons[index]->Launch();
			return TRUE;
		}
	}
	return CDialogEx::OnCommand(wParam, lParam);
}


void CFSEAppLauncherWindowsDlg::OnDestroy() {
	// 清理资源
	DestroyExplorerBrowser();

	CDialogEx::OnDestroy();
}


// 重载 the action of the OK button and Enter key
void CFSEAppLauncherWindowsDlg::OnOK() {
}


LRESULT CFSEAppLauncherWindowsDlg::WindowProc(UINT message, WPARAM wParam,
                                              LPARAM lParam) {
	// 先让 DWM 尝试处理消息
	LRESULT lResult = 0;
	if (DwmDefWindowProc(GetSafeHwnd(), message, wParam, lParam, &lResult)) {
		return lResult;
	}

	// DWM 未处理，调用基类默认处理
	return CDialogEx::WindowProc(message, wParam, lParam);
}


void CFSEAppLauncherWindowsDlg::OnActivate(UINT nState, CWnd* pWndOther,
                                           BOOL bMinimized) {
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);

	// 当窗口失去焦点时
	if (nState == WA_INACTIVE) {
		// If the active window is not a UAC dialog or a shell flyout,
		//  immediately minimize the window of Simple App Launcher
		if (CheckActiveWindow()) {
			ShowWindow(SW_MINIMIZE);
		}
	} else {
		ExtendFrameIntoClientArea();
	}
}


int CFSEAppLauncherWindowsDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	int result = CDialogEx::OnCreate(lpCreateStruct);
	if (result == -1) {
		return -1;
	}

	ApplyDarkModeSettings(GetSafeHwnd());

	BOOL bTRUE = TRUE;
	BOOL bDarkMode = IsDarkMode();
	COLORREF rgb = bDarkMode ? RGB(32, 32, 32) : RGB(243, 243, 243);
	INT nValue = 2/* DWMWCP_ROUND *//* DWMSBT_MAINWINDOW */;

	DwmSetWindowAttribute(GetSafeHwnd(),
	                      DWMWA_DISALLOW_PEEK,
	                      &bTRUE,
	                      sizeof(bTRUE));
	DwmSetWindowAttribute(GetSafeHwnd(),
	                      DWMWA_USE_HOSTBACKDROPBRUSH,
	                      &bTRUE,
	                      sizeof(bTRUE));
	if (bDarkMode) {
		DwmSetWindowAttribute(GetSafeHwnd(),
		                      DWMWA_USE_IMMERSIVE_DARK_MODE,
		                      &bDarkMode,
		                      sizeof(bDarkMode));
	}
	DwmSetWindowAttribute(GetSafeHwnd(),
	                      DWMWA_TEXT_COLOR,
	                      &rgb,
	                      sizeof(rgb));
	DwmSetWindowAttribute(GetSafeHwnd(),
	                      DWMWA_WINDOW_CORNER_PREFERENCE,
	                      &nValue,
	                      sizeof(nValue));
	DwmSetWindowAttribute(GetSafeHwnd(),
	                      DWMWA_SYSTEMBACKDROP_TYPE,
	                      &nValue,
	                      sizeof(nValue));

	CRect rect;
	GetWindowRect(&rect);

	// Inform the application of the frame change.
	SetWindowPos(NULL, rect.left, rect.top,
	             rect.Width(), rect.Height(),
	             SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);

	return result;	// 返回 0 表示成功，-1 表示失败会销毁窗口
}


HBRUSH CFSEAppLauncherWindowsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd,
                                             UINT nCtlColor) {
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (IsDarkMode()) {
		// App mode: Dark
		if (nCtlColor == CTLCOLOR_STATIC) {
			pDC->SetBkColor(RGB(32, 32, 32));	// Solid Background / Base
			pDC->SetTextColor(RGB(255, 255, 255));	// Text / Primary
		}
		return CreateSolidBrush(RGB(32, 32, 32));	// Solid Background / Base
	}

	// App mode: Light
	if (nCtlColor == CTLCOLOR_STATIC) {
		pDC->SetBkColor(RGB(243, 243, 243));	// Solid Background / Base
		pDC->SetTextColor(RGB(26, 26, 26));	// Text / Primary
	}

	return CreateSolidBrush(RGB(243, 243, 243));	// Solid Background / Base
}


LRESULT CFSEAppLauncherWindowsDlg::OnDpiChangedMessage(WPARAM wParam,
                                                       LPARAM lParam) {
	// Call this function to correctly redraw the window when not minimized
	ExtendFrameIntoClientArea();

	// Call this function to correctly redraw the window when minimized
	if (m_pExplorerBrowser) {
		m_pExplorerBrowser->SetRect(NULL, NewRectForExplorerBrowser());
	}

	UpdateButtonLayout();  // 更新按钮布局

	return 0;
}


void CFSEAppLauncherWindowsDlg::OnMove(int x, int y) {
	if (!(IsZoomed() || IsIconic())) {
		ShowWindow(SW_SHOWMAXIMIZED);
		return;
	}

	CDialogEx::OnMove(x, y);
}


LRESULT CFSEAppLauncherWindowsDlg::OnNcCalcSizeMessage(WPARAM wParam,
                                                       LPARAM lParam) {
	if (wParam == TRUE) {	// 当 wParam 为 TRUE 时，表示需要计算客户区大小
		return 0;	// 返回 0 表示整个窗口区域都将作为客户区（即移除标准框架）
	}

	return Default();	// 否则调用默认处理（例如窗口最小化/最大化时的计算）
}


BOOL CFSEAppLauncherWindowsDlg::OnNcCreate(LPCREATESTRUCT lpCreateStruct) {
	EnableNonClientDpiScaling(GetSafeHwnd());

	return CDialogEx::OnNcCreate(lpCreateStruct);
}


void CFSEAppLauncherWindowsDlg::OnSettingChange(UINT uFlags,
                                                LPCTSTR lpszSection) {
	CDialogEx::OnSettingChange(uFlags, lpszSection);

	// 检测是否不是系统颜色主题（Immersive Color Set）的变化
	if (!lpszSection || _tcsicmp(lpszSection, _T("ImmersiveColorSet"))) {
		return;
	}

	// Update the dark mode of the title bar
	BOOL bDarkMode = IsDarkMode();
	COLORREF rgb = bDarkMode ? RGB(32, 32, 32) : RGB(243, 243, 243);

	DwmSetWindowAttribute(GetSafeHwnd(),
	                      DWMWA_USE_IMMERSIVE_DARK_MODE,
	                      &bDarkMode,
	                      sizeof(bDarkMode));
	DwmSetWindowAttribute(GetSafeHwnd(),
	                      DWMWA_TEXT_COLOR,
	                      &rgb,
	                      sizeof(rgb));

	// 强制所有按钮重绘
	for (auto btn : m_buttons) {
		if (btn) {
			btn->Invalidate();
		}
	}
}


void CFSEAppLauncherWindowsDlg::OnSize(UINT nType, int cx, int cy) {
	if (!(IsZoomed() || IsIconic())) {
		ShowWindow(SW_SHOWMAXIMIZED);
		return;
	}

	CDialogEx::OnSize(nType, cx, cy);

	// 调整 ExplorerBrowser 大小
	if (m_pExplorerBrowser) {
		m_pExplorerBrowser->SetRect(NULL, NewRectForExplorerBrowser());
	}

	UpdateButtonLayout();
}


void CFSEAppLauncherWindowsDlg::ApplyDarkModeSettings(HWND hWnd) {
	HMODULE hUxtheme = LoadLibraryEx(L"uxtheme.dll",
	                                 NULL,
	                                 LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (!hUxtheme) {
		return;
	}

	// Windows 10 1903 and above only
	auto SetPreferredAppMode = (SetPreferredAppModeProc)GetProcAddress(
			hUxtheme, MAKEINTRESOURCEA(135));
	if (SetPreferredAppMode) {
		SetPreferredAppMode(AllowDark);
	}

	// 刷新颜色策略并允许窗口深色模式
	auto RefreshImmersiveColorPolicyState =
			(RefreshImmersiveColorPolicyStateProc)GetProcAddress(
					hUxtheme, MAKEINTRESOURCEA(104));
	auto AllowDarkModeForWindow = (AllowDarkModeForWindowProc)GetProcAddress(
			hUxtheme, MAKEINTRESOURCEA(133));

	if (RefreshImmersiveColorPolicyState) {
		RefreshImmersiveColorPolicyState();
	}
	if (AllowDarkModeForWindow) {
		AllowDarkModeForWindow(hWnd, TRUE);
	}

	FreeLibrary(hUxtheme);
}


// Check that the activated window is definitely not a UAC dialog or a shell flyout
BOOL CFSEAppLauncherWindowsDlg::CheckActiveWindow() {
	CWnd* cWndForeground = GetForegroundWindow();
	if (!cWndForeground) {
		return FALSE;
	}

	DWORD pid = NULL;
	GetWindowThreadProcessId(cWndForeground->GetSafeHwnd(), &pid);
	if (!pid) {
		return FALSE;
	}

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,
	                              FALSE, pid);
	if (!hProcess) {
		return FALSE;
	}

	// Check that the process name does not match the specified string
	WCHAR szExeName[MAXCHAR];
	DWORD dwSize = MAXCHAR;
	if (QueryFullProcessImageName(hProcess, 0, szExeName, &dwSize)) {
		CString strProcessName(szExeName);
		strProcessName.MakeUpper();

		if (strProcessName.Find(L"CONSENT.EXE") < 0 &&
		    strProcessName.Find(L"FSEAPPLAUNCHER.WINDOWS.EXE") < 0 &&
						strProcessName.Find(L"MICROSOFT.CMDPAL.UI.EXE") < 0 &&
		    strProcessName.Find(L"SHELLEXPERIENCEHOST.EXE") < 0 &&
		    strProcessName.Find(L"SHELLHOST.EXE") < 0 &&
		    strProcessName.Find(L"SLIDETOSHUTDOWN.EXE") < 0) {
			CloseHandle(hProcess);
			return TRUE;	// It is not a UAC dialog or a shell flyout
		}
	}

	// It is a UAC dialog or a shell flyout
	CloseHandle(hProcess);

	return FALSE;
}


void CFSEAppLauncherWindowsDlg::CreateButtons() {
	// 确保之前创建的按钮被删除
	for (auto btn : m_buttons) {
		if (btn) {
			btn->DestroyWindow();
		}
	}
	m_buttons.clear();

	// 循环创建每个按钮
	BOOL bDarkMode = IsDarkMode();
	for (int i = 0; i < NUM_BUTTONS; ++i) {
		CLauncherButton* pBtn = new CLauncherButton(g_ButtonInfos[i], bDarkMode);
		if (!pBtn->Create(NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
		                  CRect(0, 0, 0, 0), this, IDC_LAUNCHER_BTN_BASE + i)) {
			delete pBtn;
			continue;
		}
		// 设置工具提示
		pBtn->SetTooltip(g_ButtonInfos[i].tooltip);
		// 保存指针
		m_buttons.push_back(pBtn);
	}

	// 初始布局
	UpdateButtonLayout();
}


void CFSEAppLauncherWindowsDlg::UpdateButtonLayout() {
	if (m_buttons.empty()) {
		return;
	}

	int iDpi = GetDpiForWindow(GetSafeHwnd());

	// 按钮尺寸：40 epx
	int btnWidth = MulDiv(40, iDpi, USER_DEFAULT_SCREEN_DPI);
	int btnHeight = btnWidth;  // 正方形

	// 按钮之间的间距
	int spacing = MulDiv(4, iDpi, USER_DEFAULT_SCREEN_DPI);

	// 右边距 = GetCalculatedMarginForDpi(Right)
	int rightMargin = GetCalculatedMarginForDpi(Right);
	// 上边距 = 标题上边距
	int topMargin = MulDiv(m_titlePaddingTop, iDpi, USER_DEFAULT_SCREEN_DPI);

	CRect clientRect;
	GetClientRect(&clientRect);

	// 从右向左布局
	int x = clientRect.right - rightMargin - btnWidth;
	int y = topMargin;  // 上边距与标题对齐

	for (size_t i = 0; i < m_buttons.size(); ++i) {
		CLauncherButton* pBtn = m_buttons[i];

		BOOL bDarkMode = IsDarkMode();
		pBtn->SetDarkMode(bDarkMode);
		pBtn->SetDpi(iDpi);
		pBtn->SetWindowPos(NULL, x, y, btnWidth, btnHeight,
		                   SWP_NOZORDER | SWP_NOACTIVATE);
		x -= (btnWidth + spacing);
	}
}


BOOL CFSEAppLauncherWindowsDlg::CreateExplorerBrowser() {
	HRESULT hr = CoCreateInstance(CLSID_ExplorerBrowser,
	                              NULL,
	                              CLSCTX_INPROC_SERVER,
	                              IID_PPV_ARGS(&m_pExplorerBrowser));

	if (FAILED(hr) || !m_pExplorerBrowser) {
		CString strError;
		strError.Format(_T("Error: Cannot create the instance of ExplorerBrowser (HRESULT: 0x%X)."), hr);
		AfxMessageBox(strError);
		EndDialog(IDCLOSE);	// 创建失败，关闭对话框
		return FALSE;
	}

	// 获取选项并设置，必须使用 EXPLORER_BROWSER_OPTIONS 数据类型，否则 GetOptions() 无法正确传参
	EXPLORER_BROWSER_OPTIONS ebOptions;
	m_pExplorerBrowser->GetOptions(&ebOptions);
	ebOptions |= EBO_NAVIGATEONCE | EBO_ALWAYSNAVIGATE |
	             EBO_NOTRAVELLOG | EBO_NOBORDER;
	m_pExplorerBrowser->SetOptions(ebOptions);

	// 设置文件夹视图
	FOLDERSETTINGS fs;
	fs.ViewMode = FVM_TILE;
	fs.fFlags = FWF_AUTOARRANGE | FWF_ABBREVIATEDNAMES | FWF_SNAPTOGRID |
	            FWF_SINGLESEL | FWF_TRANSPARENT | FWF_SINGLECLICKACTIVATE |
	            FWF_NOWEBVIEW | FWF_FULLROWSELECT | FWF_NOHEADERINALLVIEWS;

	// 初始化 ExplorerBrowser
	hr = m_pExplorerBrowser->Initialize(GetSafeHwnd(),
	                                    NewRectForExplorerBrowser(),
	                                    &fs);

	if (SUCCEEDED(hr)) {
		LPITEMIDLIST pidl = NULL;
		// Virtual folder "Applications"
		if (SUCCEEDED(
						SHParseDisplayName(L"shell:::{4234d49b-0245-4df3-b780-3893943456e1}",
						                   NULL,
						                   &pidl,
						                   0,
						                   NULL))) {
			hr = m_pExplorerBrowser->BrowseToIDList(pidl, SBSP_ABSOLUTE);
			if (SUCCEEDED(hr)){
				SetGroupingByName();
			} else {
				AfxMessageBox(_T("Error: Cannot navigate to the Applications folder."));
			}
			CoTaskMemFree(pidl);
		} else {
			AfxMessageBox(_T("Error: Cannot find the Applications folder."));
		}
	} else {
		CString strError;
		strError.Format(_T("Error: Failed to initialize ExplorerBrowser (HRESULT: 0x%X)."), hr);
		AfxMessageBox(strError);
	}

	return TRUE;
}


void CFSEAppLauncherWindowsDlg::DestroyExplorerBrowser() {
	if (m_pExplorerBrowser) {
		m_pExplorerBrowser->Destroy();
		m_pExplorerBrowser->Release();
		m_pExplorerBrowser = NULL;
	}
}


// Extend the frame into the client area.
HRESULT CFSEAppLauncherWindowsDlg::ExtendFrameIntoClientArea() {
	MARGINS margins = {GetCalculatedMarginForDpi(Left),
	                   GetCalculatedMarginForDpi(Right),
	                   GetCalculatedMarginForDpi(Top),
	                   GetCalculatedMarginForDpi(Bottom)};

	return DwmExtendFrameIntoClientArea(GetSafeHwnd(), &margins);
}


INT CFSEAppLauncherWindowsDlg::GetCalculatedMarginForDpi(INT marginOrientation) const {
	int iDpi = GetDpiForWindow(GetSafeHwnd());

	switch (marginOrientation) {
	case Left:
		return MulDiv(GetSystemMetricsForDpi(SM_CXPADDEDBORDER, iDpi) + m_ncPaddingNormal,
		              iDpi,
		              USER_DEFAULT_SCREEN_DPI);

	case Top:
		return MulDiv(GetSystemMetricsForDpi(SM_CXPADDEDBORDER, iDpi) +
		              m_ncPaddingNormal + m_ncPaddingTopIncrement,
		              iDpi,
		              USER_DEFAULT_SCREEN_DPI);

	case Right:
		return MulDiv(GetSystemMetricsForDpi(SM_CXPADDEDBORDER, iDpi) + m_ncPaddingNormal,
		              iDpi,
		              USER_DEFAULT_SCREEN_DPI);

	case Bottom:
		return MulDiv(GetSystemMetricsForDpi(SM_CXPADDEDBORDER, iDpi) + m_ncPaddingNormal,
		              iDpi,
		              USER_DEFAULT_SCREEN_DPI);

	default:
		AfxMessageBox(_T("Error: Invalid parameter."));
	}

	return 0;
}


CRect CFSEAppLauncherWindowsDlg::NewRectForExplorerBrowser() {
	CRect rect;

	// 获取显示区域
	GetClientRect(&rect);

	// 调整区域，留出一些边距
	INT ncPaddingLeft = GetCalculatedMarginForDpi(Left);
	INT ncPaddingTop = GetCalculatedMarginForDpi(Top);
	INT ncPaddingRight = GetCalculatedMarginForDpi(Right);
	INT ncPaddingBottom = GetCalculatedMarginForDpi(Bottom);
	rect.DeflateRect((ncPaddingLeft + ncPaddingRight) / 2,
	                 (ncPaddingTop + ncPaddingBottom) / 2);
	rect.OffsetRect((ncPaddingLeft - ncPaddingRight) / 2,
	                (ncPaddingTop - ncPaddingBottom) / 2);

	return rect;
}


BOOL CFSEAppLauncherWindowsDlg::IsDarkMode() const {
	// 首次调用时动态获取函数地址
	if (!g_pShouldAppsUseDarkMode) {
		HMODULE hUxtheme =
				LoadLibraryEx(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
		if (hUxtheme) {
			g_pShouldAppsUseDarkMode = (ShouldAppsUseDarkModeProc)GetProcAddress(
					hUxtheme, MAKEINTRESOURCEA(132));	// 132 对应 ShouldAppsUseDarkMode
			FreeLibrary(hUxtheme);
		}
	}

	return g_pShouldAppsUseDarkMode ? g_pShouldAppsUseDarkMode() : FALSE;
}


// Paint the title on the custom frame.
void CFSEAppLauncherWindowsDlg::PaintTitle(CPaintDC* pDC) {
	CRect rcClient;
	GetClientRect(&rcClient);

	HTHEME hTheme = OpenThemeData(NULL, L"CompositedWindow::Window");
	if (!hTheme) {
		return;
	}

	CDC pDCPaint;
	if (pDCPaint.CreateCompatibleDC(pDC)) {
		int cx = rcClient.Width();
		int cy = rcClient.Height();

		// Define the BITMAPINFO structure used to draw text.
		// Note that biHeight is negative. This is done because
		// DrawThemeTextEx() needs the bitmap to be in top-to-bottom
		// order.
		BITMAPINFO dib = {};
		dib.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		dib.bmiHeader.biWidth = cx;
		dib.bmiHeader.biHeight = -cy;
		dib.bmiHeader.biPlanes = 1;
		dib.bmiHeader.biBitCount = 32;
		dib.bmiHeader.biCompression = BI_RGB;

		HBITMAP hbm = CreateDIBSection(pDC->m_hDC,
		                               &dib,
		                               DIB_RGB_COLORS,
		                               NULL,
		                               NULL,
		                               0);
		if (hbm) {
			CBitmap* pBmOld = CBitmap::FromHandle((HBITMAP)pDCPaint.SelectObject(hbm));
			int iDpi = GetDpiForWindow(GetSafeHwnd());

			// Setup the theme drawing options.
			DTTOPTS DttOpts = {sizeof(DTTOPTS)};
			DttOpts.dwFlags = DTT_COMPOSITED | DTT_GLOWSIZE | DTT_TEXTCOLOR;
			DttOpts.iGlowSize = 15;
			DttOpts.crText = IsDarkMode() ? RGB(255, 255, 255) : RGB(26, 26, 26);

			// Select a font.
			CFont font;
			CFont* fontOld = NULL;
			LOGFONT lgFont;
			if (SUCCEEDED(GetThemeSysFont(hTheme, TMT_CAPTIONFONT, &lgFont))) {
				int nHeight = -MulDiv(28, iDpi, USER_DEFAULT_SCREEN_DPI);
				lgFont.lfHeight = nHeight;
				lgFont.lfWeight = FW_SEMIBOLD;
				wcscpy_s(lgFont.lfFaceName, LF_FACESIZE, L"Segoe UI Variable Display");
				if (font.CreateFontIndirect(&lgFont)) {
					fontOld = pDCPaint.SelectObject(&font);
				}
			}

			// Draw the title.
			CRect rcPaint = rcClient;
			rcPaint.left += GetCalculatedMarginForDpi(Left);
			rcPaint.top += MulDiv(m_titlePaddingTop, iDpi, USER_DEFAULT_SCREEN_DPI);
			rcPaint.right -= GetCalculatedMarginForDpi(Right);
			rcPaint.bottom = rcPaint.top +
			                 MulDiv(m_titleRectHeight, iDpi, USER_DEFAULT_SCREEN_DPI);
			DrawThemeTextEx(hTheme,
			                pDCPaint.m_hDC,
			                0,
			                0,
			                L"Apps",
			                -1,
			                DT_END_ELLIPSIS | DT_LEFT | DT_NOPREFIX |
			                DT_SINGLELINE | DT_VCENTER,
			                &rcPaint,
			                &DttOpts);

			// Blit text to the frame.
			pDC->BitBlt(0, 0, cx, cy, &pDCPaint, 0, 0, SRCCOPY);

			pDCPaint.SelectObject(pBmOld);
			if (fontOld) {
				pDCPaint.SelectObject(fontOld);
			}
			DeleteObject(hbm);
		}
	}
	CloseThemeData(hTheme);
}


void CFSEAppLauncherWindowsDlg::SetGroupingByName() {
	if (!m_pExplorerBrowser) {
		return;
	}

	// Get the current view
	IFolderView2* pFolderView2 = NULL;

	if (SUCCEEDED(m_pExplorerBrowser->GetCurrentView(
								IID_PPV_ARGS(&pFolderView2))) &&
						pFolderView2) {
		// 设置按名称分组, TRUE indicates ascending
		pFolderView2->SetGroupBy(PKEY_ItemNameDisplay, TRUE);

		pFolderView2->Release();
	}
}