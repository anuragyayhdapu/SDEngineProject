#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include <algorithm>
#include <stdexcept>


constexpr float DEV_CONSOLE_RELATIVE_WIDTH_FRACTION = 0.975f;
constexpr float DEV_CONSOLE_RELATIVE_HEIGHT_FRACTION = 0.5f;
constexpr float DEV_CONSOLE_Y_POS_FRACTION = DEV_CONSOLE_RELATIVE_HEIGHT_FRACTION + 0.05f;
constexpr int VALID_KEYCODE_RANGE_START = 32;
constexpr int VALID_KEYCODE_RANGE_END = 126;
constexpr int MAX_NUM_LINES = 25;
constexpr float CARET_WIDTH_FRACTION = 0.2f;
constexpr float CARET_BLINK_RATE = 0.5f; // caret should blink every 0.5 seconds

const Rgba8 BACKGROUND_COLOR				   = Rgba8( 32, 33, 35, 200 );	  // Dark Gray
const Rgba8 DevConsole::ERROR_COLOR			   = Rgba8( 255, 102, 102, 255 ); // Bright Red
const Rgba8 DevConsole::WARNING_COLOR		   = Rgba8( 255, 255, 102, 255 ); // Bright Yellow
const Rgba8 DevConsole::INFO_MAJOR_COLOR	   = Rgba8( 102, 255, 102, 255 ); // Bright Green
const Rgba8 DevConsole::INFO_MINOR_COLOR	   = Rgba8( 102, 153, 255, 255 ); // Bright Blue
const Rgba8 DevConsole::COMMAND_ECHO_COLOR	   = Rgba8( 255, 204, 102, 255 ); // Bright Orange
const Rgba8 DevConsole::COMMAND_ECHO_COLOR_DIM = Rgba8( 206, 173, 115, 255 ); // Bright Orange
const Rgba8 DevConsole::INPUT_TEXT_COLOR	   = Rgba8( 255, 255, 255, 255 ); // White
const Rgba8 DevConsole::INPUT_CARET_COLOR	   = Rgba8( 255, 192, 203, 255 ); // Pink

const std::string CLEAR_COMMAND = "clear";


//------------------------------------------------------------------------------------------------
DevConsole::DevConsole(DevConsoleConfig const& config) :
	m_config(config)
{
}


//------------------------------------------------------------------------------------------------
DevConsole::~DevConsole()
{
}


//------------------------------------------------------------------------------------------------
void DevConsole::Startup()
{
	//InitWelcomeText();
	SubscribeToEvents();
	InitCaretStopwatch();
}


//------------------------------------------------------------------------------------------------
void DevConsole::InitWelcomeText()
{
	DevConsoleLine welcomeText;
	welcomeText.m_color = INFO_MAJOR_COLOR;
	welcomeText.m_text = "Type help for a list of commands";

	m_lines.push_back(welcomeText);
}


//------------------------------------------------------------------------------------------------
void DevConsole::SubscribeToEvents()
{
	g_theEventSystem->SubscribeToEvent("KeyPressed", EventHandler_KeyPressed);
	g_theEventSystem->SubscribeToEvent("CharInput", EventHandler_CharInput);
	g_theEventSystem->SubscribeToEvent(CLEAR_COMMAND, EventHandler_ClearTextCommand);
	g_theEventSystem->SubscribeToEvent(HELP_COMMAND, EventHandler_HelpCommand);
	//g_theEventSystem->SubscribeToEvent("eventLog", EventHandler_EventLogger);
}


//------------------------------------------------------------------------------------------------
void DevConsole::InitCaretStopwatch()
{
	m_caretStopwatch = new Stopwatch(CARET_BLINK_RATE);
	m_caretStopwatch->Start();
}


void DevConsole::LoadFonts()
{
	std::string fontPath = "Data/Fonts/" + m_config.m_fontName;
	m_bitmapFont = m_config.m_renderer->CreateOrGetBitmapFont(fontPath.c_str());
}

//------------------------------------------------------------------------------------------------
void DevConsole::Shutdown()
{
	UnsubscribeFromEvents();
}


