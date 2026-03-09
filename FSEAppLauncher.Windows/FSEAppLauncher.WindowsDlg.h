
// FSEAppLauncher.WindowsDlg.h: header file
//

#pragma once


#include "BaseDialog.h"
#include "AboutDlg.h"
#include "LauncherButton.h"



enum class MarginOrientation { Left, Top, Right, Bottom };


struct PointSize {
	CPoint point;
	CSize size;
};



// CFSEAppLauncherWindowsDlg dialog
class CFSEAppLauncherWindowsDlg : public CBaseDialog {
private:
	static CONST INT m_ncPaddingNormal;
	static CONST INT m_ncPaddingTopIncrement;
	static CONST INT m_titlePaddingTop;
	static CONST INT m_titleRectHeight;
	static CONST CSize m_btnSize;
	static CONST INT m_btnMargin;

	static CONST PCTSTR applicationFolderPath;

	CFont m_fntIcon;                          // the font for icons

	IExplorerBrowser* m_pExplorerBrowser;

	std::vector<CLauncherButton*> m_buttons;  // button array

	// Hit test the frame for resizing and moving.
	LRESULT HitTestNCA(WPARAM wParam, LPARAM lParam);

	// update the font for icons (when DPI is changed)
	VOID UpdateIconFont();
	// generate the top-left point and size of the button
	PointSize NewButtonPointSizeForDpi(INT index);
	// create the buttons
	VOID CreateButtons();
	// update the button layout (when DPI is changed)
	VOID UpdateButtonLayout();
	// invalidate and redraw the buttons
	VOID InvalidateButtons();

	BOOL CreateExplorerBrowser();
	VOID DestroyExplorerBrowser();

	HRESULT ExtendFrameIntoClientArea();
	INT NewMarginForDpi(MarginOrientation marginOrientation) const;
	CRect NewExplorerBrowserRectForDpi();
	VOID PaintTitle(CPaintDC* pDC);
	VOID SetGroupingByName();


// Construction
public:
	CFSEAppLauncherWindowsDlg(CWnd* pParent = nullptr);  // standard constructor
	virtual ~CFSEAppLauncherWindowsDlg();                // destructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FSEAPPLAUNCHERWINDOWS_DIALOG };
#endif


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
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
};
