// CombineRecordings.cpp : implementation file
//

#include "stdafx.h"
#include "dvredit.h"
#include "CombineRecordings.h"
#include ".\combinerecordings.h"

// CCombineRecordings dialog

IMPLEMENT_DYNAMIC(CCombineRecordings, CDialog)
CCombineRecordings::CCombineRecordings(CWnd* pParent /*=NULL*/)
	: CDialog(CCombineRecordings::IDD, pParent)
{
	m_hWorkerThread = NULL;
}

CCombineRecordings::~CCombineRecordings()
{
	if (m_hWorkerThread)
		CloseHandle(m_hWorkerThread);
}

void CCombineRecordings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCombineRecordings)
    DDX_Control(pDX, IDC_LIST_COMBINE, m_ListBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCombineRecordings, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_COMBINE_ADD, OnBnClickedButtonCombineAdd)
	ON_BN_CLICKED(IDC_BUTTON_COMBINE_REMOVE, OnBnClickedButtonCombineRemove)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CCombineRecordings message handlers

void CCombineRecordings::OnBnClickedButtonCombineAdd()
{
	CFileDialog sFileDlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, L"Recorded TV (*.dvr-ms)|*.dvr-ms||", this);
	if (IDOK == sFileDlg.DoModal())
	{
		m_ListBox.AddString(sFileDlg.GetPathName());
	}	
}

void CCombineRecordings::OnBnClickedButtonCombineRemove()
{
	m_ListBox.DeleteString(m_ListBox.GetCurSel());
}

