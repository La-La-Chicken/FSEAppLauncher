#pragma once


#include "FSEAppLauncher.Windows.h"
#include <afxbutton.h>


// 按钮启动类型
enum class LaunchType { Exe, KeyCombination, Uri };

// 按钮信息结构体
struct ButtonInfo {
	// Segoe Fluent Icons character
	const TCHAR*     iconChar;
	// tooltip
	const CString    tooltip;
	// launch type
	const LaunchType type;
	// target identifier (directory/URI/key combination)
	const CString    target;
	// error message to show when failed to launch
	const TCHAR*     errorMessage;
};

// Declare the array. See .cpp for the definition.
extern const ButtonInfo g_ButtonInfos[];
extern const int NUM_BUTTONS;



class CLauncherButton : public CMFCButton {
	DECLARE_DYNAMIC(CLauncherButton)   // Add the runtime class declaration.


public:
	CLauncherButton(const ButtonInfo& info);

	// Execute operations.
	void Launch();


protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()


private:
	// button info
	ButtonInfo m_info;

	// the Steam logo
	CPngImage m_imgSteam;
	// Indicates whether it is a Steam button.
	BOOL m_bIsSteam = FALSE;
	// helper function for loading icons
	BOOL LoadSteamImage();
	// helper function for drawing icons
	void DrawSteamImage(CDC* pDC, const CRect& rect);

	BOOL IsFileExists(const CString& fileName);
	void SendKeyCombination(const CString& keyCombination);
};
