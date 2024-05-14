#include "Game/EngineBuildPreferences.hpp"

#include "Engine/Net/NetSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


//----------------------------------------------------------------------------------------------------------
#if !defined( ENGINE_DISABLE_NET )


//----------------------------------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment( lib, "Ws2_32.lib" )



//----------------------------------------------------------------------------------------------------------
NetSystem::NetSystem( const NetSystemConfig& config )
	: m_config( config )
{
	// set mode
	if ( m_config.m_modeString == "Client" || m_config.m_modeString == "client" )
	{
		m_mode = Mode::CLIENT;
	}
	else if ( m_config.m_modeString == "Server" || m_config.m_modeString == "server" )
	{
		m_mode = Mode::SERVER;
	}
}


//----------------------------------------------------------------------------------------------------------
NetSystem::~NetSystem()
{
}


//----------------------------------------------------------------------------------------------------------
void NetSystem::Startup()
{
	// create buffers
	m_recvBuffer = new char[ m_config.m_recvBufferSize ];
	m_sendBuffer = new char[ m_config.m_sendBufferSize ];

	// subscribe to events
	SubscribeToEvents();

	// startup
	if ( m_mode == Mode::CLIENT )
	{
		StartupClient();
	}
	else if ( m_mode == Mode::SERVER )
	{
		StartupServer();
	}
}


//----------------------------------------------------------------------------------------------------------
void NetSystem::BeginFrame()
{
	if ( m_mode == Mode::CLIENT )
	{
		BeginFrameClient();
	}
	else if ( m_mode == Mode::SERVER )
	{
		BeginFrameServer();
	}
}


//----------------------------------------------------------------------------------------------------------
void NetSystem::StartupClient()
{
	// split host address string to get host IP and host port
	Strings		splitString = SplitStringOnDelimiter( m_config.m_hostAddressString, ':' );
	std::string hostIpStr	= splitString[ 0 ];
	std::string hostPortStr = splitString[ 1 ];

	// startup windows sockets
	WSADATA data;
	int		startupResult = WSAStartup( MAKEWORD( 2, 2 ), &data );
	if ( startupResult != 0 )
	{
		if ( startupResult == WSASYSNOTREADY )
		{
			DebuggerPrintf( "The underlying network subsystem is not ready for network communication." );
			g_theDevConsole->AddLine( DevConsole::ERROR_COLOR, "The underlying network subsystem is not ready for network communication." );
		}
		else if ( startupResult == WSAVERNOTSUPPORTED )
		{
			DebuggerPrintf( "The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation." );
			g_theDevConsole->AddLine( DevConsole::ERROR_COLOR, "The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation." );
		}
		else if ( startupResult == WSAEINPROGRESS )
		{
			DebuggerPrintf( "A blocking Windows Sockets 1.1 operation is in progress." );
			g_theDevConsole->AddLine( DevConsole::ERROR_COLOR, "A blocking Windows Sockets 1.1 operation is in progress." );
		}
		else if ( startupResult == WSAEPROCLIM )
		{
			DebuggerPrintf( "A limit on the number of tasks supported by the Windows Sockets implementation has been reached." );
			g_theDevConsole->AddLine( DevConsole::ERROR_COLOR, "A limit on the number of tasks supported by the Windows Sockets implementation has been reached." );
		}
		else if ( startupResult == WSAEFAULT )
		{
			DebuggerPrintf( "The lpWSAData parameter is not a valid pointer." );
			g_theDevConsole->AddLine( DevConsole::ERROR_COLOR, "The lpWSAData parameter is not a valid pointer." );
		}
	}
	else // startup result == 0, success
	{
		// create client socket
		m_clientSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

		// set mode = non-blocking
		unsigned long mode		   = 1; // non-blocking mode socket
		int			  ioModeResult = ioctlsocket( m_clientSocket, FIONBIO, &mode );
		if ( ioModeResult == SOCKET_ERROR )
		{
			int lastError = WSAGetLastError();
			if ( lastError == WSANOTINITIALISED )
			{
				DebuggerPrintf( "A successful WSAStartup call must occur before using this function." );
				g_theDevConsole->AddLine( DevConsole::ERROR_COLOR, "A successful WSAStartup call must occur before using this function." );
			}
			else if ( lastError == WSAENETDOWN )
			{
				DebuggerPrintf( "The network subsystem has failed." );
				g_theDevConsole->AddLine( DevConsole::ERROR_COLOR, "The network subsystem has failed." );
			}
			else if ( lastError == WSAEINPROGRESS )
			{
				DebuggerPrintf( "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function." );
				g_theDevConsole->AddLine( DevConsole::ERROR_COLOR, "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function." );
			}
			else if ( lastError == WSAENOTSOCK )
			{
				DebuggerPrintf( "The descriptor s is not a socket." );
				g_theDevConsole->AddLine( DevConsole::ERROR_COLOR, "The descriptor s is not a socket." );
			}
			else if ( lastError == WSAEFAULT )
			{
				DebuggerPrintf( "The argp parameter is not a valid part of the user address space." );
				g_theDevConsole->AddLine( DevConsole::ERROR_COLOR, "The argp parameter is not a valid part of the user address space." );
			}
		}
		else // ioModeResult == 0, success
		{
			// get host address from string
			IN_ADDR addr;
			int		inetResult = inet_pton( AF_INET, hostIpStr.c_str(), &addr );
			if ( inetResult != 1 )
			{
				if ( inetResult == 0 )
				{
					DebuggerPrintf( "the pAddrBuf parameter points to a string that is not a valid IPv4 dotted-decimal string or a valid IPv6 address string" );
					g_theDevConsole->AddLine( DevConsole::ERROR_COLOR, "the pAddrBuf parameter points to a string that is not a valid IPv4 dotted-decimal string or a valid IPv6 address string" );
				}
				else if ( inetResult == -1 )
				{
					int lastError = WSAGetLastError();
					if ( lastError == WSAEAFNOSUPPORT )
					{
						DebuggerPrintf( "The address family specified in the Family parameter is not supported. This error is returned if the Family parameter specified was not AF_INET or AF_INET6" );
						g_theDevConsole->AddLine( DevConsole::ERROR_COLOR, "The address family specified in the Family parameter is not supported. This error is returned if the Family parameter specified was not AF_INET or AF_INET6" );
					}
					else if ( lastError == WSAEFAULT )
					{
						DebuggerPrintf( "The pszAddrString or pAddrBuf parameters are NULL or are not part of the user address space." );
						g_theDevConsole->AddLine( DevConsole::ERROR_COLOR, "The pszAddrString or pAddrBuf parameters are NULL or are not part of the user address space." );
					}
				}
			}
			else // inetResult == 1, success
			{
				m_hostAddress = ntohl( addr.S_un.S_addr );

				// get host port from string
				m_hostPort = ( unsigned short ) ( atoi( hostPortStr.c_str() ) );

				m_clientState = ClientState::READY_TO_CONNECT;
			}
		}
	}
}


