// DefaultDecoder.cpp : implementation file
//

#include "stdafx.h"
#include "dvredit.h"
#include "DefaultDecoder.h"
#include ".\defaultdecoder.h"
#include <strmif.h>
#include <uuids.h>

// CDefaultDecoder dialog

IMPLEMENT_DYNAMIC(CDefaultDecoder, CDialog)
CDefaultDecoder::CDefaultDecoder(CWnd* pParent /*=NULL*/)
	: CDialog(CDefaultDecoder::IDD, pParent)
{

}

CDefaultDecoder::~CDefaultDecoder()
{

}

BOOL CDefaultDecoder::OnInitDialog()
{
	CComPtr<IEnumMoniker> pDecoders;
	CComPtr<IFilterMapper2> pFilterMapper2;
	
	CComboBox* pVideoCombo = (CComboBox*) GetDlgItem(IDC_COMBO_VIDEO);
	CComboBox* pAudioCombo = (CComboBox*) GetDlgItem(IDC_COMBO_AUDIO);

    HRESULT hr = pFilterMapper2.CoCreateInstance( CLSID_FilterMapper2 );

	GUID gVideoDecoderInput[2];
	gVideoDecoderInput[0] = MEDIATYPE_Video;
	gVideoDecoderInput[1] = MEDIASUBTYPE_MPEG2_VIDEO;

	GUID gVideoDecoderOutput[2];
	gVideoDecoderOutput[0] = MEDIATYPE_Video;
	gVideoDecoderOutput[1] = MEDIASUBTYPE_YUY2;

    hr = pFilterMapper2->EnumMatchingFilters( 
                                &pDecoders, 
                                NULL, 
                                FALSE, 
                                MERIT_NORMAL,          //any merit
                                TRUE, 
                                1, 
                                gVideoDecoderInput, 
                                NULL, 
                                NULL, 
                                FALSE, 
                                TRUE, 
                                1, 
                                gVideoDecoderOutput, 
                                NULL, 
                                NULL );

	IMoniker* pMuxFilterMoniker;
	CComPtr<IBindCtx> pBindCtx;
	hr = ::CreateBindCtx( 0, &pBindCtx );
	
	long i = 0;

	do
	{
		hr = pDecoders->Next( 1, &pMuxFilterMoniker, NULL);
		if (hr != S_OK)
			break;
		i++;
	}while (SUCCEEDED(hr));

	pVideoCombo->InitStorage(i, MAX_PATH);
	
	hr = pDecoders->Reset();

	i = 0;
	do
	{
		hr = pDecoders->Next( 1, &pMuxFilterMoniker, NULL);
		if (hr != S_OK)
			break;

		IPropertyBag *pPropBag;
		hr = pMuxFilterMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pPropBag);
		if(SUCCEEDED(hr))
		{
			VARIANT varBstr;
			varBstr.vt = VT_BSTR;
			varBstr.bstrVal = 0;
			hr = pPropBag->Read(L"FriendlyName", &varBstr, 0);
			if(SUCCEEDED(hr)) 
			{
				ASSERT(varBstr.vt == VT_BSTR);
				pVideoCombo->AddString(varBstr.bstrVal);
				SysFreeString(varBstr.bstrVal);
			}

			hr = pPropBag->Read(L"CLSID", &varBstr, 0);
			if(SUCCEEDED(hr)) 
			{
				ASSERT(varBstr.vt == VT_BSTR);
				LPWSTR pVideoGUID = new OLECHAR[MAX_PATH];
				if (pVideoGUID)
				{
					wcscpy(pVideoGUID, varBstr.bstrVal);
					pVideoCombo->SetItemDataPtr(i, pVideoGUID);
				}
				SysFreeString(varBstr.bstrVal);
				i++;
			}
	    
			pPropBag->Release(); 
		}
		pMuxFilterMoniker = NULL;
	}while (SUCCEEDED(hr));

	pDecoders.Release();
	pMuxFilterMoniker = NULL;

	GUID gAudioDecoderInput[2];
	gAudioDecoderInput[0] = MEDIATYPE_Audio;
	gAudioDecoderInput[1] = MEDIASUBTYPE_MPEG1Payload;

    hr = pFilterMapper2->EnumMatchingFilters( 
                                &pDecoders, 
                                NULL, 
                                FALSE, 
                                MERIT_NORMAL,          //any merit
                                TRUE, 
                                1, 
                                gAudioDecoderInput, 
                                NULL, 
                                NULL, 
                                FALSE, 
                                TRUE, 
                                0, 
                                NULL, 
                                NULL, 
                                NULL );

	i = 0;
	
	do
	{
		hr = pDecoders->Next( 1, &pMuxFilterMoniker, NULL);
		if (hr != S_OK)
			break;
		i++;
	}while (SUCCEEDED(hr));

	pAudioCombo->InitStorage(i, MAX_PATH);
	hr = pDecoders->Reset();

	i = 0;
	do
	{
		hr = pDecoders->Next( 1, &pMuxFilterMoniker, NULL);
		if (hr != S_OK)
			break;

		IPropertyBag *pPropBag;
		hr = pMuxFilterMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pPropBag);
		if(SUCCEEDED(hr))
		{
			VARIANT varBstr;
			varBstr.vt = VT_BSTR;
			varBstr.bstrVal = 0;
			hr = pPropBag->Read(L"FriendlyName", &varBstr, 0);
			if(SUCCEEDED(hr)) 
			{
				ASSERT(varBstr.vt == VT_BSTR);
				pAudioCombo->AddString(varBstr.bstrVal);
				SysFreeString(varBstr.bstrVal);
			}

			hr = pPropBag->Read(L"CLSID", &varBstr, 0);
			if(SUCCEEDED(hr)) 
			{
				ASSERT(varBstr.vt == VT_BSTR);
				LPWSTR pAudioGUID = new OLECHAR[MAX_PATH];
				if (pAudioGUID)
				{
					wcscpy(pAudioGUID, varBstr.bstrVal);
					pAudioCombo->SetItemDataPtr(i, pAudioGUID);
				}
				SysFreeString(varBstr.bstrVal);
				i++;
			}
	    
			pPropBag->Release(); 
		}
		pMuxFilterMoniker = NULL;
	}while (SUCCEEDED(hr));

	pDecoders.Release();
	pMuxFilterMoniker = NULL;

	pFilterMapper2.Release();

	return TRUE;
}

