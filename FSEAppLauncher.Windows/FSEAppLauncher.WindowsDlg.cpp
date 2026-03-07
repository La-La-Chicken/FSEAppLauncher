
// FSEAppLauncher.WindowsDlg.cpp: implementation file
//

#include "pch.h"
#include "FSEAppLauncher.WindowsDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CFSEAppLauncherWindowsDlg dialog



CFSEAppLauncherWindowsDlg::CFSEAppLauncherWindowsDlg(CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_FSEAPPLAUNCHERWINDOWS_DIALOG, pParent) {
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


CFSEAppLauncherWindowsDlg::~CFSEAppLauncherWindowsDlg() {
	if (m_pExplorerBrowser) {
		m_pExplorerBrowser->Destroy();
		m_pExplorerBrowser->Release();
		m_pExplorerBrowser = NULL;
	}
}


void CFSEAppLauncherWindowsDlg::DoDataExchange(CDataExchange* pDX) {
	CBaseDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFSEAppLauncherWindowsDlg, CBaseDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_ACTIVATE()
	ON_MESSAGE(WM_DPICHANGED, &CFSEAppLauncherWindowsDlg::OnDpiChangedMessage)
	ON_WM_MOVE()
	ON_WM_SETTINGCHANGE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CFSEAppLauncherWindowsDlg message handlers

BOOL CFSEAppLauncherWindowsDlg::OnInitDialog() {
	CBaseDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	ShowWindow(SW_MAXIMIZE);

	// TODO: Add extra initialization here
	CMenu* pMenu = GetSystemMenu(FALSE);
	pMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
	pMenu->EnableMenuItem(SC_MOVE, MF_BYCOMMAND | MF_GRAYED);

	// Initialize ExplorerBrowser.
	if (!CreateExplorerBrowser()) {
		return FALSE;
	}

	// Initialize the buttons.
	UpdateIconFont();
	CreateButtons();

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CFSEAppLauncherWindowsDlg::OnSysCommand(UINT nID, LPARAM lParam) {
	if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	} else {
		CBaseDialog::OnSysCommand(nID, lParam);
	}
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFSEAppLauncherWindowsDlg::OnPaint() {
	CPaintDC dc(this); // device context for painting

	if (IsIconic()) {
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int iDpi = GetDpiForWindow(GetSafeHwnd());

		// Center icon in client rectangle
		// Use the Per-Monitor version of GetSystemMetrics.
		int cxIcon = GetSystemMetricsForDpi(SM_CXICON, iDpi);
		int cyIcon = GetSystemMetricsForDpi(SM_CYICON, iDpi);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	} else {
		CBaseDialog::OnPaint();
	}

	PaintTitle(&dc);
}


// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
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
	return CBaseDialog::OnCommand(wParam, lParam);
}


void CFSEAppLauncherWindowsDlg::OnDestroy() {
	if (m_pExplorerBrowser) {
		m_pExplorerBrowser->Destroy();
		m_pExplorerBrowser->Release();
		m_pExplorerBrowser = NULL;
	}

	CBaseDialog::OnDestroy();
}


// 重载 the action of the OK button and Enter key
void CFSEAppLauncherWindowsDlg::OnOK() {
}


LRESULT CFSEAppLauncherWindowsDlg::WindowProc(UINT message, WPARAM wParam,
                                              LPARAM lParam) {
	// 先让 DWM 尝试处理消息.
	LRESULT lResult = 0;
	if (DwmDefWindowProc(GetSafeHwnd(), message, wParam, lParam, &lResult)) {
		return lResult;
	}

	// DWM 未处理, 调用基类默认处理.
	return CBaseDialog::WindowProc(message, wParam, lParam);
}


void CFSEAppLauncherWindowsDlg::OnActivate(UINT nState, CWnd* pWndOther,
                                           BOOL bMinimized) {
	CBaseDialog::OnActivate(nState, pWndOther, bMinimized);

	if (nState == WA_INACTIVE) {
		return;
	}

	// If the window is active
	ExtendFrameIntoClientArea();

	// Redraw the icons of the buttons.
	for (auto btn : m_buttons) {
		if (btn) {
			btn->Invalidate();
		}
	}
}


LRESULT CFSEAppLauncherWindowsDlg::OnDpiChangedMessage(WPARAM wParam,
                                                       LPARAM lParam) {
	// Call this function to correctly redraw the window
	//  when not minimized and not activated.
	ExtendFrameIntoClientArea();

	// Update the font of icons.
	UpdateIconFont();
	for (auto btn : m_buttons) {
		if (btn) {
			btn->SetFont(&m_fntIcon);
		}
	}

	// Redraw the window to refresh the title when not minimized.
	RedrawWindow(NULL,
	             NULL,
	             RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);

	return 0;
}


void CFSEAppLauncherWindowsDlg::OnMove(int x, int y) {
	if (!(IsZoomed() || IsIconic())) {
		ShowWindow(SW_SHOWMAXIMIZED);
		return;
	}

	CBaseDialog::OnMove(x, y);
}


void CFSEAppLauncherWindowsDlg::OnSettingChange(UINT uFlags,
                                                LPCTSTR lpszSection) {
	CBaseDialog::OnSettingChange(uFlags, lpszSection);

	// Notify ExplorerBrowser to switch between light / dark mode.
	if (m_pExplorerBrowser) {
		CComPtr<IShellView3> spShellView3;
		if (SUCCEEDED(m_pExplorerBrowser->GetCurrentView(
		     IID_PPV_ARGS(&spShellView3))) &&
		    spShellView3) {
			HWND hwndView = NULL;
			if (SUCCEEDED(spShellView3->GetWindow(&hwndView)) && hwndView) {
				// Send WM_SETTINGCHANGE to the HWND of spShellView3.
				::SendMessage(hwndView, WM_SETTINGCHANGE, uFlags, (LPARAM)lpszSection);
			}
		}
	}
}


void CFSEAppLauncherWindowsDlg::OnSize(UINT nType, int cx, int cy) {
	if (nType == SIZE_RESTORED) {
		ShowWindow(SW_SHOWMAXIMIZED);
		return;
	}

	CBaseDialog::OnSize(nType, cx, cy);

	if (nType == SIZE_MAXIMIZED) {
		// Resize ExplorerBrowser.
		if (m_pExplorerBrowser) {
			m_pExplorerBrowser->SetRect(NULL, NewRectForExplorerBrowser());
		}
		UpdateButtonLayout();
	}
}


// Update the font info of the icons.
void CFSEAppLauncherWindowsDlg::UpdateIconFont() {
	if (m_fntIcon.GetSafeHandle()) {
		m_fntIcon.DeleteObject();
	}

	int iDpi = GetDpiForWindow(GetSafeHwnd());
	LOGFONT lf = {};
	lf.lfHeight = -MulDiv(18, iDpi, USER_DEFAULT_SCREEN_DPI);
	lf.lfWeight = 550;
	_tcscpy_s(lf.lfFaceName, LF_FACESIZE, _T("Segoe Fluent Icons"));
	BOOL bCreated = m_fntIcon.CreateFontIndirect(&lf);

	if (!bCreated) {
		AfxMessageBox(_T("Error: App Launcher only supports Windows 11 full edition."));
		EndDialog(IDCLOSE);
	}
}


// Create the buttons.
void CFSEAppLauncherWindowsDlg::CreateButtons() {
	// Destroy old buttons.
	for (auto btn : m_buttons) {
		if (btn) {
			btn->DestroyWindow();
		}
	}
	m_buttons.clear();

	// Create new buttons.
	for (int i = 0; i < NUM_BUTTONS; ++i) {
		CLauncherButton* pBtn = new CLauncherButton(g_ButtonInfos[i]);
		if (!pBtn->Create(g_ButtonInfos[i].iconChar,
		                  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		                  CRect(0, 0, 0, 0),
		                  this,
		                  IDC_LAUNCHER_BTN_BASE + i)) {
			delete pBtn;
			continue;
		}
		// Set the tooltip.
		pBtn->SetTooltip(g_ButtonInfos[i].tooltip);
		// Set the font of icons.
		pBtn->SetFont(&m_fntIcon);
		// Save the pointer.
		m_buttons.push_back(pBtn);
	}

	// Initialize the layout.
	UpdateButtonLayout();
}


void CFSEAppLauncherWindowsDlg::UpdateButtonLayout() {
	if (m_buttons.empty()) {
		return;
	}

	int iDpi = GetDpiForWindow(GetSafeHwnd());

	// Button: 40 epx + 2 px padding.
	int btnWidth = MulDiv(40, iDpi, USER_DEFAULT_SCREEN_DPI) + 2;
	int btnHeight = btnWidth;

	// Margin: 4 epx - 2 px padding.
	int spacing = MulDiv(4, iDpi, USER_DEFAULT_SCREEN_DPI) - 2;

	// Right margin of the button set: Right padding of the dialog - 1 px padding.
	int rightMargin = GetCalculatedMarginForDpi(MarginOrientation::Right) - 1;
	// Top margin of the button set: Top margin of the title - 1 px padding.
	int topMargin = MulDiv(m_titlePaddingTop, iDpi, USER_DEFAULT_SCREEN_DPI) - 1;

	CRect clientRect;
	GetClientRect(&clientRect);

	// right-to-left layout
	int x = clientRect.right - rightMargin - btnWidth;
	int y = clientRect.top + topMargin;

	for (size_t i = 0; i < m_buttons.size(); ++i) {
		CLauncherButton* pBtn = m_buttons[i];

		// Place the buttons at the top of the Z-order.
		pBtn->SetWindowPos(&wndTop,
		                   x,
		                   y,
		                   btnWidth,
		                   btnHeight,
		                   SWP_NOACTIVATE);
		x -= spacing + btnWidth;
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
		EndDialog(IDCLOSE);	// Create failed, close the dialog.
		return FALSE;
	}

	// Get the options and set them.
	EXPLORER_BROWSER_OPTIONS ebOptions;
	m_pExplorerBrowser->GetOptions(&ebOptions);
	ebOptions |= EBO_NAVIGATEONCE | EBO_ALWAYSNAVIGATE |
	             EBO_NOTRAVELLOG | EBO_NOBORDER;
	m_pExplorerBrowser->SetOptions(ebOptions);

	// Set up the folder view.
	FOLDERSETTINGS fs;
	fs.ViewMode = FVM_TILE;
	fs.fFlags = FWF_AUTOARRANGE | FWF_ABBREVIATEDNAMES | FWF_SNAPTOGRID |
	            FWF_SINGLESEL | FWF_TRANSPARENT | FWF_SINGLECLICKACTIVATE |
	            FWF_NOWEBVIEW | FWF_FULLROWSELECT | FWF_NOHEADERINALLVIEWS;

	// Initialize ExplorerBrowser.
	hr = m_pExplorerBrowser->Initialize(GetSafeHwnd(),
	                                    NewRectForExplorerBrowser(),
	                                    &fs);

	if (SUCCEEDED(hr)) {
		LPITEMIDLIST pidl = NULL;
		// virtual folder "Applications"
		if (SUCCEEDED(
						SHParseDisplayName(_T("shell:::{4234d49b-0245-4df3-b780-3893943456e1}"),
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


// Extend the frame into the client area.
HRESULT CFSEAppLauncherWindowsDlg::ExtendFrameIntoClientArea() {
	MARGINS margins = {GetCalculatedMarginForDpi(MarginOrientation::Left),
	                   GetCalculatedMarginForDpi(MarginOrientation::Right),
	                   GetCalculatedMarginForDpi(MarginOrientation::Top),
	                   GetCalculatedMarginForDpi(MarginOrientation::Bottom)};

	return DwmExtendFrameIntoClientArea(GetSafeHwnd(), &margins);
}


INT CFSEAppLauncherWindowsDlg::GetCalculatedMarginForDpi(MarginOrientation marginOrientation) const {
	int iDpi = GetDpiForWindow(GetSafeHwnd());

	switch (marginOrientation) {
	case MarginOrientation::Left:
		return MulDiv(GetSystemMetricsForDpi(SM_CXPADDEDBORDER, iDpi) + m_ncPaddingNormal,
		              iDpi,
		              USER_DEFAULT_SCREEN_DPI);

	case MarginOrientation::Top:
		return MulDiv(GetSystemMetricsForDpi(SM_CXPADDEDBORDER, iDpi) +
		              m_ncPaddingNormal + m_ncPaddingTopIncrement,
		              iDpi,
		              USER_DEFAULT_SCREEN_DPI);

	case MarginOrientation::Right:
		return MulDiv(GetSystemMetricsForDpi(SM_CXPADDEDBORDER, iDpi) + m_ncPaddingNormal,
		              iDpi,
		              USER_DEFAULT_SCREEN_DPI);

	case MarginOrientation::Bottom:
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

	// Get the display area.
	GetClientRect(&rect);

	// 调整区域, 留出一些边距.
	INT ncPaddingLeft = GetCalculatedMarginForDpi(MarginOrientation::Left);
	INT ncPaddingTop = GetCalculatedMarginForDpi(MarginOrientation::Top);
	INT ncPaddingRight = GetCalculatedMarginForDpi(MarginOrientation::Right);
	INT ncPaddingBottom = GetCalculatedMarginForDpi(MarginOrientation::Bottom);
	rect.DeflateRect((ncPaddingLeft + ncPaddingRight) / 2,
	                 (ncPaddingTop + ncPaddingBottom) / 2);
	rect.OffsetRect((ncPaddingLeft - ncPaddingRight) / 2,
	                (ncPaddingTop - ncPaddingBottom) / 2);

	return rect;
}


// Paint the title on the custom frame.
void CFSEAppLauncherWindowsDlg::PaintTitle(CPaintDC* pDC) {
	CRect rcClient;
	GetClientRect(&rcClient);

	HTHEME hTheme = OpenThemeData(NULL, _T("CompositedWindow::Window"));
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

		CBitmap* pBm = CBitmap::FromHandle(CreateDIBSection(pDC->m_hDC,
		                                                    &dib,
		                                                    DIB_RGB_COLORS,
		                                                    NULL,
		                                                    NULL,
		                                                    0));
		if (pBm) {
			CBitmap* pBmOld = pDCPaint.SelectObject(pBm);
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
				_tcscpy_s(lgFont.lfFaceName, LF_FACESIZE, _T("Segoe UI Variable Display"));
				if (font.CreateFontIndirect(&lgFont)) {
					fontOld = pDCPaint.SelectObject(&font);
				}
			}

			// Draw the title.
			CRect rcPaint = rcClient;
			rcPaint.left += GetCalculatedMarginForDpi(MarginOrientation::Left);
			rcPaint.top += MulDiv(m_titlePaddingTop, iDpi, USER_DEFAULT_SCREEN_DPI);
			rcPaint.right -= GetCalculatedMarginForDpi(MarginOrientation::Right);
			rcPaint.bottom = rcPaint.top +
			                 MulDiv(m_titleRectHeight, iDpi, USER_DEFAULT_SCREEN_DPI);
			DrawThemeTextEx(hTheme,
			                pDCPaint.m_hDC,
			                0,
			                0,
			                _T("Apps"),
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
			DeleteObject(HBITMAP(pBm));
		}
	}
	CloseThemeData(hTheme);
}


void CFSEAppLauncherWindowsDlg::SetGroupingByName() {
	if (!m_pExplorerBrowser) {
		return;
	}

	// Get the current view.
	IFolderView2* pFolderView2 = NULL;

	if (SUCCEEDED(m_pExplorerBrowser->GetCurrentView(
								IID_PPV_ARGS(&pFolderView2))) &&
						pFolderView2) {
		// 设置按名称分组, TRUE indicates ascending
		pFolderView2->SetGroupBy(PKEY_ItemNameDisplay, TRUE);

		pFolderView2->Release();
	}
}