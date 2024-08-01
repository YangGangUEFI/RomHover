// dllmain.cpp : Implementation of DllMain.

#include "pch.h"
#include "framework.h"
#include "resource.h"
#include "RomHover_i.h"
#include "dllmain.h"

CRomHoverModule _AtlModule;

class CRomHoverApp : public CWinApp
{
public:

// Overrides
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CRomHoverApp, CWinApp)
END_MESSAGE_MAP()

CRomHoverApp theApp;

BOOL CRomHoverApp::InitInstance()
{
	return CWinApp::InitInstance();
}

int CRomHoverApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
