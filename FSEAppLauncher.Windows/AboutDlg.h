#pragma once


#include "BaseDialog.h"



class CAboutDlg : public CBaseDialog {
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif


// Implementation
protected:
	virtual BOOL OnInitDialog() override;
	DECLARE_MESSAGE_MAP()
};
