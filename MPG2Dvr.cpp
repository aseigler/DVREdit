// MPG2Dvr.cpp : implementation file
// Converts MPG files to dvr-ms format

#include "stdafx.h"
#include "MPG2Dvr.h"
#include "dvredit.h"
#include "dvreditdlg.h"
#include <shlobj.h>

HRESULT FindPinOnFilter(
    IBaseFilter *pFilter,
    const GUID *pMajorType,
    PIN_DIRECTION PinDir,
    BOOL fConnected,
    IPin ** ppPin
    )
{
    HRESULT         hr = S_OK;
    IEnumPins *     pEnumPin = NULL;
    IPin *          pConnectedPin = NULL;
    PIN_DIRECTION   PinDirection;
    ULONG           ulFetched;
    DWORD           nFound = 0;

    *ppPin = NULL;

    hr = pFilter->EnumPins( &pEnumPin );
    if(SUCCEEDED(hr))
    {
        while (S_OK == (hr = pEnumPin->Next( 1, ppPin, &ulFetched )))
        {
			if (*ppPin)
			{
				hr = (*ppPin)->ConnectedTo( &pConnectedPin );
			
				if (pConnectedPin)
				{
					pConnectedPin->Release();
					pConnectedPin = NULL;
				}

				if (((VFW_E_NOT_CONNECTED == hr) && !fConnected ) ||
					((S_OK == hr ) &&  fConnected))
				{
					hr = (*ppPin)->QueryDirection( &PinDirection );
					if ((S_OK == hr) && (PinDirection == PinDir))
					{
						if (pMajorType)
						{
							CComPtr<IEnumMediaTypes> pEnumMediaTypes;
							if (SUCCEEDED( hr = (*ppPin)->EnumMediaTypes( &pEnumMediaTypes ) ))
							{
								AM_MEDIA_TYPE *pMediaType;

								while (S_OK == pEnumMediaTypes->Next( 1, &pMediaType, NULL ))
								{
									hr = (pMediaType->majortype == *pMajorType) ? S_OK : S_FALSE;
									if (pMediaType->pbFormat)
										CoTaskMemFree((PVOID)pMediaType->pbFormat);
									CoTaskMemFree((PVOID)pMediaType);
									if (SUCCEEDED( hr ))
									{
										break;
									}
								}
							}
							if (S_OK == hr)
							{
								break;
							}
						}
						else
						{
							break;
						}
					}
				}
				(*ppPin)->Release();
			}
        }
    }
    pEnumPin->Release();
    return hr;

} // FindPinOnFilter

// MPG2Dvr dialog

IMPLEMENT_DYNAMIC(MPG2Dvr, CDialog)
MPG2Dvr::MPG2Dvr(CWnd* pParent /*=NULL*/)
	: CDialog(MPG2Dvr::IDD, pParent)
{
	CAttributesDialog m_attrDlg(AfxGetApp()->m_pMainWnd);
}

MPG2Dvr::~MPG2Dvr()
{
}

void MPG2Dvr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL MPG2Dvr::OnInitDialog(void)
{
	return (SUCCEEDED(m_pGraph.CoCreateInstance(CLSID_FilterGraph)));
}

BEGIN_MESSAGE_MAP(MPG2Dvr, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SOURCE_BROWSE, OnBnClickedButtonSourceBrowse)
	ON_BN_CLICKED(IDC_BUTTON_DEST_BROWSE, OnBnClickedButtonDestBrowse)
	ON_BN_CLICKED(IDC_BUTTON_ATTRIBUTES, OnBnClickedButtonAttributes)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// MPG2Dvr message handlers

void MPG2Dvr::OnBnClickedButtonSourceBrowse()
{
	CFileDialog sFileDlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, L"Supported Files (*.mpg;*.mpeg;*.vob;*.ts;*.tp;*.wmv)|*.mpg;*.mpg;*.vob;*.ts;*.tp;*.wmv||", this);
        
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