//------------------------------------------------------------------------------------------------
void DevConsole::UnsubscribeFromEvents()
{
	g_theEventSystem->UnsubscribeFromEvent("KeyPressed", EventHandler_KeyPressed);
	g_theEventSystem->UnsubscribeFromEvent("CharInput", EventHandler_CharInput);
	g_theEventSystem->UnsubscribeFromEvent(CLEAR_COMMAND, EventHandler_ClearTextCommand);
	g_theEventSystem->UnsubscribeFromEvent(HELP_COMMAND, EventHandler_HelpCommand);
	//g_theEventSystem->UnsubscribeFromEvent("eventLog", EventHandler_EventLogger);
}


//------------------------------------------------------------------------------------------------
void DevConsole::BeginFrame()
{
	if (m_caretStopwatch->IsStopped())
	{
		m_caretStopwatch->Start();
	}

	// if stopwatch has elasped, then toggle the caret visibility
	bool hasDurationElapsed = m_caretStopwatch->DecrementDurationIfElapsed();
	if (hasDurationElapsed)
	{
		m_caretVisible = !m_caretVisible;
	}
}


//------------------------------------------------------------------------------------------------
void DevConsole::EndFrame()
{
}


//------------------------------------------------------------------------------------------------
void DevConsole::Execute(std::string const& consoleCommandText)
{
	// 1. separate string based on space
	Strings consoleCommandStrings = SplitCommandIgnoreInQuotes( consoleCommandText, ' ' );
	if (consoleCommandStrings.empty())
		return;

	std::string command = consoleCommandStrings[0];
	
	// parse args
	EventArgs args;
	for (int index = 1; index < consoleCommandStrings.size(); index++)
	{
		std::string commandArgument = consoleCommandStrings[index];
		Strings		splitArgs		= SplitCommandIgnoreInQuotes( commandArgument, '=' );
		
		if (!splitArgs.empty())
		{
			std::string key = splitArgs[0];
			std::string value = "";
			if (splitArgs.size() > 1)
			{
				value = splitArgs[1];
			}

			args.SetValue(key, value);
		}
	}


	if (command == ECHO_COMMAND)
	{
		HandleEchoCommand( args );
	}
	else
	{
		// fire
		FireEvent( command, args );

		// echo to dev console
		Echo( consoleCommandText, args );
	}


	// clear input line
	m_inputText.clear();
	m_caretPosition = 0;
}


//----------------------------------------------------------------------------------------------------------
Strings DevConsole::SplitCommandIgnoreInQuotes( std::string const& consoleCommandText, char delimiterToSplitOn )
{
	Strings splitStrings;

	if ( consoleCommandText.empty() )
		return splitStrings;

	int	 start	  = 0;
	int	 end	  = start;
	bool inquotes = false;

	for ( int index = 0; index < consoleCommandText.size(); index++ )
	{
		if (consoleCommandText[index] == '\"')
		{
			inquotes = !inquotes;
		}

		if ( consoleCommandText[ index ] == delimiterToSplitOn && !inquotes )
		{
			std::string splitString = consoleCommandText.substr( start, end - start );
			splitStrings.push_back( splitString );

			start = end + 1;
		}
		
		end++;
	}

	std::string splitString = consoleCommandText.substr( start, end - start );
	splitStrings.push_back( splitString );

	return splitStrings;
}


//------------------------------------------------------------------------------------------------
void DevConsole::Echo(std::string command, EventArgs& args)
{
	if (command == HELP_COMMAND) // help command is logged in the help event handler
		return;

	std::string logType = args.GetValue("logType", "");

	DevConsoleLine consoleLog;
	std::string comment = "";

	if (logType == "incorrect_args")
	{
		consoleLog.m_color = ERROR_COLOR;
		consoleLog.m_text = "Incorrect Args : " + command;
		comment = args.GetValue("comment", "");
	}
	else if (logType == "unknown_command")
	{
		consoleLog.m_color = ERROR_COLOR;
		consoleLog.m_text = "Unknown Command: " + command;
	}
	else if (logType == "no_subscribers")
	{
		consoleLog.m_color = WARNING_COLOR;
		consoleLog.m_text = "No Subscribers : " + command;
	}

	// 1. one line which echos the command in the echo color
	AddLine(COMMAND_ECHO_COLOR, command);

	// 2. Error or warning line, if command is not success
	if (!consoleLog.m_text.empty())
	{
		AddLine(consoleLog.m_color, consoleLog.m_text);
	}

	// 3. comment, eg. if args wrong, then how to write proper args
	if (!comment.empty())
	{
		AddLine(WARNING_COLOR, comment);
	}
}


