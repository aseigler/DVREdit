// Dvr2MPG.cpp : implementation file
//
//*************************************************************************
// This code does nothing right now, it is the implementation for dvr-ms
// to MPEG2 conversion, I am writing a filter to do this
//*************************************************************************
#include "stdafx.h"
#include "Dvr2MPG.h"
//#include "mux.h"
// {C4C4C4F2-0049-4E2B-98FB-9537F6CE516D}

#include <initguid.h>

DEFINE_GUID(CLSID_DTFilter, 
0xC4C4C4F2, 0x0049, 0x4E2B, 0x98, 0xFB, 0x95, 0x37, 0xF6, 0xCE, 0x51, 0x6D);

// {CBF52633-3F6A-4639-B1DD-EF0E9AE99BC6}
//DEFINE_GUID(CLSID_DVREditMPEG2Multiplexer, 
//0xcbf52633, 0x3f6a, 0x4639, 0xb1, 0xdd, 0xef, 0xe, 0x9a, 0xe9, 0x9b, 0xc6);

// Dvr2MPG dialog

IMPLEMENT_DYNAMIC(Dvr2MPG, CDialog)
Dvr2MPG::Dvr2MPG(CWnd* pParent /*=NULL*/)
	: CDialog(Dvr2MPG::IDD, pParent)
{
}

Dvr2MPG::~Dvr2MPG()
{
}

void Dvr2MPG::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL Dvr2MPG::OnInitDialog(void)
{
	return (SUCCEEDED(m_pGraph.CoCreateInstance(CLSID_FilterGraph)));
}

BEGIN_MESSAGE_MAP(Dvr2MPG, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SOURCE_BROWSE, OnBnClickedButtonSourceBrowse)
	ON_BN_CLICKED(IDC_BUTTON_DEST_BROWSE, OnBnClickedButtonDestBrowse)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// Dvr2MPG message handlers

void Dvr2MPG::OnBnClickedButtonSourceBrowse()
{
	CFileDialog sFileDlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, L"Recorded TV Files (*.dvr-ms;*.dvr)|*.dvr-ms;*.dvr||", this);
        
	if (IDOK == sFileDlg.DoModal())
	{
		SetDlgItemText(IDC_EDIT_SOURCE_FILE, sFileDlg.GetPathName());
		if (FAILED(OpenSourceFile(sFileDlg.GetPathName())))
		{
			MessageBoxEx(m_hWnd, L"Error opening source file!!!", NULL, MB_OK | MB_ICONERROR, NULL);
		}
		m_csSourceFile = sFileDlg.GetPathName();
	}	
}

void Dvr2MPG::OnBnClickedButtonDestBrowse()
{
	CFileDialog sFileDlg(FALSE, NULL, NULL, OFN_NOREADONLYRETURN | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT, L"MPEG Files (*.mpg;*.mpeg)|*.mpg;*.mpg||", this);
        
	if (IDOK == sFileDlg.DoModal())
	{
		SetDlgItemText(IDC_EDIT_DEST_FILE, sFileDlg.GetPathName());
		if (FAILED(OpenDestFile(sFileDlg.GetPathName())))
		{
			MessageBoxEx(m_hWnd, L"Error saving destination file!!!", NULL, MB_OK | MB_ICONERROR, NULL);
		}
		m_csDestFile = sFileDlg.GetPathName();
	}	
}

HRESULT Dvr2MPG::OpenSourceFile(CString csSourceFile)
{
	HRESULT hr = S_OK;

    CComQIPtr<IFileSourceFilter> pFileSource;

    hr = m_pSource.CoCreateInstance(CLSID_StreamBufferSource);
	if (SUCCEEDED(hr))
	{
		hr = m_pGraph->AddFilter( m_pSource, L"Stream Buffer Source" );
		if (SUCCEEDED(hr))
		{
			hr = E_NOINTERFACE;
			if (pFileSource = m_pSource)
			{
				hr = pFileSource->Load( (LPCWSTR) csSourceFile, NULL );
			}
		}
	}
return hr;
}

