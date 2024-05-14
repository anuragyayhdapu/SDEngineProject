#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "KeyButtonState.hpp"
#include "AnalogJoystick.hpp"


class XboxController
{
	friend class InputSystem;

public:
	XboxController();
	~XboxController();

	bool IsConnected() const;
	int GetControllerID() const;

	const AnalogJoystick& GetLeftStick() const;
	const AnalogJoystick& GetRightStick() const;
	float GetLeftTrigger() const;
	float GetRightTrigger() const;
	const KeyButtonState& GetButton(XboxButtonID buttonID) const;
	bool IsButtonDown(XboxButtonID buttonID) const;
	bool WasButtonJustPressed(XboxButtonID buttonID) const;
	bool WasButtonJustReleased(XboxButtonID buttonID) const;

private:
	void Update();
	void Reset();
	void UpdateJoyStick(AnalogJoystick& out_joystick, short rawX, short rawY);
	void UpdateTrigger(float& out_triggerValue, unsigned char rawValue);
	void UpdateButton(XboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonFlag);

private:
	int m_controllerID = -1;
	bool m_isConnected = false;
	float m_leftTrigger = 0.f;
	float m_rightTrigger = 0.f;
	KeyButtonState m_buttons[(int)XboxButtonID::NUM] = {};
	AnalogJoystick m_leftStick;
	AnalogJoystick m_rightStick;
};