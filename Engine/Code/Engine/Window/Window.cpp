#include "Engine/Input/InputSystem.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"

#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include <commdlg.h>
#include <shobjidl.h> 

// Global and class static variables
Window* Window::s_mainWindow = nullptr;

//-----------------------------------------------------------------------------------------------
// Handles Windows (Win32) messages/events; i.e. the OS is trying to tell us something happened.
// This function is called by Windows whenever we ask it for notifications
//
LRESULT CALLBACK WindowsMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
	Window* windowContext = Window::GetWindowContext();
	GUARANTEE_OR_DIE(windowContext != nullptr, "WindowContext was null!");

	switch (wmMessageCode)
	{
		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
	case WM_CLOSE:
	{
		FireEvent(QUIT_COMMAND);
		return 0; // "Consumes" this message (tells Windows "okay, we handled it")
	}

	// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
	case WM_KEYDOWN:
	{
		unsigned char keyCode = (unsigned char)wParam;
		EventArgs args;
		args.SetValue("Keycode", std::to_string(keyCode));
		FireEvent("KeyPressed", args);

		return 0; // "Consumes" this message (tells Windows "okay, we handled it")

		break;
	}

	// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
	case WM_KEYUP:
	{
		unsigned char keyCode = (unsigned char)wParam;
		EventArgs args;
		args.SetValue("Keycode", std::to_string(keyCode));
		FireEvent("KeyReleased", args);
		
		return 0; // "Consumes" this message (tells Windows "okay, we handled it")

		break;
	}

	case WM_CHAR:
	{
		unsigned char keyCode = (unsigned char)wParam;
		EventArgs args;
		args.SetValue("Keycode", std::to_string(keyCode));
		FireEvent("CharInput", args);

		return 0; // "Consumes" this message (tells Windows "okay, we handled it")
	}

	// Treat this special mouse-button windows message as if it were an ordinary key down for us:
	case WM_LBUTTONDOWN:
	{
		unsigned char keyCode = KEYCODE_LEFT_MOUSE;
		EventArgs args;
		args.SetValue("Keycode", std::to_string(keyCode));
		FireEvent("KeyPressed", args);

		return 0; // "Consumes" this message (tells Windows "okay, we handled it")

		break;
	}
	// Treat this special mouse-button windows message as if it were an ordinary key down for us:
	case WM_RBUTTONDOWN:
	{
		unsigned char keyCode = KEYCODE_RIGHT_MOUSE;
		EventArgs args;
		args.SetValue("Keycode", std::to_string(keyCode));

		//-------------------------------------------------------------------------
		// TODO: remove hack later, always copying clipboard on right click
		// Try opening the clipboard
		if (OpenClipboard(windowHandle))
		{
			// Get handle of clipboard object for ANSI text
			HANDLE hData = GetClipboardData(CF_TEXT);
			if (hData)
			{
				// Lock the handle to get the actual text pointer
				char* pszText = static_cast<char*>(GlobalLock(hData));
				if (pszText)
				{
					std::string clipboardText(pszText);
					args.SetValue("data", clipboardText);
				}

				// Release the lock
				GlobalUnlock(hData);
			}

			// Release the clipboard
			CloseClipboard();
		}
		//-------------------------------------------------------------------------


		FireEvent("KeyPressed", args);

		return 0; // "Consumes" this message (tells Windows "okay, we handled it")

		break;
	}
	// Treat this special mouse-button windows message as if it were an ordinary key up for us:
	case WM_LBUTTONUP:
	{
		unsigned char keyCode = KEYCODE_LEFT_MOUSE;
		EventArgs args;
		args.SetValue("Keycode", std::to_string(keyCode));
		FireEvent("KeyReleased", args);

		return 0; // "Consumes" this message (tells Windows "okay, we handled it")

		break;
	}
	// Treat this special mouse-button windows message as if it were an ordinary key up for us:
	case WM_RBUTTONUP:
	{
		unsigned char keyCode = KEYCODE_RIGHT_MOUSE;
		EventArgs args;
		args.SetValue("Keycode", std::to_string(keyCode));
		FireEvent("KeyReleased", args);

		return 0; // "Consumes" this message (tells Windows "okay, we handled it")

		break;
	}
	}

	// Send back to Windows any unhandled/unconsumed messages we want other apps to see (e.g. play/pause in music apps, etc.)
	return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
}



