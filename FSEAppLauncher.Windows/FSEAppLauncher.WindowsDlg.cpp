
// FSEAppLauncher.WindowsDlg.cpp: implementation file
//

#include "pch.h"
#include "FSEAppLauncher.WindowsDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



CONST INT CFSEAppLauncherWindowsDlg::m_ncPaddingNormal = 48;
CONST INT CFSEAppLauncherWindowsDlg::m_ncPaddingTopIncrement = 64;
CONST INT CFSEAppLauncherWindowsDlg::m_titlePaddingTop = 46;
CONST INT CFSEAppLauncherWindowsDlg::m_titleRectHeight = 40;
CONST CSize CFSEAppLauncherWindowsDlg::m_btnSize = {40, 40};
CONST INT CFSEAppLauncherWindowsDlg::m_btnMargin = 4;

CONST PCTSTR CFSEAppLauncherWindowsDlg::applicationFolderPath =
	_T("shell:::{4234d49b-0245-4df3-b780-3893943456e1}");



// CFSEAppLauncherWindowsDlg dialog



CFSEAppLauncherWindowsDlg::CFSEAppLauncherWindowsDlg(CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_FSEAPPLAUNCHERWINDOWS_DIALOG, pParent),
	  m_pExplorerBrowser(nullptr) {
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


CFSEAppLauncherWindowsDlg::~CFSEAppLauncherWindowsDlg() {
	DestroyExplorerBrowser();
}


BEGIN_MESSAGE_MAP(CFSEAppLauncherWindowsDlg, CBaseDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_ACTIVATE()
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
	if (pSysMenu) {
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty()) {
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
	DestroyExplorerBrowser();

	CBaseDialog::OnDestroy();
}


// Overload the action of the OK button and Enter key.
void CFSEAppLauncherWindowsDlg::OnOK() {}


LRESULT CFSEAppLauncherWindowsDlg::WindowProc(UINT message, WPARAM wParam,
                                              LPARAM lParam) {
	BOOL fCallDWP = TRUE;
	BOOL fDwmEnabled = FALSE;
	LRESULT lRet = 0;
	HRESULT hr = S_OK;

	// Winproc worker for custom frame issues.
	hr = DwmIsCompositionEnabled(&fDwmEnabled);
	if (SUCCEEDED(hr)) {
		fCallDWP = !DwmDefWindowProc(GetSafeHwnd(), message, wParam, lParam, &lRet);

		switch (message) {
		case WM_CREATE:
		case WM_ACTIVATE:
		case WM_PAINT:
			fCallDWP = TRUE;
			lRet = 0;
			break;

		case WM_NCCALCSIZE:
			// Handle the non-client size message.
			if (wParam == TRUE) {
				// Calculate new NCCALCSIZE_PARAMS based on custom NCA inset.
				NCCALCSIZE_PARAMS* pncsp = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);

				pncsp->rgrc[0].left = pncsp->rgrc[0].left + 0;
				pncsp->rgrc[0].top = pncsp->rgrc[0].top + 0;
				pncsp->rgrc[0].right = pncsp->rgrc[0].right - 0;
				pncsp->rgrc[0].bottom = pncsp->rgrc[0].bottom - 0;

				// No need to pass the message on to the DefWindowProc.
				fCallDWP = FALSE;
				lRet = 0;
			}
			break;

		case WM_NCHITTEST:
			// Handle hit testing in the NCA if not handled by DwmDefWindowProc.
			if (lRet == 0) {
				lRet = HitTestNCA(wParam, lParam);

				if (lRet != HTNOWHERE) {
					fCallDWP = FALSE;
				}
			}
			break;

		case WM_DPICHANGED:
			// Correctly redraw the window when not minimized and not activated.
			ExtendFrameIntoClientArea();

			// Update the font of icons.
			UpdateIconFont();
			for (CLauncherButton* btn : m_buttons) {
				if (btn) {
					btn->SetFont(&m_fntIcon);
				}
			}

			// Redraw the window to refresh the title when not minimized.
			RedrawWindow(nullptr,
			             nullptr,
			             RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);

			// When the DPI changes, the WM_SIZE message is also sent.

			fCallDWP = FALSE;
			lRet = 0;
		}
	}

	// Winproc worker for the rest of the application.
	if (fCallDWP) {
		return CBaseDialog::WindowProc(message, wParam, lParam);
	}
	return lRet;
}


void CFSEAppLauncherWindowsDlg::OnActivate(UINT nState, CWnd* pWndOther,
                                           BOOL bMinimized) {
	CBaseDialog::OnActivate(nState, pWndOther, bMinimized);

	if (nState == WA_INACTIVE) {
		return;
	}

	// If the window is active
	ExtendFrameIntoClientArea();

	// Redraw the buttons.
	InvalidateButtons();
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
			HWND hwndView = nullptr;
			if (SUCCEEDED(spShellView3->GetWindow(&hwndView)) && hwndView) {
				// Send WM_SETTINGCHANGE to the HWND of spShellView3.
				::SendMessage(hwndView, WM_SETTINGCHANGE, uFlags, reinterpret_cast<LPARAM>(lpszSection));
			}
		}
	}
}