//----------------------------------------------------------------------------------------------------------
void NetSystem::BeginFrameClient()
{
	if ( m_clientState == ClientState::READY_TO_CONNECT )
	{
		// attempt to connect if haven't already
		sockaddr_in addr;
		addr.sin_family			  = AF_INET;
		addr.sin_addr.S_un.S_addr = htonl( m_hostAddress );
		addr.sin_port			  = htons( m_hostPort );
		int connectResult		  = connect( m_clientSocket, ( sockaddr* ) ( &addr ), ( int ) sizeof( addr ) );
		
		DebuggerPrintf( "%d %d\n", connectResult, WSAGetLastError() );

		if ( connectResult == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK )
		{
			m_clientState = ClientState::CONNECTING;
		}
	}
	else if ( m_clientState == ClientState::CONNECTING )
	{
		DebuggerPrintf( "%d\n", WSAGetLastError() );

		/*if ( WSAGetLastError() == WSAEWOULDBLOCK )
		{*/
			fd_set writeSet;
			FD_ZERO( &writeSet );
			FD_SET( m_clientSocket, &writeSet );

			struct timeval timeout;
			timeout.tv_sec	= 0; // Set timeout to 0 seconds
			timeout.tv_usec = 0;

			// check if our connection attempt failed
			int connectFailedTestResult = select( 0, NULL, &writeSet, NULL, &timeout );
			if ( connectFailedTestResult > 0 )
			{
				// Connection attempt completed (either successfully or with an error)
				int optVal;
				int optLen = sizeof( int );
				if ( getsockopt( m_clientSocket, SOL_SOCKET, SO_ERROR, ( char* ) &optVal, &optLen ) != SOCKET_ERROR )
				{
					if ( optVal == 0 )
					{
						DebuggerPrintf( "Connection established.\n" );
						g_theDevConsole->AddLine( DevConsole::INFO_MINOR_COLOR, "Connection established." );

						m_clientState = ClientState::CONNECTED;
					}
					else
					{
						DebuggerPrintf( "Connection failed with error: %d\n", optVal );
						g_theDevConsole->AddLine( DevConsole::ERROR_COLOR, Stringf( "Connection failed with error: %d", optVal ) );

						// m_clientState = ClientState::READY_TO_CONNECT;
					}
				}
				else
				{
					DebuggerPrintf( "getsockopt failed with error: %d\n", WSAGetLastError() );
					g_theDevConsole->AddLine( DevConsole::ERROR_COLOR, Stringf( "getsockopt failed with error: %d", WSAGetLastError() ) );

					// m_clientState = ClientState::READY_TO_CONNECT;
				}
			}
			else if ( connectFailedTestResult == 0 )
			{
				DebuggerPrintf( "Connection attempt timed out.\n" );
				g_theDevConsole->AddLine( DevConsole::ERROR_COLOR, "Connection attempt timed out." );

				closesocket( m_clientSocket );
				WSACleanup();

				StartupClient();

				// m_clientState = ClientState::READY_TO_CONNECT;
			}
			else
			{
				DebuggerPrintf( "select failed with error: %d\n", WSAGetLastError() );
				g_theDevConsole->AddLine( DevConsole::ERROR_COLOR, Stringf( "select failed with error: %d", WSAGetLastError() ) );

				// m_clientState = ClientState::READY_TO_CONNECT;
			}
		//}
	}
	else if ( m_clientState == ClientState::CONNECTED )
	{
		bool hasDataToRecv = false;
		do
		{
			hasDataToRecv = false;

			// poll for data to receive using select
			fd_set readSet;
			FD_ZERO( &readSet );
			FD_SET( m_clientSocket, &readSet );

			timeval timeout;
			timeout.tv_sec	= 0;
			timeout.tv_usec = 0;

			int selectResult = select( 0, &readSet, NULL, NULL, &timeout );
			if ( selectResult > 0 && FD_ISSET( m_clientSocket, &readSet ) )
			{
				// we have a connection, send and receive on the client socket
				int recvResult = recv( m_clientSocket, m_recvBuffer, sizeof( m_recvBuffer ), 0 );
				if ( recvResult == SOCKET_ERROR )
				{
					LogLastErrorMessage( WSAGetLastError() );
					closesocket( m_clientSocket );
					m_clientSocket = ~0ull;
					m_serverState  = ServerState::LISTENING;
				}
				else if ( recvResult == 0 )
				{
					// connection closed
					closesocket( m_clientSocket );
					m_clientSocket = ~0ull;
					m_serverState  = ServerState::LISTENING;
				}
				else // recvResult > 0, received some data
				{
					hasDataToRecv = true;

					for ( int index = 0; index < recvResult; index++ )
					{
						if ( m_recvBuffer[ index ] == '\0' )
						{
							// we have a complete message
							/*g_theDevConsole->ToggleOpen( true );
							g_theDevConsole->AddLine( DevConsole::INFO_MINOR_COLOR, "Received message from server: " + m_recvRemaining );*/

							// execute command
							g_theDevConsole->Execute( m_recvRemaining );

							m_recvRemaining.clear();
						}
						else
						{
							m_recvRemaining += m_recvBuffer[ index ];
						}
					}
				}
			}

		} while ( hasDataToRecv );
	}
}


