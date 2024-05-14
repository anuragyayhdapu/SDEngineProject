#pragma once

#include "Engine/Input/XboxController.hpp"
#include "Engine/Input/KeyButtonState.hpp"
#include "Engine/Math/IntVec2.hpp"

extern const unsigned char KEYCODE_F1;
extern const unsigned char KEYCODE_F2;
extern const unsigned char KEYCODE_F3;
extern const unsigned char KEYCODE_F4;
extern const unsigned char KEYCODE_F5;
extern const unsigned char KEYCODE_F6;
extern const unsigned char KEYCODE_F7;
extern const unsigned char KEYCODE_F8;
extern const unsigned char KEYCODE_F9;
extern const unsigned char KEYCODE_F10;
extern const unsigned char KEYCODE_F11;
extern const unsigned char KEYCODE_F12;

extern const unsigned char KEYCODE_ESCAPE;
extern const unsigned char KEYCODE_SPACE;
extern const unsigned char KEYCODE_ENTER;
extern const unsigned char KEYCODE_CONTROL;

extern const unsigned char KEYCODE_UP;
extern const unsigned char KEYCODE_DOWN;
extern const unsigned char KEYCODE_LEFT;
extern const unsigned char KEYCODE_RIGHT;
extern const unsigned char KEYCODE_BACKSPACE;
extern const unsigned char KEYCODE_INSERT;
extern const unsigned char KEYCODE_DELETE;
extern const unsigned char KEYCODE_HOME;
extern const unsigned char KEYCODE_END;

extern const unsigned char KEYCODE_LEFT_MOUSE;
extern const unsigned char KEYCODE_RIGHT_MOUSE;

extern const unsigned char KEYCODE_LEFT_BRACKET;
extern const unsigned char KEYCODE_RIGHT_BRACKET;
extern const unsigned char KEYCODE_SQUARE_BRACKET_LEFT;
extern const unsigned char KEYCODE_SQUARE_BRACKET_RIGHT;

extern const unsigned char KEYCODE_SHIFT;

extern const unsigned char KEYCODE_TILDE;
extern const unsigned char KEYCODE_COMMA;
extern const unsigned char KEYCODE_PERIOD;
extern const unsigned char KEYCODE_SEMICOLON;
extern const unsigned char KEYCODE_SINGLE_QUOTE;

extern const unsigned char KEYCODE_NUMPAD0;
extern const unsigned char KEYCODE_NUMPAD1;
extern const unsigned char KEYCODE_NUMPAD2;

extern const unsigned char KEYCODE_INVALID;

constexpr int NUM_KEYCODES		   = 256;
constexpr int NUM_XBOX_CONTROLLERS = 4;

struct MouseState
{
	IntVec2 m_cursorClientPosition;
	IntVec2 m_cursorClientDelta;

	bool m_currentHidden = false;
	bool m_desiredHidden = false;

	bool m_currentRelative = false;
	bool m_desiredRelative = false;
};


struct InputSystemConfig
{
};

// Engine input system; owns 4 XboxControllers, keyboard key (and mouse) states.
class InputSystem
{
public:
	InputSystem( InputSystemConfig const& config );
	~InputSystem();

	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	// bool WasAnyKeyPressed();
	bool WasKeyJustPressed( unsigned char keyCode );
	bool WasKeyJustReleased( unsigned char keyCode );
	bool IsKeyDown( unsigned char keyCode );
	bool HandleKeyPressed( unsigned char keyCode );
	bool HandleKeyReleased( unsigned char keyCode );

	const XboxController& GetController( int controllerID );

	void SetCursorMode( bool hidden, bool relative );
	void ShowMouseCursor();
	void HideMouseCursor();

	// Returns the cursor position in pixels, relative to the client region
	IntVec2 GetCursorClientPosition() const;

	// Returns the cursor position, normalized to the range [0,1], relative to the client region,
	// with the y-axis inverted to map Windows conventions to game screen camera conventions
	Vec2 GetCursorNormalizedPosition() const;

	// Returns the current frame cursor delta in pixel, relative to the client region
	// GetCursorClientDelta is used to know how much the cursor has moved since the last frame
	IntVec2 GetCursorClientDelta() const;

protected:
	KeyButtonState m_keyStates[ NUM_KEYCODES ]			 = {};
	XboxController m_controllers[ NUM_XBOX_CONTROLLERS ] = {};

	InputSystemConfig m_config;

	MouseState m_mouseState;

	static bool EventHandler_KeyReleased( EventArgs& args );
	static bool EventHandler_KeyPressed( EventArgs& args );
};