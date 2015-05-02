#include "CTimer.h"

//-----------------------------------------------------------------------------
// Name : CTimer (constructor)
//-----------------------------------------------------------------------------
CTimer::CTimer(void)
{
	QueryPerformanceFrequency((LARGE_INTEGER *)&m_PerfFreq);
	QueryPerformanceCounter((LARGE_INTEGER *) &m_lastTime); 
	m_TimeScale			= 1.0f / m_PerfFreq;
	m_avgTimeDelta		= 0.0;
	m_SampleCount		= 0;

	m_FrameRate			= 0;
	m_FPSFrameCount		= 0;
	m_FPSTimeElapsed	= 0.0f;
}

//-----------------------------------------------------------------------------
// Name : CTimer (destructor)
//-----------------------------------------------------------------------------
CTimer::~CTimer(void)
{
}

//-----------------------------------------------------------------------------
// Name : frameAdvanced
// Desc : signals that a frame has advanced and there is a need to recalculate the time delta and the FPS
//-----------------------------------------------------------------------------
void CTimer::frameAdvanced()
{
	__int64 currTime;

	QueryPerformanceCounter((LARGE_INTEGER *)&currTime);
	float curTimeDelta=(currTime - m_lastTime) * m_TimeScale;//the current time delta between frames
	m_lastTime=currTime;

	if ( fabsf(curTimeDelta - m_avgTimeDelta) < 1.0f  )
	{
		// Wrap FIFO frame time buffer.
		memmove( &m_timeDeltas[1], m_timeDeltas, (MAX_SAMPLE_COUNT - 1) * sizeof(float) );
		m_timeDeltas[ 0 ] = curTimeDelta;
		if ( m_SampleCount < MAX_SAMPLE_COUNT ) 
			m_SampleCount++;

	} // End if

	// Count up the new average elapsed time
	m_avgTimeDelta = 0.0f;

	for ( ULONG i = 0; i < m_SampleCount; i++ )
		m_avgTimeDelta += m_timeDeltas[ i ];

	if ( m_SampleCount > 0 )
		m_avgTimeDelta /= m_SampleCount;

	// Calculate Frame Rate
	m_FPSFrameCount++;
	m_FPSTimeElapsed += curTimeDelta;

	if ( m_FPSTimeElapsed > 1.0f) 
	{
		m_FrameRate			= m_FPSFrameCount;
		m_FPSFrameCount		= 0;
		m_FPSTimeElapsed	= 0.0f;
	} // End If Second Elapsed

}

//-----------------------------------------------------------------------------
// Name : getTimeElapsed
//-----------------------------------------------------------------------------
float CTimer::getTimeElapsed()
{
	return m_avgTimeDelta;
}

//-----------------------------------------------------------------------------
// Name : getFPS
//-----------------------------------------------------------------------------
unsigned long CTimer::getFPS()
{
	return m_FrameRate;
}

//-----------------------------------------------------------------------------
// Name : getCurrentTime
//-----------------------------------------------------------------------------
float CTimer::getCurrentTime()
{
	__int64 currTime;

	QueryPerformanceCounter((LARGE_INTEGER *)&currTime);

	return currTime * m_TimeScale;
}