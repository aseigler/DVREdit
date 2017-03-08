// Reference2Content.cpp : implementation file
//

#include "stdafx.h"
#include <sbe.h>
#include <dshow.h>
#include "dvredit.h"
#include "Reference2Content.h"
#include ".\reference2content.h"


// CReference2Content dialog

IMPLEMENT_DYNAMIC(CReference2Content, CDialog)
CReference2Content::CReference2Content(CWnd* pParent /*=NULL*/)
	: CDialog(CReference2Content::IDD, pParent)
{
}

CReference2Content::~CReference2Content()
{
}

void CReference2Content::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CReference2Content, CDialog)
	ON_BN_CLICKED(IDC_BROWSE_CONTENT_RECORDING, OnBnClickedBrowseContentRecording)
	ON_BN_CLICKED(IDC_BROWSE_REFERENCE_RECORDING, OnBnClickedBrowseReferenceRecording)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CReference2Content message handlers

void CReference2Content::OnBnClickedBrowseReferenceRecording()
{
	CFileDialog sFileDlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, L"Reference recordings (*.*)|*.*||", this);
	if (IDOK == sFileDlg.DoModal())
	{
		SetDlgItemText(IDC_EDIT_REFERENCE_RECORDING, sFileDlg.GetPathName());
		m_csReferenceFile = sFileDlg.GetPathName();
	}	
}

void CReference2Content::OnBnClickedBrowseContentRecording()
{
	CFileDialog sFileDlg(FALSE, NULL, NULL, OFN_NOREADONLYRETURN | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT, L"Content recordings (*.dvr-ms)|*.dvr-ms||", this);
	if (IDOK == sFileDlg.DoModal())
	{
		SetDlgItemText(IDC_EDIT_CONTENT_RECORDING, sFileDlg.GetPathName());
		m_csContentFile = sFileDlg.GetPathName();
	}	
}

void CReference2Content::OnBnClickedOk()
{
	CComPtr<IStreamBufferRecComp> pRecComp;
	HRESULT hr = pRecComp.CoCreateInstance(CLSID_StreamBufferComposeRecording);
	if (SUCCEEDED(hr))
	{
		hr = pRecComp->Initialize((LPCWSTR) m_csContentFile, (LPCWSTR)m_csReferenceFile);
		if (SUCCEEDED(hr))
		{
			hr = pRecComp->Append((LPCWSTR)m_csReferenceFile);
		}
	}
	OnOK();
}
