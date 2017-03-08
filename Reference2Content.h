#pragma once


// CReference2Content dialog

class CReference2Content : public CDialog
{
	DECLARE_DYNAMIC(CReference2Content)

public:
	CReference2Content(CWnd* pParent = NULL);   // standard constructor
	virtual ~CReference2Content();

// Dialog Data
	enum { IDD = IDD_REFERENCE2CONTENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CString m_csReferenceFile;
	CString m_csContentFile;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBrowseContentRecording();
	afx_msg void OnBnClickedBrowseReferenceRecording();
	afx_msg void OnBnClickedOk();
};

