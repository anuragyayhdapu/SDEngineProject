#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

//-----------------------------------------------------------------------------------------------
// To disable audio entirely (and remove requirement for fmod.dll / fmod64.dll) for any game,
//	#define ENGINE_DISABLE_AUDIO in your game's Code/Game/EngineBuildPreferences.hpp file.
//
// Note that this #include is an exception to the rule "engine code doesn't know about game code".
//	Purpose: Each game can now direct the engine via #defines to build differently for that game.
//	Downside: ALL games must now have this Code/Game/EngineBuildPreferences.hpp file.
//
// SD1 NOTE: THIS MEANS *EVERY* GAME MUST HAVE AN EngineBuildPreferences.hpp FILE IN ITS CODE/GAME FOLDER!!
#include "Game/EngineBuildPreferences.hpp"
#if !defined( ENGINE_DISABLE_AUDIO )


//-----------------------------------------------------------------------------------------------
// Link in the appropriate FMOD static library (32-bit or 64-bit)
//
#if defined( _WIN64 )
#pragma comment( lib, "ThirdParty/fmod/fmod64_vc.lib" )
#else
#pragma comment( lib, "ThirdParty/fmod/fmod_vc.lib" )
#endif


//-----------------------------------------------------------------------------------------------
// Initialization code based on example from "FMOD Studio Programmers API for Windows"
//
AudioSystem::AudioSystem()
	: m_fmodSystem( nullptr )
{
}

AudioSystem::AudioSystem(AudioSystemConfig const& config)
	: m_config(config)
{
}


//-----------------------------------------------------------------------------------------------
AudioSystem::~AudioSystem()
{
}


//------------------------------------------------------------------------------------------------
void AudioSystem::Startup()
{
	FMOD_RESULT result;
	result = FMOD::System_Create( &m_fmodSystem );
	ValidateResult( result );

	result = m_fmodSystem->init( 512, FMOD_INIT_3D_RIGHTHANDED, nullptr );
	ValidateResult( result );
}


//------------------------------------------------------------------------------------------------
void AudioSystem::Shutdown()
{
	FMOD_RESULT result = m_fmodSystem->release();
	ValidateResult( result );

	m_fmodSystem = nullptr; // #Fixme: do we delete/free the object also, or just do this?
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::BeginFrame()
{
	m_fmodSystem->update();
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::EndFrame()
{
}


//-----------------------------------------------------------------------------------------------
SoundID AudioSystem::CreateOrGetSound( const std::string& soundFilePath, AudioType audioType)
{
	std::map< std::string, SoundID >::iterator found = m_registeredSoundIDs.find( soundFilePath );
	if( found != m_registeredSoundIDs.end() )
	{
		return found->second;
	}
	else
	{
		FMOD::Sound* newSound = nullptr;

		if ( audioType == AudioType::Audio2D )
		{
			m_fmodSystem->createSound( soundFilePath.c_str(), FMOD_DEFAULT, nullptr, &newSound );
		}
		else // Audio3D
		{
			m_fmodSystem->createSound(soundFilePath.c_str(), FMOD_3D, nullptr, &newSound);
		}
		
		if( newSound )
		{
			SoundID newSoundID = m_registeredSounds.size();
			m_registeredSoundIDs[ soundFilePath ] = newSoundID;
			m_registeredSounds.push_back( newSound );
			return newSoundID;
		}
	}

	return MISSING_SOUND_ID;
}


//-----------------------------------------------------------------------------------------------
SoundPlaybackID AudioSystem::StartSound( SoundID soundID, bool isLooped, float volume, float balance, float speed, bool isPaused )
{
	size_t numSounds = m_registeredSounds.size();
	if( soundID < 0 || soundID >= numSounds )
		return MISSING_SOUND_ID;

	FMOD::Sound* sound = m_registeredSounds[ soundID ];
	if( !sound )
		return MISSING_SOUND_ID;

	FMOD::Channel* channelAssignedToSound = nullptr;
	m_fmodSystem->playSound( sound, nullptr, isPaused, &channelAssignedToSound );
	if( channelAssignedToSound )
	{
		int loopCount = isLooped ? -1 : 0;
		unsigned int playbackMode = isLooped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		float frequency;
		channelAssignedToSound->setMode(playbackMode);
		channelAssignedToSound->getFrequency( &frequency );
		channelAssignedToSound->setFrequency( frequency * speed );
		channelAssignedToSound->setVolume( volume );
		channelAssignedToSound->setPan( balance );
		channelAssignedToSound->setLoopCount( loopCount );
	}

	return (SoundPlaybackID) channelAssignedToSound;
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::StopSound( SoundPlaybackID soundPlaybackID )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to stop sound on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->stop();
}


//-----------------------------------------------------------------------------------------------
// Volume is in [0,1]
//
void AudioSystem::SetSoundPlaybackVolume( SoundPlaybackID soundPlaybackID, float volume )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set volume on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->setVolume( volume );
}


//-----------------------------------------------------------------------------------------------
// Balance is in [-1,1], where 0 is L/R centered
//
void AudioSystem::SetSoundPlaybackBalance( SoundPlaybackID soundPlaybackID, float balance )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set balance on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->setPan( balance );
}


