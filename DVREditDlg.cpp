// DVREditDlg.cpp : implementation file
//
//****************************************************************************************
// This is the core file for the editing portion of the app.  Comments are inline in this
// code file.  DVREditDlg.h is the header for this file
//****************************************************************************************
#include "stdafx.h"
#include "dvredit.h"
#include "DVREditDlg.h"
#include <windows.h>
#include "filever.h"
#include ".\dvreditdlg.h"
#include "hyperlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable:4244 4005)
/*
/   DisplayError takes window handle, string, and hresult, puts the hresult into the 
/	string, then displays the error in a message box.
*/
int DisplayError(HWND hWnd, wchar_t* pszMessage, HRESULT hr)
{
	wchar_t szMessage[MAX_PATH] = L"\0";
	
	if (SUCCEEDED(StringCchPrintf(szMessage, MAX_PATH, pszMessage, hr)))
		return MessageBoxEx(hWnd, szMessage, L"Error!", MB_OK | MB_ICONERROR, NULL);
	
	else return 0;
}
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg(CWnd* pParent = NULL);
// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	CHyperLink m_Email;
	CHyperLink m_TGB;

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedStaticMailto();
};

CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/) : CDialog(CAboutDlg::IDD, pParent)
{
}


// Sets up the version info on the help->about window
BOOL CAboutDlg::OnInitDialog(void)
{
	CString csVersion;
	csVersion.Format(L"Version %d.%d.%d.%d", rmj, rmn, rmm, rup);
	SetDlgItemText(IDC_VERSION, (LPCWSTR) csVersion);
	CString csEmail = L"mailto:alexseigler@hotmail.com";
	CString csTGB = L"http://www.thegreenbutton.com";
	m_Email.SetURL(csEmail);
	m_TGB.SetURL(csTGB);
	m_Email.SetUnderline(TRUE);
	m_TGB.SetUnderline(TRUE);
	
	return CDialog::OnInitDialog();
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_STATIC_MAILTO, m_Email);
	DDX_Control(pDX, IDC_STATIC_TGB, m_TGB);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CDVREditDlg dialog


// Sets colors, icon, keyboard shortcuts, etc.
CDVREditDlg::CDVREditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDVREditDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pToolTip = NULL;
	CBrush m_brush(RGB(192,192,192));
	m_hAccelTable = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR));
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CDVREditDlg::~CDVREditDlg(void)
{
	if (m_pToolTip)
		delete m_pToolTip;
}

// Maps variables to buttons and stuff like that
void CDVREditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDVREditDlg)
    DDX_Control(pDX, IDC_SLIDER, m_Seekbar);
    DDX_Control(pDX, IDC_BUTTON_STEP_FORWARD, m_ButtonStepForward);
	DDX_Control(pDX, IDC_BUTTON_STEP_BACKWARD, m_ButtonStepBackward);
	DDX_Control(pDX, IDC_BUTTON_PREVIOUS_SEGMENT, m_ButtonSegmentPrev);
	DDX_Control(pDX, IDC_BUTTON_NEXT_SEGMENT, m_ButtonSegmentNext);
	DDX_Control(pDX, IDC_BUTTON_CLEAR_SEGMENT, m_ButtonSegmentClear);
	DDX_Control(pDX, IDC_BUTTON_IN, m_ButtonIn);
	DDX_Control(pDX, IDC_BUTTON_OUT, m_ButtonOut);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_ButtonStop);
    DDX_Control(pDX, IDC_BUTTON_PLAY, m_ButtonPlay);
	DDX_Control(pDX, IDC_BUTTON_COMMERCIAL_SKIP, m_ButtonCommercialSkip);
	DDX_Control(pDX, IDC_BUTTON_SEGMENT_START, m_ButtonSegmentStart);
	DDX_Control(pDX, IDC_BUTTON_SEGMENT_END, m_ButtonSegmentEnd);
	DDX_Control(pDX, IDC_BUTTON_LOAD, m_ButtonLoad);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_ButtonSave);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDVREditDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
    ON_WM_CLOSE()
    ON_WM_DESTROY()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_MESSAGE(WM_SAVETHREAD_FINISHED, OnSaveThreadFinished)
	ON_MESSAGE(WM_DUMPTHREAD_FINISHED, OnDumpThreadFinished)
	ON_MESSAGE(WM_GRAPHNOTIFY, OnGraphNotify)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_COMMAND(ID_FILE_OPEN32771, OnFileOpen)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON_STEP_FORWARD, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON_COMMERCIAL_SKIP, OnBnClickedButtonCommercialSkip)
	ON_BN_CLICKED(IDC_BUTTON_STEP_BACKWARD, OnBnClickedButtonStepBackward)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
	ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
	ON_COMMAND(ID_TOOLS_DVRTOMPG, OnToolsDvr2Mpg)
	ON_COMMAND(ID_TOOLS_MPGTODVR, OnToolsMpg2Dvr)
	ON_BN_CLICKED(IDC_BUTTON_IN, OnBnClickedButtonIn)
	ON_BN_CLICKED(IDC_BUTTON_OUT, OnBnClickedButtonOut)
	ON_BN_CLICKED(IDC_CLEAR, OnBnClickedClear)
	ON_BN_CLICKED(IDC_BUTTON_PREVIOUS_SEGMENT, OnBnClickedButtonPreviousSegment)
	ON_BN_CLICKED(IDC_BUTTON_NEXT_SEGMENT, OnBnClickedButtonNextSegment)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_SEGMENT, OnBnClickedButtonClearSegment)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_SEGMENT_START, OnBnClickedButtonSegmentStart)
	ON_BN_CLICKED(IDC_BUTTON_SEGMENT_END, OnBnClickedButtonSegmentEnd)
	ON_COMMAND(ID_ACCELERATOR_PAUSE, OnAcceleratorPause)
	ON_COMMAND(ID_ACCELERATOR_PLAY, OnAcceleratorPlay)
	ON_COMMAND(ID_ACCELERATOR_RECORD, OnAcceleratorRecord)
	ON_COMMAND(ID_ACCELERATOR_BACK, OnAcceleratorBack)
	ON_COMMAND(ID_ACCELERATOR_FORWARD, OnAcceleratorForward)
	ON_COMMAND(ID_ACCELERATOR_STOP, OnAcceleratorStop)
	ON_COMMAND(ID_ACCELERATOR_IN1, OnAcceleratorIn)
	ON_COMMAND(ID_ACCELERATOR_IN2, OnAcceleratorIn)
	ON_COMMAND(ID_ACCELERATOR_OUT1, OnAcceleratorOut)
	ON_COMMAND(ID_ACCELERATOR_OUT2, OnAcceleratorOut)
	ON_COMMAND(ID_TOOLS_OPENREFERENCERECORDING, OnToolsOpenReferenceRecording)
	ON_COMMAND(ID_TOOLS_APPENDRECORDINGS, OnToolsAppendRecordings)
	ON_COMMAND(ID_TOOLS_DUMPELEMENTARYSTREAMS, OnToolsDumpElementaryStreams)
	ON_COMMAND(ID_TOOLS_DEFAULTDECODERS, OnToolsDefaultDecoders)
	ON_COMMAND(ID_TOOLS_EDITMETADATA, &CDVREditDlg::OnToolsEditmetadata)
END_MESSAGE_MAP()

BOOL CDVREditDlg::fSuppressEditedBy(void)
{
    HKEY              hKeyNames ;
    DWORD             Type;
    DWORD             Status;
	DWORD			  dwSuppress;
	DWORD			  Size = sizeof(dwSuppress);
	
	Status = RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\DVREdit",
                          0L, KEY_READ, &hKeyNames) ;
	if (Status != ERROR_SUCCESS)
		return FALSE;

	Status = RegQueryValueEx(hKeyNames, L"SuppressEditedBy", NULL,
                                 &Type, (LPBYTE) &dwSuppress, &Size) ;

	RegCloseKey (hKeyNames);

	if (Status != ERROR_SUCCESS)
		return FALSE;
	
	return (BOOL) dwSuppress ? TRUE : FALSE;
}

// CDVREditDlg message handlers
// For window resizing

BOOL CDVREditDlg::LoadMainWindowPlacement(void)
{
    WINDOWPLACEMENT   WindowPlacement ;
    TCHAR*            szWindowPlacement = NULL;
    HKEY              hKeyNames ;
    DWORD             Size = 0;
    DWORD             Type;
    DWORD             Status;
    STARTUPINFO       StartupInfo ;

	GetStartupInfo (&StartupInfo) ;

    Status = RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\DVREdit",
                          0L, KEY_READ, &hKeyNames) ;

	if (Status != ERROR_SUCCESS)
		return FALSE;

	Status = RegQueryValueEx(hKeyNames, L"WindowPosition", NULL,
                                 &Type, (LPBYTE)szWindowPlacement, &Size) ;

	if (Status != ERROR_SUCCESS)
		return FALSE;

	szWindowPlacement = (TCHAR*) new BYTE[Size];

	if (szWindowPlacement == NULL)
		return FALSE;


    // get the window placement data

	Status = RegQueryValueEx(hKeyNames, L"WindowPosition", NULL,
                                 &Type, (LPBYTE)szWindowPlacement, &Size) ;
    RegCloseKey (hKeyNames) ;

	szWindowPlacement[Size-1] = 0;

    if (Status == ERROR_SUCCESS) 
	{
		int            iNumScanned ;

        iNumScanned = swscanf (	szWindowPlacement,
			                    TEXT("%d %d %d %d %d %d %d %d %d"),
                                &WindowPlacement.showCmd,
                                &WindowPlacement.ptMinPosition.x,
                                &WindowPlacement.ptMinPosition.y,
                                &WindowPlacement.ptMaxPosition.x,
                                &WindowPlacement.ptMaxPosition.y,
                                &WindowPlacement.rcNormalPosition.left,
                                &WindowPlacement.rcNormalPosition.top,
                                &WindowPlacement.rcNormalPosition.right,
                                &WindowPlacement.rcNormalPosition.bottom) ;


		if (StartupInfo.dwFlags == STARTF_USESHOWWINDOW) 
		{
			WindowPlacement.showCmd = StartupInfo.wShowWindow ;
        }
        
		WindowPlacement.length = sizeof(WINDOWPLACEMENT);
        WindowPlacement.flags = WPF_SETMINPOSITION;
        if (!SetWindowPlacement(&WindowPlacement)) 
		{
			delete[] szWindowPlacement;
			return (FALSE);
        }
	 }

	delete[] szWindowPlacement;

    if (Status != ERROR_SUCCESS) {
        // open registry failed, use Max as default
        //ShowWindow (hWnd, SW_SHOWMAXIMIZED) ;
        return (FALSE) ;
    }
    return TRUE;
}

BOOL CDVREditDlg::SaveMainWindowPlacement(void)
{
	WINDOWPLACEMENT   WindowPlacement ;
	TCHAR             ObjectType [2] ;
	TCHAR*            szWindowPlacement = NULL;
	HKEY              hKeyPosition = 0 ;
	DWORD             Size ;
	DWORD             Status ;
	DWORD             dwDisposition ;

	ObjectType [0] = TEXT(' ') ;
	ObjectType [1] = TEXT('\0') ;

	WindowPlacement.length = sizeof(WINDOWPLACEMENT);
	if (!GetWindowPlacement(&WindowPlacement)) {
		return FALSE;
	}

	szWindowPlacement = new TCHAR[sizeof(WINDOWPLACEMENT)];
	if (szWindowPlacement == NULL)
	{
		MessageBoxEx(m_hWnd, L"Out of memory saving window placement!", NULL, MB_OK | MB_ICONERROR, NULL);
		return FALSE;
	}
    if FAILED(StringCchPrintf(szWindowPlacement, sizeof(WINDOWPLACEMENT),TEXT("%d %d %d %d %d %d %d %d %d"),
              WindowPlacement.showCmd,
              WindowPlacement.ptMinPosition.x,
              WindowPlacement.ptMinPosition.y,
              WindowPlacement.ptMaxPosition.x,
              WindowPlacement.ptMaxPosition.y,
              WindowPlacement.rcNormalPosition.left,
              WindowPlacement.rcNormalPosition.top,
              WindowPlacement.rcNormalPosition.right,
              WindowPlacement.rcNormalPosition.bottom))
	{
		MessageBoxEx(m_hWnd, L"Error saving window placement information!", NULL, MB_OK | MB_ICONERROR, NULL);
		delete[] szWindowPlacement;
		return FALSE;
	}
	

    // try to create it first
    Status = RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\DVREdit", 0L,
                            ObjectType, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | KEY_WRITE,
                            NULL, &hKeyPosition, &dwDisposition) ;

    // if it has been created before, then open it
    if (dwDisposition == REG_OPENED_EXISTING_KEY) {
        Status = RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\DVREdit", 0L,
                              KEY_WRITE, &hKeyPosition) ;
    }

    // we got the handle, now store the window placement data
    if (Status == ERROR_SUCCESS) {
        Size = (lstrlen (szWindowPlacement) + 1) * sizeof (TCHAR) ;

        Status = RegSetValueEx(hKeyPosition, L"WindowPosition", 0,
                               REG_SZ, (LPBYTE)szWindowPlacement, Size) ;

        RegCloseKey(hKeyPosition);

    }

	delete[] szWindowPlacement;
    return (Status == ERROR_SUCCESS);
}