void CFSEAppLauncherWindowsDlg::OnSize(UINT nType, int cx, int cy) {
	CBaseDialog::OnSize(nType, cx, cy);

	switch (nType) {
	case SIZE_MAXIMIZED:
	case SIZE_RESTORED:
		// Resize ExplorerBrowser and buttons.
		if (m_pExplorerBrowser) {
			m_pExplorerBrowser->SetRect(nullptr, NewExplorerBrowserRectForDpi());
		}
		UpdateButtonLayout();
		// Do not "break".

	case SIZE_MINIMIZED:
		// Invalidate the buttons to avoid overlapping appearances.
		InvalidateButtons();
	}
}


LRESULT CFSEAppLauncherWindowsDlg::HitTestNCA(WPARAM wParam, LPARAM lParam) {
	// Get the point coordinates for the hit test.
	CPoint ptMouse = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

	// Get the window rectangle.
	CRect rcWindow;
	GetWindowRect(&rcWindow);

	// Get the frame rectangle, adjusted for the style without a caption.
	CRect rcFrame = {0};
	AdjustWindowRectEx(&rcFrame, WS_OVERLAPPEDWINDOW & ~WS_CAPTION, FALSE, NULL);

	// Determine if the hit test is for resizing. Default middle (1,1).
	USHORT uRow = 1;
	USHORT uCol = 1;
	BOOL fOnResizeBorder = FALSE;

	// Determine if the point is at the top or bottom of the window.
	if (ptMouse.y >= rcWindow.top &&
	     ptMouse.y < rcWindow.top + NewMarginForDpi(MarginOrientation::Top)) {
		fOnResizeBorder = (ptMouse.y < (rcWindow.top - rcFrame.top));
		uRow = 0;
	} else if (ptMouse.y < rcWindow.bottom &&
	            ptMouse.y >= rcWindow.bottom -
	                          NewMarginForDpi(MarginOrientation::Bottom)) {
		uRow = 2;
	}

	// Determine if the point is at the left or right of the window.
	if (ptMouse.x >= rcWindow.left &&
	     ptMouse.x < rcWindow.left + NewMarginForDpi(MarginOrientation::Left)) {
		uCol = 0;  // left side
	} else if (ptMouse.x < rcWindow.right &&
	            ptMouse.x >= rcWindow.right -
	                          NewMarginForDpi(MarginOrientation::Right)) {
		uCol = 2;  // right side
	}

	// Hit test (HTTOPLEFT, ... HTBOTTOMRIGHT)
	LRESULT hitTests[3][3] = {
		{HTTOPLEFT, fOnResizeBorder ? HTTOP : HTCAPTION, HTTOPRIGHT},
		{HTLEFT, HTNOWHERE, HTRIGHT},
		{HTBOTTOMLEFT, HTBOTTOM, HTBOTTOMRIGHT},
	};

	return hitTests[uRow][uCol];
}


