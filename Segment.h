//#pragma once

#include <dshow.h>
#include "afxtempl.h"

// CSegment command target

class CSegment : public CObject
{
public:
	CSegment(REFERENCE_TIME rtStart, REFERENCE_TIME rtStop);
	virtual ~CSegment();
	BOOL SetInTime(REFERENCE_TIME rtInTime);
	BOOL SetOutTime(REFERENCE_TIME rtOutTime);
	REFERENCE_TIME InTime(void) {return m_rtInTime;};
	REFERENCE_TIME OutTime(void) {return m_rtOutTime;};
	REFERENCE_TIME SegmentLength(void) {return ((m_rtOutTime - m_rtInTime) + 1);};
private:
	REFERENCE_TIME m_rtInTime;
	REFERENCE_TIME m_rtOutTime;
	REFERENCE_TIME m_rtEnd;
};

// CSegment member functions

class CSegmentQueue : public CTypedPtrList<CObList, CSegment*>
{
private:
	CRITICAL_SECTION QueueAccessControlCriticalSection; 

public:
	CSegmentQueue(void);
	~CSegmentQueue(void);
	void Enqueue(CSegment* pSegment);
	CSegment* Dequeue(void);
	DWORD TotalTimeInSeconds(void);
	unsigned __int64 TotalTimeInRefTime(void);
};