BOOL CDVREditDlg::CheckRegisterDVREdit(void)
{
	HKEY hKeyDVREdit = 0;
	BOOL fCorrect = FALSE;
	DWORD dwStatus;

	dwStatus = RegOpenKeyEx(HKEY_CLASSES_ROOT, L"Applications\\dvredit.exe", 0L, KEY_READ, &hKeyDVREdit);

	if (dwStatus == ERROR_SUCCESS)
	{
		HKEY hKeyCommand = 0;
		dwStatus = RegOpenKeyEx(HKEY_CLASSES_ROOT, L"Applications\\dvredit.exe\\shell\\open\\command", 0L, KEY_READ, &hKeyCommand);

		if (dwStatus == ERROR_SUCCESS)
		{
			fCorrect = TRUE;
			RegCloseKey(hKeyCommand);
		}

		RegCloseKey(hKeyDVREdit);
	}

	else 
	{
		dwStatus = RegCreateKey(HKEY_CLASSES_ROOT, L"Applications\\DVREdit.exe", &hKeyDVREdit);
		if (dwStatus == ERROR_SUCCESS)
		{
			HKEY hKeySupportedTypes = 0;
			dwStatus = RegCreateKey(hKeyDVREdit, L"SupportedTypes", &hKeySupportedTypes);
			if (dwStatus == ERROR_SUCCESS)
			{
				dwStatus = RegSetValueEx(hKeySupportedTypes, L".dvr\0", NULL, REG_NONE, NULL, 0L);
				dwStatus = RegSetValueEx(hKeySupportedTypes, L".dvr-ms\0", NULL, REG_NONE, NULL, 0L);
				dwStatus = RegSetValueEx(hKeySupportedTypes, L".wtv\0", NULL, REG_NONE, NULL, 0L);

				RegCloseKey(hKeySupportedTypes);
			}
			RegCloseKey(hKeyDVREdit);
		}
	}

	if (fCorrect == FALSE)
	{
		HKEY hKeyDVR = 0;
		HKEY hKeyDVRMS = 0;

		dwStatus = RegOpenKeyEx(HKEY_CLASSES_ROOT, L".DVR\\OpenWithList\\DVREdit.exe", 0L, KEY_READ | KEY_WRITE, &hKeyDVR);
		if (dwStatus != ERROR_SUCCESS)
			dwStatus = RegCreateKey(HKEY_CLASSES_ROOT, L".DVR\\OpenWithList\\DVREdit.exe", &hKeyDVR);

		if (dwStatus == ERROR_SUCCESS)
			RegCloseKey(hKeyDVR);

		dwStatus = RegOpenKeyEx(HKEY_CLASSES_ROOT, L".dvr-ms\\OpenWithList\\DVREdit.exe", 0L, KEY_READ | KEY_WRITE, &hKeyDVRMS);
		if (dwStatus != ERROR_SUCCESS)
			dwStatus = RegCreateKey(HKEY_CLASSES_ROOT, L".dvr-ms\\OpenWithList\\DVREdit.exe", &hKeyDVRMS);

		if (dwStatus == ERROR_SUCCESS)
			RegCloseKey(hKeyDVRMS);		

		HKEY hKeyCommand = 0;
		dwStatus = RegOpenKeyEx(HKEY_CLASSES_ROOT, L"Applications\\dvredit.exe\\shell\\open\\command", 0L, KEY_READ | KEY_WRITE, &hKeyCommand);
		if (dwStatus != ERROR_SUCCESS)
			dwStatus = RegCreateKey(HKEY_CLASSES_ROOT, L"Applications\\dvredit.exe\\shell\\open\\command", &hKeyCommand);

		WCHAR* szBuf = NULL;
		DWORD dwSize = 0;
        dwSize = GetCurrentDirectory(0, szBuf);
		szBuf = new WCHAR[dwSize];
		if (szBuf == NULL)
			return FALSE;

		dwSize = GetCurrentDirectory(dwSize, szBuf);
		if (dwSize == 0)
			return FALSE;

		CString sz = szBuf;
		sz.Append(L"\\dvredit.exe");

		delete[] szBuf;
		szBuf = NULL;

		szBuf = new WCHAR[MAX_PATH+1];
		if (szBuf == NULL)
			return FALSE;

		if FAILED(StringCchPrintf(szBuf, MAX_PATH, L"\"%s\" %s", (LPCWSTR) sz, L"\"%1\""))
		{
			MessageBoxEx(m_hWnd, L"Failed to register DVREdit as dvr-ms file handler.", NULL, MB_OK | MB_ICONERROR, NULL);
			delete[] szBuf;
			return FALSE;
		}

		if (dwStatus == ERROR_SUCCESS)
			dwStatus = RegSetValueEx(hKeyCommand, NULL, NULL, REG_SZ, (LPBYTE) szBuf, sizeof(WCHAR) * wcslen(szBuf) + 1);
		
		delete[] szBuf;
	}

	return (dwStatus == ERROR_SUCCESS);
}

void CDVREditDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	RECT client;

	GetClientRect(&client);
	m_pGraph.SetVideoPosition(client.left + 7, client.top + 100, client.right - 14, client.bottom - 128);
	
	CRect rcClientStart;
	CRect rcClientNow;

	if (m_StatusBarCtrl)
	{
		m_StatusBarCtrl.SetWindowPos( &CWnd::wndBottom, client.left, client.bottom - 21, client.right, client.bottom, SWP_FRAMECHANGED);
		CRect rect;
		m_StatusBarCtrl.GetClientRect(&rect);
		long lWidth = (rect.right - rect.left) / 6;
		int widths[6] = {rect.left + lWidth, rect.left + lWidth*2, rect.left + lWidth*3, rect.left + lWidth*4, rect.left + lWidth*5, -1};
		m_StatusBarCtrl.SetParts(6, widths);
	}

	if (m_Seekbar)
	{
		m_Seekbar.SetWindowPos(&CWnd::wndBottom, client.left + 7, client.top + 50, client.right - 14, client.top + 79, SWP_FRAMECHANGED);
	}
}
// Converts reference times to hour:minute:second for status bar (bottom)
void RefTimeToHMS(REFERENCE_TIME refTime, CString* csHMS)
{
	int cTotalSeconds = (double(refTime) / dblUNITS);
	int cHours = cTotalSeconds / 3600;
	int cLeftover = cTotalSeconds % 3600;
	int cMinutes = cLeftover / 60;
	int cSeconds = cLeftover % 60;
	
	CTimeSpan ts( 0, cHours, cMinutes, cSeconds ); 
	
	*csHMS = ts.Format(L"%H:%M:%S"); 
}

// This fires when the file is done saving to clean up stuff
LRESULT CDVREditDlg::OnSaveThreadFinished(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = S_OK;

	if (m_hSaveThread)
	{
		hr = m_pRecComp->Close();
		if (FAILED(hr))
			DisplayError(m_hWnd, L"Saving - Close: 0x%x", hr);
					
		CloseHandle(m_hSaveThread);
	}

	if (FAILED(m_pSaveDialog->StopProgressDialog()))
		MessageBoxEx(m_hWnd, L"Saving - Failed to stop progress dialog.", NULL, MB_OK | MB_ICONERROR, NULL);
	
	m_pSaveDialog->Release();
	
	m_pSegmentQueue->RemoveAll();

	m_pRecComp.Release();

	SetDlgItemInt(IDC_SEGMENT_NUMBER, 0);
	
	if (m_szDestFile)
		delete[] m_szDestFile;
	
	m_szDestFile = NULL;

    if (m_szSourceFile)
		delete[] m_szSourceFile;

	m_szSourceFile = NULL;

	return TRUE;
}

// Allows the preview graph to handle resizing the window
LRESULT CDVREditDlg::OnGraphNotify(WPARAM wParam, LPARAM lParam)
{
	return m_pGraph.ProcessEvents(m_hWnd);
}

// Closing the app, cleanup
void CDVREditDlg::OnClose() 
{
    StopSeekTimer();
	m_pGraph.Stop(m_hWnd);

	if (m_pSegmentQueue)
		delete m_pSegmentQueue;
	
	m_pSegmentQueue = NULL;

	if (m_pCurrentSegment)
		delete m_pCurrentSegment;

	m_pCurrentSegment = NULL;
	
	m_pGraph.Dispose();
	// Release COM
    CoUninitialize();

	SaveMainWindowPlacement();

	if (m_szSourceFile)
		delete[] m_szSourceFile;

	m_szSourceFile = NULL;

    CDialog::OnClose();
}

// Closing app, cleanup
void CDVREditDlg::OnDestroy() 
{
    StopSeekTimer();
	m_pGraph.Stop(m_hWnd);
	
	if (m_pSegmentQueue)
		delete m_pSegmentQueue;
	
	m_pGraph.Dispose();

	m_pSegmentQueue = NULL;
	
	if (m_pCurrentSegment)
		delete m_pCurrentSegment;
	
	m_pCurrentSegment = NULL;

	SaveMainWindowPlacement();
	if (m_szSourceFile)
		delete[] m_szSourceFile;

	m_szSourceFile = NULL;
	
	if (m_pToolTip)
	{
		m_pToolTip->DestroyWindow();
		delete m_pToolTip;
	}

	m_pToolTip = NULL;

    CDialog::OnDestroy();
}
// Catch for tooltips and keyboard shortcuts
BOOL CDVREditDlg::PreTranslateMessage(MSG* pMsg)
{
    if (NULL != m_pToolTip)
        m_pToolTip->RelayEvent(pMsg);

	if (m_hAccelTable) 
	{
		if (::TranslateAccelerator(m_hWnd, m_hAccelTable, pMsg)) 
		{
			return(TRUE);
		}
	}
    return CDialog::PreTranslateMessage(pMsg);
}