void CDefaultDecoder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDefaultDecoder, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

void CDefaultDecoder::OnBnClickedOk()
{
	CComboBox* pVideoCombo = (CComboBox*) GetDlgItem(IDC_COMBO_VIDEO);
	CComboBox* pAudioCombo = (CComboBox*) GetDlgItem(IDC_COMBO_AUDIO);

	LPWSTR pVideoString = (LPWSTR) pVideoCombo->GetItemDataPtr(pVideoCombo->GetCurSel());
	LPWSTR pAudioString = (LPWSTR) pAudioCombo->GetItemDataPtr(pVideoCombo->GetCurSel());
	
	if (FAILED(SetDefaultDecodersInRegistry(pVideoString, pAudioString)))
	{
		MessageBox(L"Sorry, setting the default decoder has failed.", NULL, MB_OK);
	}

	int j = pVideoCombo->GetCount();
	for (int i = 0; i < j; i++)
	{
		pVideoString = (LPWSTR) pVideoCombo->GetItemDataPtr(i);
		if (pVideoString)
			delete[] pVideoString;
	}
	
	j = pAudioCombo->GetCount();
	for (int i = 0; i < j; i++)
	{
		pAudioString = (LPWSTR) pAudioCombo->GetItemDataPtr(i);
		if (pAudioString)
			delete[] pAudioString;
	}

	OnOK();
}

HRESULT CDefaultDecoder::SetDefaultDecodersInRegistry(LPWSTR pVideoDecoderGUID, LPWSTR pAudioDecoderGUID)
{
	HKEY hKeyDVREdit;
	DWORD dwDisposition;
	long status = ERROR_SUCCESS;
	
	status = RegCreateKeyEx(	HKEY_LOCAL_MACHINE, 
								L"Software\\DVREdit\\",
								0,
								NULL,
								NULL,
								KEY_WRITE | KEY_READ,
								NULL,
								&hKeyDVREdit,
								&dwDisposition);

	if (status != ERROR_SUCCESS)
		return status;

	status = RegSetValueEx(		hKeyDVREdit,
								L"VideoDecoder",
								NULL,
								REG_SZ,
								(const BYTE*) pVideoDecoderGUID,
								(wcslen(pVideoDecoderGUID) + 1) * sizeof(WCHAR));

	if (status != ERROR_SUCCESS)
	{
		RegCloseKey(hKeyDVREdit);
		return status;
	}

	status = RegSetValueEx(		hKeyDVREdit,
								L"AudioDecoder",
								NULL,
								REG_SZ,
								(const BYTE*) pAudioDecoderGUID,
								(wcslen(pAudioDecoderGUID) + 1) * sizeof(WCHAR));

	if (status != ERROR_SUCCESS)
	{
		RegCloseKey(hKeyDVREdit);
		return status;
	}

	status = RegCloseKey(hKeyDVREdit);
	return status;
}
