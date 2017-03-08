// DVREditDlg.h : header file
//

#pragma once

#include <atlbase.h>
#include <sbe.h>
#include <initguid.h>
#include <dshow.h>
#include "resource.h"
#include "graph.h"
#include "qedit.h"
#include "MPG2Dvr.h"
#include "Dvr2MPG.h"
#include "Reference2Content.h"
#include <strsafe.h>
#include <winuser.h>
#include "CombineRecordings.h"
#include "DefaultDecoder.h"
#include "MetadataEditor.h"
//#include "DVREditDump.h"

# pragma comment(lib, "strmiids.lib")
# pragma comment(lib, "WMVCORE.lib ")
#ifdef _DEBUG
# pragma comment(lib, "strmbasd.lib")
#else
# pragma comment(lib, "strmbase.lib")
#endif

// {119CA0C3-0907-4bca-9416-A644FD45B889}
DEFINE_GUID(CLSID_DVREditDump, 
0x119ca0c3, 0x907, 0x4bca, 0x94, 0x16, 0xa6, 0x44, 0xfd, 0x45, 0xb8, 0x89);

// { 36a5f770-fe4c-11ce-a8ed-00aa002feab5 }
DEFINE_GUID(CLSID_Dump,
0x36a5f770, 0xfe4c, 0x11ce, 0xa8, 0xed, 0x00, 0xaa, 0x00, 0x2f, 0xea, 0xb5);

DEFINE_GUID(CLSID_DTFilter, 
0xC4C4C4F2, 0x0049, 0x4E2B, 0x98, 0xFB, 0x95, 0x37, 0xF6, 0xCE, 0x51, 0x6D);

#define WM_SAVETHREAD_FINISHED WM_APP + 101
#define WM_GRAPHNOTIFY WM_APP + 102
#define WM_DUMPTHREAD_FINISHED WM_APP + 103

const int TICKLEN=100, TIMERID=55;
static const double dblUNITS = 1e7;

#include "segment.h"

// CDVREditDlg dialog
class CDVREditDlg : public CDialog
{
// Construction
public:
	CDVREditDlg(CWnd* pParent = NULL);	// standard constructor
	~CDVREditDlg(void);
// Dialog Data
	enum { IDD = IDD_DVREDIT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	CSegment* m_pCurrentSegment;
		
	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual void SetupToolTips();
	CToolTipCtrl* m_pToolTip;
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	HACCEL  m_hAccelTable;
	void CopyAttributes(IProgressDialog* pProgressDialog);
	void EnablePlayback(BOOL fOnOff);
	BOOL SaveMainWindowPlacement(void);
	BOOL LoadMainWindowPlacement(void);
	BOOL fSuppressEditedBy(void);
	BOOL m_fSuppressEditedBy;
	BOOL CheckRegisterDVREdit(void);
	// Generated message map functions
	//{{AFX_MSG(CDVREditDlg)
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	HRESULT OpenSourceFile();
	HRESULT OpenDestFile();
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CGraph m_pGraph;
	REFERENCE_TIME m_rtStart, m_rtStop;
	UINT_PTR m_wTimerID;
	HANDLE m_hSaveThread;
	

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDVREditDlg)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL


public:
	BOOL fCommandLine;
	afx_msg void OnFileExit();
	afx_msg void OnFileOpen();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	void StartSeekTimer(void);
    void StopSeekTimer(void);
    void HandleTrackbar(WPARAM wReq);
    void ReadMediaPosition(void);
	CSegmentQueue* m_pSegmentQueue;
	CComPtr<IStreamBufferRecComp> m_pRecComp;
	IProgressDialog* m_pSaveDialog;
	LRESULT OnSaveThreadFinished(WPARAM wParam, LPARAM lParam);
	LRESULT OnDumpThreadFinished(WPARAM wParam, LPARAM lParam);
	LRESULT OnGraphNotify(WPARAM wParam, LPARAM lParam);
	LPWSTR m_szSourceFile;
	LPWSTR m_szDestFile;
	CString m_csFileToDump;
    // Dialog Data
	//{{AFX_DATA(CDVREditDlg)
	CStatusBarCtrl m_StatusBarCtrl;
	CSliderCtrl	m_Seekbar;
	CBitmapButton m_ButtonStepForward;
	CBitmapButton m_ButtonStepBackward;
	CBitmapButton m_ButtonSegmentPrev;
	CBitmapButton m_ButtonSegmentNext;
	CBitmapButton m_ButtonSegmentClear;
	CBitmapButton m_ButtonCommercialSkip;
	CBitmapButton m_ButtonPlaySegment;
	CBitmapButton m_ButtonPlayAll;
	CBitmapButton m_ButtonIn;
	CBitmapButton m_ButtonOut;
	CBitmapButton m_ButtonStop;
	CBitmapButton m_ButtonPlay;
	CBitmapButton m_ButtonSegmentStart;
	CBitmapButton m_ButtonSegmentEnd;
	CBitmapButton m_ButtonLoad;
	CBitmapButton m_ButtonSave;
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC *);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_DATA
	afx_msg void OnBnClickedButtonCommercialSkip();
	afx_msg void OnBnClickedButtonStepBackward();
	afx_msg void OnFileSaveas();
	afx_msg void OnHelpAbout();
	afx_msg void OnToolsDvr2Mpg();
	afx_msg void OnToolsMpg2Dvr();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonEnd();
	afx_msg void OnBnClickedButtonIn();
	afx_msg void OnBnClickedButtonOut();
	afx_msg void OnBnClickedClear();
	afx_msg void OnBnClickedButtonNewSegment();
	afx_msg void OnBnClickedButtonPreviousSegment();
	afx_msg void OnBnClickedButtonNextSegment();
	afx_msg void OnBnClickedButtonClearSegment();
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonSegmentStart();
	afx_msg void OnBnClickedButtonSegmentEnd();
	afx_msg void OnAcceleratorPause();
	afx_msg void OnAcceleratorPlay();
	afx_msg void OnAcceleratorRecord();
	afx_msg void OnAcceleratorBack();
	afx_msg void OnAcceleratorForward();
	afx_msg void OnAcceleratorStop();
	afx_msg void OnToolsDvrtoWMV();
	afx_msg void OnAcceleratorIn();
	afx_msg void OnAcceleratorOut();
	afx_msg void OnToolsOpenReferenceRecording();
	afx_msg void OnToolsAppendRecordings();
	afx_msg void OnToolsDumpElementaryStreams();
	afx_msg void OnToolsDefaultDecoders();
	afx_msg void OnToolsEditmetadata();
};