//-----------------------------------------------------------------------------------------------
// Speed is frequency multiplier (1.0 == normal)
//	A speed of 2.0 gives 2x frequency, i.e. exactly one octave higher
//	A speed of 0.5 gives 1/2 frequency, i.e. exactly one octave lower
//
void AudioSystem::SetSoundPlaybackSpeed( SoundPlaybackID soundPlaybackID, float speed )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set speed on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	float frequency;
	FMOD::Sound* currentSound = nullptr;
	channelAssignedToSound->getCurrentSound( &currentSound );
	if( !currentSound )
		return;

	int ignored = 0;
	currentSound->getDefaults( &frequency, &ignored );
	channelAssignedToSound->setFrequency( frequency * speed );
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::ValidateResult( FMOD_RESULT result )
{
	if( result != FMOD_OK )
	{
		ERROR_RECOVERABLE( Stringf( "Engine/Audio SYSTEM ERROR: Got error result code %i - error codes listed in fmod_common.h\n", (int) result ) );
	}
}






void AudioSystem::SetNumListeners(int numListeners)
{
	m_fmodSystem->set3DNumListeners(numListeners);
}

void AudioSystem::UpdateListener(int listenerIndex, const Vec3& listenerGamePosition, const Vec3& listenerGameForward, const Vec3& listenerGameUp)
{
	FMOD_VECTOR listenerFmodPosition { -listenerGamePosition.y, listenerGamePosition.z , -listenerGamePosition.x };
	FMOD_VECTOR listenerFmodForward { -listenerGameForward.y, listenerGameForward.z , -listenerGameForward.x };
	FMOD_VECTOR listenerFmodUp { -listenerGameUp.y, listenerGameUp.z , -listenerGameUp.x };
	FMOD_VECTOR fmodVelocity { 0.f, 0.f, 0.f };
	
	m_fmodSystem->set3DListenerAttributes(listenerIndex, &listenerFmodPosition, &fmodVelocity, &listenerFmodForward, &listenerFmodUp);
}

SoundPlaybackID AudioSystem::StartSoundAt(SoundID soundID, const Vec3& gameSoundPosition, bool isLooped, float volume, float balance, float speed, bool isPaused)
{
	size_t numSounds = m_registeredSounds.size();
	if ( soundID < 0 || soundID >= numSounds )
		return MISSING_SOUND_ID;

	FMOD::Sound* sound = m_registeredSounds[ soundID ];
	if ( !sound )
		return MISSING_SOUND_ID;

	FMOD::Channel* channelAssignedToSound = nullptr;
	m_fmodSystem->playSound(sound, nullptr, isPaused, &channelAssignedToSound);
	if ( channelAssignedToSound )
	{
		int loopCount = isLooped ? -1 : 0;
		unsigned int playbackMode = isLooped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		float frequency;
		channelAssignedToSound->setMode(playbackMode);
		channelAssignedToSound->getFrequency(&frequency);
		channelAssignedToSound->setFrequency(frequency * speed);
		channelAssignedToSound->setVolume(volume);
		channelAssignedToSound->setPan(balance);
		channelAssignedToSound->setLoopCount(loopCount);

		FMOD_VECTOR fmodSoundPosition { -gameSoundPosition.y, gameSoundPosition.z , -gameSoundPosition.x };
		FMOD_VECTOR fmodVelocity { 0.f, 0.f, 0.f }; 
		channelAssignedToSound->set3DAttributes(&fmodSoundPosition, &fmodVelocity);
	}

	return ( SoundPlaybackID ) channelAssignedToSound;
}

void AudioSystem::SetSoundPosition(SoundPlaybackID soundPlaybackID, const Vec3& gameSoundPosition)
{
	if (!IsPlaying(soundPlaybackID))
		return;

	if ( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE("WARNING: attempt to set position on missing sound playback ID!");
		return;
	}

	FMOD::Channel* channelAssignedToSound = ( FMOD::Channel* ) soundPlaybackID;

	FMOD_VECTOR fmodSoundPosition { -gameSoundPosition.y, gameSoundPosition.z , -gameSoundPosition.x};
	FMOD_VECTOR fmodVelocity { 0.f, 0.f, 0.f };
	channelAssignedToSound->set3DAttributes(&fmodSoundPosition, &fmodVelocity);
}

