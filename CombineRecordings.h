#pragma once
#include <atlbase.h>
#include <sbe.h>
#include <initguid.h>
#include <dshow.h>
#include "filever.h"
#include "dvreditdlg.h"

// CCombineRecordings dialog

class CCombineRecordings : public CDialog
{
	DECLARE_DYNAMIC(CCombineRecordings)

public:
	CCombineRecordings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCombineRecordings();
	CListBox m_ListBox;
	CComPtr<IStreamBufferRecComp> m_pRecComp;
	CString m_csSourceFile;
	HANDLE m_hWorkerThread;
// Dialog Data
	enum { IDD = IDD_COMBINERECORDINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg void OnBnClickedButtonCombineAdd();
	afx_msg void OnBnClickedButtonCombineRemove();
	afx_msg void OnBnClickedOk();
};
