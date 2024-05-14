#pragma once

// Represents a physical (or virtual) controller button (or keyboard key).
class KeyButtonState
{
public:
	bool m_isPressed;
	bool m_wasPressedLastFrame;
};