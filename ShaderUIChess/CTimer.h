#pragma once

#include <Windows.h>
#include <math.h>

const ULONG MAX_SAMPLE_COUNT	= 50;		   // Maximum frame time sample count

class CTimer
{
public:
	CTimer(void);
	virtual ~CTimer(void);

	void			frameAdvanced  ();
	float			getTimeElapsed ();
	float			getCurrentTime ();
	unsigned long	getFPS		   ();

private:
	__int64					m_lastTime;
	__int64					m_PerfFreq;
	float					m_TimeScale;
	float					m_avgTimeDelta;
	float                   m_timeDeltas[MAX_SAMPLE_COUNT];
	ULONG					m_SampleCount;

	unsigned long			m_FrameRate;       // Stores current framerate
	unsigned long			m_FPSFrameCount;   // Elapsed frames in any given second
	float					m_FPSTimeElapsed;  // How much time has passed during FPS sample

};