// Update the font info of the icons.
VOID CFSEAppLauncherWindowsDlg::UpdateIconFont() {
	if (m_fntIcon.GetSafeHandle()) {
		m_fntIcon.DeleteObject();
	}

	int iDpi = GetDpiForWindow(GetSafeHwnd());
	LOGFONT lf = {};
	lf.lfHeight = -MulDiv(CLauncherButton::m_buttonIconSize, iDpi,
	                      USER_DEFAULT_SCREEN_DPI);
	lf.lfWeight = 550;
	_tcscpy_s(lf.lfFaceName, LF_FACESIZE, _T("Segoe Fluent Icons"));
	BOOL bCreated = m_fntIcon.CreateFontIndirect(&lf);

	if (!bCreated) {
		AfxMessageBox(_T("Error: App Launcher only supports Windows 11 full edition."));
		EndDialog(IDCLOSE);
	}
}


PointSize CFSEAppLauncherWindowsDlg::NewButtonPointSizeForDpi(INT index) {
	PointSize btnPointSize;

	int iDpi = GetDpiForWindow(GetSafeHwnd());

	// Button size + 2 px padding.
	btnPointSize.size.cx = MulDiv(m_btnSize.cx, iDpi, USER_DEFAULT_SCREEN_DPI) + 2;
	btnPointSize.size.cy = MulDiv(m_btnSize.cy, iDpi, USER_DEFAULT_SCREEN_DPI) + 2;

	// Margin - 2 px padding.
	int margin = MulDiv(m_btnMargin, iDpi, USER_DEFAULT_SCREEN_DPI) - 2;

	// Right margin of the button set: Right padding of the dialog - 1 px padding.
	int rightMargin = NewMarginForDpi(MarginOrientation::Right) - 1;
	// Top margin of the button set: Top margin of the title - 1 px padding.
	int topMargin = MulDiv(m_titlePaddingTop, iDpi, USER_DEFAULT_SCREEN_DPI) - 1;

	CRect clientRect;
	GetClientRect(&clientRect);

	// right-to-left layout
	btnPointSize.point.x = clientRect.right - rightMargin - btnPointSize.size.cx -
	                       (margin + btnPointSize.size.cx) * index;
	btnPointSize.point.y = clientRect.top + topMargin;

	return btnPointSize;
}


// Create the buttons.
VOID CFSEAppLauncherWindowsDlg::CreateButtons() {
	// Destroy old buttons.
	for (CLauncherButton* btn : m_buttons) {
		if (btn) {
			btn->DestroyWindow();
		}
	}
	m_buttons.clear();

	// Create new buttons.
	for (INT i = 0; i < CLauncherButton::NUM_BUTTONS; ++i) {
		CLauncherButton* pBtn =
			new CLauncherButton(CLauncherButton::g_ButtonInfos[i]);
		PointSize btnPointSize = NewButtonPointSizeForDpi(i);

		if (!pBtn->Create(CLauncherButton::g_ButtonInfos[i].iconChar,
		                  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		                  CRect(btnPointSize.point.x,
		                        btnPointSize.point.y,
		                        btnPointSize.point.x + btnPointSize.size.cx,
		                        btnPointSize.point.y + btnPointSize.size.cy),
		                  this,
		                  IDC_LAUNCHER_BTN_BASE + i)) {
			delete pBtn;
			continue;
		}
		// Set the tooltip.
		pBtn->SetTooltip(CLauncherButton::g_ButtonInfos[i].tooltip);
		// Set the font of icons.
		pBtn->SetFont(&m_fntIcon);
		// Save the pointer.
		m_buttons.push_back(pBtn);
	}
}


VOID CFSEAppLauncherWindowsDlg::UpdateButtonLayout() {
	if (m_buttons.empty()) {
		return;
	}

	INT numExistingButtons = static_cast<INT>(m_buttons.size());
	for (INT i = 0; i < numExistingButtons; ++i) {
		CLauncherButton* pBtn = m_buttons[i];
		PointSize btnPointSize = NewButtonPointSizeForDpi(i);

		// Place the buttons at the top of the Z-order.
		pBtn->SetWindowPos(&wndTop,
		                   btnPointSize.point.x,
		                   btnPointSize.point.y,
		                   btnPointSize.size.cx,
		                   btnPointSize.size.cy,
		                   SWP_NOACTIVATE);
	}
}