void MPG2Dvr::OnBnClickedButtonDestBrowse()
{
	CFileDialog sFileDlg(FALSE, NULL, NULL, OFN_NOREADONLYRETURN | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT, L"Recorded TV Files (*.dvr-ms;*.dvr)|*.dvr-ms;*.dvr||", this);
        
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

void MPG2Dvr::OnBnClickedButtonAttributes()
{
	m_attrDlg.DoModal();
}

HRESULT MPG2Dvr::OpenSourceFile(CString csSourceFile)
{
	HRESULT hr = S_OK;

    CComQIPtr<IFileSourceFilter> pFileSource;

    hr = m_pSource.CoCreateInstance(CLSID_WMAsfReader);
	if (SUCCEEDED(hr))
	{
		hr = m_pGraph->AddFilter( m_pSource, L"Async Video Reader" );
		if (SUCCEEDED(hr))
		{
			hr = E_NOINTERFACE;
			if (pFileSource = m_pSource)
			{
				hr = pFileSource->Load( (LPCWSTR) csSourceFile, NULL );
				GetDuration(csSourceFile);
				pFileSource.Release();
			}
		}
	}

/*
	hr = m_pAudioSource.CoCreateInstance(CLSID_AsyncReader);
	if (SUCCEEDED(hr))
	{
		hr = m_pGraph->AddFilter( m_pAudioSource, L"Async Audio Reader" );
		if (SUCCEEDED(hr))
		{
			hr = E_NOINTERFACE;
			if (pFileSource = m_pAudioSource)
			{
				hr = pFileSource->Load(L"E:\\drop\\test\\proof2\\audio.ac3", NULL );
				pFileSource.Release();
			}
		}
	}
*/
return hr;
}

HRESULT MPG2Dvr::OpenDestFile(CString csDestFile)
{
	HRESULT hr = S_OK;

	hr = m_pStreamBufferSink.CoCreateInstance(CLSID_StreamBufferSink);
	if (SUCCEEDED(hr))
	{
		hr = m_pGraph->AddFilter(m_pStreamBufferSink, L"Stream Buffer Sink");
	}
return hr;
}

HRESULT MPG2Dvr::ConfigureStreamBuffer(void)
{
	CComPtr<IStreamBufferConfigure3> pSBConfig;
	HRESULT hr = pSBConfig.CoCreateInstance( CLSID_StreamBufferConfig );
    if (SUCCEEDED(hr))
    {
		CComQIPtr<IStreamBufferInitialize> pSBInit(pSBConfig);
		if (pSBInit)
		{
            HKEY hKey;
            LONG error;
            DWORD dwIgnore = 0;
            
			error = RegCreateKeyEx(HKEY_LOCAL_MACHINE, L"Software\\DVREdit", 0, 0, 0, KEY_ALL_ACCESS, 0, &hKey, &dwIgnore);
			if (error == ERROR_SUCCESS)
			{
				hr = pSBInit->SetHKEY(hKey);
				if (SUCCEEDED(hr))
				{
					hr = pSBConfig->SetBackingFileCount(6, 8);
					if (SUCCEEDED(hr))
						hr = pSBConfig->SetBackingFileDuration(180);
					if (SUCCEEDED(hr))
						hr = pSBConfig->SetNamespace(NULL);
				}
				RegCloseKey(hKey);
			}
		}
	}
return hr;
}

void MPG2Dvr::OnBnClickedOk()
{
	HRESULT hr = S_OK;
	if (m_pSource && m_pStreamBufferSink)
	{
		hr = ConnectPins();
		if (SUCCEEDED(hr))
		{
			CComQIPtr <IStreamBufferSink> pSBSink(m_pStreamBufferSink);
			hr = ConfigureStreamBuffer();
			if (SUCCEEDED(hr))
			{
				CComQIPtr<IStreamBufferInitialize> pSBInit(pSBSink);
				if (pSBInit)
				{
					HKEY hKey;
					LONG error;
					
					error = RegOpenKey(HKEY_LOCAL_MACHINE, L"Software\\DVREdit", &hKey);
					if (error == ERROR_SUCCESS)
					{					
						hr = pSBInit->SetHKEY(hKey);
						if (SUCCEEDED(hr))
						{
							hr = pSBSink->LockProfile(NULL);
							if (SUCCEEDED(hr))
							{
								hr = pSBSink->CreateRecorder((LPCWSTR) m_csDestFile, RECORDING_TYPE_CONTENT, &m_pUnk);
								if (SUCCEEDED(hr))
								{
									m_pRecControl = m_pUnk;
									AddAttributes();
									RunGraph();
									OnOK();
								}
							}
						}
						RegCloseKey(hKey);
					}
				}
			}
			else DisplayError(m_hWnd, L"Error configuring stream buffer sink: 0x%x", hr);
		}
		else DisplayError(m_hWnd, L"Error connecting pins: 0x%x", hr);
	}
}

HRESULT MPG2Dvr::ConnectPins(void)
{
	HRESULT hr = S_OK;

#ifdef DEBUG
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
#endif

	//CComPtr <IBaseFilter> DeMux;
	CComPtr <IBaseFilter> pTee;
	CComPtr <IBaseFilter> pNullRenderer;
	//CComPtr <IBaseFilter> pAnalyzer;
	//CComPtr <IBaseFilter> pMPEG1Splitter;
	//CComPtr <IBaseFilter> pToTS;
/*
	hr = pToTS.CoCreateInstance(CLSID_MorphFilter);
	if (FAILED(hr))
	{
		DisplayError(m_hWnd, L"Failed to create morph filter: 0x%x", hr);
		return hr;
	}
*/
	//hr = pAnalyzer.CoCreateInstance(CLSID_Mpeg2VideoStreamAnalyzer);
	if (FAILED(hr))
	{
		DisplayError(m_hWnd, L"Failed to create video stream analyzer: 0x%x", hr);
		return hr;
	}

	hr = pTee.CoCreateInstance(CLSID_InfTee);
	if (FAILED(hr))
	{
		DisplayError(m_hWnd, L"Failed to create infinite pin tee: 0x%x", hr);
		return hr;
	}

	hr = pNullRenderer.CoCreateInstance(CLSID_NullRenderer);
	if (FAILED(hr))
	{
		DisplayError(m_hWnd, L"Failed to create null renderer: 0x%x", hr);
		return hr;
	}

	//hr = pMPEG1Splitter.CoCreateInstance(CLSID_MPEG1Splitter);
	if (FAILED(hr))
		return hr;

	//hr = DeMux.CoCreateInstance(CLSID_MPEG2Demultiplexer);
	if (FAILED(hr))
	{/*
		if (hr == REGDB_E_CLASSNOTREG)
		{
			CComPtr <IFilterMapper2> pFilterMap;

			hr = pFilterMap.CoCreateInstance(CLSID_FilterMapper2);
			IEnumMoniker* pRegisteredMuxFilters;

			GUID gDeMuxOut[4];
			gDeMuxOut[0] = MEDIATYPE_Video;
			gDeMuxOut[1] = GUID_NULL;
			gDeMuxOut[2] = MEDIATYPE_Audio;
			gDeMuxOut[3] = GUID_NULL;

			GUID gDeMuxIn[2];
			gDeMuxIn[0] = MEDIATYPE_Stream;
			gDeMuxIn[1] = MEDIASUBTYPE_MPEG2_PROGRAM;

			hr = pFilterMap->EnumMatchingFilters( &pRegisteredMuxFilters,
													0,
													FALSE,
													0, 
													TRUE,
													2,  
													gDeMuxIn,
													NULL,
													NULL,
													FALSE,
													TRUE,
													1,
													gDeMuxOut,
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
					CHAR szFilename[ MAX_PATH ];

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

						if (lstrcmpiW(strFilter, L"{731B8592-4001-46D4-B1A5-33EC792B4501}") == 0)
							break;
			
						SysFreeString(varbstrClsid.bstrVal);
					}
			    
					pPropBag->Release(); 
				}
			}while SUCCEEDED(hr);
			CComQIPtr<IFilterGraph2> pFG(m_pGraph);
			hr = pFG->AddSourceFilterForMoniker(pMuxFilterMoniker, pBindCtx, L"Mux", &DeMux);
			if (FAILED(hr))
				return hr;
		}
		*/
		if (FAILED(hr))
		{
			DisplayError(m_hWnd, L"Failed to create demultiplexer: 0x%x", hr);
			return hr;
		}
	}

	if (pTee && pNullRenderer)
	{
		//hr = m_pGraph->AddFilter( pToTS, L"pToTS");
		if (FAILED(hr))
		{
			DisplayError(m_hWnd, L"Failed to add pToTS filter to graph: 0x%x", hr);
			return hr;
		}

		//hr = m_pGraph->AddFilter(DeMux, L"Demux");
		if (FAILED(hr))
		{
			DisplayError(m_hWnd, L"Failed to add demultiplexer filter to graph: 0x%x", hr);
			return hr;
		}

		hr = m_pGraph->AddFilter(pTee, L"Tee");
		if (FAILED(hr))
		{
			DisplayError(m_hWnd, L"Failed to add infinite pin tee filter to graph: 0x%x", hr);
			return hr;
		}
		
		//hr = m_pGraph->AddFilter(pAnalyzer, L"Analyzer");
		if (FAILED(hr))
		{
			DisplayError(m_hWnd, L"Failed to add analyzer filter to graph: 0x%x", hr);
			return hr;
		}
		
		hr = m_pGraph->AddFilter(pNullRenderer, L"NULL Renderer");
		if (FAILED(hr))
		{
			DisplayError(m_hWnd, L"Failed to add analyzer filter to graph: 0x%x", hr);
			return hr;
		}

		CComPtr <IPin> pOutPin, pInPin;

		// connect MPEG file to the demux
		hr = FindPinOnFilter( m_pSource, &MEDIATYPE_Video, PINDIR_OUTPUT, FALSE, &pOutPin);
		if (FAILED(hr))
		{
			DisplayError(m_hWnd, L"Failed to find stream output on source file filter: 0x%x", hr);
			return hr;
		}

		//hr = FindPinOnFilter( pToTS, &MEDIATYPE_Stream, PINDIR_INPUT, FALSE, &pInPin);
		if (FAILED(hr))
		{
			DisplayError(m_hWnd, L"Failed to find stream input on demultiplexer: 0x%x", hr);
			return hr;
		}

		//hr = m_pGraph->ConnectDirect(pOutPin, pInPin, NULL);
		if (FAILED(hr))
		{
			DisplayError(m_hWnd, L"Failed to find connect source stream to demultiplexer: 0x%x", hr);
			return hr;
		}

		//pOutPin.Release();
		//pInPin.Release();

		// connect MPEG file to the demux
		//hr = FindPinOnFilter( pToTS, &MEDIATYPE_Stream, PINDIR_OUTPUT, FALSE, &pOutPin);
		if (FAILED(hr))
		{
			DisplayError(m_hWnd, L"Failed to find stream output on source file filter: 0x%x", hr);
			return hr;
		}
		//hr = FindPinOnFilter( DeMux, &MEDIATYPE_Stream, PINDIR_INPUT, FALSE, &pInPin);
		if (FAILED(hr))
		{
			DisplayError(m_hWnd, L"Failed to find stream input on demultiplexer: 0x%x", hr);
			return hr;
		}

/*		BOOL fMPEG1 = FALSE;
		//hr = m_pGraph->ConnectDirect(pOutPin, pInPin, NULL);
		if (FAILED(hr))
		{
			pInPin.Release();
			pOutPin.Release();
//			hr = m_pGraph->RemoveFilter(DeMux);
//			hr = m_pGraph->RemoveFilter(pAnalyzer);
//			hr = m_pGraph->AddFilter(pMPEG1Splitter, L"MPEG-1 Splitter");
			if (SUCCEEDED(hr))
			{
				hr = FindPinOnFilter(m_pSource, &MEDIATYPE_NULL, PINDIR_OUTPUT, FALSE, &pOutPin);
				if (FAILED(hr))
				{
					DisplayError(m_hWnd, L"Failed to find video output on demultiplexer: 0x%x", hr);
					return hr;
				}
//				hr = FindPinOnFilter( pMPEG1Splitter, &MEDIATYPE_Video, PINDIR_INPUT, FALSE, &pInPin);
				if (FAILED(hr))
				{
					DisplayError(m_hWnd, L"Failed to find video input on infinite pin tee: 0x%x", hr);
					return hr;
				}
				hr = m_pGraph->Connect(pOutPin, pInPin);
				if (FAILED(hr))
				{
					DisplayError(m_hWnd, L"Failed to connect video output on demultiplexor to video input on infinite pin tee: 0x%x", hr);
					return hr;
				}
				pInPin.Release();
				pOutPin.Release();
			}
			fMPEG1 = TRUE;
			//DisplayError(m_hWnd, L"Failed to connect source stream to demultiplexer: 0x%x", hr);
			//return hr;
		}

		pOutPin.Release();
		pInPin.Release();

		CComPtr<IPin> pAudio, pVideo;
		CComQIPtr<IMpeg2Demultiplexer> pDemux(DeMux);

		AM_MEDIA_TYPE* mtAudio = (AM_MEDIA_TYPE *)CoTaskMemAlloc(sizeof(AM_MEDIA_TYPE)); 
		AM_MEDIA_TYPE* mtVideo = (AM_MEDIA_TYPE *)CoTaskMemAlloc(sizeof(AM_MEDIA_TYPE));
		ZeroMemory(mtAudio, sizeof(AM_MEDIA_TYPE));
		ZeroMemory(mtVideo, sizeof(AM_MEDIA_TYPE));

		mtAudio->majortype = MEDIATYPE_Audio;
		mtAudio->subtype = MEDIASUBTYPE_DOLBY_AC3;
		mtAudio->formattype = FORMAT_WaveFormatEx;
		mtAudio->pbFormat = (BYTE *) & g_AC3WaveFormatEx;
		mtAudio->cbFormat = sizeof(g_AC3WaveFormatEx);
		mtAudio->bFixedSizeSamples = TRUE;
		
		mtVideo->majortype = MEDIATYPE_Video;
		mtVideo->subtype = MEDIASUBTYPE_MPEG2_VIDEO;
		mtVideo->formattype = FORMAT_MPEG2Video;
		mtVideo->pbFormat = g_ATSCVideoFormat;
		mtVideo->cbFormat = sizeof(g_ATSCVideoFormat);
		mtVideo->bFixedSizeSamples = TRUE;

		hr = pDemux->CreateOutputPin(mtAudio, L"Audio", &pAudio);
		hr = pDemux->CreateOutputPin(mtVideo, L"Video", &pVideo);		

		CComQIPtr<IMPEG2PIDMap> pMapAudio(pAudio);
		CComQIPtr<IMPEG2PIDMap> pMapVideo(pVideo);

		ULONG ulVideo = 0xE0;
		ULONG ulAudio = 0x14;
		pMapVideo->MapPID(1, &ulVideo, MEDIA_TRANSPORT_PACKET);
		pMapAudio->MapPID(1, &ulAudio, MEDIA_TRANSPORT_PACKET);

		// since we have to tee the video so that the graph runs 1x,
		// we tee the video off the mux here..			
		if (fMPEG1 == FALSE)
		{
			hr = FindPinOnFilter( DeMux, &MEDIATYPE_Video, PINDIR_OUTPUT, FALSE, &pOutPin);
			if (FAILED(hr))
			{
				DisplayError(m_hWnd, L"Failed to find video output on demultiplexer: 0x%x", hr);
				return hr;
			}
		}
		else
		{
			hr = FindPinOnFilter( pMPEG1Splitter, &MEDIATYPE_Video, PINDIR_OUTPUT, FALSE, &pOutPin);
			if (FAILED(hr))
			{
				DisplayError(m_hWnd, L"Failed to find video output on demultiplexer: 0x%x", hr);
				return hr;
			}
		}*/
		hr = FindPinOnFilter( pTee, &MEDIATYPE_Video, PINDIR_INPUT, FALSE, &pInPin);
		if (FAILED(hr))
		{
			DisplayError(m_hWnd, L"Failed to find video input on infinite pin tee: 0x%x", hr);
			return hr;
		}
		hr = m_pGraph->Connect(pOutPin, pInPin);
		if (FAILED(hr))
		{
			DisplayError(m_hWnd, L"Failed to connect video output on demultiplexor to video input on infinite pin tee: 0x%x", hr);
			return hr;
		}

		pOutPin.Release();
		pInPin.Release();

		// ...then connect one pin to the null renderer...
		hr = FindPinOnFilter( pTee, &MEDIATYPE_Video, PINDIR_OUTPUT, FALSE, &pOutPin);
		if (FAILED(hr))
		{
			DisplayError(m_hWnd, L"Failed to find video output on infinite pin tee: 0x%x", hr);
			return hr;
		}

		hr = FindPinOnFilter( pNullRenderer, &MEDIATYPE_Video, PINDIR_INPUT, FALSE, &pInPin);
		if (FAILED(hr))
		{
			DisplayError(m_hWnd, L"Failed to find video input on null renderer: 0x%x", hr);
			return hr;
		}
		hr = m_pGraph->ConnectDirect(pOutPin, pInPin, NULL);
		if (FAILED(hr))
		{
			DisplayError(m_hWnd, L"Failed to connect video output on infinite pin tee to video input on null renderer: 0x%x", hr);
			return hr;
		}

		pOutPin.Release();
		pInPin.Release();

		hr = FindPinOnFilter( pTee, &MEDIATYPE_Video, PINDIR_OUTPUT, FALSE, &pOutPin);
		if (FAILED(hr))
		{
			DisplayError(m_hWnd, L"Failed to find video output on infinite pin tee: 0x%x", hr);
			return hr;
		}
		
		//hr = FindPinOnFilter( pAnalyzer, NULL, PINDIR_INPUT, FALSE, &pInPin);
		if (FAILED(hr))
		{
			DisplayError(m_hWnd, L"Failed to find video input on analyzer: 0x%x", hr);
			return hr;
		}
		//hr = m_pGraph->ConnectDirect(pOutPin, pInPin, NULL);
		if (FAILED(hr))
		{
			DisplayError(m_hWnd, L"Failed to connect video output on infinite pin tee to video input on stream buffer sink: 0x%x", hr);
			return hr;
		}

		//pOutPin.Release();
		//pInPin.Release();

		//hr = FindPinOnFilter( pAnalyzer, &MEDIATYPE_Video, PINDIR_OUTPUT, FALSE, &pOutPin);
		if (FAILED(hr))
		{
			DisplayError(m_hWnd, L"Failed to find video output on analyzer: 0x%x", hr);
			return hr;
		}
		
		hr = FindPinOnFilter( m_pStreamBufferSink, NULL, PINDIR_INPUT, FALSE, &pInPin);
		if (FAILED(hr))
		{
			DisplayError(m_hWnd, L"Failed to find video input on stream buffer sink: 0x%x", hr);
			return hr;
		}
		hr = m_pGraph->Connect(pOutPin, pInPin);
		if (FAILED(hr))
		{
			DisplayError(m_hWnd, L"Failed to connect video output on infinite pin tee to video input on stream buffer sink: 0x%x", hr);
			return hr;
		}

		pOutPin.Release();
		pInPin.Release();

//		if (fMPEG1)
//		{
//			hr = FindPinOnFilter( pMPEG1Splitter, &MEDIATYPE_Audio, PINDIR_OUTPUT, FALSE, &pOutPin);
//			if (FAILED(hr))
//			{
//				DisplayError(m_hWnd, L"Failed to find audio output on demultiplexer filter: 0x%x", hr);
//				return hr;
//			}
//		}
//		else
//		{
//			hr = FindPinOnFilter( DeMux, &MEDIATYPE_Audio, PINDIR_OUTPUT, FALSE, &pOutPin);
//			if (SUCCEEDED(hr))
//			{
//				CComPtr<IBaseFilter> pAudioSource;

//				CComQIPtr<IFileSourceFilter> pFileSource;

//				hr = pAudioSource.CoCreateInstance(CLSID_AsyncReader);
//				if (SUCCEEDED(hr))
//				{
//					hr = m_pGraph->AddFilter( pAudioSource, L"Audio Async Reader" );
//					if (SUCCEEDED(hr))
//					{
//						hr = E_NOINTERFACE;
//						if (pFileSource = pAudioSource)
//						{
//							hr = pFileSource->Load( (LPCWSTR) L"d:\\drop\\test\\LV.ac3", NULL );

							hr = FindPinOnFilter( m_pSource, &MEDIATYPE_Audio, PINDIR_OUTPUT, FALSE, &pOutPin);
							if (FAILED(hr))
							{
								DisplayError(m_hWnd, L"Failed to find audio output on demultiplexer filter: 0x%x", hr);
								return hr;
							}							
//						}
//					}
//				}
				//DisplayError(m_hWnd, L"Failed to find audio output on demultiplexer filter: 0x%x", hr);
				//return hr;
//			}
		
		hr = FindPinOnFilter( m_pStreamBufferSink, NULL, PINDIR_INPUT, FALSE, &pInPin);
		if (FAILED(hr))
		{
			DisplayError(m_hWnd, L"Failed to find audio input on stream buffer sink: 0x%x", hr);
			return hr;
		}	
		hr = m_pGraph->Connect(pOutPin, pInPin);
		if (FAILED(hr))
		{
			DisplayError(m_hWnd, L"Failed to connect audio output on demultiplexer to audio input on stream buffer sink: 0x%x", hr);
			return hr;
		}

		pOutPin.Release();
		pInPin.Release();
	}
return hr;
}


void MPG2Dvr::RunGraph(void)
{

	HRESULT hr = S_OK;

	REFERENCE_TIME  rtStart = 0;
	hr = m_pRecControl->Start(&rtStart);
    if (SUCCEEDED(hr))
	{
		hr = m_pGraph->QueryInterface(IID_IMediaControl, (void **) &m_pGraphC);	
		if (SUCCEEDED(hr))
		{
			m_pMediaEvent = m_pGraph;
			
			hr = m_pGraphC->Run();
			if (SUCCEEDED(hr))
				
			{	
				hr =  CoCreateInstance(CLSID_ProgressDialog, NULL, CLSCTX_INPROC_SERVER, IID_IProgressDialog, (void**) &m_pProgressDialog);

				if (FAILED(hr))
				{
					return;
				}

				m_pDurationInSeconds = (m_Duration / dblUNITS);
				
				m_pProgressDialog->SetTitle(L"Saving...");
				m_pProgressDialog->SetLine(1, L"Saving MPEG to dvr-ms file:", TRUE, NULL);
				m_pProgressDialog->SetLine(2, (LPCWSTR) m_csDestFile, FALSE, NULL);

				m_pProgressDialog->StartProgressDialog(m_hWnd, NULL, PROGDLG_MODAL | PROGDLG_AUTOTIME, NULL);

				m_pProgressDialog->SetProgress(0, m_pDurationInSeconds);
				

				int iSeconds = 0;
				long evcode = S_OK, lParam1 = 0, lParam2 = 0;
				HANDLE hEvent;
				DWORD dwWait = 0;

				hr = m_pMediaEvent->GetEventHandle((OAEVENT*) &hEvent);

				Sleep(40000);

				while (hr == S_OK) // still running
				{
					dwWait = WaitForSingleObject(hEvent, 1000);
					if (dwWait == WAIT_OBJECT_0)
					{
						hr = m_pMediaEvent->GetEvent(&evcode, (LONG_PTR*) &lParam1, (LONG_PTR*) &lParam2, 0);
						if (evcode != EC_COMPLETE)
							hr = m_pGraphC->Run();

						m_pMediaEvent->FreeEventParams(evcode, lParam1, lParam2);
					}
					//hr = m_pMediaEvent->WaitForCompletion(1000, &evcode);
					iSeconds++;
					m_pProgressDialog->SetProgress(iSeconds, m_pDurationInSeconds);
				}	// Wait for completion.
				
				if (evcode == EC_COMPLETE)
				{
					hr = m_pRecControl->Stop(0);
					if (SUCCEEDED(hr))
					{
						hr = m_pGraphC->Stop();
					}
				}

				m_pProgressDialog->StopProgressDialog();

				m_pProgressDialog->Release();
				
			}
		}
	}
}

void MPG2Dvr::AddAttributes(void)
{
	
	//CComQIPtr <IStreamBufferRecordingAttribute> pIAttrib(m_pRecControl);
	CComQIPtr<IWMHeaderInfo> pSourceRecordingAttributes(m_pSource);
	CComQIPtr<IStreamBufferRecordingAttribute> pDestRecordingAttributes(m_pRecControl);
	
	HRESULT hr = E_FAIL;

    WORD wAttributes;
    hr = pSourceRecordingAttributes->GetAttributeCount(0, &wAttributes);
	if (FAILED(hr))
		return;

    for (WORD i = 0; i < wAttributes; i++) 
	{
        WCHAR *wszAttributeName = NULL;
        WORD cchNameLength = 0;
        WMT_ATTR_DATATYPE datatype;

        BYTE *abAttribute = NULL;
        WORD cbAttribute = 0;
		WORD pStreamNumber = 0;

        hr = pSourceRecordingAttributes->GetAttributeByIndex(i, &pStreamNumber,
            wszAttributeName, &cchNameLength,
            &datatype,
            abAttribute, &cbAttribute);

		if (FAILED(hr))
			return;

		wszAttributeName = new WCHAR[cchNameLength];
		if (NULL == wszAttributeName)
		{
			return;
		}

		abAttribute = new BYTE[cbAttribute];
		if (NULL == abAttribute)
		{
			delete[] wszAttributeName; 
			return;
		}
		
		hr = pSourceRecordingAttributes->GetAttributeByIndex(i, &pStreamNumber,
			wszAttributeName, &cchNameLength,
			&datatype,
			abAttribute, &cbAttribute);
        
		if (SUCCEEDED(hr)) 
		{
			pDestRecordingAttributes->SetAttribute(0, wszAttributeName, (STREAMBUFFER_ATTR_DATATYPE) datatype, abAttribute, cbAttribute);
		}
		delete[] wszAttributeName;
		delete[] abAttribute;
	}
	
	pSourceRecordingAttributes.Release();
	pDestRecordingAttributes.Release();
/*
	if (!m_attrDlg.m_csTitle.IsEmpty())
		pIAttrib->SetAttribute(NULL, L"Title", STREAMBUFFER_TYPE_STRING, (BYTE*) (LPCWSTR) m_attrDlg.m_csTitle, (WORD) (sizeof(WCHAR) * (wcslen(m_attrDlg.m_csTitle)+1)));
	
	if (!m_attrDlg.m_csSubTitle.IsEmpty())
		pIAttrib->SetAttribute(NULL, L"WM/SubTitle", STREAMBUFFER_TYPE_STRING, (BYTE*) (LPCWSTR) m_attrDlg.m_csSubTitle, (WORD) (sizeof(WCHAR) * (wcslen(m_attrDlg.m_csSubTitle)+1)));
		
	if (!m_attrDlg.m_csDescription.IsEmpty())
		pIAttrib->SetAttribute(NULL, L"WM/SubTitleDescription", STREAMBUFFER_TYPE_STRING, (BYTE*) (LPCWSTR) m_attrDlg.m_csDescription, (WORD) (sizeof(WCHAR) * (wcslen(m_attrDlg.m_csDescription)+1)));

	if (!m_attrDlg.m_csGenre.IsEmpty())
		pIAttrib->SetAttribute(NULL, L"WM/Genre", STREAMBUFFER_TYPE_STRING, (BYTE*) (LPCWSTR) m_attrDlg.m_csGenre, (WORD) (sizeof(WCHAR) * (wcslen(m_attrDlg.m_csGenre)+1)));

	if (!m_attrDlg.m_csMediaCredits.IsEmpty())
		pIAttrib->SetAttribute(NULL, L"WM/MediaCredits", STREAMBUFFER_TYPE_STRING, (BYTE*) (LPCWSTR) m_attrDlg.m_csMediaCredits, (WORD) (sizeof(WCHAR) * (wcslen(m_attrDlg.m_csMediaCredits)+1)));

	CString csAppID = L"DVREdit by Alex Seigler (aseigler)";
	CString csAppVer = L"V.YYYY.MM.DD";
	//csAppVer.Format(L"%d.%d.%d.%d", rmj, rmn, rmm, rup);
		
	pIAttrib->SetAttribute(NULL, L"WM/ToolName", STREAMBUFFER_TYPE_STRING, (BYTE*) (LPCWSTR) csAppID, (WORD) (sizeof(WCHAR) * (wcslen(csAppID)+1)));
	pIAttrib->SetAttribute(NULL, L"WM/ToolVersion", STREAMBUFFER_TYPE_STRING, (BYTE*) (LPCWSTR) csAppVer, (WORD) (sizeof(WCHAR) * (wcslen(csAppVer)+1)));
*/

}
HRESULT MPG2Dvr::GetDuration(CString csSourceFile)
{
	HRESULT hr = S_OK;
	CComPtr<IGraphBuilder> pGraph;
	hr = pGraph.CoCreateInstance(CLSID_FilterGraph);
	hr = pGraph->RenderFile((LPCWSTR) csSourceFile, NULL);

	CComQIPtr <IMediaSeeking> pSeek;
	pSeek = pGraph;

	pSeek->GetDuration(&m_Duration);

	pGraph.Release();
	return hr;
}
// CAttributesDialog dialog

IMPLEMENT_DYNAMIC(CAttributesDialog, CDialog)
CAttributesDialog::CAttributesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAttributesDialog::IDD, pParent)
{
  m_csSubTitle = _T("");;
  m_csTitle = _T("");;
  m_csGenre = _T("");;
  m_csDescription = _T("");;
  m_csMediaCredits = _T("");;
}