// Configures tooltips for buttons
void CDVREditDlg::SetupToolTips(void)
{
	if(!m_pToolTip->Create(this))
	{
		MessageBoxEx(m_hWnd, L"Tooltip creation has failed.  No tooltips will be available.", NULL, MB_OK | MB_ICONSTOP, NULL);
	}
	
	//m_pToolTip->AddTool(GetDlgItem(
	m_pToolTip->AddTool(GetDlgItem(IDC_BUTTON_PLAY), L"Play / Pause");
	m_pToolTip->AddTool(GetDlgItem(IDC_BUTTON_STOP), L"Stop");
	
	m_pToolTip->AddTool(GetDlgItem(IDC_BUTTON_STEP_FORWARD), L"Step forward");
	m_pToolTip->AddTool(GetDlgItem(IDC_BUTTON_STEP_BACKWARD), L"Step backward");
	
	m_pToolTip->AddTool(GetDlgItem(IDC_BUTTON_COMMERCIAL_SKIP), L"Skip forward 30 seconds");

	m_pToolTip->AddTool(GetDlgItem(IDC_BUTTON_IN), L"Mark In");
	m_pToolTip->AddTool(GetDlgItem(IDC_BUTTON_OUT), L"Mark Out");
	
	m_pToolTip->AddTool(GetDlgItem(IDC_BUTTON_PREVIOUS_SEGMENT), L"Previous segment");
	m_pToolTip->AddTool(GetDlgItem(IDC_BUTTON_NEXT_SEGMENT), L"Next segment");
	m_pToolTip->AddTool(GetDlgItem(IDC_BUTTON_CLEAR_SEGMENT), L"Clear segment");
	
	m_pToolTip->AddTool(GetDlgItem(IDC_BUTTON_SEGMENT_START), L"Move to start of current segment");
	m_pToolTip->AddTool(GetDlgItem(IDC_BUTTON_SEGMENT_END), L"Move to end of current segment");
	m_pToolTip->AddTool(GetDlgItem(IDC_BUTTON_LOAD), L"Open a file for editing");
	m_pToolTip->AddTool(GetDlgItem(IDC_BUTTON_SAVE), L"Save file");

	m_pToolTip->Activate(TRUE);
	
}
// Called when dialog first opens, details within
BOOL CDVREditDlg::OnInitDialog()
{
	
	// Enable tooltips
	CDVREditDlg::EnableToolTips(TRUE);
	m_pToolTip = new CToolTipCtrl;
 
	if (m_pToolTip)
		SetupToolTips();

	// Setting up status bar (bottom of screen)
	RECT client;

	GetClientRect(&client);
	m_Seekbar.Create(WS_VISIBLE|WS_CHILD|TBS_HORZ|TBS_TOP|TBS_AUTOTICKS|TBS_ENABLESELRANGE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, CRect(client.left + 7,client.top + 50,client.right - 14, client.top + 79), this, IDC_SLIDER);
	m_Seekbar.EnableWindow(FALSE);

	m_StatusBarCtrl.Create(WS_CHILD|WS_VISIBLE|CCS_BOTTOM|SBARS_SIZEGRIP, CRect(0,0,0,0), this, IDC_STATUS_BAR);
	
	CRect rect;
	m_StatusBarCtrl.GetClientRect(&rect);
	long lWidth = (rect.right - rect.left) / 6;
	//int widths[6] = {rect.left + 88, rect.left + 185, rect.left + 285, rect.left + 335, rect.left + 460, rect.left + 515};
	int widths[6] = {rect.left + lWidth, rect.left + lWidth*2, rect.left + lWidth*3, rect.left + lWidth*4, rect.left + lWidth*5, rect.left + lWidth*6};
	m_StatusBarCtrl.SetParts(6, widths);
	m_StatusBarCtrl.SetText(L" Reference Time: ", 0, 0);
	m_StatusBarCtrl.SetText(L"000000000000000", 1, 0);
	m_StatusBarCtrl.SetText(L" Segment Length: ", 2, 0);	
	m_StatusBarCtrl.SetText(L"00:00:00", 3, 0);
	m_StatusBarCtrl.SetText(L" Total Segments: ", 4, 0);
	m_StatusBarCtrl.SetText(L"00:00:00", 5, 0);

	// Commenting out this section.  Hopefully nobody is running XP anymore 
	/*
	// Checking version of sbe.dll, display error if not MCE 2004
	WCHAR szPath[MAX_PATH+1];
	GetSystemDirectory(szPath, sizeof(szPath)/sizeof(WCHAR));
	if SUCCEEDED(StringCchCat(szPath, sizeof(szPath)/sizeof(WCHAR), L"\\sbe.dll"))
	{
		DWORD dwHandle = 0;
		DWORD versionInfoSize = GetFileVersionInfoSize(szPath, &dwHandle );
		
		BYTE *pVersionInfo = new BYTE[versionInfoSize];
		if (NULL == pVersionInfo)
			MessageBoxEx(m_hWnd, L"Out of memory!", NULL, MB_OK | MB_ICONERROR, NULL);
		else
		{
			GetFileVersionInfo(szPath, 0, versionInfoSize, pVersionInfo);

			VS_FIXEDFILEINFO *pFileInfo;
			UINT fileInfoLen;

			VerQueryValue(pVersionInfo, L"\\", (void**)&pFileInfo, &fileInfoLen);

			if (pFileInfo->dwFileVersionMS < 0x00060005) // 6.5.x.x, MCE 2004 RTM
			{
				CString csWarning;
				csWarning.Format(L"Warning, this program only functions correctly with Microsoft® Windows® XP Media Center Edition 2004.\t\r\n\r\nYour version of sbe.dll: \t%d.%d.%d.%d\r\nCorrect (minimum) version: \t6.5.2600.1217\r\n\r\nYou may not be able to save files without an update.",
					HIWORD(pFileInfo->dwFileVersionMS),
					LOWORD(pFileInfo->dwFileVersionMS),
					HIWORD(pFileInfo->dwFileVersionLS),
					LOWORD(pFileInfo->dwFileVersionLS));

				MessageBoxEx(m_hWnd, (LPCWSTR) csWarning, NULL, MB_OK | MB_ICONERROR, NULL);
			}

			else if ((pFileInfo->dwFileVersionLS < 0x0a28084d) && (pFileInfo->dwFileVersionMS < 0x00060006)) //x.x.2600.2125
			{
				CString csWarning;
				csWarning.Format(L"Warning, there is a known issue with the version of sbe.dll currently installed on your system which may cause this program to deadlock (hang).  Please obtain and install Windows XP SP2 or later to correct this problem.\t\r\n\r\nYour version of sbe.dll: \t%d.%d.%d.%d\r\nCorrect (minimum) version: \t6.5.2600.2125\r\n\r\nSaving files may cause this application to hang without an update.",
					HIWORD(pFileInfo->dwFileVersionMS),
					LOWORD(pFileInfo->dwFileVersionMS),
					HIWORD(pFileInfo->dwFileVersionLS),
					LOWORD(pFileInfo->dwFileVersionLS));

				MessageBoxEx(m_hWnd, (LPCWSTR) csWarning, NULL, MB_OK | MB_ICONERROR, NULL);
			}
			
			delete [] pVersionInfo;
			pVersionInfo = NULL;
		}
	}

	else 
	{
		MessageBoxEx(m_hWnd, L"Warning, this program only functions correctly with Microsoft® Windows® XP Media Center Edition 2004.", NULL, MB_OK | MB_ICONERROR, NULL);
	}
	// End commenting out legacy XP stuff
	*/
	// Loading pictures for the buttons
	m_ButtonPlay.LoadBitmaps(PLAYU, PLAYD, 0, PLAYX);
	m_ButtonPlaySegment.LoadBitmaps(SEGMENT_PLAYU, SEGMENT_PLAYD, 0, SEGMENT_PLAYX);
	m_ButtonStop.LoadBitmaps(STOPU, STOPD, 0, STOPX);
	m_ButtonPlayAll.LoadBitmaps(STOPU, STOPD, 0, STOPX);
	m_ButtonStepForward.LoadBitmaps(STEP_FORWARDU, STEP_FORWARDD, 0, STEP_FORWARDX);
	m_ButtonStepBackward.LoadBitmaps(STEP_BACKWARDU, STEP_BACKWARDD, 0, STEP_BACKWARDX);
	m_ButtonSegmentPrev.LoadBitmaps(SEGMENT_PREVU, SEGMENT_PREVD, 0, SEGMENT_PREVX);
	m_ButtonSegmentNext.LoadBitmaps(SEGMENT_NEXTU, SEGMENT_NEXTD, 0, SEGMENT_NEXTX);
	m_ButtonSegmentClear.LoadBitmaps(SEGMENT_CLEARU, SEGMENT_CLEARD, 0, SEGMENT_CLEARX);
	m_ButtonIn.LoadBitmaps(INU, IND, 0, INX);
	m_ButtonOut.LoadBitmaps(OUTU, OUTD, 0, OUTX);
	m_ButtonCommercialSkip.LoadBitmaps(IDB_30U, IDB_30D, 0, IDB_30X);
	m_ButtonSegmentStart.LoadBitmaps(SEGMENT_STARTU, SEGMENT_STARTD, 0, SEGMENT_STARTX);
	m_ButtonSegmentEnd.LoadBitmaps(SEGMENT_ENDU, SEGMENT_ENDD, 0, SEGMENT_ENDX);
	m_ButtonLoad.LoadBitmaps(LOADU, LOADD, 0, 0);
	m_ButtonSave.LoadBitmaps(SAVEU, SAVED, 0, SAVEX);

	m_fSuppressEditedBy = FALSE;
	m_fSuppressEditedBy = fSuppressEditedBy();
	LoadMainWindowPlacement();
	CDialog::OnInitDialog();
	
	// Setting start and end reference times, file not loaded yet
	m_rtStart, m_rtStop = 0;
	
	m_pSegmentQueue = NULL;
	m_pCurrentSegment = NULL;
	m_szSourceFile = NULL;
	m_szDestFile = NULL;
	m_hSaveThread = NULL;
	m_pSaveDialog = NULL;
	
	// Initialize COM
	if FAILED(::CoInitializeEx (NULL, COINIT_APARTMENTTHREADED))//COINIT_MULTITHREADED))
	{
		MessageBoxEx(m_hWnd, L"Error initializing COM, exiting!", NULL, MB_OK | MB_ICONERROR, NULL);
		CDialog::OnOK();
	}
	
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	CheckRegisterDVREdit();

	if (fCommandLine)
	{
		OpenSourceFile();
	}

		
	return FALSE;
}