//------------------------------------------------------------------------------------------------
void DevConsole::AddLine(Rgba8 const& color, std::string const& text)
{
	DevConsoleLine newLine;
	newLine.m_text = text;
	newLine.m_color = color;

	//-------------------------------------------------------------------------
	// lock
	m_addLineMutex.lock();

	m_lines.push_back(newLine);

	m_addLineMutex.unlock();
	// unlock
	//-------------------------------------------------------------------------
}


void DevConsole::ResetCaretBlinking()
{
	m_caretVisible = true;
	m_caretStopwatch->Restart();
}


//------------------------------------------------------------------------------------------------
void DevConsole::Render(DevConsoleRenderConfig const& renderConfig) const
{
	if (m_isOpen)
	{
		//-------------------------------------------------------------------------
		// lock
		m_renderMutex.lock();

		m_config.m_renderer->BeginCamera(*renderConfig.m_camera);

		AABB2 devConsoleBounds = RenderBackground(renderConfig);
		RenderText(devConsoleBounds);
		RenderCaret(devConsoleBounds);
		
		m_config.m_renderer->EndCamera(*renderConfig.m_camera);

		m_renderMutex.unlock();
		// unlock
		//-------------------------------------------------------------------------
	}
}


//------------------------------------------------------------------------------------------------
AABB2 DevConsole::RenderBackground(DevConsoleRenderConfig const& renderConfig) const
{
	AABB2 cameraBounds = AABB2
	(
		renderConfig.m_camera->GetOrthographicBottomLeft(),
		renderConfig.m_camera->GetOrthographicTopRight()
	);

	Vec2 devConsoleDimensions;
	devConsoleDimensions.x = cameraBounds.GetDimensions().x * DEV_CONSOLE_RELATIVE_WIDTH_FRACTION;
	devConsoleDimensions.y = cameraBounds.GetDimensions().y * DEV_CONSOLE_RELATIVE_HEIGHT_FRACTION;
	AABB2 devConsoleBounds;
	devConsoleBounds.SetDimensions(devConsoleDimensions);

	Vec2 devConsoleCenter = cameraBounds.GetCenter();
	devConsoleCenter.y = (devConsoleCenter.y * DEV_CONSOLE_Y_POS_FRACTION) + cameraBounds.m_mins.y;
	devConsoleBounds.SetCenter(devConsoleCenter);

	std::vector<Vertex_PCU> verts;
	AddVertsForAABB2(verts, devConsoleBounds, BACKGROUND_COLOR);
	m_config.m_renderer->BindTexture(nullptr);
	m_config.m_renderer->DrawVertexArray((int)verts.size(), verts.data());

	return devConsoleBounds;
}


//------------------------------------------------------------------------------------------------
void DevConsole::RenderText(AABB2 devConsoleBounds) const
{
	RenderProcessedLines(devConsoleBounds);
	RenderInputLine(devConsoleBounds);
}


//------------------------------------------------------------------------------------------------
void DevConsole::RenderInputLine(AABB2 devConsoleBounds) const
{
	// configure font params
	float boundsHeight = devConsoleBounds.GetDimensions().y;
	float cellHeight = boundsHeight / (float)MAX_NUM_LINES;
	Vec2 alignment(0.f, 0.f);
	TextBoxMode mode = TextBoxMode::SHRINK;
	float cellAspect = 1.f;
	Texture& fontTexture = m_bitmapFont->GetTexture();
	Rgba8 textColor = INPUT_TEXT_COLOR;

	// add verts
	std::vector<Vertex_PCU> verts;
	m_bitmapFont->AddVertsForTextInBox2D(verts, devConsoleBounds, cellHeight, m_inputText, textColor, cellAspect, alignment, mode);

	// render
	m_config.m_renderer->BindTexture(&fontTexture);
	m_config.m_renderer->DrawVertexArray((int)verts.size(), verts.data());
}