//----------------------------------------------------------------------------------------------------------
void NetSystem::StartupServer()
{
	// split host address string to get host IP and host port
	Strings		splitString = SplitStringOnDelimiter( m_config.m_hostAddressString, ':' );
	std::string hostIpStr	= splitString[ 0 ];
	std::string hostPortStr = splitString[ 1 ];

	// startup windows sockets
	WSADATA data;
	int		startupResult = WSAStartup( MAKEWORD( 2, 2 ), &data );
	if ( startupResult != 0 )
	{
		switch ( startupResult )
		{
		case WSASYSNOTREADY:
			Log( "The underlying network subsystem is not ready for network communication.", DevConsole::ERROR_COLOR );
			break;
		case WSAVERNOTSUPPORTED:
			Log( "The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation.", DevConsole::ERROR_COLOR );
			break;
		case WSAEINPROGRESS:
			Log( "A blocking Windows Sockets 1.1 operation is in progress.", DevConsole::ERROR_COLOR );
			break;
		case WSAEPROCLIM:
			Log( "A limit on the number of tasks supported by the Windows Sockets implementation has been reached.", DevConsole::ERROR_COLOR );
			break;
		case WSAEFAULT:
			Log( "The lpWSAData parameter is not a valid pointer.", DevConsole::ERROR_COLOR );
			break;
		default:
			break;
		}
	}
	else // startup result == 0, success
	{
		// create listen socket
		m_listenSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
		if ( m_listenSocket == INVALID_SOCKET )
		{
			LogLastErrorMessage( WSAGetLastError() );
			WSACleanup();
			return;
		}

		// set mode = non-blocking
		unsigned long mode		   = 1; // non-blocking mode socket
		int			  ioModeResult = ioctlsocket( m_listenSocket, FIONBIO, &mode );
		if ( ioModeResult == SOCKET_ERROR )
		{
			LogLastErrorMessage( WSAGetLastError() );
			closesocket( m_listenSocket );
			WSACleanup();
			return;
		}
		else // ioModeResult == 0, success
		{
			// get host port from string
			m_hostPort = ( unsigned short ) ( atoi( hostPortStr.c_str() ) );

			m_hostAddress = INADDR_ANY;

			// bind the listen socket to a port
			sockaddr_in addr;
			addr.sin_family			  = AF_INET;
			addr.sin_addr.S_un.S_addr = htonl( m_hostAddress );
			addr.sin_port			  = htons( m_hostPort );
			int bindResult			  = bind( m_listenSocket, ( sockaddr* ) ( &addr ), ( int ) sizeof( addr ) );

			if ( bindResult == SOCKET_ERROR )
			{
				LogLastErrorMessage( WSAGetLastError() );
				closesocket( m_listenSocket );
				WSACleanup();
				return;
			}
			else // bindResult == 0
			{
				// listen for connections to accept
				int listenResult = listen( m_listenSocket, SOMAXCONN );
				if ( listenResult == SOCKET_ERROR )
				{
					LogLastErrorMessage( WSAGetLastError() );
					closesocket( m_listenSocket );
					WSACleanup();
					return;
				}
				else // listenResult == 0
				{
					m_serverState = ServerState::LISTENING;
					Log( "Server listening for connections", DevConsole::INFO_MINOR_COLOR );
				}
			}
		}
	}
}