// Makes about dialog work
void CDVREditDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDVREditDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDVREditDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// Copies attributes from source to destination file.  Comments inline
void CDVREditDlg::CopyAttributes(IProgressDialog * pProgressDialog)
{
	HRESULT hr = S_OK;

	LONGLONG duration = m_pSegmentQueue->TotalTimeInRefTime();		
	
	CComPtr <IFileSourceFilter> pSourceFile;    
	hr = pSourceFile.CoCreateInstance(CLSID_StreamBufferRecordingAttributes);
	if (FAILED(hr))
	{
		DisplayError(m_hWnd, L"Copying attributes, error creating source filter: 0x%x", hr);
		return;
	}

	// Here we load the source file and add it to the graph
	hr = pSourceFile->Load( (LPCWSTR) m_szSourceFile, NULL );
	if (FAILED(hr))
	{
		DisplayError(m_hWnd, L"Copying attributes, error adding loading file into source filter: 0x%x", hr);
		return;		
	}
	// Setting up attribute interfaces
	CComQIPtr <IStreamBufferRecordingAttribute> pIAttrib(m_pRecComp);
	CComQIPtr <IStreamBufferRecordingAttribute> pRecordingAttributes(pSourceFile);

	CAttributesDialog attrDlg;

	LONGLONG* qwEncodeTime = 0;
	WORD wAttributes;
	
	// How many attributes are we dealing with here?
	hr = pRecordingAttributes->GetAttributeCount(0, &wAttributes);
	if (FAILED(hr))
	{
		pIAttrib.Release();
		pRecordingAttributes.Release();
		DisplayError(m_hWnd, L"Copying attributes, error getting attribute count: 0x%x", hr);
		return;		
	}
	
	// Loop through each attribute, and copy it.  Some are special cases.
	for (WORD i = 0; i < wAttributes; i++) {
        WCHAR *wszAttributeName = NULL;
        WORD cchNameLength = 0;
        STREAMBUFFER_ATTR_DATATYPE datatype;
        BYTE *abAttribute = NULL;
        WORD cbAttribute = 0;

        hr = pRecordingAttributes->GetAttributeByIndex(i, 0,
            wszAttributeName, &cchNameLength,
            &datatype,
            abAttribute, &cbAttribute);
		if (FAILED(hr))
		{
			pIAttrib.Release();
			pRecordingAttributes.Release();
			DisplayError(m_hWnd, L"Copying attributes, error getting attribute by index: 0x%x", hr);
			return;	
		}

		wszAttributeName = new WCHAR[cchNameLength];
		if (NULL == wszAttributeName)
		{
			return;
		}

		abAttribute = new BYTE[cbAttribute];
		if (NULL == abAttribute)
		{
			delete[] wszAttributeName; 
			return;
		}
		
		hr = pRecordingAttributes->GetAttributeByIndex(i, 0,
			wszAttributeName, &cchNameLength,
			&datatype,
			abAttribute, &cbAttribute);
		if (FAILED(hr))
		{
			delete[] abAttribute;
			delete[] wszAttributeName;
			pIAttrib.Release();
			pRecordingAttributes.Release();
			DisplayError(m_hWnd, L"Copying attributes, error getting attribute by index: 0x%x", hr);
			return;	
		}
		// We're changing these, because the destination file is shorter than the source file.				
		if (CompareString(LOCALE_INVARIANT, NORM_IGNORECASE, wszAttributeName, -1, L"WM/MediaOriginalRunTime", -1) == CSTR_EQUAL)
		{
			pIAttrib->SetAttribute(NULL, wszAttributeName, datatype, (BYTE*) &duration, sizeof(duration));
			delete[] wszAttributeName;
			delete[] abAttribute;
			continue;
		}					
		
		// This is nasty.  We have to change the end time to be the encode time plus the new duration
		if (CompareString(LOCALE_INVARIANT, NORM_IGNORECASE, wszAttributeName, -1, L"WM/WMRVEncodeTime", -1) == CSTR_EQUAL)
		{
			qwEncodeTime = (LONGLONG*) abAttribute;
			LONGLONG endTime = *qwEncodeTime + duration;
			pIAttrib->SetAttribute(NULL, L"WM/WMRVEndTime", datatype, (BYTE*) &endTime, sizeof(endTime));
		}

		if (CompareString(LOCALE_INVARIANT, NORM_IGNORECASE, wszAttributeName, -1, L"WM/WMRVEndTime", -1) == CSTR_EQUAL)
		{
			delete[] wszAttributeName;
			delete[] abAttribute;
			continue;
		}
		// HACK HACK: to make metadata editor in SBE happy with WM/Picture
		if (CompareStringW(LOCALE_INVARIANT, NORM_IGNORECASE, wszAttributeName, -1, g_wszWMPicture, -1) == CSTR_EQUAL)
		{			
			WM_PICTURE*  WMPicture = reinterpret_cast <WM_PICTURE*> (abAttribute);
			
			size_t cchMIME, cchDesc;
			if (FAILED
					(StringCchLength(WMPicture->pwszMIMEType, cbAttribute, &cchMIME)
					|| StringCchLength(WMPicture->pwszDescription, cbAttribute, &cchDesc)))
				continue;

			cchMIME++;
			cchDesc++;

			cbAttribute =	(cchMIME + cchDesc) * sizeof(WCHAR)			// pwszMIMEType, pwszDescription
							+ RTL_FIELD_SIZE(WM_PICTURE,bPictureType)	// bPictureType
							+ RTL_FIELD_SIZE(WM_PICTURE,dwDataLen)		// dwDataLen
							+ WMPicture->dwDataLen;						// pbData

			BYTE* pbAttribute = new BYTE[cbAttribute];
			if (NULL == pbAttribute)
			{
				hr = E_OUTOFMEMORY;
				break;
			}

			ZeroMemory(pbAttribute, cbAttribute);

			hr = StringCchCopy((LPWSTR)(pbAttribute), cchMIME, WMPicture->pwszMIMEType);
			if FAILED(hr)
				continue;

			pbAttribute += (cchMIME) * sizeof(WCHAR);
			
			*pbAttribute = WMPicture->bPictureType;

			pbAttribute += RTL_FIELD_SIZE(WM_PICTURE,bPictureType);

			hr = StringCchCopy((LPWSTR)(pbAttribute), cchDesc, WMPicture->pwszDescription);
			if FAILED(hr)
				continue;
			
			pbAttribute += (cchDesc) * sizeof(WCHAR);

			*(DWORD*)(pbAttribute) = WMPicture->dwDataLen;
			
			pbAttribute += RTL_FIELD_SIZE(WM_PICTURE,dwDataLen);

			CopyMemory(pbAttribute, WMPicture->pbData, WMPicture->dwDataLen);

			pbAttribute = pbAttribute - cbAttribute + WMPicture->dwDataLen;
			
			delete[] abAttribute;
			abAttribute = pbAttribute;
		}

		if (datatype == STREAMBUFFER_TYPE_STRING)
		{
			// This is user editable, if they changed it, we commit the change here but do not write it yet
			if (CompareString(LOCALE_INVARIANT, NORM_IGNORECASE, wszAttributeName, -1, L"Title", -1) == CSTR_EQUAL)
			{
				attrDlg.m_csTitle.Append((LPCWSTR) abAttribute);
				delete[] wszAttributeName;
				delete[] abAttribute;
				continue;
			}

			// This is user editable, if they changed it, we commit the change here but do not write it yet
			if (CompareString(LOCALE_INVARIANT, NORM_IGNORECASE, wszAttributeName, -1, L"WM/SubTitle", -1) == CSTR_EQUAL)
			{
				attrDlg.m_csSubTitle.Append((LPCWSTR) abAttribute);
				delete[] wszAttributeName;
				delete[] abAttribute;
				continue;
			}

			// This is user editable, if they changed it, we commit the change here but do not write it yet
			if (CompareString(LOCALE_INVARIANT, NORM_IGNORECASE, wszAttributeName, -1, L"WM/Genre", -1) == CSTR_EQUAL)
			{
				attrDlg.m_csGenre.Append((LPCWSTR) abAttribute);
				delete[] wszAttributeName;
				delete[] abAttribute;
				continue;
			}

			// This is user editable, if they changed it, we commit the change here but do not write it yet
			if (CompareString(LOCALE_INVARIANT, NORM_IGNORECASE, wszAttributeName, -1, L"WM/SubTitleDescription", -1) == CSTR_EQUAL)
			{
				attrDlg.m_csDescription.Append((LPCWSTR) abAttribute);
				delete[] wszAttributeName;
				delete[] abAttribute;
				continue;
			}

			// This is user editable, if they changed it, we commit the change here but do not write it yet
			if (CompareString(LOCALE_INVARIANT, NORM_IGNORECASE, wszAttributeName, -1, L"WM/MediaCredits", -1) == CSTR_EQUAL)
			{
				attrDlg.m_csMediaCredits.Append((LPCWSTR) abAttribute);
				delete[] wszAttributeName;
				delete[] abAttribute;
				continue;
			}
		}
		// Actually write the attribute to the new file here
		pIAttrib->SetAttribute(NULL, wszAttributeName, datatype, abAttribute, cbAttribute);
		delete[] abAttribute;
		delete[] wszAttributeName;
		
	}// Next attribute now
	
	// Ask user if they want to change these attributes
	attrDlg.DoModal();
	
	// If string has not already beed added to the end of the description, add this string
	if (!wcsstr((LPCWSTR) attrDlg.m_csDescription, L"(Edited with DVREdit)"))
	{
		if (!m_fSuppressEditedBy)
			attrDlg.m_csDescription.Append(L" (Edited with DVREdit)");
	}
	
	// Set the attribute to what the user wanted
	if (!attrDlg.m_csTitle.IsEmpty())
		pIAttrib->SetAttribute(NULL, L"Title", STREAMBUFFER_TYPE_STRING, (BYTE*) (LPCWSTR) attrDlg.m_csTitle, (WORD) (sizeof(WCHAR) * (wcslen(attrDlg.m_csTitle)+1)));
	
	// Set the attribute to what the user wanted
	if (!attrDlg.m_csSubTitle.IsEmpty())
		pIAttrib->SetAttribute(NULL, L"WM/SubTitle", STREAMBUFFER_TYPE_STRING, (BYTE*) (LPCWSTR) attrDlg.m_csSubTitle, (WORD) (sizeof(WCHAR) * (wcslen(attrDlg.m_csSubTitle)+1)));
	
	// Set the attribute to what the user wanted
	if (!attrDlg.m_csDescription.IsEmpty())
		pIAttrib->SetAttribute(NULL, L"WM/SubTitleDescription", STREAMBUFFER_TYPE_STRING, (BYTE*) (LPCWSTR) attrDlg.m_csDescription, (WORD) (sizeof(WCHAR) * (wcslen(attrDlg.m_csDescription)+1)));
	
	// Set the attribute to what the user wanted
	if (!attrDlg.m_csGenre.IsEmpty())
		pIAttrib->SetAttribute(NULL, L"WM/Genre", STREAMBUFFER_TYPE_STRING, (BYTE*) (LPCWSTR) attrDlg.m_csGenre, (WORD) (sizeof(WCHAR) * (wcslen(attrDlg.m_csGenre)+1)));

	// Set the attribute to what the user wanted
	if (!attrDlg.m_csMediaCredits.IsEmpty())
		pIAttrib->SetAttribute(NULL, L"WM/MediaCredits", STREAMBUFFER_TYPE_STRING, (BYTE*) (LPCWSTR) attrDlg.m_csMediaCredits, (WORD) (sizeof(WCHAR) * (wcslen(attrDlg.m_csMediaCredits)+1)));

	// Adding two attributes for easy identification of file origin and program version
	CString csAppID = L"DVREdit by Alex Seigler (aseigler)";
	CString csAppVer = L"V.YYYY.MM.DD";
	csAppVer.Format(L"%d.%d.%d.%d", rmj, rmn, rmm, rup);
	
	// Set these two attributes
	pIAttrib->SetAttribute(NULL, L"WM/ToolName", STREAMBUFFER_TYPE_STRING, (BYTE*) (LPCWSTR) csAppID, (WORD) (sizeof(WCHAR) * (wcslen(csAppID)+1)));
	pIAttrib->SetAttribute(NULL, L"WM/ToolVersion", STREAMBUFFER_TYPE_STRING, (BYTE*) (LPCWSTR) csAppVer, (WORD) (sizeof(WCHAR) * (wcslen(csAppVer)+1)));

	pIAttrib.Release();
	pRecordingAttributes.Release();
}

// This keeps the video playing correctly when you minimize the dialog or cover it with a different app
BOOL CDVREditDlg::OnEraseBkgnd(CDC *pDC)
{
    // Intercept background erasing for the movie window, since the
    // video renderer will keep the screen painted.  Without this code,
    // your video window might get painted over with gray (the default
    // background brush) when it is obscured by another window and redrawn.
    CRect rc;

    // Get the bounding rectangle for the movie screen
    //GetDlgItem(IDC_VIEWER)->GetWindowRect(&rc);
    //ScreenToClient(&rc);

    // Exclude the clipping region occupied by our movie screen
    //pDC->ExcludeClipRect(&rc);

CBrush brNew(RGB(0,0,255));  //Creates a blue brush
CBrush* pOldBrush = (CBrush*)pDC->SelectObject(&brNew);

pDC->GetClipBox(rc); // Gets the co-ordinates of the client
                     // area to repaint.
pDC->PatBlt(0,0,rc.Width(),rc.Height(),PATCOPY);
                     // Repaints client area with current brush.
pDC->SelectObject(pOldBrush);

    // Erase the remainder of the dialog as usual
    return CDialog::OnEraseBkgnd(pDC);
}

// Called when you click ok in the file->open dialog, comments inline
HRESULT CDVREditDlg::OpenSourceFile()
{
	HRESULT hr = S_OK;
	
	// Setting up the preview graph
	hr = m_pGraph.SetFilterGraph(m_szSourceFile, m_hWnd);
	if (SUCCEEDED(hr))
	{
		hr = m_pGraph.SetVideoWindow(m_hWnd);//GetDlgItem(IDC_VIEWER)->GetSafeHwnd());
		if (SUCCEEDED(hr))
		{
			hr = m_pGraph.SetEventWindow(m_hWnd);
			if (SUCCEEDED(hr))
			{
				// Enabling control buttons disabled on startup
				EnablePlayback(TRUE);
				
				// Setting initial status bar info
				CString csHMS;
				RefTimeToHMS(m_pGraph.Duration, &csHMS);
				m_StatusBarCtrl.SetText((LPCWSTR) csHMS, 3, 0);
				m_StatusBarCtrl.SetText((LPCWSTR) csHMS, 5, 0);
				m_Seekbar.SetFocus();
			}
			else DisplayError(m_hWnd, L"Opening, error setting event window: 0x%x", hr);
		}
		else DisplayError(m_hWnd, L"Opening, error setting video window: 0x%x", hr);
	}
	else DisplayError(m_hWnd, L"Opening, error in graph: 0x%x", hr);
return hr;
}

