#include "pch.h"
#include "AboutDlg.h"



CAboutDlg::CAboutDlg() : CBaseDialog(IDD_ABOUTBOX) {}



BOOL CAboutDlg::OnInitDialog() {
	BOOL bResult = CBaseDialog::OnInitDialog();

	CMenu* pMenu = GetSystemMenu(FALSE);
	pMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
	pMenu->EnableMenuItem(SC_MOVE, MF_BYCOMMAND | MF_GRAYED);

	return bResult;
}


BEGIN_MESSAGE_MAP(CAboutDlg, CBaseDialog)
END_MESSAGE_MAP()