//------------------------------------------------------------------------------------------------
void DevConsole::RenderProcessedLines(AABB2 devConsoleBounds) const
{
	// configure font params
	float boundsHeight = devConsoleBounds.GetDimensions().y;
	float cellHeight = boundsHeight / (float)MAX_NUM_LINES;
	Vec2 alignment(0.f, 0.f);
	TextBoxMode mode = TextBoxMode::SHRINK;
	float cellAspect = 1.f;
	Texture& fontTexture = m_bitmapFont->GetTexture();

	std::string visibleText;
	std::vector<Vertex_PCU> verts;

	// 2. loop backward from m_lines and call add verts in box for every line, till max visible lines
	for (int lineNum = 0; lineNum < MAX_NUM_LINES - 1; lineNum++)
	{
		int lineIndex = (int)m_lines.size() - lineNum - 1;
		if (lineIndex < 0)
			break;

		AABB2 lineBounds = devConsoleBounds;
		lineBounds.m_mins.y += (cellHeight * (lineNum + 1));
		lineBounds.m_maxs.y = lineBounds.m_mins.y + cellHeight;

		std::string line = m_lines[lineIndex].m_text;
		Rgba8 lineColor = m_lines[lineIndex].m_color;

		// add verts
		m_bitmapFont->AddVertsForTextInBox2D(verts, lineBounds, cellHeight, line, lineColor, cellAspect, alignment, mode);
	}

	// render
	m_config.m_renderer->BindTexture(&fontTexture);
	m_config.m_renderer->DrawVertexArray((int)verts.size(), verts.data());
}


//------------------------------------------------------------------------------------------------
void DevConsole::RenderCaret(AABB2 devConsoleBounds) const
{
	//return;

	if (!m_caretVisible)
		return;

	float boundsHeight = devConsoleBounds.GetDimensions().y;
	float textCellHeight = boundsHeight / (float)MAX_NUM_LINES;
	float textCellWidth = m_bitmapFont->GetGlyphAspect('|') * textCellHeight;

	// caret bounds & position
	float caretXPos = (m_inputText.size() + m_caretPosition) * textCellWidth; 
	Vec2 caretPosition = Vec2(caretXPos, 0.f);
	float caretHeight = textCellHeight;
	float caretWidth = textCellWidth * CARET_WIDTH_FRACTION;
	Vec2 caretDimension(caretWidth, caretHeight);
	AABB2 caret;
	caret.m_mins = devConsoleBounds.m_mins;
	caret.m_maxs = caret.m_mins + caretDimension;
	caret.Translate(caretPosition);

	// add verts
	std::vector<Vertex_PCU> verts;
	AddVertsForAABB2(verts, caret, INPUT_CARET_COLOR);

	// render
	m_config.m_renderer->BindTexture(nullptr);
	m_config.m_renderer->DrawVertexArray((int)verts.size(), verts.data());
}


//------------------------------------------------------------------------------------------------
void DevConsole::ToggleOpen(bool newState)
{
	m_isOpen = newState;
}


//------------------------------------------------------------------------------------------------
bool DevConsole::HandleCharInput(EventArgs& args)
{
	if (!m_isOpen)
		return false;

	unsigned char keyCode = (unsigned char)args.GetValue("Keycode", KEYCODE_INVALID);

	if (!IsKeyCodeValid(keyCode))
	{
		return true; // event consumed
	}

	if (m_caretPosition >= 0) // caret at the end of line
	{
		m_inputText.push_back(keyCode);
	}
	else
	{
		int insertPos = m_caretPosition + (int)m_inputText.size();
		int numCharactersToInsert = 1;
		m_inputText.insert(insertPos, numCharactersToInsert, keyCode);
	}

	ResetCaretBlinking();

	return true;
}


std::vector<unsigned char> invalidKeyCodes = 
{
	'!', '#', '$', '%', '&', '(', ')',
	'*', '+', ':', ';', '<',
	'>', '?', '@', '[', ']', '^', '`', '{',
	'|', '}', '~'
};


//------------------------------------------------------------------------------------------------
bool DevConsole::IsKeyCodeValid(unsigned char keyCode)
{
	if (keyCode >= VALID_KEYCODE_RANGE_START && keyCode <= VALID_KEYCODE_RANGE_END)
	{
		auto iterator = std::find(invalidKeyCodes.begin(), invalidKeyCodes.end(), keyCode);
		if (iterator == invalidKeyCodes.end())
		{
			return true; // valid char
		}
	}

	return false; // invalid char
}


