
// FSEAppLauncher.Windows.h: PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// 主符号


// CFSEAppLauncherApp:
// 有关此类的实现，请参阅 FSEAppLauncher.Windows.cpp
//

class CFSEAppLauncherApp : public CWinApp {
public:
	CFSEAppLauncherApp();

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CFSEAppLauncherApp theApp;
