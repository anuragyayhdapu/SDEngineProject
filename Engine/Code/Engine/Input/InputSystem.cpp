#include "Engine/Input/InputSystem.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include <Windows.h>	// to access the virtual key codes (VK_) & get mouse position

extern InputSystem* g_theInput;

const unsigned char KEYCODE_F1 = VK_F1;
const unsigned char KEYCODE_F2 = VK_F2;
const unsigned char KEYCODE_F3 = VK_F3;
const unsigned char KEYCODE_F4 = VK_F4;
const unsigned char KEYCODE_F5 = VK_F5;
const unsigned char KEYCODE_F6 = VK_F6;
const unsigned char KEYCODE_F7 = VK_F7;
const unsigned char KEYCODE_F8 = VK_F8;
const unsigned char KEYCODE_F9 = VK_F9;
const unsigned char KEYCODE_F10 = VK_F10;
const unsigned char KEYCODE_F11 = VK_F11;
const unsigned char KEYCODE_F12 = VK_F12;

const unsigned char KEYCODE_ESCAPE = VK_ESCAPE;
const unsigned char KEYCODE_SPACE = VK_SPACE;
const unsigned char KEYCODE_ENTER = VK_RETURN;
const unsigned char KEYCODE_BACKSPACE = VK_BACK;
const unsigned char KEYCODE_INSERT = VK_INSERT;
const unsigned char KEYCODE_DELETE = VK_DELETE;
const unsigned char KEYCODE_HOME = VK_HOME;
const unsigned char KEYCODE_END = VK_END;
const unsigned char KEYCODE_CONTROL = VK_CONTROL;

const unsigned char KEYCODE_UP		= VK_UP;
const unsigned char KEYCODE_DOWN	= VK_DOWN;
const unsigned char KEYCODE_LEFT	= VK_LEFT;
const unsigned char KEYCODE_RIGHT	= VK_RIGHT;

const unsigned char KEYCODE_LEFT_MOUSE = VK_LBUTTON;
const unsigned char KEYCODE_RIGHT_MOUSE = VK_RBUTTON;

const unsigned char KEYCODE_LEFT_BRACKET		 = 0xDB;
const unsigned char KEYCODE_RIGHT_BRACKET		 = 0xDD;
const unsigned char KEYCODE_SQUARE_BRACKET_LEFT = VK_OEM_4;
const unsigned char KEYCODE_SQUARE_BRACKET_RIGHT = VK_OEM_6;

const unsigned char KEYCODE_SHIFT = VK_SHIFT;

const unsigned char KEYCODE_TILDE = 0xC0;
const unsigned char KEYCODE_COMMA = VK_OEM_COMMA;
const unsigned char KEYCODE_PERIOD = VK_OEM_PERIOD;
const unsigned char KEYCODE_SEMICOLON = VK_OEM_1;
const unsigned char KEYCODE_SINGLE_QUOTE = VK_OEM_7;

const unsigned char KEYCODE_NUMPAD0 = VK_NUMPAD0;
const unsigned char KEYCODE_NUMPAD1 = VK_NUMPAD1;
const unsigned char KEYCODE_NUMPAD2 = VK_NUMPAD2;


const unsigned char KEYCODE_INVALID = 255;



InputSystem::InputSystem(InputSystemConfig const& config)
	: m_config(config)
{
}

InputSystem::~InputSystem()
{
}

void InputSystem::Startup()
{
	// set xbox controller ids
	for (int contollerNumber = 0; contollerNumber < NUM_XBOX_CONTROLLERS; contollerNumber++)
	{
		m_controllers[contollerNumber].m_controllerID = contollerNumber;
	}

	// subscribe event handlers to event system
	g_theEventSystem->SubscribeToEvent("KeyReleased", EventHandler_KeyReleased);
	g_theEventSystem->SubscribeToEvent("KeyPressed", EventHandler_KeyPressed);
}

void InputSystem::Shutdown()
{
	// un-subscribe event handlers from event system
	g_theEventSystem->UnsubscribeFromEvent("KeyReleased", EventHandler_KeyReleased);
	g_theEventSystem->UnsubscribeFromEvent("KeyPressed", EventHandler_KeyPressed);
}