HRESULT Dvr2MPG::OpenDestFile(CString csDestFile)
{
	HRESULT hr = S_OK;

	//hr = m_pMPEG2File.CoCreateInstance(CLSID_FileWriter);
	CComPtr <IFilterMapper2> pFilterMap;

	hr = pFilterMap.CoCreateInstance(CLSID_FilterMapper2);
    IEnumMoniker* pRegisteredMuxFilters;


    hr = pFilterMap->EnumMatchingFilters( &pRegisteredMuxFilters,
                                              0,
                                              FALSE,
                                              0, 
                                              TRUE,
                                              NULL,  
                                              NULL,
                                              NULL,
                                              NULL,
                                              TRUE,
                                              FALSE,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL );
	pFilterMap.Release();
	if (FAILED(hr))
		return hr;

	IMoniker* pMuxFilterMoniker;
	CComPtr<IBindCtx> pBindCtx;
	hr = ::CreateBindCtx( 0, &pBindCtx );
		if (FAILED(hr))
			return hr;
	do
	{
		hr = pRegisteredMuxFilters->Next( 1, &pMuxFilterMoniker, NULL);
		if (FAILED(hr))
			return hr;
	   
		IPropertyBag *pPropBag;
		hr = pMuxFilterMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pPropBag);
		if(SUCCEEDED(hr))
		{
			CLSID filterCLSID;
			HRESULT hrGotFilename=E_FAIL;

			// open clsid/{filter-clsid} key
			VARIANT varbstrClsid;
			varbstrClsid.vt = VT_BSTR;
			varbstrClsid.bstrVal = 0;
			hr = pPropBag->Read(L"CLSID", &varbstrClsid, 0);
			if(SUCCEEDED(hr)) {
				ASSERT(varbstrClsid.vt == VT_BSTR);
				WCHAR *strFilter = varbstrClsid.bstrVal;

				HRESULT hrTemp = CLSIDFromString(varbstrClsid.bstrVal, &filterCLSID);
				ASSERT( hrTemp == S_OK);

				if (lstrcmpiW(strFilter, L"{ADA776D8-72EB-4D0D-8F0E-83230951D16A}") == 0)
					break;
	//            hrGotFilename = GetFilenameFromCLSID( varbstrClsid, szFilename );

				SysFreeString(varbstrClsid.bstrVal);
			}
	    
			pPropBag->Release(); 
		}
	}while SUCCEEDED(hr);
	CComQIPtr<IFilterGraph2> pFG(m_pGraph);
	hr = pFG->AddSourceFilterForMoniker(pMuxFilterMoniker, pBindCtx, L"Mux", &m_pMPEG2File);
	if (FAILED(hr))
		return hr;

	if (SUCCEEDED(hr))
	{
		CComQIPtr <IFileSinkFilter> pFileSink(m_pMPEG2File);
		if (pFileSink)
		{
			//hr = pFileSink->SetMode(AM_FILE_OVERWRITE);
			if (SUCCEEDED(hr))
			{
				if (!wcsstr((LPCWSTR) csDestFile, L".mpg"))
					csDestFile.Append(L".mpg");

				hr = pFileSink->SetFileName((LPCWSTR) csDestFile, NULL);
				if (SUCCEEDED(hr))
				{
					hr = m_pGraph->AddFilter(m_pMPEG2File, L"MPEG2 File");
				}
			}
		}
	}
return hr;
}

void Dvr2MPG::OnBnClickedOk()
{
	HRESULT hr = S_OK;
	if (m_pSource && m_pMPEG2File)
	{
		hr = ConnectPins();
		if (SUCCEEDED(hr))
		{
			RunGraph();
			OnOK();
		}
	}
}

void Dvr2MPG::RunGraph(void)
{
	HRESULT hr = S_OK;

    if (SUCCEEDED(hr))
	{
		hr = m_pGraph->QueryInterface(IID_IMediaControl, (void **) &m_pGraphC);	
		if (SUCCEEDED(hr))
		{
			m_pMediaEvent = m_pGraph;

			hr = m_pGraphC->Run();
			if (SUCCEEDED(hr))
			{					
				long evcode = S_OK;
				
				hr = E_ABORT;

				while (hr == E_ABORT) // still running
				{
					hr = m_pMediaEvent->WaitForCompletion(10, &evcode);
				}	// Wait for completion.
				
				if (evcode == EC_COMPLETE)
				{
					if (SUCCEEDED(hr))
					{
						hr = m_pGraphC->Stop();
					}
				}
			}
		}
	}
}