STDMETHODIMP CopyMetadata(LPCWSTR wszSource, IStreamBufferRecComp* pRecComp)
{
	HRESULT hr = S_OK;

	CComPtr<IFileSourceFilter> pSourceFile;

	hr = pSourceFile.CoCreateInstance(CLSID_StreamBufferRecordingAttributes);
	if (FAILED(hr))
		return hr;

	hr = pSourceFile->Load(wszSource, NULL);
	if (FAILED(hr))
		return hr;
	
	CComQIPtr<IStreamBufferRecordingAttribute> pSourceRecordingAttributes(pSourceFile);
	CComQIPtr<IStreamBufferRecordingAttribute> pDestRecordingAttributes(pRecComp);

    WORD wAttributes;
    hr = pSourceRecordingAttributes->GetAttributeCount(0, &wAttributes);
	if (FAILED(hr))
		return hr;

    for (WORD i = 0; i < wAttributes; i++) 
	{
        WCHAR *wszAttributeName = NULL;
        WORD cchNameLength = 0;
        STREAMBUFFER_ATTR_DATATYPE datatype;

        BYTE *abAttribute = NULL;
        WORD cbAttribute = 0;

        hr = pSourceRecordingAttributes->GetAttributeByIndex(i, 0,
            wszAttributeName, &cchNameLength,
            &datatype,
            abAttribute, &cbAttribute);

		if (FAILED(hr))
			return hr;

		wszAttributeName = new WCHAR[cchNameLength];
		if (NULL == wszAttributeName)
		{
			return E_OUTOFMEMORY;
		}

		abAttribute = new BYTE[cbAttribute];
		if (NULL == abAttribute)
		{
			delete[] wszAttributeName; 
			return E_OUTOFMEMORY;
		}
		
		hr = pSourceRecordingAttributes->GetAttributeByIndex(i, 0,
			wszAttributeName, &cchNameLength,
			&datatype,
			abAttribute, &cbAttribute);

		if (CompareStringW(LOCALE_INVARIANT, NORM_IGNORECASE, wszAttributeName, -1, g_wszWMPicture, -1) == CSTR_EQUAL)
		{			
			WM_PICTURE*  WMPicture;
			WMPicture = reinterpret_cast <WM_PICTURE*> (abAttribute);
			
			size_t cchMIME, cchDesc;
			if (FAILED
					(StringCchLength(WMPicture->pwszMIMEType, cbAttribute, &cchMIME)
					|| StringCchLength(WMPicture->pwszDescription, cbAttribute, &cchDesc)))
				return hr;

			cchMIME++;
			cchDesc++;

			cbAttribute =	(cchMIME + cchDesc) * sizeof(WCHAR)			// pwszMIMEType, pwszDescription
							+ RTL_FIELD_SIZE(WM_PICTURE,bPictureType)	// bPictureType
							+ RTL_FIELD_SIZE(WM_PICTURE,dwDataLen)		// dwDataLen
							+ WMPicture->dwDataLen;						// pbData

			BYTE* pbAttribute = new BYTE[cbAttribute];
			if (NULL == pbAttribute)
			{
				hr = E_OUTOFMEMORY;
				break;
			}

			ZeroMemory(pbAttribute, cbAttribute);

			hr = StringCchCopy((LPWSTR)(pbAttribute), cchMIME, WMPicture->pwszMIMEType);
			if FAILED(hr)
				break;

			pbAttribute += (cchMIME) * sizeof(WCHAR);
			
			*pbAttribute = WMPicture->bPictureType;

			pbAttribute += RTL_FIELD_SIZE(WM_PICTURE,bPictureType);

			hr = StringCchCopy((LPWSTR)(pbAttribute), cchDesc, WMPicture->pwszDescription);
			if FAILED(hr)
				break;
			
			pbAttribute += (cchDesc) * sizeof(WCHAR);

			*(DWORD*)(pbAttribute) = WMPicture->dwDataLen;
			
			pbAttribute += RTL_FIELD_SIZE(WM_PICTURE,dwDataLen);

			CopyMemory(pbAttribute, WMPicture->pbData, WMPicture->dwDataLen);

			pbAttribute = pbAttribute - cbAttribute + WMPicture->dwDataLen;
			
			delete[] abAttribute;
			abAttribute = pbAttribute;
		}

		if (SUCCEEDED(hr)) 
		{
			pDestRecordingAttributes->SetAttribute(0, wszAttributeName, datatype, abAttribute, cbAttribute);
		}
		delete[] wszAttributeName;
		delete[] abAttribute;
	}

	// Adding two attributes for easy identification of file origin and program version
	CString csAppID = L"DVREdit by Alex Seigler (aseigler)";
	CString csAppVer = L"V.YYYY.MM.DD";
	csAppVer.Format(L"%d.%d.%d.%d", rmj, rmn, rmm, rup);
	
	// Set these two attributes
	pDestRecordingAttributes->SetAttribute(NULL, L"WM/ToolName", STREAMBUFFER_TYPE_STRING, (BYTE*) (LPCWSTR) csAppID, (WORD) (sizeof(WCHAR) * (wcslen(csAppID)+1)));
	pDestRecordingAttributes->SetAttribute(NULL, L"WM/ToolVersion", STREAMBUFFER_TYPE_STRING, (BYTE*) (LPCWSTR) csAppVer, (WORD) (sizeof(WCHAR) * (wcslen(csAppVer)+1)));
	
	pSourceRecordingAttributes.Release();
	pDestRecordingAttributes.Release();
	pSourceFile.Release();
return hr;
}

DWORD GetDurationInSeconds(LPCWSTR wszSrcFilename)
{
	HRESULT hr = S_OK;

	CComPtr <IGraphBuilder> pGraph;
	hr = pGraph.CoCreateInstance(CLSID_FilterGraph);
	if (FAILED(hr))
		return 0;

	CComPtr<IBaseFilter> pSource;
	hr = pSource.CoCreateInstance(CLSID_StreamBufferSource);
	if (FAILED(hr))
		return 0;

	hr = pGraph->AddFilter(pSource, L"Source");
	if (FAILED(hr))
		return 0;

	CComQIPtr<IFileSourceFilter> pSourceFile(pSource);
	hr = pSourceFile->Load(wszSrcFilename, NULL);
	pSourceFile.Release();
	if (FAILED(hr))
		return 0;
	

	CComQIPtr<IStreamBufferMediaSeeking> pSeek(pSource);
	LONGLONG Duration = 0;
	hr = pSeek->GetDuration(&Duration);
	pSeek.Release();
	if (FAILED(hr))
		return 0;

	DWORD dwReturn = double(Duration) / dblUNITS;

	pSource.Release();
	pGraph.Release();
	
	return dwReturn;
}

DWORD WINAPI SaveThread(LPVOID pvThis)
{
	CCombineRecordings *pCombine = (CCombineRecordings*) pvThis;
	HRESULT hr = S_OK;
	hr = pCombine->m_pRecComp->Append((LPCWSTR) pCombine->m_csSourceFile);
	if (FAILED(hr))
	{
		pCombine->MessageBox(L"Failed to append!", NULL, MB_OK);
	}
	return hr;
}

