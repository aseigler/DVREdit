// Segment.cpp : implementation file
//

#include "stdafx.h"
#include "dvredit.h"
//#include "Segment.h"
#include "dvreditdlg.h"

// CSegment

CSegment::CSegment(REFERENCE_TIME rtStart, REFERENCE_TIME rtStop)
{
	m_rtInTime = rtStart;
	m_rtOutTime = rtStop;
	m_rtEnd = rtStop;
}

CSegment::~CSegment()
{
}

BOOL CSegment::SetInTime(REFERENCE_TIME rtInTime)
{
	if (rtInTime >= 0)
	{
		m_rtInTime = rtInTime;
		return TRUE;
	}
	return FALSE;
}

BOOL CSegment::SetOutTime(REFERENCE_TIME rtOutTime)
{
	if (rtOutTime <= 0xffffffffffffffff)
	{
		if (rtOutTime <= m_rtEnd)
		{
			m_rtOutTime = rtOutTime;
			return TRUE;
		}
	}
return FALSE;
}

CSegmentQueue::CSegmentQueue(void)
{
	InitializeCriticalSection(&QueueAccessControlCriticalSection);
}

CSegmentQueue::~CSegmentQueue(void)
{
	DeleteCriticalSection(&QueueAccessControlCriticalSection);
}

void CSegmentQueue::Enqueue(CSegment* pSegment)
{
	EnterCriticalSection(&QueueAccessControlCriticalSection);

	AddTail(pSegment);

	LeaveCriticalSection(&QueueAccessControlCriticalSection);
}

CSegment* CSegmentQueue::Dequeue(void)
{
	CSegment* pReturn = NULL;
    
	EnterCriticalSection(&QueueAccessControlCriticalSection);

	if (!IsEmpty())
		pReturn = RemoveHead();

	LeaveCriticalSection(&QueueAccessControlCriticalSection);

	return pReturn;
}

DWORD CSegmentQueue::TotalTimeInSeconds(void)
{
	return (double(TotalTimeInRefTime()) / dblUNITS);
}

unsigned __int64 CSegmentQueue::TotalTimeInRefTime(void)
{
	REFERENCE_TIME rtTemp = 0;
	CSegment* pSegment = NULL;

	EnterCriticalSection(&QueueAccessControlCriticalSection);

	POSITION pos = GetHeadPosition();

	if (pos != NULL)
		pSegment = GetHead();

	while (pos != NULL)
	{
		rtTemp += pSegment->SegmentLength();
		pSegment = GetNext(pos);
	}

	LeaveCriticalSection(&QueueAccessControlCriticalSection);

	return rtTemp;
}