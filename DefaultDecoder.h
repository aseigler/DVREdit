#pragma once


// CDefaultDecoder dialog

class CDefaultDecoder : public CDialog
{
	DECLARE_DYNAMIC(CDefaultDecoder)

public:
	CDefaultDecoder(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDefaultDecoder();

// Dialog Data
	enum { IDD = IDD_DEFAULTDECODER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL OnInitDialog();
	afx_msg void OnCbnDropdownComboVideo();
	afx_msg void OnCbnSelchangeComboVideo();
	afx_msg void OnBnClickedOk();
	HRESULT SetDefaultDecodersInRegistry(LPWSTR pVideoDecoderGUID, LPWSTR pAudioDecoderGUID);
};
