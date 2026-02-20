
// FSEAppLauncher.WindowsDlg.h: 头文件
//

#pragma once


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

typedef BOOL(WINAPI* AllowDarkModeForWindowProc)(HWND, BOOL);
typedef void(WINAPI* RefreshImmersiveColorPolicyStateProc)();
typedef BOOL(WINAPI* AllowDarkModeForAppProc)(BOOL);
typedef DWORD(WINAPI* SetPreferredAppModeProc)(DWORD);

typedef BOOL(WINAPI* ShouldAppsUseDarkModeProc)();
static ShouldAppsUseDarkModeProc g_pShouldAppsUseDarkMode = nullptr;

enum PreferredAppMode { Default, AllowDark, ForceDark, ForceLight };

enum MarginOrientation { Left, Top, Right, Bottom, TitleTop, TitleHeight };



// CFSEAppLauncherWindowsDlg 对话框
class CFSEAppLauncherWindowsDlg : public CDialogEx {
private:
	const INT m_ncPaddingNormal = 48;
	const INT m_ncPaddingTopIncrement = 64;
	const INT m_titlePaddingTop = 46;
	const INT m_titleRectHeight = 40;
	IExplorerBrowser* m_pExplorerBrowser = NULL;
	void ApplyDarkModeSettings(HWND hWnd);
 BOOL CheckActiveWindow();
	BOOL CreateExplorerBrowser();
	void DestroyExplorerBrowser();
	HRESULT ExtendFrameIntoClientArea();
	INT GetCalculatedMarginForDpi(INT marginOrientation) const;
	CRect NewRectForExplorerBrowser();
	BOOL IsDarkMode() const;
	void PaintTitle();
	void SetGroupingByName();

// 构造
public:
	CFSEAppLauncherWindowsDlg(CWnd* pParent = nullptr);	// 标准构造函数
	virtual ~CFSEAppLauncherWindowsDlg();	// 添加析构函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FSEAPPLAUNCHERWINDOWS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnDestroy();
	virtual void OnOK();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnDpiChangedMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMove(int x, int y);
	afx_msg LRESULT OnNcCalcSizeMessage(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
};
