//----------------------------------------------------------------------------- 
// File: Audio.h
//
// Desc: Implementation file for the audio class. Manages all audio data
//
// Copyright (c) 2001 Dan
//-----------------------------------------------------------------------------
/*

//-----------------------------------------------------------------------------
// Includes
#include <fmod.h>
#include <fmod_errors.h>
#include <conio.h>
#include <D3DX8math.h>
#include "Audio.h"
#include "main.h"
#include "Buffer.h"
#include "Camera.h"
#include "Tracer.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Library Files
#pragma comment(lib, "fmodvc.lib")
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
CAudio::CAudio()
{
    TRACE_ENTER_FN( CAudio::CAudio )

    Log( "CAudio::CAudio()\n" );

    TRACE_LEAVE_FN()
}

CAudio::~CAudio()
{
    TRACE_ENTER_FN( CAudio::~CAudio )

    Log( "CAudio::~CAudio()\n" );

    Release();

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Checks for the correct DLL version, then initalizes the sound system
//-----------------------------------------------------------------------------
bool CAudio::Init()
{
    TRACE_ENTER_FN( CAudio::Init )

    Log( "CAudio::Init()\n" );

    Reset();

    // See if the DLL is the correct version
    if( FSOUND_GetVersion() < FMOD_VERSION )
    {
        char str[256];
        sprintf( str, "Required FMOD version: FMOD %.02f\n", FMOD_VERSION );
        Log( "if( FSOUND_GetVersion() < FMOD_VERSION )\n" );
        Log( str );

        TRACE_LEAVE_FN()

        return false;
    }

    // Use DirectSound
    FSOUND_SetOutput( FSOUND_OUTPUT_DSOUND );

    // Initalize Everything
    if( !FSOUND_Init( 44100, 32, FSOUND_INIT_GLOBALFOCUS ) )
    {
        Log( "if( !FSOUND_Init()\n" );

        char str[256];
        sprintf( str, "%s\n", FMOD_ErrorString( FSOUND_GetError() ) );
        Log( str );

        TRACE_LEAVE_FN()

        return false;
    }

    // Every world unit is equal to 1 meter
//    FSOUND_3D_Listener_SetDistanceFactor( 1.0f );

    // Set the Doppler shift to normal
//    FSOUND_3D_Listener_SetDopplerFactor( 1.0f );

    // Make sounds fad away normally
//    FSOUND_3D_Listener_SetRolloffFactor( 1.0f );

    // Set the volume all the way on all audio channels
    FSOUND_SetVolume( FSOUND_ALL, 255 );

    TRACE_LEAVE_FN()

    return true;
}



//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Releases all resources that were being used
//-----------------------------------------------------------------------------
bool CAudio::Release()
{
    Log( "CAudio::Release()\n" );

    // End any playback on all open sound channels
    FSOUND_StopSound( FSOUND_ALL );

    // Release all loaded sounds and set them to NULL
    for( int a = 0; a < MAX_SOUNDS; a++ )
    {
        if( m_Sounds[a].sound != NULL )
        {
            // Free the sound from memory
            FSOUND_Sample_Free( m_Sounds[a].sound );

            // Set it to NULL
            m_Sounds[a].sound = NULL;
        }
    }

    // Make sure we can start fresh
    Reset();

    // Uninitalize the sound system
    FSOUND_Close();

    return true;
}


//-----------------------------------------------------------------------------
// Name: LoadSound()
// Desc: Loads a sound clip from a file. The str parameter is the full path
//       and file name of the sound clip to load, and the LPHSOUND parameter
//       is the handle to the sound, which is loaded with correct information
//       if the function is successful.
//-----------------------------------------------------------------------------
bool CAudio::LoadSound( const char* str, HSOUND &hSound )
{
    TRACE_ENTER_FN( CAudio::LoadSound )

    // The index of the first available index for the sound
    int index;

    // If an error occurs, the handle will be invalid
    hSound.ID = -1;
    hSound.strFile = "";

    // Make sure that there is still room for more sounds
    if( m_NumSounds >= MAX_SOUNDS )
    {
        Log( "if( m_NumSounds >= MAX_SOUNDS )\n" );

        TRACE_LEAVE_FN()

        return false;
    }

    // Searches for the first available slot to load the new sound into
    for( index = 0; index < MAX_SOUNDS; index++ )
        if( m_Sounds[index].sound == NULL )
            break;

    // Load the sound sample into the array.
   // m_Sounds[index].sound = FSOUND_Sample_Load( FSOUND_FREE, str, FSOUND_HW3D | FSOUND_LOOP_NORMAL, 0 );

    // Make sure it was loaded correctly. If it wasn't, get the last error and
    // then return false;
 //   if( NULL == m_Sounds[index].sound )
 //   {
  //      char str[256];
   //     Log( "NULL == m_Sounds[index].sound @ CAudio::LoadSound() : " );
   //     sprintf( str, "%s\n", FMOD_ErrorString( FSOUND_GetError() ) );
    //    Log( str );

     //   TRACE_LEAVE_FN()

     ///   return false;
  //  }

    // Copy the file name for refrence
    m_Sounds[index].strFile = str;

    // Do not loop the sample when played
//    FSOUND_Sample_SetLoopMode( m_Sounds[index].sound, FSOUND_LOOP_OFF );

    // Set the attributes for the handle to this sound.
    hSound.ID = index;
    hSound.strFile = str;

    // Increment the number of loaded sound samples
    m_NumSounds++;
    
    TRACE_LEAVE_FN()

    return true;
}


//-----------------------------------------------------------------------------
// Name: PlaySound()
// Desc: Plays the sound associated with the handle
//       NOTE: Velocity is not currently implemented
//-----------------------------------------------------------------------------
bool CAudio::PlaySound( const HSOUND hSound, VECT vPosition, VECT vVelocity )
{
    // Set up the position and velocity vectors
    float fPos[3] = { vPosition.x, vPosition.y, vPosition.z };
    float fVel[3] = { vVelocity.x, vVelocity.y, vVelocity.z };

    // Make sure the ID is valid (not negative)
    if( hSound.ID < 0 )
    {
        Log( "if( hSound.ID < 0 )\n" );
        return false;
    }

    // The ID can never be more than the number possible
    if( hSound.ID > MAX_SOUNDS )
    {
        Log( "if( hSound.ID > MAX_SOUNDS )\n" );
        return false;
    }

    // If the handle points to a valid sound, play it
    if( NULL == m_Sounds[hSound.ID].sound )
    {
        Log( "if( m_Sounds[hSound.ID] != NULL ) @ CAudio::PlaySound()\n" );
        return false;
    }

    // Play the sound, and get the channel it is on. Sound starts paused
    // so that attributes can be set
    int iChannel = FSOUND_PlaySoundEx( FSOUND_FREE, m_Sounds[hSound.ID].sound, NULL, true );

    // Make sure that the sample was correctly assigned a channel
    if( -1 == iChannel )
    {
        Log( "if( -1 == iChannel ) @ CAudio::PlaySound()\n" );
        return false;
    }

    // Set the sound's orientation in world space
    FSOUND_3D_SetAttributes( iChannel, fPos, NULL );

    // Unpause the sound
    FSOUND_SetPaused( iChannel, false );

    return true;
}


//-----------------------------------------------------------------------------
// Name: StopSound()
// Desc: Ends the specified sound and sets the play position to the begining
//-----------------------------------------------------------------------------
bool CAudio::StopSound( const HSOUND hSound )
{
    return true;
}


//-----------------------------------------------------------------------------
// Name: RemoveSound()
// Desc: Removes a sound from memory
//-----------------------------------------------------------------------------
bool CAudio::RemoveSound( const HSOUND hSound )
{
    // The ID must be zero or more, otherwise its invaild
    if( hSound.ID < 0 )
    {
        Log( "if( hSound.ID < 0 ) @ CAudio::RemoveSound()\n" );
        return false;
    }

    // Cant be more thanis possible
    if( hSound.ID > MAX_SOUNDS )
    {
        Log( "if( hSound.ID > MAX_SOUNDS ) @ CAudio::RemoveSound()\n" );
        return false;
    }

    // If the handle is refrencing an object thats already been cleared, don't
    // do anything to it
    if( m_Sounds[hSound.ID].sound == NULL )
    {
        Log( "if( m_Sounds[hSound.ID] == NULL ) @ CAudio::RemoveSound()\n" );
        return false;
    }

    // Finally free the sample and set it to NULL
    FSOUND_Sample_Free( m_Sounds[hSound.ID].sound );

    // Clear this enlement in the sound list
    m_Sounds[hSound.ID].sound = NULL;
    m_Sounds[hSound.ID].strFile= "";
    
    // Decrement the number of loaded sounds
    m_NumSounds--;

    return true;
}


//-----------------------------------------------------------------------------
// Name: Reset()
// Desc: Sets all elements to NULL, does NOT free memory occupied by sounds
//-----------------------------------------------------------------------------
void CAudio::Reset()
{
    // Cycle through all elements and set them to null
    for( int a = 0; a < MAX_SOUNDS; a++ )
    {
        m_Sounds[a].sound = NULL;
        m_Sounds[a].strFile = "";
    }

    // No sounds are loaded
    m_NumSounds = 0;
}


//-----------------------------------------------------------------------------
// Name: SetMusicPlaylist()
// Desc: Sets a playlist to be played automatically. 'str' must be a full path
//       and file name of a supported playlist file type
//-----------------------------------------------------------------------------
bool CAudio::SetMusicPlaylist( const char* str )
{
    return true;
}


//-----------------------------------------------------------------------------
// Name: SetListenerPos()
// Desc: Sets the listener's orientation in space. The Camera contains a vector
//       for position, direction, and 'up'. It acts as a persons head
//-----------------------------------------------------------------------------
bool CAudio::SetListenerPos( CCamera* camera, VECT vVelocity )
{
    float fVel[3] = { vVelocity.x, vVelocity.y, vVelocity.z };
    float fFrom[3];
    float ax, ay, az, ux, uy, uz;
    
    camera->GetFromVec( fFrom[0], fFrom[1], fFrom[2] );
    camera->GetToVec( ax, ay, az );
    camera->GetUpVec( ux, uy, uz );

    fVel[0] = vVelocity.x;
    fVel[1] = vVelocity.y;
    fVel[2] = vVelocity.z;

    FSOUND_3D_Listener_SetAttributes( fFrom, NULL, ax, ay, az, ux, uy, uz );

    return true;
}


//-----------------------------------------------------------------------------
// Name: IsAlreadyLoaded()
// Desc: If a sound sample with the same file name is already loaded, return
//       'true'
//-----------------------------------------------------------------------------
bool CAudio::IsAlreadyLoaded( const char* str )
{
    // If an exact match of the file name is found, return true
    for( int a = 0; a < MAX_SOUNDS; a++ )
        if( m_Sounds[a].strFile == str )
            return true;

    // No match was found
    return false;
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Updates the sound buffers
//-----------------------------------------------------------------------------
bool CAudio::Render()
{
//    FSOUND_3D_Update();

    return true;

}
*/