bool DevConsole::HandleKeyPressed(unsigned char keyCode, std::string data)
{
	if (keyCode == KEYCODE_TILDE)	// tilde ~
	{
		ToggleOpen(true);
		return true; // open dev console, and consume the tilde key event
	}
	else
	{
		if (m_isOpen)
		{
			if (keyCode == KEYCODE_ESCAPE)
			{
				HandleEscapeKeyPressed();
				ResetCaretBlinking();

				return true; // after closing the dev console, consume this keypress so it doesn't affect app
			}
			else if (keyCode == KEYCODE_ENTER)
			{
				HandleEnterPressed();
			}
			else if (keyCode == KEYCODE_BACKSPACE)
			{
				HandleBackspaceKeyPressed();
			}
			else if (keyCode == KEYCODE_DELETE)
			{
				HandleDeleteKeyPressed();
			}
			else if (keyCode == KEYCODE_LEFT || keyCode == KEYCODE_RIGHT)
			{
				HandleArrowKeyPressed(keyCode);
			}
			else if (keyCode == KEYCODE_HOME)
			{
				HandleHomeKeyPressed();
			}
			else if (keyCode == KEYCODE_END)
			{
				HandleEndKeyPressed();
			}
			else if (keyCode == KEYCODE_RIGHT_MOUSE)
			{
				HandleRightMousePressed(data);
			}

			ResetCaretBlinking();

			return true; // consume every keypress if dev console is open
		}
		else // !m_isOpen 
		{
			return false; // dev console is not open, do not consume any keypress event
		}
	}
}


//------------------------------------------------------------------------------------------------
void DevConsole::HandleEnterPressed()
{
	if (m_inputText.empty())
	{
		ToggleOpen(false);
		return;
	}

	// handle commands
	Execute(m_inputText);
}


//------------------------------------------------------------------------------------------------
void DevConsole::HandleBackspaceKeyPressed()
{
	if (m_inputText.empty())
		return;

	if (m_caretPosition + (int)m_inputText.size() <= 0)	// caret before the text
		return;

	if (m_caretPosition >= 0)	// caret at the text end
	{
		m_inputText.pop_back();
		return;
	}

	int deletePos = m_caretPosition + (int)m_inputText.size();
	if (deletePos <= 0)
		return; // cant delete by backspace if caret is before the text
	
	int erasePos = deletePos - 1;
	int numCharactersToDelete = 1;
	m_inputText.erase(erasePos, numCharactersToDelete);
}


//------------------------------------------------------------------------------------------------
void DevConsole::HandleDeleteKeyPressed()
{
	if (m_inputText.empty())
		return;
	
	if (m_caretPosition >= 0) // if caret is at the end of the line
		return;

	// delete one character from string to the right of caret
	int deletePos = m_caretPosition + (int)m_inputText.size();
	if (deletePos < 0)
		return;	// cant delete before the first character

	int numCharactersToDelete = 1;
	m_inputText.erase(deletePos, numCharactersToDelete);
	m_caretPosition += 1;
}


//------------------------------------------------------------------------------------------------
void DevConsole::HandleEscapeKeyPressed()
{
	if (m_inputText.empty())
	{
		ToggleOpen(false);
	}
	else
	{
		m_inputText.clear();

		// reset caret position
		m_caretPosition = 0;
	}
}


//------------------------------------------------------------------------------------------------
void DevConsole::HandleArrowKeyPressed(unsigned char keyCode)
{
	if (keyCode == KEYCODE_LEFT)
	{
		if (m_inputText.size() + m_caretPosition <= 0) // if caret is before the first character
		{
			return;	// can't go any left
		}
		else
		{
			m_caretPosition--;
		}
	}
	else // KEYCODE_RIGHT
	{
		if (m_caretPosition >= 0)	// if caret at the end of line
		{
			return; // can't go any right
		}
		else
		{
			m_caretPosition++;
		}
	}
}


//------------------------------------------------------------------------------------------------
void DevConsole::HandleHomeKeyPressed()
{
	if (m_inputText.empty())
		return;

	m_caretPosition = -1 * (int)m_inputText.size(); 
}


//------------------------------------------------------------------------------------------------
void DevConsole::HandleEndKeyPressed()
{
	if (m_inputText.empty())
		return;

	m_caretPosition = 0;
}

