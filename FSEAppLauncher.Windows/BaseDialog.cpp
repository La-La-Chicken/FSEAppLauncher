#include "pch.h"
#include "BaseDialog.h"



typedef BOOL(WINAPI* AllowDarkModeForWindowProc)(HWND, BOOL);
typedef void(WINAPI* RefreshImmersiveColorPolicyStateProc)();
typedef DWORD(WINAPI* SetPreferredAppModeProc)(DWORD);

typedef BOOL(WINAPI* ShouldAppsUseDarkModeProc)();
static ShouldAppsUseDarkModeProc g_pShouldAppsUseDarkMode = nullptr;

enum PreferredAppMode { Default, AllowDark, ForceDark, ForceLight };



void ApplyDarkModeSettings(HWND hWnd) {
	HMODULE hUxtheme = LoadLibraryEx(_T("uxtheme.dll"),
	                                 nullptr,
	                                 LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (!hUxtheme) {
		return;
	}

	// Windows 10 1903 and above only.
	auto SetPreferredAppMode = (SetPreferredAppModeProc)GetProcAddress(
			hUxtheme, MAKEINTRESOURCEA(135));
	if (SetPreferredAppMode) {
		SetPreferredAppMode(AllowDark);
	}

	// Refresh the color policy state and allow dark mode for the window.
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


BOOL IsDarkMode() {
	// Get the function address dynamically on the first call.
	if (!g_pShouldAppsUseDarkMode) {
		HMODULE hUxtheme =
				LoadLibraryEx(_T("uxtheme.dll"), nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
		if (hUxtheme) {
			g_pShouldAppsUseDarkMode = (ShouldAppsUseDarkModeProc)GetProcAddress(
					hUxtheme, MAKEINTRESOURCEA(132));	// 132 is ShouldAppsUseDarkMode
			FreeLibrary(hUxtheme);
		}
	}

	return g_pShouldAppsUseDarkMode ? g_pShouldAppsUseDarkMode() : FALSE;
}



BEGIN_MESSAGE_MAP(CBaseDialog, CDialogEx)
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
	ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()


int CBaseDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	int result = CDialogEx::OnCreate(lpCreateStruct);
	if (result == -1) {
		return -1;
	}

	ApplyDarkModeSettings(GetSafeHwnd());

	BOOL bTRUE = TRUE;
	BOOL bDarkMode = IsDarkMode();
	COLORREF rgb = bDarkMode ? RGB(32, 32, 32) : RGB(243, 243, 243);
	INT nValue = 2;	// = DWMWCP_ROUND, DWMSBT_MAINWINDOW

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
	SetWindowPos(nullptr, rect.left, rect.top,
	             rect.Width(), rect.Height(),
	             SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);

	// Returning 0 indicates success, and -1 indicates
	//  failure destroys the window.
	return result;
}


HBRUSH CBaseDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	BOOL bDarkMode = IsDarkMode();

	if (nCtlColor == CTLCOLOR_STATIC) {
		// Solid Background / Base
		pDC->SetBkColor(bDarkMode ? RGB(32, 32, 32) : RGB(243, 243, 243));
		// Text / Primary
		pDC->SetTextColor(bDarkMode ? RGB(255, 255, 255) : RGB(26, 26, 26));
	}

	// Solid Background / Base
	return CreateSolidBrush(bDarkMode ? RGB(32, 32, 32) : RGB(243, 243, 243));
}


void CBaseDialog::OnSettingChange(UINT uFlags, LPCTSTR lpszSection) {
	CDialogEx::OnSettingChange(uFlags, lpszSection);

	// Check if it is not a change in the system's Immersive Color Set.
	if (!lpszSection || _tcsicmp(lpszSection, _T("ImmersiveColorSet"))) {
		return;
	}

	// Update the dark mode of the title bar.
	BOOL bDarkMode = IsDarkMode();
	COLORREF rgbBackground = bDarkMode ? RGB(32, 32, 32) : RGB(243, 243, 243);

	DwmSetWindowAttribute(GetSafeHwnd(),
	                      DWMWA_USE_IMMERSIVE_DARK_MODE,
	                      &bDarkMode,
	                      sizeof(bDarkMode));
	// The text is the same color as the background to make it visually invisible.
	DwmSetWindowAttribute(GetSafeHwnd(),
	                      DWMWA_TEXT_COLOR,
	                      &rgbBackground,
	                      sizeof(rgbBackground));

	// Redraw the window when not minimized.
	RedrawWindow(nullptr, nullptr,
	             RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
}
