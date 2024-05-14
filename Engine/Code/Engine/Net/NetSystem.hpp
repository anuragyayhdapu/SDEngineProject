#pragma once

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/EngineCommon.hpp"


#include <string>
#include <vector>



//----------------------------------------------------------------------------------------------------------
struct NetSystemConfig
{
	std::string m_modeString		= "";
	std::string m_hostAddressString = "";
	int			m_sendBufferSize	= 2048;
	int			m_recvBufferSize	= 2048;
};


//----------------------------------------------------------------------------------------------------------
class NetSystem
{
public:
	enum class Mode
	{
		NONE = 0,
		CLIENT,
		SERVER,
	};

	enum class ClientState
	{
		INVALID,
		READY_TO_CONNECT,
		CONNECTING,
		CONNECTED,
	};

	enum class ServerState
	{
		INVALID,
		LISTENING,
		CONNECTED,
	};

	NetSystem( const NetSystemConfig& config );
	~NetSystem();

	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	

	NetSystemConfig m_config;
	Mode			m_mode = Mode::NONE;

	ClientState m_clientState = ClientState::INVALID;
	ServerState m_serverState = ServerState::INVALID;

	uintptr_t m_clientSocket = ~0ull;
	uintptr_t m_listenSocket = ~0ull;

	unsigned long  m_hostAddress = 0;
	unsigned short m_hostPort	 = 0;

	char* m_sendBuffer = nullptr;
	char* m_recvBuffer = nullptr;

	std::vector<std::string> m_sendQueue;
	std::string				 m_recvRemaining;


protected:
	void StartupClient();
	void BeginFrameClient();

	void StartupServer();
	void BeginFrameServer();

	void Log( const char* message, Rgba8 echoColor );
	void LogLastErrorMessage( int lastErrorCode );

	void SubscribeToEvents();
	void UnsubscribeFromEvents();

	static bool Event_RemoteCommand( EventArgs& args );
	static bool Event_BurstTest( EventArgs& args );

	void SendMessageData(const std::string& message);
};