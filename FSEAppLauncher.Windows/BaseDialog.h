#pragma once


#include "framework.h"
#include "FSEAppLauncher.Windows.h"
#include <afxdialogex.h>



void ApplyDarkModeSettings(HWND hWnd);
BOOL IsDarkMode();



class CBaseDialog : public CDialogEx {
public:
	using CDialogEx::CDialogEx;

protected:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnNcCalcSizeMessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};
