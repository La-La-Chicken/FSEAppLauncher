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
	                                 NULL,
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
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_NCCALCSIZE, &CBaseDialog::OnNcCalcSizeMessage)
END_MESSAGE_MAP()


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


LRESULT CBaseDialog::OnNcCalcSizeMessage(WPARAM wParam, LPARAM lParam) {
	if (wParam == TRUE) {	// 当 wParam 为 TRUE 时，表示需要计算客户区大小
		return 0;	// 返回 0 表示整个窗口区域都将作为客户区（即移除标准框架）
	}

	return Default();	// 否则调用默认处理（例如窗口最小化/最大化时的计算）
}
