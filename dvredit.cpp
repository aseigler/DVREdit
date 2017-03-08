// dvredit.cpp : Defines the class behaviors for the application.
//
//***********************************************************************
// Does nothing but open dialog for main part of app
//***********************************************************************
#include "stdafx.h"
#include "dvredit.h"
#include "DVREditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDVREditApp

BEGIN_MESSAGE_MAP(CDVREditApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDVREditApp construction

CDVREditApp::CDVREditApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CDVREditApp object

CDVREditApp theApp;


// CDVREditApp initialization

BOOL CDVREditApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	//SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CDVREditDlg dlg;
	m_pMainWnd = &dlg;

	dlg.fCommandLine = FALSE;

	CString sz = m_lpCmdLine;
	sz.TrimLeft();
	sz.TrimRight();
	
	if (!sz.IsEmpty())
	{
		if (wcsstr((LPCWSTR) sz, L".dvr"))
		{
			HANDLE              hFindFile;
			WIN32_FIND_DATA     findData;

			sz.TrimLeft(L"\"");
			sz.TrimRight(L"\"");
			hFindFile =  FindFirstFile((LPCTSTR)sz, &findData);
                
            if ( hFindFile != INVALID_HANDLE_VALUE )
            {
				dlg.m_szSourceFile = new OLECHAR[MAX_PATH];
				if (dlg.m_szSourceFile == NULL)
				{
					MessageBox(GetDesktopWindow(), L"Out of memory creating source file string.", L"Out of memory", NULL);
					return FALSE;
				}
				
				if (SUCCEEDED(StringCchCopy(dlg.m_szSourceFile, MAX_PATH, (LPCWSTR) sz)))
					dlg.fCommandLine = TRUE;
				
				CloseHandle(hFindFile);
            }
		}
	}

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
