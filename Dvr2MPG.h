#pragma once

#include <dshow.h>
#include "qedit.h"
#include <atlbase.h>
#include <sbe.h>
#include "resource.h"

extern HRESULT FindPinOnFilter(IBaseFilter *pFilter, const GUID *pMajorType, PIN_DIRECTION PinDir, BOOL fConnected, IPin ** ppPin);

// Dvr2MPG dialog

class Dvr2MPG : public CDialog
{
	DECLARE_DYNAMIC(Dvr2MPG)

public:
	
	Dvr2MPG(CWnd* pParent = NULL);   // standard constructor
	virtual ~Dvr2MPG();

// Dialog Data
	enum { IDD = IDD_DVR2MPG };

protected:
	BOOL OnInitDialog(void);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CComPtr <IGraphBuilder> m_pGraph;
	CComQIPtr<IMediaEvent> m_pMediaEvent;
	CComPtr<IMediaControl> m_pGraphC;
	CComPtr <IBaseFilter> m_pSource;
	CComPtr <IBaseFilter> m_pMPEG2File;
	CComPtr <IUnknown> m_pUnk;
	CComQIPtr<IStreamBufferRecordControl> m_pRecControl;
	CString m_csSourceFile;
	CString m_csDestFile;
	LONGLONG m_Duration;
	HRESULT OpenSourceFile(CString csSourceFile);
	HRESULT OpenDestFile(CString csDestFile);
	HRESULT ConnectPins(void);
	void RunGraph(void);
	void AddAttributes(void);
    DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSourceBrowse();
	afx_msg void OnBnClickedButtonDestBrowse();
	afx_msg void OnBnClickedOk();
};
#pragma once


