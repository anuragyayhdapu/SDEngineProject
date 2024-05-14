#pragma once

#include "Engine/Core/EngineCommon.hpp"

#include <vector>
#include <map>
#include <string>
#include <mutex>


typedef bool (*EventCallbackFuncPtr) (EventArgs & eventArgs);

typedef std::vector<EventCallbackFuncPtr> EventSubscriberList;


struct EventSystemConfig
{
	// TOOD
};

class EventSystem
{
public:
	EventSystem(EventSystemConfig config);
	~EventSystem();

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void SubscribeToEvent(std::string const& eventName,  EventCallbackFuncPtr  callbackFunc);
	void UnsubscribeFromEvent(std::string const& eventName, EventCallbackFuncPtr  callbackFunc);
	void UnsubscribeFromAllEvents(EventCallbackFuncPtr  callbackFunc);
	void FireEvent(std::string const& eventName, EventArgs& eventArgs);
	void FireEvent(std::string const& eventName);

protected:
	void HandleHelpCommand(EventArgs& args);

private:
	std::map<std::string, EventSubscriberList> m_subscribersForEventNames;

	std::mutex m_registerMutex;
	std::recursive_mutex m_fireMutex;
};



//-------------------------------------------------------------------------
// Event system wrapper methods
void SubscribeToEvent(std::string const& eventName, EventCallbackFuncPtr  callbackFunc);
void UnsubscribeFromEvent(std::string const& eventName, EventCallbackFuncPtr  callbackFunc);
void UnsubscribeFromAllEvents(EventCallbackFuncPtr  callbackFunc);
void FireEvent(std::string const& eventName, EventArgs& eventArgs);
void FireEvent(std::string const& eventName);