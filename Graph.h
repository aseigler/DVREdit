#pragma once

extern int DisplayError(HWND hWnd, wchar_t* pszMessage, HRESULT hr);

class CGraph
{
	private:
		CComPtr<IGraphBuilder>  m_pGraph;
		CComPtr<IMediaEventEx>  m_pEvent;
		CComPtr<IMediaControl>  m_pControl;
		CComPtr <IStreamBufferMediaSeeking> m_pSBSeek;
		CComPtr <IBaseFilter> m_pVMR;
		FILTER_STATE            m_state;
		BOOL CanFrameStep();
		HRESULT GetDefaultDecodersFromRegistry();
		GUID m_guidVideoDecoder;
		GUID m_guidAudioDecoder;

	public:
		
		FILTER_STATE State() { return m_state; }
		void Dispose(void);
		HRESULT SetFilterGraph(CString csSourceFile, HWND pParent);

		HRESULT SetEventWindow(HWND hwnd);
		HRESULT ProcessEvents(HWND pParent);
		HRESULT SetVideoPosition(LONG x, LONG y, LONG cx, LONG cy);
	    HRESULT GetCurrentPosition(REFERENCE_TIME *rtNow);
		HRESULT SetVideoWindow(HWND hwnd);
		HRESULT GetState(OAFilterState *pfs);
		HRESULT SetPositions(REFERENCE_TIME *posStart, REFERENCE_TIME *posStop);
		HRESULT Stop(HWND pParent);
		HRESULT Pause(HWND pParent);
		HRESULT Run(HWND pParent);
		HRESULT StepForward(HWND pParent);
		HRESULT StepBackward(HWND pParent);
		HRESULT CommercialSkip(HWND pParent);
		HRESULT FastForward();
		HRESULT Rewind();
		LONGLONG Duration;
		        
		CGraph(void);
		~CGraph(void);
};
