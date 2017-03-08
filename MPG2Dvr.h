#pragma once

#include <dshow.h>
#include <atlbase.h>
#include <sbe.h>
#include "filever.h"
#include "bdaiface.h"
#include "resource.h"
#include <initguid.h>
#include <dshowasf.h>

// {C1F400A4-3F08-11d3-9F0B-006008039E37}
//DEFINE_GUID(CLSID_NullRenderer,
//0xC1F400A4, 0x3f08, 0x11d3, 0x9f, 0x0b, 0x00, 0x60, 0x08, 0x03, 0x9e, 0x37);

static
WAVEFORMATEX
g_AC3WaveFormatEx = {
    0,    //  wFormatTag
    2,                      //  nChannels
    48000,                  //  nSamplesPerSec (others: 96000)
    0,                      //  nAvgBytesPerSec
    0,                      //  nBlockAlign
    16,                     //  wBitsPerSample (others: 20, 24, 0)
    0                       //  cbSize
} ;

static
BYTE
g_ATSCVideoFormat [] = {
    0x00, 0x00, 0x00, 0x00,                         //  .hdr.rcSource.left              = 0x00000000
    0x00, 0x00, 0x00, 0x00,                         //  .hdr.rcSource.top               = 0x00000000
    0xC0, 0x02, 0x00, 0x00,                         //  .hdr.rcSource.right             = 0x000002c0
    0xE0, 0x01, 0x00, 0x00,                         //  .hdr.rcSource.bottom            = 0x000001e0
    0x00, 0x00, 0x00, 0x00,                         //  .hdr.rcTarget.left              = 0x00000000
    0x00, 0x00, 0x00, 0x00,                         //  .hdr.rcTarget.top               = 0x00000000
    0x00, 0x00, 0x00, 0x00,                         //  .hdr.rcTarget.right             = 0x00000000
    0x00, 0x00, 0x00, 0x00,                         //  .hdr.rcTarget.bottom            = 0x00000000
    0x00, 0x1B, 0xB7, 0x00,                         //  .hdr.dwBitRate                  = 0x00b71b00
    0x00, 0x00, 0x00, 0x00,                         //  .hdr.dwBitErrorRate             = 0x00000000
    0xB1, 0x8B, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, //  .hdr.AvgTimePerFrame            = 0x0000000000028bb1
    0x00, 0x00, 0x00, 0x00,                         //  .hdr.dwInterlaceFlags           = 0x00000000
    0x00, 0x00, 0x00, 0x00,                         //  .hdr.dwCopyProtectFlags         = 0x00000000
    0x10, 0x00, 0x00, 0x00,                         //  .hdr.dwPictAspectRatioX         = 0x00000010
    0x09, 0x00, 0x00, 0x00,                         //  .hdr.dwPictAspectRatioY         = 0x00000009
    0x00, 0x00, 0x00, 0x00,                         //  .hdr.dwReserved1                = 0x00000000
    0x00, 0x00, 0x00, 0x00,                         //  .hdr.dwReserved2                = 0x00000000
    0x28, 0x00, 0x00, 0x00,                         //  .hdr.bmiHeader.biSize           = 0x00000028
    0xC0, 0x02, 0x00, 0x00,                         //  .hdr.bmiHeader.biWidth          = 0x000002c0
    0xE0, 0x01, 0x00, 0x00,                         //  .hdr.bmiHeader.biHeight         = 0x000001e0
    0x00, 0x00,                                     //  .hdr.bmiHeader.biPlanes         = 0x0000
    0x00, 0x00,                                     //  .hdr.bmiHeader.biBitCount       = 0x0000
    0x00, 0x00, 0x00, 0x00,                         //  .hdr.bmiHeader.biCompression    = 0x00000000
    0x00, 0x00, 0x00, 0x00,                         //  .hdr.bmiHeader.biSizeImage      = 0x00000000
    0xD0, 0x07, 0x00, 0x00,                         //  .hdr.bmiHeader.biXPelsPerMeter  = 0x000007d0
    0x42, 0xD8, 0x00, 0x00,                         //  .hdr.bmiHeader.biYPelsPerMeter  = 0x0000d842
    0x00, 0x00, 0x00, 0x00,                         //  .hdr.bmiHeader.biClrUsed        = 0x00000000
    0x00, 0x00, 0x00, 0x00,                         //  .hdr.bmiHeader.biClrImportant   = 0x00000000
    0xC0, 0x27, 0xC8, 0x00,                         //  .dwStartTimeCode                = 0x00c827c0
    0x4C, 0x00, 0x00, 0x00,                         //  .cbSequenceHeader               = 0x0000004c
    0xFF, 0xFF, 0xFF, 0xFF,                         //  .dwProfile                      = 0xffffffff
    0xFF, 0xFF, 0xFF, 0xFF,                         //  .dwLevel                        = 0xffffffff
    0x00, 0x00, 0x00, 0x00,                         //  .Flags                          = 0x00000000
                                                    //  .dwSequenceHeader [1]
    0x00, 0x00, 0x01, 0xB3, 0x2C, 0x01, 0xE0, 0x37,
    0x1D, 0x4C, 0x23, 0x81, 0x10, 0x11, 0x11, 0x12,
    0x12, 0x12, 0x13, 0x13, 0x13, 0x13, 0x14, 0x14,
    0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x15, 0x15,
    0x15, 0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x16,
    0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17,
    0x18, 0x18, 0x18, 0x19, 0x18, 0x18, 0x18, 0x19,
    0x1A, 0x1A, 0x1A, 0x1A, 0x19, 0x1B, 0x1B, 0x1B,
    0x1B, 0x1B, 0x1C, 0x1C, 0x1C, 0x1C, 0x1E, 0x1E,
    0x1E, 0x1F, 0x1F, 0x21, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
} ;

