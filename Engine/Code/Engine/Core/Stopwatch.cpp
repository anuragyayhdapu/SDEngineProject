#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//---------------------------------------------------------------------------------------------
Stopwatch::Stopwatch(float duration)
{
	m_clock = new Clock();
	m_duration = duration;
}


//---------------------------------------------------------------------------------------------
Stopwatch::Stopwatch(Clock const* clock, float duration)
{
	m_clock = clock;
	m_duration = duration;
}


//---------------------------------------------------------------------------------------------
void Stopwatch::Start()
{
	m_startTime = m_clock->GetTotalSeconds();
}


//---------------------------------------------------------------------------------------------
void Stopwatch::Restart()
{
	bool isStopped = IsStopped();
	if (!isStopped)
	{
		m_startTime = m_clock->GetTotalSeconds();
	}
}


//---------------------------------------------------------------------------------------------
void Stopwatch::Stop()
{
	m_startTime = 0.f;
}


//---------------------------------------------------------------------------------------------
float Stopwatch::GetElapsedTime() const
{
	bool isStopped = IsStopped();
	if (isStopped)
	{
		return 0.f;
	}
	else
	{
		float currentTime = m_clock->GetTotalSeconds();
		float timeElapsed = currentTime - m_startTime;

		//DebuggerPrintf("Elapsed time: %f: \n", timeElapsed);

		return timeElapsed;
	}
}


//---------------------------------------------------------------------------------------------
float Stopwatch::GetElapsedFraction() const
{
	float elapsedTime = GetElapsedTime();
	float fraction = elapsedTime / m_duration;
	return fraction;
}


//---------------------------------------------------------------------------------------------
bool Stopwatch::IsStopped() const
{
	if (m_startTime == 0.f)
	{
		return true;
	}
	else
	{
		return false;
	}
}


//---------------------------------------------------------------------------------------------
bool Stopwatch::HasDurationElapsed() const
{
	float elapsedTime = GetElapsedTime();
	bool isStopped = IsStopped();
	if ((elapsedTime > m_duration) && (!isStopped))
	{
		return true;
	}
	else
	{
		return false;
	}
}


//---------------------------------------------------------------------------------------------
bool Stopwatch::DecrementDurationIfElapsed()
{
	bool hasDurationElapsed = HasDurationElapsed();
	bool isStopped = IsStopped();

	if (hasDurationElapsed && !isStopped)
	{
		m_startTime += m_duration;
		return true;
	}

	return false;
}
