
// OTA_Tools.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// COTA_ToolsApp:
// See OTA_Tools.cpp for the implementation of this class
//

class COTA_ToolsApp : public CWinAppEx
{
public:
	COTA_ToolsApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern COTA_ToolsApp theApp;