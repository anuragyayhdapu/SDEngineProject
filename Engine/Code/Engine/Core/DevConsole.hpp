#pragma once

#include "Engine/Core/Clock.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Rgba8.hpp"

#include <string>
#include <vector>
#include <mutex>


//---------------------------------------------------------------------------------------------
class Renderer;
class Camera;
class BitmapFont;
class Stopwatch;
class AABB2;


//---------------------------------------------------------------------------------------------
// Stores the text and color for an individual line of text
struct DevConsoleLine
{
	Rgba8		m_color;
	std::string m_text;
};


//---------------------------------------------------------------------------------------------
// Dev console defaults. A Renderer must be provided
struct DevConsoleConfig
{
	Renderer* m_renderer	= nullptr;
	std::string m_fontName	= "SquirrelFixedFont";
	float m_fontAspect		= 0.7f;
	float m_linesOnScreen	= 40.f;
	int m_maxCommandHistory = 128;
};

struct DevConsoleRenderConfig
{
	Camera* m_camera = nullptr;
};


//------------------------------------------------------------------------------------------------
// Class for a dev console that allows enterting text and executing commands. Can be toggled with
// tilde ('~') and renders within a transparent box with configurable bouns. Other features
// include specific coloring for different lines of text and a blinking caret.
class DevConsole
{
public:
	DevConsole(DevConsoleConfig const& config);
	~DevConsole();


	void Startup();   // Subscribes to any events needed, prints an initial line of text, and starts the caret timer.
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void LoadFonts();


	// Adds a line of text to the current list of lines being shown, Individual lines are delimited
	// with the newline ('\n') character
	void AddLine(Rgba8 const& color, std::string const& text);

	// Renders just visible text lines within the bounds specified. Bounds are in terms of the camera
	// being used to render. The current input line renders at the bottom with all others lines rendered
	// above it, with the most recent lines at the bottom
	void Render(DevConsoleRenderConfig const& renderConfig) const;

	// Toggles between open and closed
	void ToggleOpen(bool newState);

	inline bool IsOpen() const { return m_isOpen; }

	// Rgba8 color consts
	static const Rgba8 ERROR_COLOR;
	static const Rgba8 WARNING_COLOR;
	static const Rgba8 INFO_MAJOR_COLOR;
	static const Rgba8 INFO_MINOR_COLOR;
	static const Rgba8 COMMAND_ECHO_COLOR;
	static const Rgba8 COMMAND_ECHO_COLOR_DIM;
	static const Rgba8 INPUT_TEXT_COLOR;
	static const Rgba8 INPUT_CARET_COLOR;


	// event-handlers
	static bool EventHandler_KeyPressed	(EventArgs& args);		// Handle key input
	static bool EventHandler_CharInput(EventArgs& args);		// Handle char input by appending valid characters to our current input line
	static bool EventHandler_ClearTextCommand(EventArgs& args);	// Clear all lines of text
	static bool EventHandler_HelpCommand(EventArgs& args);		// Display all currently registered commands in the event system
	static bool EventHandler_EventLogger(EventArgs& args);		

	// Parses the current input line and executes it using the event system. Commands and arguments
	// are delimited from each other with space (' ') and argument names and values are delimited with
	// equals ('='). Echoes the command to the dev console as well as any command output
	void Execute( std::string const& consoleCommandText );

	DevConsoleConfig m_config;

protected:
	

	bool m_isOpen = false; // True if the dev console is currently visible and accepting input
	
	void Echo(std::string command, EventArgs& args);

	

	
	Strings SplitCommandIgnoreInQuotes( std::string const& consoleCommandText, char delimiterToSplitOn );

	// input
	std::vector<DevConsoleLine> m_lines;	 // All lines added to the dev console since the last time it was cleared
	std::string					m_inputText; // Our current line of input text
	BitmapFont*					m_bitmapFont = nullptr;
	void						InitWelcomeText();
	bool						HandleCharInput(EventArgs& args);
	bool						IsKeyCodeValid(unsigned char keyCode);
	bool						HandleKeyPressed(unsigned char keyCode, std::string data );
	void						HandleEnterPressed();
	void						HandleBackspaceKeyPressed();
	void						HandleDeleteKeyPressed();
	void						HandleEscapeKeyPressed();
	void						HandleArrowKeyPressed(unsigned char keyCode);
	void						HandleHomeKeyPressed();
	void						HandleEndKeyPressed();
	void						HandleRightMousePressed(std::string data);
	void						HandleClearCommand();
	void						HandleHelpCommand(std::string commaSeparatedEventList);
	void						HandleEventLogging(EventArgs const& args);
	void						HandleEchoCommand( EventArgs const& args );


	// caret
	int			m_caretPosition = 0;		// Index of caret in our current input text
	bool		m_caretVisible = true;		// true if our caret is currently in the visible phase of blinking
	Stopwatch*	m_caretStopwatch = nullptr;	// Stopwatch for controlling caret visibility
	void		InitCaretStopwatch();
	void		ResetCaretBlinking();
	
	// history
	std::vector<std::string> m_commandHistory;	  // History of all commands executed
	int						 m_histotyIndex = -1; // Our current index in our history of commands as we are scrolling

	void SubscribeToEvents();
	void UnsubscribeFromEvents();

	AABB2 RenderBackground(DevConsoleRenderConfig const& renderConfig) const;
	void RenderText(AABB2 devConsoleBounds) const;
	void RenderInputLine(AABB2 devConsoleBounds) const;
	void RenderProcessedLines(AABB2 devConsoleBounds) const;
	void RenderCaret(AABB2 devConsoleBounds) const;

private:
	std::mutex m_addLineMutex;
	mutable std::mutex m_renderMutex;
};

