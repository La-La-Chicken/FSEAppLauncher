#pragma once
#include <afxdialogex.h>



class CBaseDialog : public CDialogEx {
public:
	using CDialogEx::CDialogEx;

protected:
	afx_msg LRESULT OnNcCalcSizeMessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};
