// for the preview graph

#include "StdAfx.h"
#include "dvreditdlg.h"
#include "graph.h"

CGraph::CGraph() : 
              m_state(State_Stopped)
{
	m_guidVideoDecoder = GUID_NULL;
	m_guidAudioDecoder = GUID_NULL;
}

CGraph::~CGraph(void)
{
	if (m_pVMR)
	{
		m_pVMR.Release();
    }
	Dispose();
}

void CGraph::Dispose(void)
{
	if (m_pSBSeek)
		m_pSBSeek.Release();

	if (m_pControl)
		m_pControl.Release();

	if (m_pEvent)
		m_pEvent.Release();

	if (m_pGraph)
	{
		// Enumerate the filters in the graph.
		IEnumFilters *pEnum = NULL;
		HRESULT hr = m_pGraph->EnumFilters(&pEnum);
		if (SUCCEEDED(hr))
		{
			IBaseFilter *pFilter = NULL;
			while (S_OK == pEnum->Next(1, &pFilter, NULL))
			{
				// Remove the filter.
				m_pGraph->RemoveFilter(pFilter);
				// Reset the enumerator.
				pEnum->Reset();
				pFilter->Release();
			}
			pEnum->Release();
		}

		m_pGraph.Release();
	}
}

HRESULT CGraph::GetDefaultDecodersFromRegistry(void)
{
	HKEY hKeyDVREdit;
	DWORD dwDisposition;
	DWORD dwType;
	DWORD dwSize;
	long status = ERROR_SUCCESS;
	CString csVideoDecoder, csAudioDecoder;

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

	status = RegQueryValueEx(	hKeyDVREdit,
								L"VideoDecoder",
								NULL,
								&dwType,
								NULL,
								&dwSize);

	if (status == ERROR_SUCCESS)
	{
		status = RegQueryValueEx(	hKeyDVREdit,
									L"VideoDecoder",
									NULL,
									&dwType,
									(LPBYTE) csVideoDecoder.GetBuffer(dwSize),
									&dwSize);

		csVideoDecoder.ReleaseBuffer();

		if (status != ERROR_SUCCESS)
		{
			RegCloseKey(hKeyDVREdit);
			return status;
		}
	}

	else if (status != ERROR_FILE_NOT_FOUND)
	{
		RegCloseKey(hKeyDVREdit);
		return status;
	}

	status = RegQueryValueEx(	hKeyDVREdit,
								L"AudioDecoder",
								NULL,
								&dwType,
								NULL,
								&dwSize);

	if (status == ERROR_SUCCESS)
	{
		status = RegQueryValueEx(	hKeyDVREdit,
									L"AudioDecoder",
									NULL,
									&dwType,
									(LPBYTE) csAudioDecoder.GetBuffer(dwSize),
									&dwSize);

		csAudioDecoder.ReleaseBuffer();

		if (status != ERROR_SUCCESS)
		{
			RegCloseKey(hKeyDVREdit);
			return status;
		}
	}
	
	else if (status != ERROR_FILE_NOT_FOUND)
	{
		RegCloseKey(hKeyDVREdit);
		return status;
	}

	if (csVideoDecoder.GetLength() > 0)
	{	
		csVideoDecoder.TrimLeft(L"{");
		csVideoDecoder.TrimRight(L"}");

		status = HRESULT_FROM_WIN32(UuidFromStringW((unsigned short *)(LPCWSTR) csVideoDecoder, &m_guidVideoDecoder));
		if (status != ERROR_SUCCESS)
		{
			RegCloseKey(hKeyDVREdit);
			return status;
		}
	}

	if (csAudioDecoder.GetLength() > 0)
	{
		csAudioDecoder.TrimLeft(L"{");
		csAudioDecoder.TrimRight(L"}");

		status = HRESULT_FROM_WIN32(UuidFromStringW((unsigned short *)(LPCWSTR) csAudioDecoder, &m_guidAudioDecoder));
		if (status != ERROR_SUCCESS)
		{
			RegCloseKey(hKeyDVREdit);
			return status;
		}
	}

return ERROR_SUCCESS;
}