// This function does the actual copying of segments from the source dvr-ms file to the destination file
// Comments inline
static DWORD WINAPI SaveThread(LPVOID lpParameter)
{
	HRESULT hr = S_OK;
	
	// Casting parameter to get dialog back
	CDVREditDlg* pDVREditDlg = (CDVREditDlg*) (lpParameter);
	
	// Initialize COM in this new thread
	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);//COINIT_MULTITHREADED);
	if (SUCCEEDED(hr))
	{
		// Take a segment out of the queue
		CSegment* pSegment = pDVREditDlg->m_pSegmentQueue->Dequeue();
		
		// if there is a segement left and we haven't failed yet...
		while (pSegment && SUCCEEDED(hr))
		{
			// Take the segment and stick it in the new file...
			hr = pDVREditDlg->m_pRecComp->AppendEx(pDVREditDlg->m_szSourceFile, pSegment->InTime(), pSegment->OutTime());
			if (FAILED(hr))
				DisplayError(pDVREditDlg->m_hWnd, L"Saving - AppendEx: 0x%x", hr);

			delete pSegment;
			pSegment = NULL;

			// now get the next segment.
			pSegment = pDVREditDlg->m_pSegmentQueue->Dequeue();
		}
		// We're done, uninitialize COM
		CoUninitialize();
	}
	
	else DisplayError(pDVREditDlg->m_hWnd, L"Saving - CoInitialize: 0x%x", hr);

	return hr;
}

// Thread for progress dialog while saving, also kicks off real saving
static DWORD WINAPI WaitThread(LPVOID lpParameter)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);//COINIT_MULTITHREADED);
	if (SUCCEEDED(hr))
	{
		CDVREditDlg* pDVREditDlg = (CDVREditDlg*) (lpParameter);
		if (pDVREditDlg)
		{
			DWORD cTotalSeconds = pDVREditDlg->m_pSegmentQueue->TotalTimeInSeconds();
			// real save thread kicked off here
			HANDLE hSaveHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) SaveThread, (LPVOID) pDVREditDlg, 0, NULL);

			if (hSaveHandle)
			{
				while (WaitForSingleObject(hSaveHandle, 200) == WAIT_TIMEOUT)
				{
					if (pDVREditDlg->m_pSaveDialog->HasUserCancelled())
					{
						hr = pDVREditDlg->m_pRecComp->Cancel();
						if (FAILED(hr))
							DisplayError(pDVREditDlg->m_hWnd, L"Saving - Cancel: 0x%x", hr);
					}

					else
					{
						DWORD cSeconds = 0;
						hr = pDVREditDlg->m_pRecComp->GetCurrentLength(&cSeconds);
						if (FAILED(hr))
							DisplayError(pDVREditDlg->m_hWnd, L"Saving - GetCurrentLength: 0x%x", hr);

						if (FAILED(pDVREditDlg->m_pSaveDialog->SetProgress(cSeconds, cTotalSeconds)))
							MessageBoxEx(pDVREditDlg->m_hWnd, L"Saving - Failed to set progress in dialog.", NULL, MB_OK | MB_ICONERROR, NULL);
					}
				}
				GetExitCodeThread(hSaveHandle, (LPDWORD) &hr);
				if (FAILED(hr))
					DisplayError(pDVREditDlg->m_hWnd, L"Saving - Thread exited with error: 0x%x", hr);
				CloseHandle(hSaveHandle);
			}

			else MessageBoxEx(pDVREditDlg->m_hWnd, L"Saving - Failed to create thread.", NULL, MB_OK | MB_ICONERROR, NULL);
			PostMessage(pDVREditDlg->m_hWnd, WM_SAVETHREAD_FINISHED, NULL, NULL);
		}
		CoUninitialize();
	}
	return hr;
}

// Enables/disables buttons disabled by default, configures seekbar to initial state
void CDVREditDlg::EnablePlayback(BOOL fOnOff)
{
	if (fOnOff == TRUE)
	{
		m_Seekbar.SetRange(0 , double(m_pGraph.Duration) / dblUNITS, TRUE);
		m_Seekbar.SetTicFreq(60);
		m_Seekbar.SetPos(0);
		m_Seekbar.SetPageSize(60);
		StartSeekTimer();

		m_rtStart = 0;
		m_rtStop = m_pGraph.Duration;
		m_Seekbar.SetSelection(m_rtStart, m_rtStop);
		
		m_Seekbar.SetFocus();
		m_pSegmentQueue = new CSegmentQueue();
		m_pCurrentSegment = new CSegment(m_rtStart, m_rtStop);
		SetDlgItemInt(IDC_SEGMENT_NUMBER, 0);
	}

	else
	{
		StopSeekTimer();
		(GetDlgItem(IDC_BUTTON_PREVIOUS_SEGMENT))->EnableWindow(FALSE);
		(GetDlgItem(IDC_BUTTON_NEXT_SEGMENT))->EnableWindow(FALSE);
		(GetDlgItem(IDC_BUTTON_CLEAR_SEGMENT))->EnableWindow(FALSE);
	}

	m_Seekbar.EnableWindow(fOnOff);
	(GetDlgItem(IDC_BUTTON_IN))->EnableWindow(fOnOff);
	(GetDlgItem(IDC_BUTTON_OUT))->EnableWindow(fOnOff);
	(GetDlgItem(IDC_BUTTON_PLAY))->EnableWindow(fOnOff);
	(GetDlgItem(IDC_BUTTON_STOP))->EnableWindow(fOnOff);
	(GetDlgItem(IDC_BUTTON_STEP_FORWARD))->EnableWindow(fOnOff);
	(GetDlgItem(IDC_BUTTON_STEP_BACKWARD))->EnableWindow(fOnOff);
	(GetDlgItem(IDC_BUTTON_COMMERCIAL_SKIP))->EnableWindow(fOnOff);
	(GetDlgItem(IDC_BUTTON_SEGMENT_START))->EnableWindow(fOnOff);
	(GetDlgItem(IDC_BUTTON_SEGMENT_END))->EnableWindow(fOnOff);
	(GetDlgItem(IDC_BUTTON_SAVE))->EnableWindow(fOnOff);
}

// Called when you click ok in the file->save as dialog, comments inline
HRESULT CDVREditDlg::OpenDestFile(void)
{
	m_pSaveDialog = NULL;
	BeginWaitCursor();

	// Creating and setting up the progress dialog
	HRESULT hr = CoCreateInstance(CLSID_ProgressDialog, NULL, CLSCTX_INPROC_SERVER, IID_IProgressDialog, (void**) &m_pSaveDialog);
	
	if (m_pSaveDialog && SUCCEEDED(hr))
	{
		if (FAILED(m_pSaveDialog->SetTitle(L"Saving...")))
			MessageBoxEx(m_hWnd, L"Saving - Failed to set dialog title.", NULL, MB_OK | MB_ICONERROR, NULL);
		
		if (FAILED(m_pSaveDialog->SetCancelMsg(L"Cancelling, please wait...", NULL)))
			MessageBoxEx(m_hWnd, L"Saving - Failed to set dialog cancel message.", NULL, MB_OK | MB_ICONERROR, NULL);

		if (FAILED(m_pSaveDialog->SetLine(2, m_szDestFile, TRUE, NULL)))
			MessageBoxEx(m_hWnd, L"Saving - Failed to set dialog line 2.", NULL, MB_OK | MB_ICONERROR, NULL);

		if (FAILED(m_pSaveDialog->SetLine(1, L"Setting up destination file:", FALSE, NULL)))
			MessageBoxEx(m_hWnd, L"Saving - Failed to set dialog line 1.", NULL, MB_OK | MB_ICONERROR, NULL);

		// main component for copying segments to new file
		hr = m_pRecComp.CoCreateInstance(CLSID_StreamBufferComposeRecording);
		if (SUCCEEDED(hr) && m_pRecComp)
		{
			// stop the preview graph and kill it
			m_pGraph.Stop(m_hWnd);
			EnablePlayback(FALSE);
			m_pGraph.Dispose();

			// initialize source and destination files
			hr = m_pRecComp->Initialize(m_szDestFile, m_szSourceFile);
			if (SUCCEEDED(hr))
			{
				// make sure the segment queue is good
				if (m_pSegmentQueue)
				{
					// read the current segment number from the dialog
					INT_PTR iIndex = GetDlgItemInt(IDC_SEGMENT_NUMBER);

					// check to see if current segment is saved or not
					POSITION pos = m_pSegmentQueue->FindIndex(iIndex);
					if (pos == 0)
					{
						// if not, save it now as a new segment
						m_pSegmentQueue->Enqueue(m_pCurrentSegment);
					}
					else // else save it as an existing segment
					{
						CSegment* pTempSegment = m_pSegmentQueue->GetAt(pos);
						pTempSegment->SetInTime(m_pCurrentSegment->InTime());
						pTempSegment->SetOutTime(m_pCurrentSegment->OutTime());
						m_pSegmentQueue->SetAt(pos, pTempSegment);
					}

					m_pCurrentSegment = NULL;

					// copy attributes from source to dest file
					CopyAttributes(m_pSaveDialog);
					
					// put stuff in progress dialog
					if (FAILED(m_pSaveDialog->StartProgressDialog(m_hWnd, NULL, PROGDLG_MODAL | PROGDLG_AUTOTIME, NULL)))
						MessageBoxEx(m_hWnd, L"Saving - Failed to start progress dialog.", NULL, MB_OK | MB_ICONERROR, NULL);
					
					if (FAILED(m_pSaveDialog->SetLine(1, L"Saving segments to file:", FALSE, NULL)))
						MessageBoxEx(m_hWnd, L"Saving - Failed to set dialog line 1.", NULL, MB_OK | MB_ICONERROR, NULL);
					
					DWORD cTotalSeconds = m_pSegmentQueue->TotalTimeInSeconds();

					if (FAILED(m_pSaveDialog->SetProgress(0, cTotalSeconds)))
						MessageBoxEx(m_hWnd, L"Saving - Failed to zero progress in dialog.", NULL, MB_OK | MB_ICONERROR, NULL);

					// start progress dialog thread, which kicks off real save thread
					m_hSaveThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) WaitThread, (LPVOID) this, 0, NULL);
				}
			}
			else // we failed, figure out what the problem is and pop up a dialog
			{
				if (FAILED(m_pSaveDialog->StopProgressDialog()))
					MessageBoxEx(m_hWnd, L"Saving - Failed to stop progress dialog.", NULL, MB_OK | MB_ICONERROR, NULL);
				
				// if the dest file exists, we always get the same error
				HANDLE hDestFile = CreateFile(m_szDestFile, 0, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
				
				CString csError;

				if (hDestFile)
				{
					if (0 == GetLastError())
					{
						// if it wasnt' the dest file, it's probably your build of sbe.dll
						csError.Format(L"Saving - Initialize failed, error: 0x%x\r\n\r\nIs your version of sbe.dll too old?", hr);
						CloseHandle(hDestFile);
						DeleteFile(m_szDestFile);
					}
					
					else if (ERROR_ALREADY_EXISTS == GetLastError())
					{
						// dest file already exists
						csError.Format(L"Error, destination file already exists!");
						CloseHandle(hDestFile);
					}

					else 
					{
						// I have no idea what happened, never got here before
						csError.Format(L"Saving - Initialize failed, error: 0x%x\r\n\r\nI don't know how you got here.", hr);
						CloseHandle(hDestFile);
					}
				}

				else csError.Format(L"Saving - Initialize failed, error: 0x%x\r\n\r\nCreate file failed, 0x%x", hr, GetLastError());

				MessageBoxEx(m_hWnd, (LPCWSTR) csError, NULL, MB_OK | MB_ICONERROR, NULL);
			}
		}
		else DisplayError(m_hWnd, L"Saving - Failed to create stream buffer instance: 0x%x", hr);
	}

	else 
		DisplayError(m_hWnd, L"Saving - Failed to create progress dialog: 0x%x", hr);