bool AudioSystem::IsPlaying(SoundPlaybackID soundPlaybackID)
{
	if ( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE("WARNING: attempt check playing state on missing sound playback ID!");
		return false;
	}

	FMOD::Channel* channelAssignedToSound = ( FMOD::Channel* ) soundPlaybackID;
	bool isSoundPlaying = false;
	channelAssignedToSound->isPlaying(&isSoundPlaying);

	return isSoundPlaying;
}


unsigned int AudioSystem::GetPlaybackPositionInMilliseconds(SoundPlaybackID soundPlaybackID)
{
	unsigned int playbackPosition;

	FMOD::Channel* channelAssignedToSound = ( FMOD::Channel* ) soundPlaybackID;
	FMOD_RESULT result = channelAssignedToSound->getPosition(&playbackPosition, FMOD_TIMEUNIT_MS);

	ValidateResult(result);
	//DebuggerPrintf("playback pos: %i ms\n", playbackPosition);

	return playbackPosition;
}






void AudioSystem::Detect(const std::string& soundFilePath, float amplitudeThreshold)
{
	// Initialize FMOD
	
	FMOD::Sound* sound;
	FMOD::Channel* channel;
	FMOD_RESULT result;
	
	//void* extradriverdata = nullptr;



	// Load audio file
	result = m_fmodSystem->createSound(soundFilePath.c_str(), FMOD_DEFAULT, nullptr, &sound);
	if ( result != FMOD_OK )
	{
		DebuggerPrintf("Failed to load audio file.");
		return;
	}

	// Set up custom DSP


	
	FMOD_DSP_DESCRIPTION dsp_description = {};
	memset(&dsp_description, 0, sizeof(FMOD_DSP_DESCRIPTION));


	strcpy_s(dsp_description.name, "Amplitude Detection DSP");
	dsp_description.version = 0x00010000;
	dsp_description.numinputbuffers = 1;
	dsp_description.numoutputbuffers = 1;
	dsp_description.read = AudioSystem::amplitude_detection_callback;

	FMOD::DSP* amplitude_detection_dsp = nullptr;
	result = m_fmodSystem->createDSP(&dsp_description, &amplitude_detection_dsp);
	if ( result != FMOD_OK )
	{
		DebuggerPrintf("Failed to create custom DSP.\n");
		return;
	}

	// Set up custom DSP data
	AmplitudeDetectionData* data = new AmplitudeDetectionData();
	data->amplitude_threshold = amplitudeThreshold;
	float frequency;
	sound->getDefaults(&frequency, nullptr);
	data->sample_rate = static_cast< unsigned int >( frequency );

	//FMOD_DSP_SetUserData(amplitude_detection_dsp, data);
	amplitude_detection_dsp->setUserData(data);

	// Add custom DSP to the audio signal chain
	FMOD::ChannelGroup* master_group;
	m_fmodSystem->getMasterChannelGroup(&master_group);
	master_group->addDSP(0, amplitude_detection_dsp);

	// Play the sound and wait for it to finish
	result = m_fmodSystem->playSound(sound, nullptr, false, &channel);
	if ( result != FMOD_OK )
	{
		DebuggerPrintf("Failed to play sound.\n");
		return;
	}

	bool is_playing = true;
	while ( is_playing )
	{
		m_fmodSystem->update();
		channel->isPlaying(&is_playing);
	}

	// Print the timestamps where the amplitude is above the threshold
	std::vector<float>& timestamps = data->timestamps;
	DebuggerPrintf("Timestamps where amplitude is above the threshold:\n");
	for ( const float& timestamp : timestamps )
	{
		DebuggerPrintf("timeStamps %f\n", timestamp);
	}

	// Clean up
	sound->release();
	amplitude_detection_dsp->release();
}

FMOD_RESULT F_CALLBACK AudioSystem::amplitude_detection_callback(FMOD_DSP_STATE* dsp_state, float* inbuffer, float* outbuffer, unsigned int length, int inchannels, int* outchannels)
{
	UNUSED(outchannels);

	AmplitudeDetectionData* data = static_cast< AmplitudeDetectionData* >( dsp_state->plugindata );

	if ( !data )
	{
		return FMOD_ERR_INVALID_PARAM;
	}

	for ( unsigned int sample = 0; sample < length; ++sample )
	{
		for ( int channel = 0; channel < inchannels; ++channel )
		{
			float amplitude = fabsf(inbuffer[ ( sample * inchannels ) + channel ]);

			if ( amplitude > data->amplitude_threshold )
			{
				float timestamp = ( float ) ( data->sample_pos + sample ) / data->sample_rate;
				data->timestamps.push_back(timestamp);
			}
		}
	}

	memcpy(outbuffer, inbuffer, length * inchannels * sizeof(float));

	// Update sample position
	data->sample_pos += length;

	return FMOD_OK;
}



#endif // !defined( ENGINE_DISABLE_AUDIO )
