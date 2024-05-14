#pragma once

#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"

#include <string>


struct WindowConfig
{
	std::string m_windowTitle	 = "UNKOWN WINDOW";
	float		m_clientAspect	 = 1.f;
	bool		m_isFullScreen	 = false;
	Vec2		m_windowSize	 = Vec2( -1, -1 );
	Vec2		m_windowPosition = Vec2( -1, -1 );
};

class Window
{
public:
	Window(WindowConfig const& config);
	~Window();

	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	static Window* GetWindowContext() { return s_mainWindow; }
	WindowConfig const& GetConfig() const { return m_config; }

	void SetWindowName(std::string const& newName);

protected:
	void CreateOSWindow();
	void RunMessagePump();


protected:
	WindowConfig m_config;
	static Window* s_mainWindow;

	void* m_hwnd = nullptr;
	IntVec2 m_clientDimensions;

public:
	void* GetHWND() const;
	IntVec2 GetClientDimensions() const;
	IntVec2 GetCursorPositionRelativeToClient() const;
	void SetCursorPositionRelativeToClient(IntVec2 const& newCursorRelativePos) const;
	bool DoesCurrentWindowHaveFocus() const;

public:
	std::string GetCurrentWorkingDirectory() const;
	void SetCurrentWorkingDirectory(std::string newDirectory) const;

	std::string OpenFileDialogAndGetFilePath() const;

protected:
	std::string GetLastErrorMessage() const;
};