return hr;
}

// Shutting down, clean up
void CDVREditDlg::OnFileExit()
{
	m_pGraph.Stop(m_hWnd);

	if (m_pSegmentQueue)
		delete m_pSegmentQueue;
	
	m_pSegmentQueue = NULL;

	m_pGraph.Dispose();

	SaveMainWindowPlacement();
	CDialog::OnOK();
}

// Right when you click file open, comments inline
void CDVREditDlg::OnFileOpen()
{
	HKEY hKey;
	LONG error;
	
	// initialize a file open dialog that only shows dvr-ms and dvr files
	CFileDialog sFileDlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, L"Recorded TV Files (*.wtx;*.dvr-ms;*.dvr)|*.wtv;*.dvr-ms;*.dvr||", this);

	// this registry key shows the path where dvr-ms files exist by default, we'll always start in that location
	error = RegOpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Media Center\\Service\\Recording", &hKey);
	if (error == ERROR_SUCCESS)
	{
		DWORD dwRegType = REG_SZ;
		DWORD dwSize = MAX_PATH;
		WCHAR wszRecordPath[MAX_PATH] = L"\0";

		error = RegQueryValueEx( hKey, L"RecordPath", 0, &dwRegType, (LPBYTE) wszRecordPath, &dwSize);
 		
		wszRecordPath[MAX_PATH-1] = 0;

		if (error == ERROR_SUCCESS)
		{
			sFileDlg.m_pOFN->lpstrInitialDir = wszRecordPath;
		}

		RegCloseKey(hKey);
	}

	// show the file dialog here
	if (IDOK == sFileDlg.DoModal())
	{
		m_szSourceFile = new OLECHAR[MAX_PATH];
		if (m_szSourceFile == NULL)
		{
			DisplayError(m_hWnd, L"Out of memory creating source file string, 0x%x.", E_OUTOFMEMORY);
			return;
		}
		StringCchCopy(m_szSourceFile, MAX_PATH, sFileDlg.GetPathName());
		// do the source file stuff
		if (FAILED(OpenSourceFile()))
		{
			MessageBoxEx(m_hWnd, L"Error opening source file.", NULL, MB_OK | MB_ICONERROR, NULL);
		}
	}	
}

// called when you click file->save as, comments inline
void CDVREditDlg::OnFileSaveas()
{
	// show only dvr-ms and dvr files
	CFileDialog sFileDlg(FALSE, NULL, NULL, OFN_NOREADONLYRETURN | OFN_PATHMUSTEXIST, L"Recorded TV Files (*.wtv;*.dvr-ms;*.dvr)|*.wtv;*.dvr-ms;*.dvr||", this);
        
	// show dialog for real
	if (IDOK == sFileDlg.DoModal())
	{
		m_szDestFile = new OLECHAR[MAX_PATH];
		if (m_szDestFile == NULL)
		{
			DisplayError(m_hWnd, L"Out of memory creating destination file string, 0x%x.", E_OUTOFMEMORY);
			return;
		}
		StringCchCopy(m_szDestFile, MAX_PATH, sFileDlg.GetPathName());

		if ((!wcsstr(m_szDestFile, L".dvr")) || (!wcsstr(m_szDestFile, L".wtv")))
		{
			if FAILED(StringCchCat(m_szDestFile, MAX_PATH, L".dvr-ms"))
			{
				MessageBoxEx(m_hWnd, L"Error saving destination file, failed to add .dvr-ms/.wtv to end of file name.", NULL, MB_OK | MB_ICONERROR, NULL);
				return;
			}
		}
		// start saving task
		if (FAILED(OpenDestFile()))
		{
			MessageBoxEx(m_hWnd, L"Error saving destination file.", NULL, MB_OK | MB_ICONERROR, NULL);
		}
	}	
}

// called when the user clicks the play button
void CDVREditDlg::OnBnClickedButton1()
{
	static OAFilterState state;
	m_pGraph.GetState(&state);
	
	// if we're already running, and you pressed play, we pause
	if (state == State_Running)
	{
		m_pGraph.Pause(m_hWnd);
	}

	// else we play
	else m_pGraph.Run(m_hWnd);
	ReadMediaPosition();
	m_Seekbar.SetFocus();
}

// called when the user clicks the stop button
void CDVREditDlg::OnBnClickedButton3()
{
	m_pGraph.Stop(m_hWnd);
	ReadMediaPosition();
	m_Seekbar.SetFocus();
}

// called when the user clicks the pause button
void CDVREditDlg::OnBnClickedButton4()
{
	static OAFilterState state;
	m_pGraph.GetState(&state);
	
	// if we're playing, pause, else do nothing
	if (state == State_Running)
	{
		m_pGraph.Pause(m_hWnd);
	}

	m_pGraph.StepForward(m_hWnd);
	ReadMediaPosition();
	m_Seekbar.SetFocus();
}

// skip forward 30 seconds
void CDVREditDlg::OnBnClickedButtonCommercialSkip()
{
	m_pGraph.CommercialSkip(m_hWnd);
	ReadMediaPosition();
	m_Seekbar.SetFocus();
}

// step back a little bit
void CDVREditDlg::OnBnClickedButtonStepBackward()
{
	static OAFilterState state;
	m_pGraph.GetState(&state);
	
	if (state == State_Running)
	{
		m_pGraph.Pause(m_hWnd);
	}

	m_pGraph.StepBackward(m_hWnd);
	ReadMediaPosition();
}

// timer fires every 50ms to update the location of the seekbar
void CDVREditDlg::StartSeekTimer() 
{
    // Cancel any pending timer event
    StopSeekTimer();

    // Create a new timer
    m_wTimerID = SetTimer(TIMERID, TICKLEN, NULL);
}

// stops the timer
void CDVREditDlg::StopSeekTimer() 
{
    // Cancel the timer
    if(m_wTimerID)        
    {                
        KillTimer(m_wTimerID);
        m_wTimerID = 0;
    }
}

// called every time the timer fires
void CDVREditDlg::OnTimer(UINT_PTR nIDEvent) 
{
    ReadMediaPosition();

    CDialog::OnTimer(nIDEvent);
}

// keeps video and seekbar in sync, comments inline
void CDVREditDlg::ReadMediaPosition()
{
    HRESULT hr;
    REFERENCE_TIME rtNow;

    // Read the current stream position
    hr = m_pGraph.GetCurrentPosition(&rtNow);
    if (FAILED(hr))
        return;

	// set the seekbar to that postion, seekbar is in seconds, graph is in reference time
	m_Seekbar.SetPos(double(rtNow) / dblUNITS);
	
	// update the status bar	
	wchar_t szRTNow[33];
	_i64tow(rtNow, szRTNow, 10);

	m_StatusBarCtrl.SetText(szRTNow, 1, 0);
	UpdateData();
}

// fires when the user moves the seekbar thumb manually
void CDVREditDlg::HandleTrackbar(WPARAM wReq)
{
    HRESULT hr;
    static OAFilterState state;

    // If the file is not seekable, the trackbar is disabled. 
    int iPosition = m_Seekbar.GetPos();
	
	// Pause when the scroll action begins.
	hr = m_pGraph.GetState(&state);

	hr = m_pGraph.Pause(m_hWnd);
    
    // Update the position continuously.
	REFERENCE_TIME rtNew = (iPosition * dblUNITS);
	REFERENCE_TIME rtEnd = m_pGraph.Duration;

	hr = m_pGraph.SetPositions(&rtNew, &rtEnd);

    // Restore the state at the end.
    if (wReq == TB_ENDTRACK)
    {
        if (state == State_Stopped)
            hr = m_pGraph.Stop(m_hWnd);
        else if (state == State_Running) 
            hr = m_pGraph.Run(m_hWnd);
    }

	// set the status bar info
	REFERENCE_TIME refTime;
	m_pGraph.GetCurrentPosition(&refTime);

	wchar_t refTimeLabel[65];
	_i64tow(refTime, refTimeLabel, 10);
	SetDlgItemText(IDC_EDIT_REF_TIME, refTimeLabel);
}

// handles movent and keyboard input, also remote control input
LRESULT CDVREditDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    // Field notifications from the DirectShow filter graph manager
    // and those posted by the application
    switch (message)
    {
        case WM_HSCROLL:
            HandleTrackbar(LOWORD(wParam));
            break;
		
		case WM_APPCOMMAND:
			switch (GET_APPCOMMAND_LPARAM(lParam))
			{
				case APPCOMMAND_MEDIA_PLAY_PAUSE:
				case APPCOMMAND_MEDIA_PLAY:
				case APPCOMMAND_MEDIA_PAUSE:
					OnBnClickedButton1();
					return TRUE;

				case APPCOMMAND_MEDIA_STOP:
					OnBnClickedButton3();
					return TRUE;

				case APPCOMMAND_MEDIA_RECORD:
					OnFileSaveas();
					return TRUE;

				case APPCOMMAND_MEDIA_PREVIOUSTRACK:
					OnBnClickedButtonSegmentStart();
					return TRUE;

				case APPCOMMAND_MEDIA_NEXTTRACK:
					OnBnClickedButtonSegmentEnd();
					return TRUE;
				
				case APPCOMMAND_MEDIA_CHANNEL_DOWN:
					OnBnClickedButtonNextSegment();
					return TRUE;

				case APPCOMMAND_MEDIA_CHANNEL_UP:
					OnBnClickedButtonPreviousSegment();
					return TRUE;
			}
			break;
	
    }

    return CDialog::WindowProc(message, wParam, lParam);
}

// when you click help-> about (duh)
void CDVREditDlg::OnHelpAbout()
{
	CAboutDlg aboutDlg(this);
	aboutDlg.DoModal();
}

// when you click tools -> dvr2mpg, does nothing if not debug build
void CDVREditDlg::OnToolsDvr2Mpg()
{
	MessageBoxEx(m_hWnd, L"Not yet implemented.", L"NYI", MB_OK | MB_ICONWARNING, NULL);
#ifdef DEBUG
	Dvr2MPG dvr2mpg(AfxGetApp()->m_pMainWnd);
	dvr2mpg.DoModal();
#endif
}

// when you click tools -> mpg2dvr, kicks off that dialog
void CDVREditDlg::OnToolsMpg2Dvr()
{
	MPG2Dvr mpg2dvr(AfxGetApp()->m_pMainWnd);
	mpg2dvr.DoModal();
}

// not used
void CDVREditDlg::OnBnClickedButtonStart()
{
	m_pGraph.GetCurrentPosition(&m_rtStart);
	
	UpdateData();

	m_Seekbar.ClearSel(FALSE);
	m_Seekbar.SetSelection(double(m_rtStart) / dblUNITS, double(m_rtStop) / dblUNITS);
	m_Seekbar.SetFocus();
}

// not used
void CDVREditDlg::OnBnClickedButtonEnd()
{
	m_pGraph.GetCurrentPosition(&m_rtStop);
	
	UpdateData();
	
	m_Seekbar.ClearSel(FALSE);
	m_Seekbar.SetSelection(double(m_rtStart) / dblUNITS, double(m_rtStop) / dblUNITS);
	m_Seekbar.SetFocus();
}