void InputSystem::BeginFrame()
{
	// poll xbox controller and set values
	for (int contollerNumber = 0; contollerNumber < NUM_XBOX_CONTROLLERS; contollerNumber++)
	{
		m_controllers[contollerNumber].Update();
	}

	// hide or show mouse
	if (m_mouseState.m_currentHidden != m_mouseState.m_desiredHidden)
	{
		m_mouseState.m_currentHidden = m_mouseState.m_desiredHidden;

		// state changed to hidden
		// TODO: change this to use Windows code
		if (m_mouseState.m_currentHidden)
		{
			while (ShowCursor(false) >= 0)
				;
		}
		else // state changed to show
		{
			while (ShowCursor(true) < 0)
				;
		}
	}

	IntVec2 oldCursorPos = m_mouseState.m_cursorClientPosition;
	IntVec2 newCusrorPos = Window::GetWindowContext()->GetCursorPositionRelativeToClient();
	m_mouseState.m_cursorClientDelta = newCusrorPos - oldCursorPos;
	m_mouseState.m_cursorClientPosition = newCusrorPos;

	/*DebuggerPrintf("old   x: %f, old   y: %f\n", oldCursorPos.x, oldCursorPos.y);
	DebuggerPrintf("new   x: %f, new   y: %f\n", newCusrorPos.x, newCusrorPos.y);
	DebuggerPrintf("delta x: %f, delta y: %f\n", m_mouseState.m_cursorClientDelta.x, m_mouseState.m_cursorClientDelta.y);
	DebuggerPrintf("----\n");*/

	// relative mode changed check
	if (m_mouseState.m_currentRelative != m_mouseState.m_desiredRelative)
	{
		m_mouseState.m_cursorClientPosition = IntVec2::ZERO;
		m_mouseState.m_cursorClientDelta = IntVec2::ZERO;

		m_mouseState.m_currentRelative = m_mouseState.m_desiredRelative;
	}

	// in relative mode, reset cursor to the center of the screen
	if (m_mouseState.m_currentRelative)
	{
		IntVec2 clientDimensions = Window::GetWindowContext()->GetClientDimensions();
		IntVec2 clientCenter(int(clientDimensions.x * 0.5f), int(clientDimensions.y * 0.5f));

		m_mouseState.m_cursorClientPosition = clientCenter;
		Window::GetWindowContext()->SetCursorPositionRelativeToClient(clientCenter);
	}
}

void InputSystem::EndFrame()
{
	// update was key pressed last frame
	for (int keyCode = 0; keyCode < NUM_KEYCODES; ++keyCode)
	{
		m_keyStates[keyCode].m_wasPressedLastFrame = m_keyStates[keyCode].m_isPressed;
	}
}


//----------------------------------------------------------------------------------------------------------
//bool InputSystem::WasAnyKeyPressed()
//{
//	for ( int keyCode = 0; keyCode < NUM_KEYCODES; keyCode++ )
//	{
//		if ( WasKeyJustPressed( keyCode ) )
//		{
//			return true;
//		}
//	}
//
//	return false;
//}


bool InputSystem::WasKeyJustPressed(unsigned char keyCode)
{
	if (IsKeyDown(keyCode))
	{
		if (m_keyStates[keyCode].m_wasPressedLastFrame == false)
		{
			return true;
		}
	}

	return false;
}

bool InputSystem::WasKeyJustReleased(unsigned char keyCode)
{
	if (!IsKeyDown(keyCode))
	{
		if (m_keyStates[keyCode].m_wasPressedLastFrame == true)
		{
			return true;
		}
	}

	return false;
}

bool InputSystem::IsKeyDown(unsigned char keyCode)
{
	return m_keyStates[keyCode].m_isPressed;
}

bool InputSystem::HandleKeyPressed(unsigned char keyCode)
{
	m_keyStates[keyCode].m_isPressed = true;

	return true;
}

bool InputSystem::HandleKeyReleased(unsigned char keyCode)
{
	m_keyStates[keyCode].m_isPressed = false;

	return true;
}

const XboxController& InputSystem::GetController(int controllerID)
{
	return m_controllers[controllerID];
}


bool InputSystem::EventHandler_KeyReleased(EventArgs& args)
{
	if (g_theInput)
	{
		bool wasConsumed = false;
		unsigned char keycode = (unsigned char)args.GetValue("Keycode", KEYCODE_INVALID);
		wasConsumed = g_theInput->HandleKeyReleased(keycode);

		//DebuggerPrintf("wm_keyUp consumed in Input system: %c \n", keycode);

		return wasConsumed;
	}

	return false;
}

bool InputSystem::EventHandler_KeyPressed(EventArgs& args)
{
	if (g_theInput)
	{
		bool wasConsumed = false;
		unsigned char keycode = (unsigned char)args.GetValue("Keycode", KEYCODE_INVALID);
		wasConsumed = g_theInput->HandleKeyPressed(keycode);

		//DebuggerPrintf("wm_keyDown consumed in Input system: %c \n", keycode);

		return wasConsumed;
	}

	return false;
}


void InputSystem::SetCursorMode(bool hidden, bool relative)
{
	m_mouseState.m_desiredHidden = hidden;
	m_mouseState.m_desiredRelative = relative;
}

void InputSystem::ShowMouseCursor()
{
	while (ShowCursor(true) < 0)
		;
}

void InputSystem::HideMouseCursor()
{
	while (ShowCursor(false) >= 0)
		;
}


IntVec2 InputSystem::GetCursorClientPosition() const
{
	return m_mouseState.m_cursorClientPosition;
}


Vec2 InputSystem::GetCursorNormalizedPosition() const
{
	Window* s_windowContext = Window::GetWindowContext();
	IntVec2 clientDimensions = s_windowContext->GetClientDimensions();
	IntVec2 cursor = GetCursorClientPosition();
	
	Vec2 normalizedCusror;
	normalizedCusror.x = RangeMap((float)cursor.x, 0.f, (float)clientDimensions.x, 0.f, 1.f);
	normalizedCusror.y = RangeMap((float)cursor.y, 0.f, (float)clientDimensions.y, 0.f, 1.f);

	normalizedCusror.y = 1 - normalizedCusror.y; // flip y axis (because sd coordinates y min (0) is at bottom, but in windows y min (0) is at top)

	return normalizedCusror; 
}


IntVec2 InputSystem::GetCursorClientDelta() const
{
	return m_mouseState.m_cursorClientDelta;
}