VOID CFSEAppLauncherWindowsDlg::InvalidateButtons() {
	for (CLauncherButton* btn : m_buttons) {
		if (btn) {
			btn->Invalidate();
		}
	}
}


BOOL CFSEAppLauncherWindowsDlg::CreateExplorerBrowser() {
	HRESULT hr = CoCreateInstance(CLSID_ExplorerBrowser,
	                              nullptr,
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
	                                    NewExplorerBrowserRectForDpi(),
	                                    &fs);

	if (SUCCEEDED(hr)) {
		LPITEMIDLIST pidl = nullptr;
		// virtual folder "Applications"
		if (SUCCEEDED(
						SHParseDisplayName(applicationFolderPath,
						                   nullptr,
						                   &pidl,
						                   0,
						                   nullptr))) {
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


VOID CFSEAppLauncherWindowsDlg::DestroyExplorerBrowser() {
	if (m_pExplorerBrowser) {
		m_pExplorerBrowser->Destroy();
		m_pExplorerBrowser->Release();
		m_pExplorerBrowser = nullptr;
	}
}


// Extend the frame into the client area.
HRESULT CFSEAppLauncherWindowsDlg::ExtendFrameIntoClientArea() {
	MARGINS margins = {NewMarginForDpi(MarginOrientation::Left),
	                   NewMarginForDpi(MarginOrientation::Right),
	                   NewMarginForDpi(MarginOrientation::Top),
	                   NewMarginForDpi(MarginOrientation::Bottom)};

	return DwmExtendFrameIntoClientArea(GetSafeHwnd(), &margins);
}


INT CFSEAppLauncherWindowsDlg::NewMarginForDpi(MarginOrientation marginOrientation) const {
	int iDpi = GetDpiForWindow(GetSafeHwnd());

	switch (marginOrientation) {
	// SM_CX(Y)FIXEDFRAME: constant value (3)
	// SM_CXPADDEDBORDER: padded border width (default: 4)
	case MarginOrientation::Left:
		return GetSystemMetricsForDpi(SM_CXFIXEDFRAME, iDpi) +
		       GetSystemMetricsForDpi(SM_CXPADDEDBORDER, iDpi) +
		       MulDiv(m_ncPaddingNormal,
		              iDpi,
		              USER_DEFAULT_SCREEN_DPI);

	case MarginOrientation::Top:
		return GetSystemMetricsForDpi(SM_CYFIXEDFRAME, iDpi) +
		       GetSystemMetricsForDpi(SM_CXPADDEDBORDER, iDpi) +
		       MulDiv(m_ncPaddingNormal + m_ncPaddingTopIncrement,
		              iDpi,
		              USER_DEFAULT_SCREEN_DPI);

	case MarginOrientation::Right:
		return GetSystemMetricsForDpi(SM_CXFIXEDFRAME, iDpi) +
		       GetSystemMetricsForDpi(SM_CXPADDEDBORDER, iDpi) +
		       MulDiv(m_ncPaddingNormal,
		              iDpi,
		              USER_DEFAULT_SCREEN_DPI);

	case MarginOrientation::Bottom:
		return GetSystemMetricsForDpi(SM_CYFIXEDFRAME, iDpi) +
		       GetSystemMetricsForDpi(SM_CXPADDEDBORDER, iDpi) +
		       MulDiv(m_ncPaddingNormal,
		              iDpi,
		              USER_DEFAULT_SCREEN_DPI);

	default:
		AfxMessageBox(_T("Error: Invalid parameter."));
	}

	return 0;
}


CRect CFSEAppLauncherWindowsDlg::NewExplorerBrowserRectForDpi() {
	CRect rect;

	// Get the display area.
	GetClientRect(&rect);

	// Adjust the area to allow for some margin.
	INT ncPaddingLeft = NewMarginForDpi(MarginOrientation::Left);
	INT ncPaddingTop = NewMarginForDpi(MarginOrientation::Top);
	INT ncPaddingRight = NewMarginForDpi(MarginOrientation::Right);
	INT ncPaddingBottom = NewMarginForDpi(MarginOrientation::Bottom);
	rect.DeflateRect((ncPaddingLeft + ncPaddingRight) / 2,
	                 (ncPaddingTop + ncPaddingBottom) / 2);
	rect.OffsetRect((ncPaddingLeft - ncPaddingRight) / 2,
	                (ncPaddingTop - ncPaddingBottom) / 2);

	return rect;
}


// Paint the title on the custom frame.
VOID CFSEAppLauncherWindowsDlg::PaintTitle(CPaintDC* pDC) {
	CRect rcClient;
	GetClientRect(&rcClient);

	HTHEME hTheme = OpenThemeData(nullptr, _T("CompositedWindow::Window"));
	if (!hTheme) {
		return;
	}

	CDC memDC;
	if (memDC.CreateCompatibleDC(pDC)) {
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

		HBITMAP hBm = CreateDIBSection(pDC->m_hDC,
		                               &dib,
		                               DIB_RGB_COLORS,
		                               nullptr,
		                               nullptr,
		                               0);
		if (hBm) {
			HGDIOBJ hBmOld = memDC.SelectObject(hBm);
			int iDpi = GetDpiForWindow(GetSafeHwnd());

			// Setup the theme drawing options.
			DTTOPTS DttOpts = {sizeof(DTTOPTS)};
			DttOpts.dwFlags = DTT_COMPOSITED | DTT_GLOWSIZE | DTT_TEXTCOLOR;
			DttOpts.iGlowSize = 15;
			DttOpts.crText = IsDarkMode() ? RGB(255, 255, 255) : RGB(26, 26, 26);

			// Select a font.
			CFont font;
			CFont* pFontOld = nullptr;
			LOGFONT lgFont;
			if (SUCCEEDED(GetThemeSysFont(hTheme, TMT_CAPTIONFONT, &lgFont))) {
				int nHeight = -MulDiv(28, iDpi, USER_DEFAULT_SCREEN_DPI);
				lgFont.lfHeight = nHeight;
				lgFont.lfWeight = FW_SEMIBOLD;
				_tcscpy_s(lgFont.lfFaceName, LF_FACESIZE, _T("Segoe UI Variable Display"));
				if (font.CreateFontIndirect(&lgFont)) {
					pFontOld = memDC.SelectObject(&font);
				}
			}

			// Draw the title.
			CRect rcPaint = rcClient;
			rcPaint.left += NewMarginForDpi(MarginOrientation::Left);
			rcPaint.top += MulDiv(m_titlePaddingTop, iDpi, USER_DEFAULT_SCREEN_DPI);
			rcPaint.right -= NewMarginForDpi(MarginOrientation::Right);
			rcPaint.bottom = rcPaint.top +
			                 MulDiv(m_titleRectHeight, iDpi, USER_DEFAULT_SCREEN_DPI);
			DrawThemeTextEx(hTheme,
			                memDC.m_hDC,
			                0,
			                0,
			                _T("Apps"),
			                -1,
			                DT_END_ELLIPSIS | DT_LEFT | DT_NOPREFIX |
			                DT_SINGLELINE | DT_VCENTER,
			                &rcPaint,
			                &DttOpts);

			// Blit text to the frame.
			pDC->BitBlt(0, 0, cx, cy, &memDC, 0, 0, SRCCOPY);

			memDC.SelectObject(hBmOld);
			if (pFontOld) {
				memDC.SelectObject(pFontOld);
			}
			DeleteObject(hBm);
		}
	}
	CloseThemeData(hTheme);
}


VOID CFSEAppLauncherWindowsDlg::SetGroupingByName() {
	if (!m_pExplorerBrowser) {
		return;
	}

	// Get the current view.
	IFolderView2* pFolderView2 = nullptr;

	if (SUCCEEDED(m_pExplorerBrowser->GetCurrentView(
			IID_PPV_ARGS(&pFolderView2))) && pFolderView2) {
		// Set to group by name. TRUE indicates ascending.
		pFolderView2->SetGroupBy(PKEY_ItemNameDisplay, TRUE);

		pFolderView2->Release();
	}
}
