
// FSEAppLauncher.WindowsDlg.h: header file
//

#pragma once


#include "BaseDialog.h"
#include "AboutDlg.h"
#include "LauncherButton.h"



enum class MarginOrientation { Left, Top, Right, Bottom };



// CFSEAppLauncherWindowsDlg dialog
class CFSEAppLauncherWindowsDlg : public CBaseDialog {
private:
	const INT m_ncPaddingNormal = 48;
	const INT m_ncPaddingTopIncrement = 64;
	const INT m_titlePaddingTop = 46;
	const INT m_titleRectHeight = 40;

	CFont m_fntIcon;                          // the font for icons

	IExplorerBrowser* m_pExplorerBrowser = NULL;

	std::vector<CLauncherButton*> m_buttons;  // button array

	void UpdateIconFont();  // update the font for icons (when DPI is changed)

	void CreateButtons();                     // create the buttons
	void UpdateButtonLayout();                // update the button layout (when DPI is changed)

	BOOL CreateExplorerBrowser();
	void DestroyExplorerBrowser();
	HRESULT ExtendFrameIntoClientArea();
	INT GetCalculatedMarginForDpi(MarginOrientation marginOrientation) const;
	CRect NewRectForExplorerBrowser();
	void PaintTitle(CPaintDC* pDC);
	void SetGroupingByName();

// Construction
public:
	CFSEAppLauncherWindowsDlg(CWnd* pParent = nullptr);  // standard constructor
	virtual ~CFSEAppLauncherWindowsDlg();                // 析构函数

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FSEAPPLAUNCHERWINDOWS_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnDestroy();
	virtual void OnOK();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam,
	                           LPARAM lParam) override;
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg LRESULT OnDpiChangedMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
};