DWORD WINAPI WorkerThread(LPVOID pvThis)
{
	CCombineRecordings *pCombine = (CCombineRecordings*) pvThis;
	HRESULT hr = S_OK;
	CComPtr<IProgressDialog> pSaveDialog;
	
	hr = CoCreateInstance(CLSID_ProgressDialog, NULL, CLSCTX_INPROC_SERVER, IID_IProgressDialog, (void**) &pSaveDialog);
	if (FAILED(hr))
	{
		pCombine->MessageBox(L"Failed to create progress dialog!", NULL, MB_OK);
		pCombine->m_pRecComp.Release();
		return 0;
	}

	hr = pSaveDialog->StartProgressDialog(pCombine->m_hWnd, NULL, PROGDLG_MODAL | PROGDLG_AUTOTIME, NULL);
	if (FAILED(hr))
	{
		pCombine->MessageBox(L"Failed to start progress dialog!", NULL, MB_OK);
		pSaveDialog.Release();
		pCombine->m_pRecComp.Release();
		return 0;
	}

	hr = pSaveDialog->SetLine(1, L"Now saving:", FALSE, NULL);
	if (FAILED(hr))
	{
		pCombine->MessageBox(L"Failed to set line 1 in progress dialog!", NULL, MB_OK);
		pSaveDialog->StopProgressDialog();
		pSaveDialog.Release();
		pCombine->m_pRecComp.Release();
		return 0;
	}

	while ((pCombine->m_ListBox.GetCount() != 0) && (!pSaveDialog->HasUserCancelled()))
	{
		hr = pSaveDialog->SetLine(2, (LPCWSTR) pCombine->m_csSourceFile, TRUE, NULL);
		if (FAILED(hr))
		{
			pCombine->MessageBox(L"Failed to set line 2 in progress dialog!", NULL, MB_OK);
			pSaveDialog->StopProgressDialog();
			pSaveDialog.Release();
			pCombine->m_pRecComp.Release();
			return 0;
		}
		DWORD cTotalSeconds = 0;
		cTotalSeconds = GetDurationInSeconds((LPCWSTR) pCombine->m_csSourceFile);
		hr = pSaveDialog->SetProgress(0, cTotalSeconds);
		if (FAILED(hr))
		{
			pCombine->MessageBox(L"Failed to zero progress in progress dialog!", NULL, MB_OK);
			pSaveDialog->StopProgressDialog();
			pSaveDialog.Release();
			pCombine->m_pRecComp.Release();
			return 0;
		}

		DWORD dwTID = 0;
		HANDLE hSaveThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) SaveThread, pvThis, 0, &dwTID);
		if (hSaveThread == NULL)
		{
			pCombine->MessageBox(L"Failed to create save thread!", NULL, MB_OK);
			pSaveDialog->StopProgressDialog();
			pSaveDialog.Release();
			pCombine->m_pRecComp.Release();
			return 0;
		}

		while ((WAIT_TIMEOUT == WaitForSingleObject(hSaveThread, 500)) && (!pSaveDialog->HasUserCancelled()))
		{
			DWORD cSeconds = 0;
			hr = pCombine->m_pRecComp->GetCurrentLength(&cSeconds);
			if (FAILED(hr))
			{
				pCombine->MessageBox(L"Failed to get current progress!", NULL, MB_OK);
				pSaveDialog->StopProgressDialog();
				pSaveDialog.Release();
				TerminateThread(hSaveThread, 0);
				GetExitCodeThread(hSaveThread, (LPDWORD) &hr);
				if (FAILED(hr))
				{
					CString csErrorString;
					csErrorString.Format(L"Thread exit code 0x%x", hr);
					pCombine->MessageBox((LPCWSTR) csErrorString, NULL, MB_OK);
				}
				CloseHandle(hSaveThread);
				pCombine->m_pRecComp.Release();
				return 0;
			}

			hr = pSaveDialog->SetProgress(cSeconds, cTotalSeconds);
			if (FAILED(hr))
			{
				pCombine->MessageBox(L"Failed to set current progress!", NULL, MB_OK);
				pSaveDialog->StopProgressDialog();
				pSaveDialog.Release();
				TerminateThread(hSaveThread, 0);
				GetExitCodeThread(hSaveThread, (LPDWORD) &hr);
				if (FAILED(hr))
				{
					CString csErrorString;
					csErrorString.Format(L"Thread exit code 0x%x", hr);
					pCombine->MessageBox((LPCWSTR) csErrorString, NULL, MB_OK);
				}
				CloseHandle(hSaveThread);
				pCombine->m_pRecComp.Release();
				return 0;
			}
		}

		if (pSaveDialog->HasUserCancelled())
		{
			pCombine->MessageBox(L"User has cancelled!", NULL, MB_OK);
			hr = pCombine->m_pRecComp->Cancel();
			if (FAILED(hr))
			{
				pCombine->MessageBox(L"Failed to cancel!", NULL, MB_OK);
				TerminateThread(hSaveThread, 0);
			}
			GetExitCodeThread(hSaveThread, (LPDWORD) &hr);
			if (FAILED(hr))
			{
				CString csErrorString;
				csErrorString.Format(L"Thread exit code 0x%x", hr);
				pCombine->MessageBox((LPCWSTR) csErrorString, NULL, MB_OK);
			}
			CloseHandle(hSaveThread);
			pSaveDialog->StopProgressDialog();
			pSaveDialog.Release();
			pCombine->m_pRecComp.Release();
			return 0;
		}
	
		GetExitCodeThread(hSaveThread, (LPDWORD) &hr);
		if (FAILED(hr))
		{
			CString csErrorString;
			csErrorString.Format(L"Thread exit code 0x%x", hr);
			pCombine->MessageBox((LPCWSTR) csErrorString, NULL, MB_OK);
			CloseHandle(hSaveThread);
			pSaveDialog->StopProgressDialog();
			pSaveDialog.Release();
			pCombine->m_pRecComp.Release();
			return 0;
		}

		CloseHandle(hSaveThread);
		hSaveThread = NULL;

		pCombine->m_ListBox.DeleteString(0);

		if (pCombine->m_ListBox.GetCount() == 0)
			break;

		pCombine->m_csSourceFile.Empty();
		pCombine->m_ListBox.GetText(0, pCombine->m_csSourceFile.GetBuffer(pCombine->m_ListBox.GetTextLen(0)));
	}
	
	pCombine->m_pRecComp.Release();
	pSaveDialog->StopProgressDialog();
	pSaveDialog.Release();
	return 0;
}