// when you click the "in" button...complicated, comments inline
void CDVREditDlg::OnBnClickedButtonIn()
{
	REFERENCE_TIME rtIn;
	// start by grabbing the current position on the graph
	m_pGraph.GetCurrentPosition(&rtIn);

	// we have to have a current segment
	if (m_pCurrentSegment)
	{
		// make sure that the in time is at least two seconds before the out time
		if (rtIn >= (m_pCurrentSegment->OutTime() - (2*dblUNITS)))
		{
			// check to make sure there aren't too many segments
			INT_PTR iIndex = m_pSegmentQueue->GetSize();

			// 0x100 is alot of segments.
			if (iIndex == 0x100)
			{
				MessageBoxEx(m_hWnd, L"Max number of segments is 256.", NULL, MB_OK | MB_ICONERROR, NULL);
				return;
			}
			
			// enable the previous segment button
			(GetDlgItem(IDC_BUTTON_PREVIOUS_SEGMENT))->EnableWindow(TRUE);
	
			// get the current segment number
			iIndex = GetDlgItemInt(IDC_SEGMENT_NUMBER);
			// check to see if this is a new segment
			POSITION pos = m_pSegmentQueue->FindIndex(iIndex);
			if (pos == 0)
			{
				// if it is, add the segment to the queue
				m_pSegmentQueue->Enqueue(m_pCurrentSegment);
			}
			else 
			{
				// this is an old segment, replace the segment object in the queue with the new one
				CSegment* pTempSegment = m_pSegmentQueue->GetAt(pos);
				pTempSegment->SetInTime(m_pCurrentSegment->InTime());
				pTempSegment->SetOutTime(m_pCurrentSegment->OutTime());
				m_pSegmentQueue->SetAt(pos, pTempSegment);
			}

			// now get the total number of setments
			iIndex = m_pSegmentQueue->GetSize();
			// make a new segment
			m_pCurrentSegment = new CSegment(m_rtStart, m_rtStop);
			if (m_pCurrentSegment == NULL)
			{
				DisplayError(m_hWnd, L"Out of memory creating new segment, 0x%x", E_OUTOFMEMORY);
				return;
			}
			
			// enable the in and out buttons
			(GetDlgItem(IDC_BUTTON_IN))->EnableWindow(TRUE);
			(GetDlgItem(IDC_BUTTON_OUT))->EnableWindow(TRUE);

			// set the segment number to the last segment (current segment)
			SetDlgItemInt(IDC_SEGMENT_NUMBER, iIndex);
			// enable the clear button
			(GetDlgItem(IDC_BUTTON_CLEAR_SEGMENT))->EnableWindow(TRUE);
		}

		// reset the selection on the seekbar to indicate the current in point and out point
		m_Seekbar.ClearSel(TRUE);
		m_Seekbar.SetSelection(double(rtIn) / dblUNITS, double(m_pCurrentSegment->OutTime()) / dblUNITS);
		
		// now, with the new segment, initialize the in point
		if (m_pCurrentSegment->SetInTime(rtIn))
		{
			// set the spot where the in point is on the seekbar
			m_Seekbar.SetTic(double(rtIn) / dblUNITS);
			UpdateData();
		}

		// update the status bar
		CString csHMS;
		RefTimeToHMS(m_pCurrentSegment->SegmentLength(), &csHMS);
		m_StatusBarCtrl.SetText((LPCWSTR) csHMS, 3, 0);

		RefTimeToHMS(m_pSegmentQueue->TotalTimeInRefTime() + m_pCurrentSegment->SegmentLength(), &csHMS);
		m_StatusBarCtrl.SetText((LPCWSTR) csHMS, 5, 0);	
	}
	// set the focus to the seekbar
	m_Seekbar.SetFocus();
}

// when you click the out button, complicated, comments inline
void CDVREditDlg::OnBnClickedButtonOut()
{
	REFERENCE_TIME rtOut;
	// get the current position from the graph
	m_pGraph.GetCurrentPosition(&rtOut);

	// gotta have a current segment
	if (m_pCurrentSegment)
	{
		// out point must be at least 2 seconds after in point
		if (rtOut <= (m_pCurrentSegment->InTime() - (2*dblUNITS)))
		{
			MessageBoxEx(m_hWnd, L"Out time must be at least 2 seconds after in time.", NULL, MB_OK | MB_ICONERROR, NULL);
			m_Seekbar.SetFocus();
			return;
		}

		// clear the seekbar selection and set it back to reset
		m_Seekbar.ClearSel(TRUE);
		m_Seekbar.SetSelection(double(m_pCurrentSegment->InTime()) / dblUNITS, double(rtOut) / dblUNITS);

		// set the out point in the current segment
		if (m_pCurrentSegment->SetOutTime(rtOut))
		{
			// update the seekbar with the spot that the out point is at
			m_Seekbar.SetTic(double(rtOut) / dblUNITS);
			UpdateData();
		}
	
		// update the status bar
		CString csHMS;
		RefTimeToHMS(m_pCurrentSegment->SegmentLength(), &csHMS);
		m_StatusBarCtrl.SetText((LPCWSTR) csHMS, 3, 0);

		if (m_pSegmentQueue)
			RefTimeToHMS(m_pSegmentQueue->TotalTimeInRefTime() + m_pCurrentSegment->SegmentLength(), &csHMS);
		m_StatusBarCtrl.SetText((LPCWSTR) csHMS, 5, 0);	
	}		
		
	// focus goes to the seekbar
	m_Seekbar.SetFocus();
}

// removes the current segment from the segment queue
void CDVREditDlg::OnBnClickedClear()
{
	if (m_pSegmentQueue)
	{
		m_pSegmentQueue->RemoveAll();
	}

	SetDlgItemInt(IDC_SEGMENT_NUMBER, 0);

	m_pCurrentSegment->SetInTime(m_rtStart);
	m_pCurrentSegment->SetOutTime(m_rtStop);

	m_Seekbar.ClearTics(TRUE);
	m_Seekbar.SetFocus();

	(GetDlgItem(IDC_BUTTON_IN))->EnableWindow(TRUE);
	(GetDlgItem(IDC_BUTTON_OUT))->EnableWindow(TRUE);
}

// not used
void CDVREditDlg::OnBnClickedButtonNewSegment()
{
	(GetDlgItem(IDC_BUTTON_PREVIOUS_SEGMENT))->EnableWindow(TRUE);

	INT_PTR iIndex = m_pSegmentQueue->GetSize();

	if (iIndex == 0x100)
	{
		MessageBoxEx(m_hWnd, L"Max number of segments is 256.", NULL, MB_OK | MB_ICONERROR, NULL);
		return;
	}

	INT_PTR iIndex2 = GetDlgItemInt(IDC_SEGMENT_NUMBER);
	POSITION pos = m_pSegmentQueue->FindIndex(iIndex2);
	if (pos == 0)
	{
		m_pSegmentQueue->Enqueue(m_pCurrentSegment);
	}
	else 
	{
		CSegment* pTempSegment = m_pSegmentQueue->GetAt(pos);
		pTempSegment->SetInTime(m_pCurrentSegment->InTime());
		pTempSegment->SetOutTime(m_pCurrentSegment->OutTime());
		m_pSegmentQueue->SetAt(pos, pTempSegment);
	}

	m_pCurrentSegment = new CSegment(m_rtStart, m_rtStop);
	
	m_Seekbar.ClearSel(TRUE);
	m_Seekbar.SetFocus();

	(GetDlgItem(IDC_BUTTON_IN))->EnableWindow(TRUE);
	(GetDlgItem(IDC_BUTTON_OUT))->EnableWindow(TRUE);

	SetDlgItemInt(IDC_SEGMENT_NUMBER, iIndex);
	(GetDlgItem(IDC_BUTTON_CLEAR_SEGMENT))->EnableWindow(TRUE);
	UpdateData();
}

// Goes back one segment, comments inline
void CDVREditDlg::OnBnClickedButtonPreviousSegment()
{
	(GetDlgItem(IDC_BUTTON_PREVIOUS_SEGMENT))->EnableWindow(FALSE);

	// current segment number
	INT_PTR iIndex = GetDlgItemInt(IDC_SEGMENT_NUMBER);

	// is the segment already in the queue?
	POSITION pos = m_pSegmentQueue->FindIndex(iIndex);
	if (pos == 0)
	{
		// if not, put it in at the end
		m_pSegmentQueue->Enqueue(m_pCurrentSegment);
	}
	else 
	{
		// else put it in at it's original position
		CSegment* pTempSegment = m_pSegmentQueue->GetAt(pos);
		pTempSegment->SetInTime(m_pCurrentSegment->InTime());
		pTempSegment->SetOutTime(m_pCurrentSegment->OutTime());
		m_pSegmentQueue->SetAt(pos, pTempSegment);
	}

	// subtract one from the segment number
	iIndex--;

	// get the segment at the new segment number
	pos = m_pSegmentQueue->FindIndex(iIndex);
	m_pCurrentSegment = m_pSegmentQueue->GetAt(pos);

	// set the segment number in the dialog
	SetDlgItemInt(IDC_SEGMENT_NUMBER, iIndex);
	
	// enable the next button, and the previous button IF there are previous segments
	(GetDlgItem(IDC_BUTTON_NEXT_SEGMENT))->EnableWindow(TRUE);

	if (iIndex > 0)
		(GetDlgItem(IDC_BUTTON_PREVIOUS_SEGMENT))->EnableWindow(TRUE);

	// update seekbar and status bar
	CString csHMS;
	RefTimeToHMS(m_pCurrentSegment->SegmentLength(), &csHMS);
	m_StatusBarCtrl.SetText((LPCWSTR) csHMS, 3, 0);

	m_Seekbar.ClearSel(TRUE);
	m_Seekbar.SetSelection( double(m_pCurrentSegment->InTime()) / dblUNITS, double(m_pCurrentSegment->OutTime()) / dblUNITS );
	m_Seekbar.SetFocus();
}

// goes forward one segment, comments inline
void CDVREditDlg::OnBnClickedButtonNextSegment()
{
	(GetDlgItem(IDC_BUTTON_NEXT_SEGMENT))->EnableWindow(FALSE);
	
	// get current segment number
	INT_PTR iIndex = GetDlgItemInt(IDC_SEGMENT_NUMBER);

	// grab the segment at the current segment number, and update it
	POSITION pos = m_pSegmentQueue->FindIndex(iIndex);
	CSegment* pTempSegment = m_pSegmentQueue->GetAt(pos);
	pTempSegment->SetInTime(m_pCurrentSegment->InTime());
	pTempSegment->SetOutTime(m_pCurrentSegment->OutTime());
	m_pSegmentQueue->SetAt(pos, pTempSegment);

	// increment the segment number
	iIndex++;

	// get the segment at the new segment number
	pos = m_pSegmentQueue->FindIndex(iIndex);
	m_pCurrentSegment = m_pSegmentQueue->GetAt(pos);

	// set the new segment number in the dialog
	SetDlgItemInt(IDC_SEGMENT_NUMBER, iIndex);
	
	// enable the next button if there are more segments
	if (m_pSegmentQueue->GetSize() > iIndex + 1)
		(GetDlgItem(IDC_BUTTON_NEXT_SEGMENT))->EnableWindow(TRUE);

	// enable the previous button if there are previous segments
	if (iIndex > 0)
		(GetDlgItem(IDC_BUTTON_PREVIOUS_SEGMENT))->EnableWindow(TRUE);

	// update seekbar and status bar
	CString csHMS;
	RefTimeToHMS(m_pCurrentSegment->SegmentLength(), &csHMS);
	m_StatusBarCtrl.SetText((LPCWSTR) csHMS, 3, 0);

	m_Seekbar.ClearSel(TRUE);
	m_Seekbar.SetSelection( double(m_pCurrentSegment->InTime()) / dblUNITS, double(m_pCurrentSegment->OutTime()) / dblUNITS );
	m_Seekbar.SetFocus();
}

