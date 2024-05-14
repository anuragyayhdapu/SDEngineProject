#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Time.hpp"

static Clock s_SystemClock;

//---------------------------------------------------------------------------------------------
Clock::Clock()
{
	if (this != &s_SystemClock)
	{
		m_parent = &s_SystemClock;
		m_parent->AddChild(this);
	}
}


//---------------------------------------------------------------------------------------------
Clock::~Clock()
{
	// unparent our children
	for (int index = 0; index < m_children.size(); index ++)
	{
		Clock* child = m_children[index];
		if (child)
		{
			child->m_parent = nullptr;
		}
	}

	// unparent ourself
	if (m_parent)
	{
		m_parent->RemoveChild(this);
	}
}


//---------------------------------------------------------------------------------------------
Clock::Clock(Clock& parent)
{
	m_parent = &parent;
	m_parent->AddChild(this);
}


//---------------------------------------------------------------------------------------------
void Clock::Reset()
{
	// Reset all book keeping variables values back to zero
	m_totalSeconds = 0.f;
	m_deltaSeconds = 0.f;
	m_frameCount = 0;

	// get the current time as the last updated time
	m_lastUpdateTimeInSeconds = static_cast<float>(GetCurrentTimeSeconds());
}


//---------------------------------------------------------------------------------------------
bool Clock::IsPaused() const
{
	return m_isPaused;
}


//---------------------------------------------------------------------------------------------
void Clock::Pause()
{
	m_isPaused = true;
}


//---------------------------------------------------------------------------------------------
void Clock::Unpause()
{
	m_isPaused = false;
}


//---------------------------------------------------------------------------------------------
void Clock::TogglePause()
{
	m_isPaused = !m_isPaused;
}


//---------------------------------------------------------------------------------------------
void Clock::StepSingleFrame()
{
	m_stepSingleFrame = true;
	m_isPaused = false;
}


//---------------------------------------------------------------------------------------------
void Clock::SetTimeScale(float timeScale)
{
	m_timeScale = timeScale;
}


//---------------------------------------------------------------------------------------------
float Clock::GetTimeScale() const
{
	return m_timeScale;
}


//---------------------------------------------------------------------------------------------
float Clock::GetDeltaSeconds() const
{
	return m_deltaSeconds;
}


//---------------------------------------------------------------------------------------------
float Clock::GetTotalSeconds() const
{
	return m_totalSeconds;
}


//---------------------------------------------------------------------------------------------
size_t Clock::GetFrameCount() const
{
	return m_frameCount;
}


//---------------------------------------------------------------------------------------------
Clock& Clock::GetSystemClock()
{
	return s_SystemClock;
}


//---------------------------------------------------------------------------------------------
void Clock::TickSystemClock()
{
	s_SystemClock.Tick();
}


//---------------------------------------------------------------------------------------------
void Clock::Tick()
{
	float currentTime = static_cast<float>(GetCurrentTimeSeconds());
	float deltaSeconds = currentTime - m_lastUpdateTimeInSeconds;
	m_lastUpdateTimeInSeconds = currentTime;
	if (deltaSeconds > m_maxDeltaSeconds)
	{
		deltaSeconds = m_maxDeltaSeconds;
	}
	
	Advance(deltaSeconds);
}


//---------------------------------------------------------------------------------------------
void Clock::Advance(float deltaTimeSeconds)
{
	// Calculates delta seconds based on pausing and time scale
	if (m_isPaused)
	{
		deltaTimeSeconds = 0.0f;
	}

	deltaTimeSeconds *= m_timeScale;

	// updates all book keeping variables
	m_deltaSeconds = deltaTimeSeconds;
	m_totalSeconds += m_deltaSeconds;
	m_frameCount += 1;

	// calls Advance on all child clocks
	for (int index = 0; index < m_children.size(); index++)
	{
		Clock* child = m_children[index];
		if (child)
		{
			child->Advance(deltaTimeSeconds);
		}
	}

	// handle pasuing after frames for stepping single frame
	if (m_stepSingleFrame)
	{
		m_isPaused = true;
		m_stepSingleFrame = false;
	}
}


//---------------------------------------------------------------------------------------------
void Clock::AddChild(Clock* childClock)
{
	if (childClock == this)	// don't add yourself in your childern list
		return;

	if (childClock)
	{
		m_children.push_back(childClock);
	}
}


//---------------------------------------------------------------------------------------------
void Clock::RemoveChild(Clock* childClock)
{
	if (childClock == nullptr)
		return;

	auto iter = std::find(m_children.begin(), m_children.end(), childClock);
	if (iter != m_children.end())
	{
		m_children.erase(iter);
	}
}
