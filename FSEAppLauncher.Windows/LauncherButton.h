#pragma once


#include <afxbutton.h>


// 按钮启动类型
enum class LaunchType { Exe, KeyCombination, Uri };

// 按钮信息结构体
struct ButtonInfo {
	const TCHAR* iconChar;       // Segoe Fluent Icons character
	const TCHAR* tooltip;        // tooltip
	LaunchType    type;          // launch type
	const CString target;        // target identifier (directory/URI/key combination)
	const CString errorMessage;  // error message to show when failed to launch
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
	ButtonInfo m_info;                 // button info

	BOOL IsFileExists(const CString& fileName);
	void SendKeyCombination(const CString& keyCombination);
};
