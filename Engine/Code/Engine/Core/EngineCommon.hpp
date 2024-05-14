#pragma once

#include "Engine/Math/Vec3.hpp"

#include <string>
// #include "Engine/Animation/FbxImporter.hpp"

#define UNUSED( x ) ( void ) ( x );
#pragma warning( disable : 26812 ) // prefer enum class to enum (yes, but not always)


//----------------------------------------------------------------------------------------------------------
#define M_PI 3.14159265358979323846f


//----------------------------------------------------------------------------------------------------------
enum XboxButtonID
{
	XBOX_BUTTON_A,
	XBOX_BUTTON_B,
	XBOX_BUTTON_X,
	XBOX_BUTTON_Y,
	XBOX_BUTTON_DPAD_UP,
	XBOX_BUTTON_DPAD_DOWN,
	XBOX_BUTTON_DPAD_LEFT,
	XBOX_BUTTON_DPAD_RIGHT,
	XBOX_BUTTON_START,
	XBOX_BUTTON_BACK,
	XBOX_BUTTON_LEFT_JOYSTICK,
	XBOX_BUTTON_RIGHT_JOYSTICK,
	XBOX_BUTTON_LEFT_BUMPER,
	XBOX_BUTTON_RIGHT_BUMPER,

	NUM
};


//----------------------------------------------------------------------------------------------------------
constexpr float RAW_MIN_JOYSTICK					 = -32768.f;
constexpr float RAW_MAX_JOYSTICK					 = 32767.f;
constexpr float NORMALIZED_MIN_JOYSTICK				 = -1.f;
constexpr float NORMALIZED_MAX_JOYSTICK				 = 1.f;

constexpr float NORMALIZED_INNER_DEAD_ZONE_THRESHOLD = 0.3f;
constexpr float NORMALIZED_OUTER_DEAD_ZONE_THRESHOLD = 0.95f;

constexpr float MIN_RAW_TRIGGER_VALUE				 = 0.f;
constexpr float MAX_RAW_TRIGGER_VALUE				 = 255.f;
constexpr float MIN_TRIGGER_OUTPUT					 = 0.f;
constexpr float MAX_TRIGGER_OUTPUT					 = 1.f;


//----------------------------------------------------------------------------------------------------------
class NamedStrings;
extern NamedStrings g_gameConfigGlackboard; // declared in EngineCommon.hpp, defined in EngineCommon.cpp

class EventSystem;
extern EventSystem*	 g_theEventSystem; // global event system singleton pointer, Constructed & Deleted by App
typedef NamedStrings EventArgs;

class DevConsole;
extern DevConsole* g_theDevConsole; // global dev console pointer, defined in EngineCommon.cpp, Constructed & Deleted by App

class JobSystem;
extern JobSystem* g_theJobSystem; // global job system pointer, defined in EngineCommon.cpp, Constructed & Deleted by App

class NetSystem;
extern NetSystem* g_theNetSystem; // global net system pointer, defined in EngineCommon.cpp, Constructed & Deleted by App


//----------------------------------------------------------------------------------------------------------
#ifdef HELP_COMMAND
#undef HELP_COMMAND
#endif

const std::string QUIT_COMMAND	  = "quit";
const std::string HELP_COMMAND	  = "help";
const std::string HELP_EVENT_ARGS = "events";
const std::string ECHO_COMMAND	  = "Echo";


//-------------------------------------------------------------------------
// Multi-threading
constexpr int NUM_DISK_IO_THREADS = 1;


//----------------------------------------------------------------------------------------------------------
// Renderer
struct LightConstants
{
	Vec3  m_sunDirection;
	float m_sunIntensity;
	float m_ambientIntensity;
	Vec3  m_worldEyePosition;
	int	  m_normalMode;
	int	  m_specularMode;
	float m_specularIntensity;
	float m_specularPower;
};

enum class VertexType
{
	Vertex_PCU,		 // Position, Color, UV coords
	Vertex_PCUTBN,	 // PCU + Tangent, BiNormal, Normal
	Vertex_Skeletal, // PCUTBN + skinning data
	UNKOWN
};