HRESULT CGraph::SetFilterGraph(CString csSourceFile, HWND pParent)
{
    HRESULT hr = S_OK;
	
	hr = m_pGraph.CoCreateInstance(CLSID_FilterGraph);
	if (FAILED(hr))
	{
		DisplayError(pParent, L"Preview Graph - Failed to create preview graph: 0x%x", hr);
		return hr;
	}

	hr = GetDefaultDecodersFromRegistry();
	if (FAILED(hr))
	{
		DisplayError(pParent, L"Preview Graph - Failed to read default decoders from registry: 0x%x", hr);
		return hr;
	}

	CComPtr<IBaseFilter> pVideoDecoder;
	if (m_guidVideoDecoder != GUID_NULL)
	{
		hr = pVideoDecoder.CoCreateInstance(m_guidVideoDecoder);
		if (FAILED(hr))
			return hr;
		
		hr = m_pGraph->AddFilter(pVideoDecoder, L"Video Decoder");
		if (FAILED(hr))
			return hr;
	}

	CComPtr<IBaseFilter> pAudioDecoder;
	if (m_guidAudioDecoder != GUID_NULL)
	{
		hr = pAudioDecoder.CoCreateInstance(m_guidAudioDecoder);
		if (FAILED(hr))
			return hr;

		hr = m_pGraph->AddFilter(pAudioDecoder, L"Audio Decoder");
		if (FAILED(hr))
			return hr;
	}

#ifdef DEBUG // set up a running object table
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
/*
	hr = m_pVMR.CoCreateInstance(CLSID_VideoMixingRenderer9);
	if (FAILED(hr))
	{
		DisplayError(pParent, L"Preview Graph - Failed to create VMR: 0x%x", hr);
		return hr;
	}	

	hr = m_pGraph->AddFilter(m_pVMR,  L"VMR");
	if (FAILED(hr))
	{
		DisplayError(pParent, L"Preview Graph - Failed to add VMR to graph: 0x%x", hr);
		return hr;
	}
*/
	hr = m_pGraph->RenderFile((LPCWSTR) csSourceFile, NULL);
	if (FAILED(hr))
	{
		DisplayError(pParent, L"Preview Graph - Failed to render file: 0x%x", hr);
		return hr;
	}

	CComPtr<IEnumFilters> pEnum;
	hr = m_pGraph->EnumFilters(&pEnum);
	if (FAILED(hr))
	{
		DisplayError(pParent, L"Preview Graph - Failed to enum filters: 0x%x", hr);
		return hr;
	}

	CComPtr<IBaseFilter> pFilter;
	ULONG cFetched;

	while(pEnum->Next(1, &pFilter, &cFetched) == S_OK)
	{
		CLSID pClassID;
		hr = pFilter->GetClassID(&pClassID);
		if (FAILED(hr))
		{
			DisplayError(pParent, L"Preview Graph - Failed to get class id: 0x%x", hr);
			return hr;
		}

		if (pClassID == CLSID_StreamBufferSource)
			break;
		
		pFilter.Release();
	}

	CComQIPtr<IStreamBufferMediaSeeking> pSBSeek(pFilter);

	if (pFilter == NULL)
	{
		DisplayError(pParent, L"Preview Graph - Failed to find stream buffer source filter.", E_FAIL);
		return E_FAIL;
	}

	if (pSBSeek)
		m_pSBSeek = pSBSeek;
	
	else
	{
		DisplayError(pParent, L"Preview Graph - Failed to QI for stream buffer seek: 0x%x", hr);
		return hr;
	}

	Duration = 0;
	
	hr = m_pSBSeek->GetDuration(&Duration);
	if (FAILED(hr))
	{
		DisplayError(pParent, L"Preview Graph - Failed to get duration: 0x%x", hr);
		return hr;
	}

	hr = m_pGraph->QueryInterface(&m_pEvent);     // IMediaEventEx
	if (FAILED(hr))
	{
		DisplayError(pParent, L"Preview Graph - Failed to QI for media event: 0x%x", hr);
		return hr;
	}
	hr = m_pGraph->QueryInterface(&m_pControl);   // IMediaControl
	if (FAILED(hr))
	{
		DisplayError(pParent, L"Preview Graph - Failed to QI for media control: 0x%x", hr);
		return hr;
	}

	return hr;
}

