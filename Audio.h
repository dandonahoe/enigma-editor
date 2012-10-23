//----------------------------------------------------------------------------- 
// File: Audio.h
//
// Desc: Declaration file for the audio class
//
// Copyright (c) 2001 Dan
//-----------------------------------------------------------------------------
#ifndef _AUDIO_H_
#define _AUDIO_H_


//-----------------------------------------------------------------------------
// Includes
//#include <fmod.h>
//#include "Singleton.h"
//#include "Buffer.h"
//#include "Camera.h"
//-----------------------------------------------------------------------------


typedef struct
{
    // The ID is unique to each sound. It is also the index of the sound
    // in the m_Sounds[] array;
    int ID;

    // Full path and file name to the sound sample
    CBuffer strFile;

} HSOUND, *LPHSOUND;


#define MAX_SOUNDS 128


//-----------------------------------------------------------------------------
// Name: class CAudio 
// Desc: 
//-----------------------------------------------------------------------------
/*
class CAudio : public CSingleton<CAudio>
{
public:
    CAudio();
    virtual ~CAudio();
    bool Init();
    bool Release();

    unsigned int GetNumSounds() const { return m_NumSounds; }
    bool LoadSound( const char* str, HSOUND &hSound );
    bool RemoveSound( const HSOUND hSound );
    bool PlaySound( const HSOUND hSound, VECT vPosition, VECT vVelocity );
    bool StopSound( const HSOUND hSound );
    bool SetMusicPlaylist( const char* str );
    bool SetListenerPos( CCamera* camera, VECT m_vVelocity );
    void Reset();
    bool Render();

private:

    typedef struct
    {
//        FSOUND_SAMPLE* sound;
        CBuffer strFile;
    } SOUNDSAMPLE;

    bool IsAlreadyLoaded( const char* str );

    SOUNDSAMPLE m_Sounds[MAX_SOUNDS];
    unsigned int m_NumSounds;

};

*/
//#define AudioManager CAudio::GetSingletonPtr()
//#define AM CAudio::GetSingletonPtr()


#endif