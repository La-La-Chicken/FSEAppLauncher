#pragma once


#include "framework.h"
#include "FSEAppLauncher.Windows.h"
#include <afxdialogex.h>


#ifndef DWMWA_USE_HOSTBACKDROPBRUSH
#define DWMWA_USE_HOSTBACKDROPBRUSH 17
#endif

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

#ifndef DWMWA_TEXT_COLOR
#define DWMWA_TEXT_COLOR 36
#endif

#ifndef DWMWA_SYSTEMBACKDROP_TYPE
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif



void ApplyDarkModeSettings(HWND hWnd);
BOOL IsDarkMode();



class CBaseDialog : public CDialogEx {
public:
	using CDialogEx::CDialogEx;


protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnNcCalcSizeMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()
};