//----------------------------------------------------------------------------------------------------------
void NetSystem::BeginFrameServer()
{
	if ( m_serverState == ServerState::LISTENING )
	{
		// poll for connections to accept using select
		fd_set readSet;
		FD_ZERO( &readSet );
		FD_SET( m_listenSocket, &readSet );

		timeval timeout;
		timeout.tv_sec	= 0;
		timeout.tv_usec = 0;

		int pollResult = select( 0, &readSet, NULL, NULL, &timeout );
		if ( pollResult > 0 && FD_ISSET( m_listenSocket, &readSet ) )
		{
			// there's a pending connection, accept it
			uintptr_t newSocket = accept( m_listenSocket, NULL, NULL );
			int		  lastError = WSAGetLastError();

			if ( newSocket == INVALID_SOCKET )
			{
				LogLastErrorMessage( lastError );
			}
			else // Not INVALID_SOCKET
			{
				// connection accepted
				unsigned long mode		   = 1; // non-blocking mode socket
				int			  ioModeResult = ioctlsocket( newSocket, FIONBIO, &mode );
				if ( ioModeResult == SOCKET_ERROR )
				{
					LogLastErrorMessage( WSAGetLastError() );
				}
				else // ioModeResult == 0, success
				{
					m_clientSocket = newSocket;
					m_serverState  = ServerState::CONNECTED;

					Log( "Client connected", DevConsole::INFO_MINOR_COLOR );
				}
			}
		}
	}
	else if ( m_serverState == ServerState::CONNECTED )
	{
		bool hasDataToRecv = false;
		do
		{
			hasDataToRecv = false;

			// poll for data to receive using select
			fd_set readSet;
			FD_ZERO( &readSet );
			FD_SET( m_clientSocket, &readSet );

			timeval timeout;
			timeout.tv_sec	= 0;
			timeout.tv_usec = 0;

			int selectResult = select( 0, &readSet, NULL, NULL, &timeout );
			if ( selectResult > 0 && FD_ISSET( m_clientSocket, &readSet ) )
			{
				// we have a connection, send and receive on the client socket
				int recvResult = recv( m_clientSocket, m_recvBuffer, sizeof( m_recvBuffer ), 0 );
				if ( recvResult == SOCKET_ERROR )
				{
					LogLastErrorMessage( WSAGetLastError() );
					closesocket( m_clientSocket );
					m_clientSocket = ~0ull;
					m_serverState  = ServerState::LISTENING;
				}
				else if ( recvResult == 0 )
				{
					// connection closed
					closesocket( m_clientSocket );
					m_clientSocket = ~0ull;
					m_serverState  = ServerState::LISTENING;
				}
				else // recvResult > 0, received some data
				{
					hasDataToRecv = true;

					for ( int index = 0; index < recvResult; index++ )
					{
						if ( m_recvBuffer[ index ] == '\0' )
						{
							// we have a complete message
							// Log( ( "Received message from client: " + m_recvRemaining ).c_str(), DevConsole::INFO_MINOR_COLOR );

							// execute command
							g_theDevConsole->Execute( m_recvRemaining );

							// echo acknowledge back to client
							std::string acknowledgeMessage = "Echo Message=\"server executed remote command: " + m_recvRemaining + "\"";
							SendMessageData( acknowledgeMessage );

							m_recvRemaining.clear();
						}
						else
						{
							m_recvRemaining += m_recvBuffer[ index ];
						}
					}
				}
			}

		} while ( hasDataToRecv );
	}
}