// CAttributesDialog dialog

extern int DisplayError(HWND hWnd, wchar_t* pszMessage, HRESULT hr);

class CAttributesDialog : public CDialog
{
	DECLARE_DYNAMIC(CAttributesDialog)

public:
	CAttributesDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAttributesDialog();

// Dialog Data
	enum { IDD = IDD_ATTRIBUTES_DIALOG };
// Dialog Data
  //{{AFX_DATA(CAttributesDialog)
  CString m_csSubTitle;
  CString m_csTitle;
  CString m_csGenre;
  CString m_csDescription;
  CString m_csMediaCredits;
  //}}AFX_DATA

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CToolTipCtrl* m_pToolTip;
	virtual BOOL OnInitDialog(void);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
};

// MPG2Dvr dialog

class MPG2Dvr : public CDialog
{
	DECLARE_DYNAMIC(MPG2Dvr)

public:
	MPG2Dvr(CWnd* pParent = NULL);   // standard constructor
	virtual ~MPG2Dvr();

// Dialog Data
	enum { IDD = IDD_MPG2DVR };
	CComQIPtr<IMediaEvent> m_pMediaEvent;
	CComPtr<IMediaControl> m_pGraphC;
	CComQIPtr<IStreamBufferRecordControl> m_pRecControl;
	IProgressDialog* m_pProgressDialog;
	DWORD m_pDurationInSeconds;

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CComPtr <IBaseFilter> m_pSource;
	CComPtr <IBaseFilter> m_pAudioSource;
	CComPtr <IBaseFilter> m_pStreamBufferSink;
	CComPtr <IUnknown> m_pUnk;
	CString m_csSourceFile;
	CAttributesDialog m_attrDlg;
	HRESULT OpenSourceFile(CString csSourceFile);
	HRESULT OpenDestFile(CString csDestFile);
	HRESULT ConnectPins(void);
	HRESULT ConfigureStreamBuffer(void);
	HANDLE m_hWaitHandle;
	HRESULT GetDuration(CString csSourceFile);
	void RunGraph(void);
	void AddAttributes(void);
    DECLARE_MESSAGE_MAP()
public:
	CComPtr <IGraphBuilder> m_pGraph;
	afx_msg void OnBnClickedButtonSourceBrowse();
	afx_msg void OnBnClickedButtonDestBrowse();
	afx_msg void OnBnClickedButtonAttributes();
	afx_msg void OnBnClickedOk();
	CString m_csDestFile;
	LONGLONG m_Duration;
	
};
#pragma once