HRESULT CGraph::ProcessEvents(HWND pParent)
{
    HRESULT hr;
    LONG evCode = 0, param1 = 0, param2 = 0;

//    ASSERT(fn);     // Assume that a callback function was provided

    // Verify that the IMediaEventEx interface is valid
    if (!m_pEvent)
        return E_UNEXPECTED;

    // Process all events
    while ((hr = m_pEvent->GetEvent(&evCode, (LONG_PTR *)&param1, 
                                  (LONG_PTR *)&param2, 0)) == S_OK)
    {
        // Call the specified callback function
        //if (fn)
          //  fn(evCode, param1, param2);
		if (evCode == EC_END_OF_SEGMENT)
			Stop(pParent);
        // Free memory associated with the callback
        m_pEvent->FreeEventParams(evCode, param1, param2);
    }

    return S_OK;
}

HRESULT CGraph::Stop(HWND pParent)
{
    HRESULT hr=S_OK;

    if (!m_pControl)
    {
        return E_UNEXPECTED;
    }

    if (m_state != State_Stopped)
    {
        hr = m_pControl->Pause();
		if (FAILED(hr))
		{
			DisplayError(pParent, L"Preview Graph Stop - Failed to pause: 0x%x", hr);
			return hr;
		}
		
		LONGLONG rtTime = 0; 

        hr = m_pControl->StopWhenReady();
		if (FAILED(hr))
		{
			DisplayError(pParent, L"Preview Graph Stop - Failed to stop: 0x%x", hr);
			return hr;
		}

		m_state = State_Stopped;
    }

    return hr;
}

HRESULT CGraph::Pause(HWND pParent)
{
    if (!m_pControl)
    {
        return E_UNEXPECTED;
    }
    else if (m_state == State_Paused)
    {
        return S_FALSE;
    }
    else
    {
		LONGLONG rtTime = 0; 
        m_state = State_Paused;
        
		HRESULT hr = m_pControl->Pause();
		if (FAILED(hr))
		{
			DisplayError(pParent, L"Preview Graph Pause - Failed to pause: 0x%x", hr);
		}
		return hr;
	}
}

HRESULT CGraph::Run(HWND pParent)
{
	HRESULT hr = S_OK;

    if (m_pControl)
    {

        LONGLONG rtTime = 0; 

		m_state = State_Running;

		hr = m_pControl->Run();
		if (FAILED(hr))
		{
			DisplayError(pParent, L"Preview Graph Run - Failed to run: 0x%x", hr);
		}
    }
    else
    {
        return E_UNEXPECTED;
    }
return hr;
}

HRESULT CGraph::StepForward(HWND pParent)
{
	HRESULT hr = S_OK;
	if (!m_pSBSeek)
    {
        return E_UNEXPECTED;
    }

	REFERENCE_TIME Pos = 0;
	hr = m_pSBSeek->GetCurrentPosition(&Pos);

	Pos += dblUNITS / 10;

    hr = m_pSBSeek->SetPositions(&Pos, AM_SEEKING_AbsolutePositioning, 
									NULL, AM_SEEKING_NoPositioning);
	return hr;
}

HRESULT CGraph::CommercialSkip(HWND pParent)
{
	HRESULT hr = S_OK;
	if (!m_pSBSeek)
    {
        return E_UNEXPECTED;
    }

	REFERENCE_TIME Pos = 0;
	hr = m_pSBSeek->GetCurrentPosition(&Pos);

	Pos += dblUNITS * 30;

    hr = m_pSBSeek->SetPositions(&Pos, AM_SEEKING_AbsolutePositioning, 
									NULL, AM_SEEKING_NoPositioning);
		
	return hr;
}

HRESULT CGraph::StepBackward(HWND pParent)
{
	HRESULT hr = S_OK;
	if (!m_pSBSeek)
    {
        return E_UNEXPECTED;
    }

	REFERENCE_TIME Pos = 0;
	hr = m_pSBSeek->GetCurrentPosition(&Pos);

	Pos -= dblUNITS / 10;

    hr = m_pSBSeek->SetPositions(&Pos, AM_SEEKING_AbsolutePositioning, 
									NULL, AM_SEEKING_NoPositioning);
	return hr;
}