//-----------------------------------------------------------------------------------------------
Window::Window(WindowConfig const& config)
	: m_config(config)
{
	s_mainWindow = this;
}


//-----------------------------------------------------------------------------------------------
Window::~Window()
{
}


//-----------------------------------------------------------------------------------------------
void Window::Startup()
{
	CreateOSWindow();
}


//-----------------------------------------------------------------------------------------------
void Window::BeginFrame()
{
	// Process OS messages (keyboard/mouse button clicked, application lost/gained focus, etc.)
	RunMessagePump();

	//Sleep(1);

	//::SetCursor(NULL);
}


//-----------------------------------------------------------------------------------------------
void Window::CreateOSWindow()
{
	// Define a window style/class
	WNDCLASSEX windowClassDescription;
	memset(&windowClassDescription, 0, sizeof(windowClassDescription));
	windowClassDescription.cbSize = sizeof(windowClassDescription);
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(WindowsMessageHandlingProcedure); // Register our Windows message-handling function
	windowClassDescription.hInstance = GetModuleHandle(NULL);
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT("Simple Window Class");
	RegisterClassEx(&windowClassDescription);

	DWORD windowStyleFlags;
	if (m_config.m_isFullScreen)
	{
		windowStyleFlags = WS_POPUP;
	}
	else
	{
		windowStyleFlags = WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_OVERLAPPED | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	}

	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect(desktopWindowHandle, &desktopRect);
	float desktopWidth = (float)(desktopRect.right - desktopRect.left);
	float desktopHeight = (float)(desktopRect.bottom - desktopRect.top);
	float desktopAspect = desktopWidth / desktopHeight;

	
	
	float clientWidth  = 0.f;
	float clientHeight = 0.f;
	if ( m_config.m_isFullScreen )
	{
		clientWidth	 = desktopWidth;
		clientHeight = desktopHeight;
	}
	else if (m_config.m_windowSize != Vec2(-1, -1) )
	{
		clientWidth = m_config.m_windowSize.x;
		clientHeight = m_config.m_windowSize.y;

		m_config.m_clientAspect = clientWidth / clientHeight;
	}
	else
	{
		// Calculate maximum client size (as some % of desktop size)

		constexpr float maxClientFractionOfDesktop = 0.90f;
		clientWidth								   = desktopWidth * maxClientFractionOfDesktop;
		clientHeight							   = desktopHeight * maxClientFractionOfDesktop;

		if ( m_config.m_clientAspect > desktopAspect )
		{
			// Client window has a wider aspect than desktop; shrink client height to match its width
			clientHeight = clientWidth / m_config.m_clientAspect;
		}
		else
		{
			// Client window has a taller aspect than desktop; shrink client width to match its height
			clientWidth = clientHeight * m_config.m_clientAspect;
		}
	}



	m_clientDimensions = IntVec2((int)clientWidth, (int)clientHeight);

	float clientMarginX = -1.f;
	float clientMarginY = -1.f;
	if ( m_config.m_isFullScreen )
	{
		clientMarginX = 0.f;
		clientMarginY = 0.f;
	}
	else if ( m_config.m_windowPosition != Vec2( -1.f, -1.f ) )
	{
		// Calculate client rect bounds by centering the client area
		clientMarginX = m_config.m_windowPosition.x;
		clientMarginY = m_config.m_windowPosition.y;
	}
	else
	{
		clientMarginX = 0.5f * ( desktopWidth - clientWidth );
		clientMarginY = 0.5f * ( desktopHeight - clientHeight );
	}

	
	/*float clientMarginX = m_config.m_isFullScreen ? 0.f : 0.5f * (desktopWidth - clientWidth);
	float clientMarginY = m_config.m_isFullScreen ? 0.f : 0.5f * (desktopHeight - clientHeight);*/
	RECT clientRect;
	clientRect.left = (int)clientMarginX;
	clientRect.right = clientRect.left + (int)clientWidth;
	clientRect.top = (int)clientMarginY;
	clientRect.bottom = clientRect.top + (int)clientHeight;

	// Calculate the outer dimensions of the physical window, including frame et. al.
	RECT windowRect = clientRect;
	AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags);

	WCHAR windowTitle[1024];
	MultiByteToWideChar(GetACP(), 0, m_config.m_windowTitle.c_str(), -1, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));
	HWND hwnd = CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		NULL);

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	m_hwnd = hwnd;

	HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
	SetCursor(cursor);

	//::SetCursor(NULL);

}