//----------------------------------------------------------------------------------------------------------
void NetSystem::EndFrame()
{
}


//----------------------------------------------------------------------------------------------------------
void NetSystem::Shutdown()
{
	if ( m_mode == Mode::CLIENT )
	{
		// close all open sockets
		closesocket( m_clientSocket );

		// shutdown windows sockets
		WSACleanup();
	}
	else if ( m_mode == Mode::SERVER )
	{
		// close all open sockets
		closesocket( m_clientSocket );
		closesocket( m_listenSocket );

		// shutdown windows sockets
		WSACleanup();
	}

	// un subscribe from events
	UnsubscribeFromEvents();

	// delete buffers
	delete[] m_sendBuffer;
	delete[] m_recvBuffer;
}


//----------------------------------------------------------------------------------------------------------
void NetSystem::Log( const char* message, Rgba8 echoColor )
{
	g_theDevConsole->AddLine( echoColor, message );
	DebuggerPrintf( message );
}


//----------------------------------------------------------------------------------------------------------
void NetSystem::LogLastErrorMessage( int lastErrorCode )
{
	LPVOID errorMsgBuffer;

	DWORD messageLength = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
		lastErrorCode, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), ( LPTSTR ) &errorMsgBuffer, 0, NULL );

	std::string errorMessage;
	if ( messageLength )
	{
		errorMessage = ( char* ) errorMsgBuffer;
		LocalFree( errorMsgBuffer ); // free the buffer
	}
	else
	{
		errorMessage = "Unknown error.";
	}

	Log( errorMessage.c_str(), DevConsole::ERROR_COLOR );
}


//----------------------------------------------------------------------------------------------------------
void NetSystem::SubscribeToEvents()
{
	SubscribeToEvent( "RemoteCommand", Event_RemoteCommand );
	SubscribeToEvent( "BurstTest", Event_BurstTest );
}


//----------------------------------------------------------------------------------------------------------
void NetSystem::UnsubscribeFromEvents()
{
	UnsubscribeFromEvent( "RemoteCommand", Event_RemoteCommand );
	UnsubscribeFromEvent( "BurstTest", Event_BurstTest );
}


//----------------------------------------------------------------------------------------------------------
bool NetSystem::Event_RemoteCommand( EventArgs& args )
{
	if ( args.HasKey( "Command" ) )
	{
		std::string command = args.GetValue( "Command", "INVALID_COMMAND" );

		// check and remove quotes
		if ( command[ 0 ] == '\"' )
		{
			command.erase( 0, 1 );
		}
		if ( command[ command.length() - 1 ] == '\"' )
		{
			command.erase( command.length() - 1, 1 );
		}

		g_theNetSystem->SendMessageData( command );
	}
	else
	{
		args.SetValue( "logType", "incorrect_args" );
	}

	return true;
}


//----------------------------------------------------------------------------------------------------------
bool NetSystem::Event_BurstTest( EventArgs& args )
{
	UNUSED( args );

	// send 20 messages in one frame
	for ( int index = 0; index < 20; index++ )
	{
		std::string message = Stringf( "Echo Message=%i", index + 1 );
		g_theNetSystem->SendMessageData( message );
	}

	return false;
}


//----------------------------------------------------------------------------------------------------------
void NetSystem::SendMessageData( const std::string& message )
{
	if ( m_clientState == ClientState::CONNECTED || m_serverState == ServerState::CONNECTED )
	{
		std::string messageToSend = message + '\0';
		// std::string messageToSend = message;

		strcpy_s( m_sendBuffer, messageToSend.length() + 1, message.c_str() );
		int sendResult = send( m_clientSocket, m_sendBuffer, ( int ) strlen( m_sendBuffer ) + 1, 0 );

		if ( sendResult == SOCKET_ERROR )
		{
			LogLastErrorMessage( WSAGetLastError() );
		}
		else
		{
			DebuggerPrintf( "Sent message to server" );
			// Log( "Sent message to server", DevConsole::INFO_MINOR_COLOR );
		}
	}
}




#endif // !defined( ENGINE_DISABLE_NET )