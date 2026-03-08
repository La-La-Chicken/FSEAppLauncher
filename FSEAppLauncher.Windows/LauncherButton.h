#pragma once


#include "FSEAppLauncher.Windows.h"
#include <afxbutton.h>


// button launch type
enum class LaunchType { Exe, KeyCombination, Uri };

struct ButtonInfo {
	// custom PNG icon index, empty indicates no custom icon
	      PCTSTR     iconPngResource;
	// Segoe Fluent Icons character
	      PCTSTR     iconChar;
	// tooltip
	      PCTSTR     tooltip;
	// launch type
	const LaunchType type;
	// target identifier (directory/URI/key combination)
	const CString    target;
	// error message to show when failed to launch
	      PCTSTR     errorMessage;
};



class CLauncherButton : public CMFCButton {
	DECLARE_DYNAMIC(CLauncherButton)   // Add the runtime class declaration.


public:
	static CONST INT m_buttonIconSize;

 // Declare the array. See .cpp for the definition.
	static CONST TCHAR keyCombinationForNotifications[];
	static CONST TCHAR keyCombinationForQuickSettings[];
	static CONST ButtonInfo g_ButtonInfos[];
	static CONST INT NUM_BUTTONS;

	CLauncherButton(const ButtonInfo& info);

	// Execute operations.
	VOID Launch();


protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()


private:
	// button info
	ButtonInfo m_info;

	// the icon
	CPngImage m_iconPng;
	// helper function for loading icons
	VOID LoadIconPng();
	// helper function for drawing icons
	VOID DrawIconPng(CDC* pDC, const CRect& rect);

	BOOL IsFileExists(const CString& fileName);
	VOID SendKeyCombination(const CString& keyCombination);
};
