#pragma once

#include "wmsdkidl.h"

// CMetadataEditor dialog

class CMetadataEditor : public CDialog
{
	DECLARE_DYNAMIC(CMetadataEditor)

public:
	CMetadataEditor(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMetadataEditor();

// Dialog Data
	enum { IDD = IDD_METADATAEDITOR };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	BOOL OnInitDialog();
	CComPtr<IWMMetadataEditor2> m_spWMEditor;
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnCbnSelchangeCombo2();
};
