
// FSEAppLauncher.Windows.h: main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CFSEAppLauncherApp:
// See FSEAppLauncher.Windows.cpp for the implementation of this class
//

class CFSEAppLauncherApp : public CWinApp {
private:
	ULONG_PTR gdiplusToken;


public:
	CFSEAppLauncherApp();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CFSEAppLauncherApp theApp;
