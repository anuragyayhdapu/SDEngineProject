#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EngineCommon.hpp"


NamedStrings g_gameConfigGlackboard;

// global event system singleton pointer, externed in EngineCommon.hpp, Constructed & Deleted by App
EventSystem* g_theEventSystem = nullptr; 

// global dev console pointer, externed in EngineCommon.hpp, Constructed & Deleted by App
DevConsole* g_theDevConsole = nullptr; 

// global job system pointer, externed in EngineCommon.hpp, Constructed & Deleted by App
JobSystem* g_theJobSystem = nullptr;