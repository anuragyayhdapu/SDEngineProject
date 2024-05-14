#pragma once

class Clock;


//---------------------------------------------------------------------------------------------
// Timer class that can be attached to any clock in a hierarchy and correctly handles duration
// regardless of update frequencey
class Stopwatch
{
public:
	// create a clock with a duration and the system clock as our clock
	explicit Stopwatch(float duration);

	// Create a clock with a duration and an explicitely specified clock to use
	Stopwatch(Clock const* clock, float duration);

	// Set the start time to the Clock's current total time
	void Start();

	// Set the start time to the Clock's current total time if we are not stopped
	void Restart();

	// Sets the start time back to zero
	void Stop();

	// Returns zero if stopped, otherwise returns the time elapsed between the clock's 
	// current time and our start time
	float GetElapsedTime() const;

	// Retuens the elapsed time as a percentage of our duration
	float GetElapsedFraction() const;

	// Returns true if our start time is zero
	bool IsStopped() const;

	// Returns true if our elapsed time is greater than our duration and we are not stopped
	bool HasDurationElapsed() const;

	// If a duration has elapsed and we are not stopped, decrements a duration by adding a
	// duration to the start time and returns true. Generally called within a loop until it
	// returns false so the caller can process each elapsed duration
	bool DecrementDurationIfElapsed();


	const Clock* m_clock = nullptr;
	float m_startTime = 0.f;
	float m_duration = 0.f;
};