HRESULT CGraph::SetVideoWindow(HWND hwnd)
{
    HRESULT hr = E_FAIL;
	
//	hr = m_pVMR_WC->SetVideoClippingWindow(hwnd);
	
	CComQIPtr<IVideoWindow> pVideo(m_pGraph);

    if (pVideo)
    {
        RECT grc;

		hr = pVideo->put_Visible(OAFALSE);
		if (FAILED(hr))
		{
			DisplayError(hwnd, L"Preview Graph Set Video Window - Failed to put invisible: 0x%x", hr);
			return hr;
		}
        
		hr = pVideo->put_Owner((OAHWND)hwnd); 
		if (FAILED(hr))
		{
			DisplayError(hwnd, L"Preview Graph Set Video Window - Failed to put owner: 0x%x", hr);
			return hr;
		}
        
		hr = pVideo->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		if (FAILED(hr))
		{
			DisplayError(hwnd, L"Preview Graph Set Video Window - Failed to put window style: 0x%x", hr);
			return hr;
		}

        GetClientRect(hwnd, &grc);
		
		hr = pVideo->SetWindowPosition(grc.left + 7, grc.top + 100, grc.right - 14, grc.bottom - 128);
		if (FAILED(hr))
		{
			DisplayError(hwnd, L"Preview Graph Set Video Window - Failed to set window position: 0x%x", hr);
			return hr;
		}		
		
		hr = pVideo->put_Visible(OATRUE);
		if (FAILED(hr))
		{
			DisplayError(hwnd, L"Preview Graph Set Video Window - Failed to put visible: 0x%x", hr);
			return hr;
		}		
		//hr = pVideo->SetWindowForeground(0);
		pVideo.Release();
    }

    return hr;
}

HRESULT CGraph::SetVideoPosition(LONG x, LONG y, LONG cx, LONG cy)
{
	CComQIPtr<IVideoWindow> pVideo(m_pGraph);
	HRESULT hr = E_FAIL;

    if (pVideo)
    {
        hr = pVideo->SetWindowPosition(x, y, cx, cy);
		pVideo.Release();
    }

    return hr;
}

HRESULT CGraph::SetEventWindow(HWND hwnd)
{
    if (m_pEvent)
    {
        return m_pEvent->SetNotifyWindow((OAHWND)hwnd, WM_GRAPHNOTIFY, 0);
    }
    else
    {
        return E_UNEXPECTED;
    }
}

HRESULT CGraph::GetCurrentPosition(REFERENCE_TIME *rtNow)
{
    return m_pSBSeek->GetCurrentPosition(rtNow);
}

HRESULT CGraph::GetState(OAFilterState *pfs)
{
	return m_pControl->GetState(10, pfs);
}

HRESULT CGraph::SetPositions(REFERENCE_TIME *posStart, REFERENCE_TIME *posStop)
{
	return m_pSBSeek->SetPositions(posStart, AM_SEEKING_AbsolutePositioning | AM_SEEKING_Segment | AM_SEEKING_NoFlush, 
									posStop, AM_SEEKING_AbsolutePositioning | AM_SEEKING_Segment | AM_SEEKING_NoFlush);
}

BOOL CGraph::CanFrameStep(void)
{
    HRESULT hr;
    IVideoFrameStep* pFS;

    // Get frame step interface
    hr = m_pGraph->QueryInterface(__uuidof(IVideoFrameStep), (PVOID *)&pFS);
    if (FAILED(hr))
        return FALSE;

    // Check if this decoder can step
    hr = pFS->CanStep(0L, NULL); 

    // Release frame step interface
    pFS->Release();

    if (hr == S_OK)
        return TRUE;
    else
        return FALSE;
}

HRESULT CGraph::FastForward()
{
	HRESULT hr = S_OK;
	if (!m_pSBSeek)
    {
        return E_UNEXPECTED;
    }

	double dbCurrentRate = 1;

	hr = m_pSBSeek->GetRate(&dbCurrentRate);

	dbCurrentRate *= 2;
		
	hr = m_pSBSeek->SetRate(dbCurrentRate);
	
	return hr;
}

HRESULT CGraph::Rewind()
{
	HRESULT hr = S_OK;
	if (!m_pSBSeek)
    {
        return E_UNEXPECTED;
    }

	double dbCurrentRate = 1;

	hr = m_pSBSeek->GetRate(&dbCurrentRate);

	if (dbCurrentRate == 1)
	{
		dbCurrentRate = abs((long long)dbCurrentRate) * -1;
	}

	else dbCurrentRate = abs((long long)dbCurrentRate) * -2;
		
	hr = m_pSBSeek->SetRate(dbCurrentRate);
		
	return hr;

}