void CCombineRecordings::OnBnClickedOk()
{
	if (m_ListBox.GetCount() < 2)
	{	
		MessageBox(L"You must specify two or more recordings", NULL, MB_OK);
		return;
	}

	CFileDialog sFileDlg(FALSE, NULL, NULL, OFN_NOREADONLYRETURN | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT, L"Recorded TV (*.dvr-ms)|*.dvr-ms||", this);
	if (IDOK == sFileDlg.DoModal())
	{
		HRESULT hr = m_pRecComp.CoCreateInstance(CLSID_StreamBufferComposeRecording);
		if (FAILED(hr))
		{
			MessageBox(L"Failed to created RecComp!", NULL, MB_OK);
			return;
		}
	
		int i = 0, n = 0;
		n = m_ListBox.GetTextLen(i);
		m_ListBox.GetText(i, m_csSourceFile.GetBuffer(n));

		hr = m_pRecComp->Initialize((LPCWSTR) sFileDlg.GetPathName(), (LPCWSTR) m_csSourceFile);
		if (FAILED(hr))
		{
			MessageBox(L"Failed to initialize RecComp!", NULL, MB_OK);
			m_pRecComp.Release();
			return;
		}

		hr = CopyMetadata((LPCWSTR) m_csSourceFile, m_pRecComp);
		if (FAILED(hr))
		{
			MessageBox(L"Failed to copy metadata!", NULL, MB_OK);
			m_pRecComp.Release();
			return;
		}
		
		DWORD dwTID;
		m_hWorkerThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) WorkerThread, this, NULL, &dwTID);
		if (NULL == m_hWorkerThread)
		{
			MessageBox(L"Failed to create append thread!", 0, MB_OK);
			m_pRecComp.Release();
			return;
		}
	}		
	
	else return;

	//OnOK();
}
