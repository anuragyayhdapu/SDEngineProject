#include "XboxController.hpp"

#include <Windows.h>
#include <Xinput.h>

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

#pragma comment(lib, "xinput9_1_0")  // linking XInput static library (.lib)

XboxController::XboxController()
{
	m_leftStick.SetDeadZoneThresholds(NORMALIZED_INNER_DEAD_ZONE_THRESHOLD,
		NORMALIZED_OUTER_DEAD_ZONE_THRESHOLD);
	m_rightStick.SetDeadZoneThresholds(NORMALIZED_INNER_DEAD_ZONE_THRESHOLD,
		NORMALIZED_OUTER_DEAD_ZONE_THRESHOLD);
}

XboxController::~XboxController()
{
}

bool XboxController::IsConnected() const
{
	return m_isConnected;
}

int XboxController::GetControllerID() const
{
	return m_controllerID;
}

const AnalogJoystick& XboxController::GetLeftStick() const
{
	return m_leftStick;
}

const AnalogJoystick& XboxController::GetRightStick() const
{
	return m_rightStick;
}

float XboxController::GetLeftTrigger() const
{
	return m_leftTrigger;
}

float XboxController::GetRightTrigger() const
{
	return m_rightTrigger;
}

const KeyButtonState& XboxController::GetButton(XboxButtonID buttonID) const
{
	return m_buttons[buttonID];
}

bool XboxController::IsButtonDown(XboxButtonID buttonID) const
{
	return m_buttons[buttonID].m_isPressed;
}

bool XboxController::WasButtonJustPressed(XboxButtonID buttonID) const
{
	if (IsButtonDown(buttonID))
	{
		if (m_buttons[buttonID].m_wasPressedLastFrame == false)
		{
			return true;
		}
	}

	return false;
}

bool XboxController::WasButtonJustReleased(XboxButtonID buttonID) const
{
	if (IsButtonDown(buttonID) == false)
	{
		if (m_buttons[buttonID].m_wasPressedLastFrame == true)
		{
			return true;
		}
	}

	return false;
}

void XboxController::Update()
{
	XINPUT_STATE xboxControllerState = {};
	DWORD errorStatus = XInputGetState(m_controllerID, &xboxControllerState);
	if (errorStatus == ERROR_SUCCESS)
	{
		m_isConnected = true;
		XINPUT_GAMEPAD const& state = xboxControllerState.Gamepad;

		UpdateJoyStick(m_leftStick, state.sThumbLX, state.sThumbLY);
		UpdateJoyStick(m_rightStick, state.sThumbRX, state.sThumbRY);

		UpdateTrigger(m_leftTrigger, state.bLeftTrigger);
		UpdateTrigger(m_rightTrigger, state.bRightTrigger);

		UpdateButton(XBOX_BUTTON_A, state.wButtons, XINPUT_GAMEPAD_A);
		UpdateButton(XBOX_BUTTON_B, state.wButtons, XINPUT_GAMEPAD_B);
		UpdateButton(XBOX_BUTTON_X, state.wButtons, XINPUT_GAMEPAD_X);
		UpdateButton(XBOX_BUTTON_Y, state.wButtons, XINPUT_GAMEPAD_Y);
		UpdateButton(XBOX_BUTTON_DPAD_UP, state.wButtons, XINPUT_GAMEPAD_DPAD_UP);
		UpdateButton(XBOX_BUTTON_DPAD_DOWN, state.wButtons, XINPUT_GAMEPAD_DPAD_DOWN);
		UpdateButton(XBOX_BUTTON_DPAD_LEFT, state.wButtons, XINPUT_GAMEPAD_DPAD_LEFT);
		UpdateButton(XBOX_BUTTON_DPAD_RIGHT, state.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT);
		UpdateButton(XBOX_BUTTON_START, state.wButtons, XINPUT_GAMEPAD_START);
		UpdateButton(XBOX_BUTTON_BACK, state.wButtons, XINPUT_GAMEPAD_BACK);
		UpdateButton(XBOX_BUTTON_LEFT_JOYSTICK, state.wButtons, XINPUT_GAMEPAD_LEFT_THUMB);
		UpdateButton(XBOX_BUTTON_RIGHT_JOYSTICK, state.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB);
		UpdateButton(XBOX_BUTTON_LEFT_BUMPER, state.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER);
		UpdateButton(XBOX_BUTTON_RIGHT_BUMPER, state.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER);
	}
	else if (errorStatus == ERROR_DEVICE_NOT_CONNECTED)
	{
		Reset();
	}
}

void XboxController::Reset()
{
	m_isConnected = false;
	m_leftTrigger = 0.f;
	m_rightTrigger = 0.f;

	for (int buttonIndex = 0; buttonIndex < (int)XboxButtonID::NUM; ++buttonIndex)
	{
		m_buttons[buttonIndex].m_isPressed = false;
		m_buttons[buttonIndex].m_wasPressedLastFrame = false;
	}

	m_leftStick.Reset();
	m_rightStick.Reset();
}

void XboxController::UpdateJoyStick(AnalogJoystick& out_joystick, short rawX, short rawY)
{
	// normalize rawX and rawY
	// rangeMap -32768 to -1 & +32767 to +1
	float rawNormalizedX = RangeMap(rawX, RAW_MIN_JOYSTICK, RAW_MAX_JOYSTICK, NORMALIZED_MIN_JOYSTICK, NORMALIZED_MAX_JOYSTICK);
	float rawNormalizedY = RangeMap(rawY, RAW_MIN_JOYSTICK, RAW_MAX_JOYSTICK, NORMALIZED_MIN_JOYSTICK, NORMALIZED_MAX_JOYSTICK);

	out_joystick.UpdatePosition(rawNormalizedX, rawNormalizedY);
}

void XboxController::UpdateTrigger(float& out_triggerValue, unsigned char rawValue)
{
	out_triggerValue = RangeMap(static_cast<float>(rawValue),
		MIN_RAW_TRIGGER_VALUE, MAX_RAW_TRIGGER_VALUE,
		MIN_TRIGGER_OUTPUT, MAX_TRIGGER_OUTPUT);
}

void XboxController::UpdateButton(XboxButtonID buttonID, unsigned short buttonFlags, unsigned short xInputButtonId)
{
	bool buttonCurrentState = (buttonFlags & xInputButtonId) == xInputButtonId;
	bool buttonPreviousState = m_buttons[buttonID].m_isPressed;

	m_buttons[buttonID].m_isPressed = buttonCurrentState;
	m_buttons[buttonID].m_wasPressedLastFrame = buttonPreviousState;
}
