#include "stdafx.h"
#include "dvredit.h"
#include "MetadataEditor.h"


// CMetadataEditor dialog

IMPLEMENT_DYNAMIC(CMetadataEditor, CDialog)

CMetadataEditor::CMetadataEditor(CWnd* pParent /*=NULL*/)
	: CDialog(CMetadataEditor::IDD, pParent)
{
	WMCreateEditor((IWMMetadataEditor**) &m_spWMEditor);
}

CMetadataEditor::~CMetadataEditor()
{
	
}

void CMetadataEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CMetadataEditor::OnInitDialog()
{
	CFileDialog sFileDlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, L"WM Media files (*.dvr-ms;*.wmv;*.wma)|*.*||", this);
	if (IDOK == sFileDlg.DoModal())
	{		
		CString csTemp;
		this->GetWindowTextW(csTemp);
		csTemp.Append(L" - ");
		csTemp.Append(sFileDlg.GetFileName());
		this->GetWindowTextW(csTemp);

		if (FAILED(m_spWMEditor->OpenEx(sFileDlg.GetPathName(), GENERIC_READ | GENERIC_WRITE, 0)))
			return FALSE;

		CComboBox* pAttribute = (CComboBox*) GetDlgItem(IDC_COMBO1);
		CComboBox* pDatatype = (CComboBox*) GetDlgItem(IDC_COMBO2);
		
		pDatatype->InitStorage(7, 32);
		pDatatype->SetItemData(pDatatype->AddString(L"WMT_TYPE_DWORD"), 0);
		pDatatype->SetItemData(pDatatype->AddString(L"WMT_TYPE_STRING"), 1);
		pDatatype->SetItemData(pDatatype->AddString(L"WMT_TYPE_BINARY"), 2);
		pDatatype->SetItemData(pDatatype->AddString(L"WMT_TYPE_BOOL"), 3);
		pDatatype->SetItemData(pDatatype->AddString(L"WMT_TYPE_QWORD"), 4);
		pDatatype->SetItemData(pDatatype->AddString(L"WMT_TYPE_WORD"), 5);
		pDatatype->SetItemData(pDatatype->AddString(L"WMT_TYPE_GUID"), 6);

		return TRUE;
	}
	return FALSE;
}

BEGIN_MESSAGE_MAP(CMetadataEditor, CDialog)
	ON_BN_CLICKED(IDOK, &CMetadataEditor::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMetadataEditor::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CMetadataEditor::OnCbnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CMetadataEditor::OnCbnSelchangeCombo2)
END_MESSAGE_MAP()


// CMetadataEditor message handlers
void CMetadataEditor::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CMetadataEditor::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CMetadataEditor::OnCbnSelchangeCombo1()
{
	// TODO: Add your control notification handler code here
}

void CMetadataEditor::OnCbnSelchangeCombo2()
{
	// TODO: Add your control notification handler code here
}