HRESULT Dvr2MPG::ConnectPins(void)
{
	HRESULT hr = S_OK;

	CComPtr<IRunningObjectTable> pROT;
	DWORD ROTCookie;
    if (FAILED( hr = GetRunningObjectTable( 0, &pROT ) ))
    {
        pROT = NULL;
    }
	
	if (pROT)
    {
        CComPtr<IMoniker> pGraphMK;

        OLECHAR osz[256];
        wsprintfW( osz, OLESTR("FilterGraph %08lx  pid %08lx"), m_pGraph.p, ::GetCurrentProcessId());

        hr = CreateItemMoniker(OLESTR("!"), osz, &pGraphMK);
        if (SUCCEEDED(hr))
        {
            hr = pROT->Register(
                ROTFLAGS_REGISTRATIONKEEPSALIVE,
                m_pGraph,
                pGraphMK,
                &ROTCookie );
        }
    }

	CComPtr <IBaseFilter> pMux;
	CComPtr <IBaseFilter> pDTAudioDecrypter;
	CComPtr <IBaseFilter> pDTVideoDecrypter;
	//CComPtr <IBaseFilter> pFileWriterAudio, pFileWriterVideo;

	//hr = pMux.CoCreateInstance(CLSID_DVREditMPEG2Multiplexer);
	if (FAILED(hr))
		return hr;

	//CComPtr <CDVREditMux> pMux = new CDVREditMux(L"DVREdit Multiplexer", NULL, &hr);
	
	

	CComPtr <IFilterMapper2> pFilterMap;

	hr = pFilterMap.CoCreateInstance(CLSID_FilterMapper2);
    IEnumMoniker* pRegisteredMuxFilters;

	GUID gMuxIn[4];
	gMuxIn[0] = MEDIATYPE_Video;
	gMuxIn[1] = GUID_NULL;
	gMuxIn[2] = MEDIATYPE_Audio;
	gMuxIn[3] = GUID_NULL;

	GUID gMuxOut[2];
	gMuxOut[0] = MEDIATYPE_Stream;
	gMuxOut[1] = MEDIASUBTYPE_MPEG2_PROGRAM;

    hr = pFilterMap->EnumMatchingFilters( &pRegisteredMuxFilters,
                                              0,
                                              FALSE,
                                              0, 
                                              TRUE,
                                              2,  
                                              gMuxIn,
                                              NULL,
                                              NULL,
                                              FALSE,
                                              TRUE,
                                              1,
                                              gMuxOut,
                                              NULL,
                                              NULL );
	pFilterMap.Release();
	if (FAILED(hr))
		return hr;

	IMoniker* pMuxFilterMoniker;
	CComPtr<IBindCtx> pBindCtx;
	hr = ::CreateBindCtx( 0, &pBindCtx );
		if (FAILED(hr))
			return hr;
	do
	{
		hr = pRegisteredMuxFilters->Next( 1, &pMuxFilterMoniker, NULL);
		if (FAILED(hr))
			return hr;
	   
		IPropertyBag *pPropBag;
		hr = pMuxFilterMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pPropBag);
		if(SUCCEEDED(hr))
		{
			CLSID filterCLSID;
			HRESULT hrGotFilename=E_FAIL;

			// open clsid/{filter-clsid} key
			VARIANT varbstrClsid;
			varbstrClsid.vt = VT_BSTR;
			varbstrClsid.bstrVal = 0;
			hr = pPropBag->Read(L"CLSID", &varbstrClsid, 0);
			if(SUCCEEDED(hr)) {
				ASSERT(varbstrClsid.vt == VT_BSTR);
				WCHAR *strFilter = varbstrClsid.bstrVal;

				HRESULT hrTemp = CLSIDFromString(varbstrClsid.bstrVal, &filterCLSID);
				ASSERT( hrTemp == S_OK);

				if (lstrcmpiW(strFilter, L"{BFFDDE97-C438-445F-9805-BAECD32CF9A4}") == 0)
					break;
	//            hrGotFilename = GetFilenameFromCLSID( varbstrClsid, szFilename );

				SysFreeString(varbstrClsid.bstrVal);
			}
	    
			pPropBag->Release(); 
		}
	}while SUCCEEDED(hr);
	CComQIPtr<IFilterGraph2> pFG(m_pGraph);
	hr = pFG->AddSourceFilterForMoniker(pMuxFilterMoniker, pBindCtx, L"Mux", &pMux);
	if (FAILED(hr))
		return hr;

	hr = pDTAudioDecrypter.CoCreateInstance(CLSID_DTFilter);
	if (FAILED(hr))
		return hr;
	hr = pDTVideoDecrypter.CoCreateInstance(CLSID_DTFilter);
	if (FAILED(hr))
		return hr;	
	

	//hr = pMux.CoCreateInstance(CLSID_DVREditMPEG2Multiplexer);
	if (FAILED(hr))
		return hr;

	if (pMux && pDTAudioDecrypter && pDTVideoDecrypter)
	{
		//hr = m_pGraph->AddFilter(pMux, L"Mux");
		if (FAILED(hr))
			return hr;

		hr = m_pGraph->AddFilter(pDTAudioDecrypter, L"Audio Decrypter");
		if (FAILED(hr))
			return hr;
		
		hr = m_pGraph->AddFilter(pDTVideoDecrypter, L"Video Decrypter");
		if (FAILED(hr))
			return hr;

/*		pFileWriterAudio.CoCreateInstance(CLSID_FileWriter);
		pFileWriterVideo.CoCreateInstance(CLSID_FileWriter);

		CComQIPtr <IFileSinkFilter2> pFileSinkAudio(pFileWriterAudio);
		CComQIPtr <IFileSinkFilter2> pFileSinkVideo(pFileWriterVideo);
		
		pFileSinkAudio->SetMode(AM_FILE_OVERWRITE);
		pFileSinkVideo->SetMode(AM_FILE_OVERWRITE);

		pFileSinkAudio->SetFileName(L"d:\\test.m2a", NULL);
		pFileSinkVideo->SetFileName(L"d:\\test.m2v", NULL);

		hr = m_pGraph->AddFilter(pFileWriterAudio, L"M2A File");
		hr = m_pGraph->AddFilter(pFileWriterVideo, L"M2V File");
*/
		CComPtr <IPin> pOutPin, pInPin;

		// connect the video source to the video decrypter
		hr = FindPinOnFilter( m_pSource, &MEDIATYPE_Video, PINDIR_OUTPUT, FALSE, &pOutPin);
		if (FAILED(hr))
			return hr;
		hr = FindPinOnFilter( pDTVideoDecrypter, &MEDIATYPE_Video, PINDIR_INPUT, FALSE, &pInPin);
		if (FAILED(hr))
			return hr;		
		hr = m_pGraph->ConnectDirect(pOutPin, pInPin, NULL);
		if (FAILED(hr))
			return hr;

		pOutPin.Release();
		pInPin.Release();

		// connect the audio source to the audio decrypter
		hr = FindPinOnFilter( m_pSource, &MEDIATYPE_Audio, PINDIR_OUTPUT, FALSE, &pOutPin);
		if (FAILED(hr))
			return hr;
		hr = FindPinOnFilter( pDTAudioDecrypter, &MEDIATYPE_Audio, PINDIR_INPUT, FALSE, &pInPin);
		if (FAILED(hr))
			return hr;
		hr = m_pGraph->ConnectDirect(pOutPin, pInPin, NULL);
		if (FAILED(hr))
			return hr;
		
		pOutPin.Release();
		pInPin.Release();
//Sleep(100000);
		// connect the video output on the decrypter to the video input on the mux
		hr = FindPinOnFilter( pDTVideoDecrypter, &MEDIATYPE_Video, PINDIR_OUTPUT, FALSE, &pOutPin);
		if (FAILED(hr))
			return hr;
		hr = FindPinOnFilter( pMux, &MEDIATYPE_Video, PINDIR_INPUT, FALSE, &pInPin);
		//hr = FindPinOnFilter(pFileWriterVideo, NULL, PINDIR_INPUT, FALSE, &pInPin);
		if (FAILED(hr))
			return hr;
		hr = m_pGraph->ConnectDirect(pOutPin, pInPin, NULL);
		if (FAILED(hr))
			return hr;

		pOutPin.Release();
		pInPin.Release();

		// connect the audio output on the decrypter to the audio input on the mux
		hr = FindPinOnFilter( pDTAudioDecrypter, &MEDIATYPE_Audio, PINDIR_OUTPUT, FALSE, &pOutPin);
		if (FAILED(hr))
			return hr;
		hr = FindPinOnFilter( pMux, &MEDIATYPE_Audio, PINDIR_INPUT, FALSE, &pInPin);
		//hr = FindPinOnFilter(pFileWriterAudio, NULL, PINDIR_INPUT, FALSE, &pInPin);
		if (FAILED(hr))
			return hr;
		hr = m_pGraph->ConnectDirect(pOutPin, pInPin, NULL);
		if (FAILED(hr))
			return hr;

		pOutPin.Release();
		pInPin.Release();

		// connect the mux output to the file
		hr = FindPinOnFilter( pMux, NULL, PINDIR_OUTPUT, FALSE, &pOutPin);
		if (FAILED(hr))
			return hr;
		hr = FindPinOnFilter( m_pMPEG2File, NULL, PINDIR_INPUT, FALSE, &pInPin);
		if (FAILED(hr))
			return hr;
		hr = m_pGraph->ConnectDirect(pOutPin, pInPin, NULL);
		if (FAILED(hr))
			return hr;

		pOutPin.Release();
		pInPin.Release();
	}
return hr;
}