//----------------------------------------------------------------------------------------------------------
void Window::SetWindowName( std::string const& newName )
{
	WCHAR windowTitle[1024];
	MultiByteToWideChar(GetACP(), 0, newName.c_str(), -1, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));
	SetWindowText((HWND)m_hwnd, windowTitle);
}



//-----------------------------------------------------------------------------------------------
// Processes all Windows messages (WM_xxx) for this app that have queued up since last frame.
// For each message in the queue, our WindowsMessageHandlingProcedure (or "WinProc") function
//	is called, telling us what happened (key up/down, minimized/restored, gained/lost focus, etc.)
void Window::RunMessagePump()
{
	MSG queuedMessage;
	for (;; )
	{
		const BOOL wasMessagePresent = PeekMessage(&queuedMessage, NULL, 0, 0, PM_REMOVE);
		if (!wasMessagePresent)
		{
			break;
		}

		TranslateMessage(&queuedMessage);
		DispatchMessage(&queuedMessage); // This tells Windows to call our "WindowsMessageHandlingProcedure" (a.k.a. "WinProc") function
	}
}




void Window::Shutdown()
{
}



void Window::EndFrame()
{
}


void* Window::GetHWND() const
{
	return m_hwnd;
}

IntVec2 Window::GetClientDimensions() const
{
	return m_clientDimensions;
}

IntVec2 Window::GetCursorPositionRelativeToClient() const
{
	POINT cursorCoords;
	::GetCursorPos(&cursorCoords); // in screen coordinates, (0,0) top-left
	::ScreenToClient((HWND)m_hwnd, &cursorCoords); // relative to windows interior

	IntVec2 cursorRelativePos(cursorCoords.x, cursorCoords.y);
	return cursorRelativePos;
}

void Window::SetCursorPositionRelativeToClient(IntVec2 const& newCursorRelativePos) const
{
	POINT newCursorCoords;
	newCursorCoords.x = (long)newCursorRelativePos.x;
	newCursorCoords.y = (long)newCursorRelativePos.y;

	::ClientToScreen((HWND)m_hwnd, &newCursorCoords);
	::SetCursorPos(newCursorCoords.x, newCursorCoords.y);
}


bool Window::DoesCurrentWindowHaveFocus() const
{
	HWND handleToActiveWindow = GetActiveWindow();
	bool hasFocus = (handleToActiveWindow == m_hwnd);

	return hasFocus;
}

std::string Window::GetCurrentWorkingDirectory() const
{
	std::string directory = "";

	const DWORD buffer_size = MAX_PATH;
	char buffer[buffer_size];

	if (GetCurrentDirectoryA(buffer_size, buffer))
	{
		directory = std::string(buffer);
	}
	else
	{
		std::string errorMessage = GetLastErrorMessage();
		ERROR_RECOVERABLE(Stringf("Unable to Get current directory. Error: %s", errorMessage.c_str()).c_str());
	}

	return directory;
}

void Window::SetCurrentWorkingDirectory(std::string newDirectory) const
{
	if (SetCurrentDirectoryA(newDirectory.c_str()) == false)
	{
		std::string errorMessage = GetLastErrorMessage();
		ERROR_RECOVERABLE(Stringf("Unable to Set current directory. Error: %s", errorMessage.c_str()).c_str());
	}
}


std::string Window::GetLastErrorMessage() const
{
	std::string errorMessage = "";

	DWORD errorCode = GetLastError();
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		errorCode,
		0, // Default language
		(LPWSTR)&errorMessage,
		0,
		NULL
	);

	return errorMessage;
}


std::string Window::OpenFileDialogAndGetFilePath() const
{
	// save current directory
	std::string currentDirectory = GetCurrentWorkingDirectory();
	std::string filePath = "";

	OPENFILENAMEA ofn;
	const DWORD filePath_Size = MAX_PATH;
	char filePathChar[filePath_Size] = {};

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = (HWND)m_hwnd;
	ofn.lpstrFile = filePathChar;
	ofn.nMaxFile = filePath_Size;
	ofn.lpstrFilter = "XML Files\0*.XML\0";  // Only XML files
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = currentDirectory.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box.
	if (GetOpenFileNameA(&ofn) == TRUE)
	{
		filePath = std::string(ofn.lpstrFile);
	}

	SetCurrentWorkingDirectory(currentDirectory);

	return filePath;
}
