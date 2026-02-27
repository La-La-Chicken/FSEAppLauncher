#pragma once


#include <afxbutton.h>


// 按钮启动类型
enum class LaunchType { Browser, Exe, Folder, Uri };

// 按钮信息结构体
struct ButtonInfo {
	const wchar_t* iconChar;  // Segoe Fluent Icons 字符
	const wchar_t* tooltip;   // 工具提示
	LaunchType     type;      // 启动类型
	const wchar_t* target;    // 目标标识符 (AUMID/CLSID/路径/URI)
};

// 声明数组（定义在 .cpp 中）
extern const ButtonInfo g_ButtonInfos[];
extern const int NUM_BUTTONS;



class CLauncherButton : public CMFCButton {
	DECLARE_DYNAMIC(CLauncherButton)   // 添加运行时类声明


public:
	CLauncherButton(const ButtonInfo& info/*, BOOL bDarkMode*/);

	// 设置 DPI 相关尺寸（由对话框在 DPI 变化时调用）
	void SetDpi(int iDpi);
	// 执行启动操作
	void Launch();


protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()


private:
	ButtonInfo m_info;                     // 按钮信息
	int m_iDpi = USER_DEFAULT_SCREEN_DPI;  // 当前 DPI

	BOOL IsFileExists(const CString& fileName);
	BOOL IsProtocolAssociated(const CString& protocol);
};