// delete the current segment, comments inline
void CDVREditDlg::OnBnClickedButtonClearSegment()
{
	// see if the current segment is already in the queue
	INT_PTR iIndex = GetDlgItemInt(IDC_SEGMENT_NUMBER);
	POSITION pos = m_pSegmentQueue->FindIndex(iIndex);

	if (pos != 0)
	{
		// if so, delete it
		m_pSegmentQueue->RemoveAt(pos);
	}

	// else, set the in and out points to the default 0 and end of show
	m_pCurrentSegment->SetInTime(m_rtStart);
	m_pCurrentSegment->SetOutTime(m_rtStop);

	// if the segment queue is now zero, disable the clear button
	if (m_pSegmentQueue->GetSize() == 0)
	{	
		iIndex = 0;
		(GetDlgItem(IDC_BUTTON_CLEAR_SEGMENT))->EnableWindow(FALSE);
	}

	// if the segment queue is now one, disable the next button
	if (m_pSegmentQueue->GetSize() == 1)
		(GetDlgItem(IDC_BUTTON_NEXT_SEGMENT))->EnableWindow(FALSE);

	// decrement the segment number if it's not zero
	if (iIndex != 0)
		iIndex--;

	// load the last segment, if it exists
	pos = m_pSegmentQueue->FindIndex(iIndex);
	if (pos)
		m_pCurrentSegment = m_pSegmentQueue->GetAt(pos);

	// set the segment number in the dialog
	SetDlgItemInt(IDC_SEGMENT_NUMBER, iIndex);

	// disable the previous button if the segment queue is empty
	if (iIndex == 0)
		(GetDlgItem(IDC_BUTTON_PREVIOUS_SEGMENT))->EnableWindow(FALSE);

	// update seekbar and status bar
	CString csHMS;
	RefTimeToHMS(m_pCurrentSegment->SegmentLength(), &csHMS);
	m_StatusBarCtrl.SetText((LPCWSTR) csHMS, 3, 0);

	RefTimeToHMS(m_pSegmentQueue->TotalTimeInRefTime(), &csHMS);
	m_StatusBarCtrl.SetText((LPCWSTR) csHMS, 5, 0);	

	m_Seekbar.ClearSel(TRUE);
	m_Seekbar.SetSelection( double(m_pCurrentSegment->InTime()) / dblUNITS, double(m_pCurrentSegment->OutTime()) / dblUNITS );
	m_Seekbar.SetFocus();
}

void CDVREditDlg::OnBnClickedButtonLoad()
{
	OnFileOpen();
}

void CDVREditDlg::OnBnClickedButtonSave()
{
	OnFileSaveas();
}

// makes seekbar and graph jump to start of current segment
void CDVREditDlg::OnBnClickedButtonSegmentStart()
{
	m_pGraph.Pause(m_hWnd);
	REFERENCE_TIME rtJump = m_pCurrentSegment->InTime();

	m_pGraph.SetPositions(&rtJump, &m_rtStop);
	ReadMediaPosition();
	m_Seekbar.SetFocus();
}

// makes seekbar and graph just to end of current segment
void CDVREditDlg::OnBnClickedButtonSegmentEnd()
{
	m_pGraph.Pause(m_hWnd);
	REFERENCE_TIME rtJump = m_pCurrentSegment->OutTime();

	m_pGraph.SetPositions(&rtJump, &m_rtStop);
	ReadMediaPosition();
	m_Seekbar.SetFocus();
}

// the rest is just keyboard and remote shortcut mappings
void CDVREditDlg::OnAcceleratorPause()
{
	OnBnClickedButton1();
}

void CDVREditDlg::OnAcceleratorPlay()
{
	OnBnClickedButton1();
}

void CDVREditDlg::OnAcceleratorRecord()
{
	OnFileSaveas();
}

void CDVREditDlg::OnAcceleratorBack()
{
	OnBnClickedButtonStart();
}

void CDVREditDlg::OnAcceleratorForward()
{
	OnBnClickedButtonEnd();
}

void CDVREditDlg::OnAcceleratorStop()
{
	OnBnClickedButton3();
}

void CDVREditDlg::OnAcceleratorIn()
{
	OnBnClickedButtonIn();
}

void CDVREditDlg::OnAcceleratorOut()
{
	OnBnClickedButtonOut();
}
void CDVREditDlg::OnToolsOpenReferenceRecording()
{
	CReference2Content ref2con(AfxGetApp()->m_pMainWnd);
	ref2con.DoModal();
}

void CDVREditDlg::OnToolsAppendRecordings()
{
	CCombineRecordings combine(AfxGetApp()->m_pMainWnd);
	combine.DoModal();
}

extern STDMETHODIMP CopyMetadata(LPCWSTR wszSource, IStreamBufferRecComp* pRecComp);

extern const GUID CLSID_DTFilter;

LRESULT CDVREditDlg::OnDumpThreadFinished(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = S_OK;
	if (m_hSaveThread)
	{
		while (WAIT_OBJECT_0 != WaitForSingleObject(m_hSaveThread, 1000))
		{
			Sleep(0);
		}

		GetExitCodeThread(m_hSaveThread, (LPDWORD) &hr);
		if (FAILED(hr))
			DisplayError(m_hWnd, L"Error 0x%x while dumping elementary streams.", hr);
					
		CloseHandle(m_hSaveThread);
	}
	m_csFileToDump.Empty();
	//Anything else to do here?

	return hr;
}

DWORD WINAPI DumpElementaryStreamsSaveThread(LPVOID pvThis)
{
	CDVREditDlg* pDVREditDlg = (CDVREditDlg*) pvThis;
	HRESULT hr = S_OK;

	CComPtr <IGraphBuilder> pGraph;
	hr = pGraph.CoCreateInstance(CLSID_FilterGraph);
	if (FAILED(hr))
		return hr;

	CComPtr<IBaseFilter> pSource;
	hr = pSource.CoCreateInstance(CLSID_StreamBufferSource);
	if (FAILED(hr))
		return hr;

	hr = pGraph->AddFilter(pSource, L"Source");
	if (FAILED(hr))
		return hr;

	CComQIPtr<IFileSourceFilter> pSourceFile(pSource);
	hr = pSourceFile->Load((LPCWSTR) pDVREditDlg->m_csFileToDump, NULL);
	pSourceFile.Release();
	if (FAILED(hr))
		return hr;

	CString csBaseFilename = pDVREditDlg->m_csFileToDump;
	csBaseFilename.TrimRight(L".dvr-ms");

	CComPtr<IEnumPins> pEnumPins;
	hr = pSource->EnumPins(&pEnumPins);
	if (FAILED(hr))
		return hr;

	int i = 0;
	CComPtr<IPin> pOutPin;
	while(pEnumPins->Next(1, &pOutPin, 0) == S_OK)
	{
		i++;
		CComPtr<IBaseFilter> pDecrypter;
		hr = pDecrypter.CoCreateInstance(CLSID_DTFilter);
		if (FAILED(hr))
			return hr;

		hr = pGraph->AddFilter(pDecrypter, L"Decrypter");
		if (FAILED(hr))
			return hr;

		CComPtr<IPin> pInPin;
		hr = FindPinOnFilter(pDecrypter, &MEDIATYPE_NULL, PINDIR_INPUT, FALSE, &pInPin);
		if (FAILED(hr))
			return hr;

		hr = pGraph->ConnectDirect(pOutPin, pInPin, NULL);
		pInPin.Release();
		if (FAILED(hr))
		{
			if (hr == VFW_E_NO_ACCEPTABLE_TYPES)
			{
				hr = pGraph->RemoveFilter(pDecrypter);
				if (FAILED(hr))
					return hr;

				pDecrypter.Release();
			}
			else return hr;
		}

		else
		{
			pOutPin.Release();

			hr = FindPinOnFilter(pDecrypter, &MEDIATYPE_NULL, PINDIR_OUTPUT, FALSE, &pOutPin);
		}
		
		extern IBaseFilter * CreateDumpFilter(LPUNKNOWN pUnk, HRESULT* phr);
		CComPtr<IBaseFilter> pDump;// = CreateDumpFilter(NULL, &hr);
		hr = pDump.CoCreateInstance(CLSID_Dump);
		if (FAILED(hr))
			return hr;

		hr = pGraph->AddFilter(pDump, L"Dump Filter");
		if (FAILED(hr))
			return hr;

		CComPtr<IEnumMediaTypes> pEnum;
		hr = pOutPin->EnumMediaTypes(&pEnum);
		if (FAILED(hr))
			return hr;

		AM_MEDIA_TYPE* pmt;
		hr = pEnum->Next(1, &pmt, NULL);
		if (FAILED(hr))
			return hr;

		pEnum.Release();

		WCHAR buf[MAX_PATH] = L"\0";
		_itow(i, buf, 10);
		csBaseFilename.Append(buf);
		CComQIPtr<IFileSinkFilter> pFileSink(pDump);
		hr = pFileSink->SetFileName((LPCWSTR) csBaseFilename, pmt);
		pFileSink.Release();
		if (FAILED(hr))
			return hr;

		hr = FindPinOnFilter(pDump, &MEDIATYPE_NULL, PINDIR_INPUT, FALSE, &pInPin);
		if (FAILED(hr))
			return hr;

		hr = pGraph->ConnectDirect(pOutPin, pInPin, NULL);
		if (FAILED(hr))
			return hr;

		pOutPin.Release();
		pInPin.Release();
	}

	pEnumPins.Release();

	hr = CoCreateInstance(CLSID_ProgressDialog, NULL, CLSCTX_INPROC_SERVER, IID_IProgressDialog, (void**) &pDVREditDlg->m_pSaveDialog);
	if (FAILED(hr))
		return hr;

	hr = pDVREditDlg->m_pSaveDialog->StartProgressDialog(pDVREditDlg->m_hWnd, NULL, PROGDLG_MODAL | PROGDLG_AUTOTIME, NULL);
	if (FAILED(hr))
		return hr;

	hr = pDVREditDlg->m_pSaveDialog->SetLine(1, L"Now dumping file:", FALSE, NULL);
	if (FAILED(hr))
		return hr;

	hr = pDVREditDlg->m_pSaveDialog->SetLine(2, (LPCWSTR) pDVREditDlg->m_csFileToDump, TRUE, NULL);
	if (FAILED(hr))
		return hr;

	CComQIPtr<IStreamBufferMediaSeeking> pSeek(pSource);
	LONGLONG Duration = 0;
	LONGLONG CurrentPosition = 0;
	hr = pSeek->GetDuration(&Duration);
	if (FAILED(hr))
		return hr;

	hr = pDVREditDlg->m_pSaveDialog->SetProgress(double(CurrentPosition) / dblUNITS, double(Duration) / dblUNITS);
	if (FAILED(hr))
		return hr;

	CComQIPtr<IMediaControl> pGraphControl(pGraph);
	hr = pGraphControl->Run();
	if (FAILED(hr))
		return hr;

	CComQIPtr<IMediaEvent> pMediaEvent(pGraph);	
	long evcode = S_OK;
		
	hr = E_ABORT;

	while (hr == E_ABORT)
	{
		hr = pMediaEvent->WaitForCompletion(500, &evcode);
			
		if ((hr != E_ABORT) && (evcode == EC_COMPLETE))
		{
			hr = pGraphControl->Stop();
			pGraphControl.Release();
		}

		hr = pSeek->GetCurrentPosition(&CurrentPosition);
		if (FAILED(hr))
			return hr;

		hr = pDVREditDlg->m_pSaveDialog->SetProgress(double(CurrentPosition) / dblUNITS, double(Duration) / dblUNITS);
		if (FAILED(hr))
			return hr;

		if (pGraphControl)
			hr = E_ABORT;
	}

	pMediaEvent.Release();
	
	pSeek.Release();
	pSource.Release();

	hr = pDVREditDlg->m_pSaveDialog->StopProgressDialog();
	if (FAILED(hr))
		return hr;

	hr = pDVREditDlg->m_pSaveDialog->Release();
	if (FAILED(hr))
		return hr;

	PostMessage(pDVREditDlg->m_hWnd, WM_DUMPTHREAD_FINISHED, NULL, NULL);
	
	return hr;
}

void CDVREditDlg::OnToolsDumpElementaryStreams()
{
	CFileDialog sFileDlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, L"Recorded TV (*.wtv;*.dvr-ms)|*.wtv;*.dvr-ms||", this);
	if (IDOK == sFileDlg.DoModal())
	{
		m_csFileToDump = L"";
		m_csFileToDump = sFileDlg.GetPathName();

		DWORD dwTID = 0;
		m_hSaveThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) DumpElementaryStreamsSaveThread, this, 0, &dwTID);
		if (m_hSaveThread == NULL)
		{
			DisplayError(m_hWnd, L"Error 0x%x creating save thread!", HRESULT_FROM_WIN32(GetLastError()));
        }
	}		
}

void CDVREditDlg::OnToolsDefaultDecoders()
{
	CDefaultDecoder decoder(AfxGetApp()->m_pMainWnd);
	decoder.DoModal();
}

void CDVREditDlg::OnToolsEditmetadata()
{
	CMetadataEditor editor(AfxGetApp()->m_pMainWnd);
	editor.DoModal();
}
