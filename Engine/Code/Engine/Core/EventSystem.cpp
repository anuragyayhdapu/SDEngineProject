#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/NamedStrings.hpp"



EventSystem::EventSystem(EventSystemConfig config)
{
	UNUSED(config);
}

EventSystem::~EventSystem()
{
}

void EventSystem::Startup()
{
}

void EventSystem::BeginFrame()
{
}

void EventSystem::EndFrame()
{
}

void EventSystem::Shutdown()
{
}

void EventSystem::SubscribeToEvent(std::string const& eventName, EventCallbackFuncPtr callbackFunc)
{
	//-------------------------------------------------------------------------
	// lock
	m_registerMutex.lock();

	EventSubscriberList& subscribersFromThisEvent = m_subscribersForEventNames[eventName];
	subscribersFromThisEvent.push_back(callbackFunc);

	m_registerMutex.unlock();
	// unlock
	//-------------------------------------------------------------------------
}

void EventSystem::UnsubscribeFromEvent(std::string const& eventName, EventCallbackFuncPtr callbackFunc)
{
	//-------------------------------------------------------------------------
	// lock
	m_registerMutex.lock();

	EventSubscriberList& subscribersFromThisEvent = m_subscribersForEventNames[eventName];

	for (int index = 0; index < (int)subscribersFromThisEvent.size(); index++)
	{
		if (subscribersFromThisEvent[index] == callbackFunc)
		{
			subscribersFromThisEvent[index] = nullptr;
		}
	}

	m_registerMutex.unlock();
	// unlock
	//-------------------------------------------------------------------------
}

void EventSystem::UnsubscribeFromAllEvents(EventCallbackFuncPtr callbackFunc)
{
	//-------------------------------------------------------------------------
	// lock
	m_registerMutex.lock();

	for (auto eventIter = m_subscribersForEventNames.begin(); eventIter != m_subscribersForEventNames.end(); eventIter++)
	{
		std::string const& eventName = eventIter->first;
		UnsubscribeFromEvent(eventName, callbackFunc);
	}

	m_registerMutex.unlock();
	// unlock
	//-------------------------------------------------------------------------
}

void EventSystem::FireEvent(std::string const& eventName, EventArgs& eventArgs)
{
	//-------------------------------------------------------------------------
	// lock
	m_fireMutex.lock();

	if (eventName == HELP_COMMAND)
		HandleHelpCommand(eventArgs);

	// if command not known:
	auto iter = m_subscribersForEventNames.find(eventName);
	if (iter == m_subscribersForEventNames.end())
	{
		eventArgs.SetValue("logType", "unknown_command");

		m_fireMutex.unlock();
		// unlock
		//-------------------------------------------------------------------------

		return;
	}

	EventSubscriberList& subscribersFromThisEvent = m_subscribersForEventNames.at(eventName);

	// if no active subscriber:
	if (subscribersFromThisEvent.empty())
	{
		eventArgs.SetValue("logType", "no_subscribers");

		m_fireMutex.unlock();
		// unlock
		//-------------------------------------------------------------------------

		return;
	}

	for (int index = 0; index < (int)subscribersFromThisEvent.size(); index++)
	{
		EventCallbackFuncPtr callbackFuncPtr = subscribersFromThisEvent[index];
		if (callbackFuncPtr != nullptr)
		{
			bool wasConsumed = callbackFuncPtr(eventArgs);
			if (wasConsumed)
			{
				if (eventArgs.HasKey("logType"))
				{
					std::string logType = eventArgs.GetValue("logType", "UNKNOWN LOG TYPE");
					if (logType != "success")
					{
						break; // Event was consumed, but with an error; tell no remaining subscribers about the event firing!
					}
				}

				eventArgs.SetValue("logType", "success"); // log
				break; // Event was consumed by this subscriber; tell no remaining subscribers about the event firing!
			}
		}
	}

	m_fireMutex.unlock();
	// unlock
	//-------------------------------------------------------------------------
}

void EventSystem::FireEvent(std::string const& eventName)
{
	//-------------------------------------------------------------------------
	// lock
	m_fireMutex.lock();

	EventArgs emptyArgs;
	FireEvent(eventName, emptyArgs);

	m_fireMutex.unlock();
	// unlock
	//-------------------------------------------------------------------------
}


void EventSystem::HandleHelpCommand(EventArgs& args)
{
	std::string commaSeparatedEventList;
	for (auto element : m_subscribersForEventNames)
	{
		std::string commandName = element.first;
		commaSeparatedEventList += (commandName + ",");
	}
	commaSeparatedEventList.pop_back(); // remove last unnecessary comma

	args.SetValue(HELP_EVENT_ARGS, commaSeparatedEventList);
}



//-------------------------------------------------------------------------------------------
void SubscribeToEvent(std::string const& eventName, EventCallbackFuncPtr callbackFunc)
{
	if (g_theEventSystem)
		g_theEventSystem->SubscribeToEvent(eventName, callbackFunc);
}

void UnsubscribeFromEvent(std::string const& eventName, EventCallbackFuncPtr callbackFunc)
{
	if (g_theEventSystem)
		g_theEventSystem->UnsubscribeFromEvent(eventName, callbackFunc);
}

void UnsubscribeFromAllEvents(EventCallbackFuncPtr callbackFunc)
{
	if (g_theEventSystem)
		g_theEventSystem->UnsubscribeFromAllEvents(callbackFunc);
}

void FireEvent(std::string const& eventName, EventArgs& eventArgs)
{
	if (g_theEventSystem)
		g_theEventSystem->FireEvent(eventName, eventArgs);
}

void FireEvent(std::string const& eventName)
{
	if (g_theEventSystem)
		g_theEventSystem->FireEvent(eventName);
}