CAttributesDialog::~CAttributesDialog()
{

}

void CAttributesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAttributesDialog)
	DDX_Text(pDX, IDC_EDIT_SUBTITLE, m_csSubTitle);
	DDX_Text(pDX, IDC_EDIT_TITLE, m_csTitle);
	DDX_Text(pDX, IDC_EDIT_GENRE, m_csGenre);
	DDX_Text(pDX, IDC_EDIT_DESCRIPTION, m_csDescription);
	DDX_Text(pDX, IDC_EDIT_MEDIACREDITS, m_csMediaCredits);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAttributesDialog, CDialog)
END_MESSAGE_MAP()

BOOL CAttributesDialog::OnInitDialog(void)
{
	SetDlgItemText(IDC_EDIT_TITLE, m_csTitle);
	SetDlgItemText(IDC_EDIT_SUBTITLE, m_csSubTitle);
	SetDlgItemText(IDC_EDIT_GENRE, m_csGenre);
	SetDlgItemText(IDC_EDIT_MEDIACREDITS, m_csMediaCredits);
	SetDlgItemText(IDC_EDIT_DESCRIPTION, m_csDescription);

	CAttributesDialog::EnableToolTips(TRUE);
	m_pToolTip = new CToolTipCtrl;
	if (m_pToolTip)
	{
		if (m_pToolTip->Create(this))
		{
			m_pToolTip->AddTool(GetDlgItem(IDC_EDIT_TITLE), L"Title or name of program");
			m_pToolTip->AddTool(GetDlgItem(IDC_EDIT_SUBTITLE), L"Subtitle or name of episode");
			m_pToolTip->AddTool(GetDlgItem(IDC_EDIT_GENRE), L"Genre of program");
			m_pToolTip->AddTool(GetDlgItem(IDC_EDIT_MEDIACREDITS), L"Actors, actresses, director, etc.");
			m_pToolTip->AddTool(GetDlgItem(IDC_EDIT_DESCRIPTION), L"Description of program");
			m_pToolTip->Activate(TRUE);

			return TRUE;
		}
	}
	return FALSE;
}

BOOL CAttributesDialog::PreTranslateMessage(MSG* pMsg)
{
    if (NULL != m_pToolTip)
        m_pToolTip->RelayEvent(pMsg);

    return CDialog::PreTranslateMessage(pMsg);
}
// CAttributesDialog message handlers