void DevConsole::HandleRightMousePressed(std::string data)
{
	if (data.empty())
		return;

	// insert the clipboard text into the input text
	std::string clipboardText = data;
	int insertPos			  = m_caretPosition + (int)m_inputText.size();
	m_inputText.insert(insertPos, clipboardText);

	ResetCaretBlinking();
}


//------------------------------------------------------------------------------------------------
void DevConsole::HandleClearCommand()
{


	m_lines.clear();
	//AddLine(COMMAND_ECHO_COLOR, "clear");

	m_inputText.clear();
	m_caretPosition = 0; 
}
 

//------------------------------------------------------------------------------------------------
void DevConsole::HandleHelpCommand(std::string commaSeparatedEventList)
{
	Strings events = SplitStringOnDelimiter(commaSeparatedEventList, ',');

	AddLine(COMMAND_ECHO_COLOR, HELP_COMMAND);
	AddLine(INFO_MAJOR_COLOR, "Registerd commands");
	for (auto event : events)
	{
		AddLine(INFO_MINOR_COLOR, event);
	}
}


//------------------------------------------------------------------------------------------------
void DevConsole::HandleEventLogging(EventArgs const& args)
{
	std::string command = args.GetValue("command", "");
	std::string logType = args.GetValue("logType", "");
	DevConsoleLine consoleLog;

	if (logType == "success")
	{
		consoleLog.m_color = COMMAND_ECHO_COLOR;
		consoleLog.m_text = command;
	}
	if (logType == "unknown_command")
	{
		consoleLog.m_color = ERROR_COLOR;
		consoleLog.m_text = "Unknown Command: " + command;
	}
	else if (logType == "no_subscribers")
	{
		consoleLog.m_color = WARNING_COLOR;
		consoleLog.m_text = "No Subscribers : " + command;
	}

	AddLine(consoleLog.m_color, consoleLog.m_text);
}


//----------------------------------------------------------------------------------------------------------
void DevConsole::HandleEchoCommand( EventArgs const& args )
{
	if ( args.HasKey( "Message" ) )
	{
		std::string message = args.GetValue( "Message", "UNKOWN MESSAGE" );

		// remove quotes
		if ( !message.empty() )
		{
			if ( message[ 0 ] == '\"' )
			{
				message = message.substr( 1, message.size() - 2 );
			}
			if (message[message.size() - 1] == '\"')
			{
				message = message.substr( 0, message.size() - 1 );
			}
		}


		AddLine( COMMAND_ECHO_COLOR_DIM, message );
	}
}



//------------------------------------------------------------------------------------------------
// Static Event Handlers

//------------------------------------------------------------------------------------------------
bool DevConsole::EventHandler_KeyPressed(EventArgs& args)
{
	unsigned char keycode = (unsigned char)args.GetValue("Keycode", KEYCODE_INVALID);
	std::string data = args.GetValue("data", "");
	bool wasHandled = false;
	if (g_theDevConsole)
	{
		wasHandled = g_theDevConsole->HandleKeyPressed(keycode, data);
	}
	return wasHandled;
}


//------------------------------------------------------------------------------------------------
bool DevConsole::EventHandler_CharInput(EventArgs& args)
{
	bool wasHandled = false;
	if (g_theDevConsole)
	{
		wasHandled = g_theDevConsole->HandleCharInput(args);
	}
	return wasHandled;
}


//------------------------------------------------------------------------------------------------
bool DevConsole::EventHandler_ClearTextCommand(EventArgs& args)
{
	UNUSED(args);

	bool wasHandled = false;
	if (g_theDevConsole)
	{
		g_theDevConsole->HandleClearCommand();
		wasHandled = true;
	}

	return wasHandled;
}


//------------------------------------------------------------------------------------------------
bool DevConsole::EventHandler_HelpCommand(EventArgs& args)
{
	bool wasConsumed = false;
	if (g_theDevConsole)
	{
		std::string eventList = args.GetValue(HELP_EVENT_ARGS, "");
		g_theDevConsole->HandleHelpCommand(eventList);
		wasConsumed = true;
	}

	return wasConsumed;
}

bool DevConsole::EventHandler_EventLogger(EventArgs& args)
{
	bool wasConsumed = false;
	if (g_theDevConsole)
	{
		g_theDevConsole->HandleEventLogging(args);
		wasConsumed = true;
	}
	return false;
}
