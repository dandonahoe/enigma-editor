//----------------------------------------------------------------------------- 
// File: Profile.h
//
// Desc: This software is provided "as is" without express or implied
//       warranties. You may freely copy and compile this source into
//       applications you distribute provided that the copyright text
//       below is included in the resulting source code, for example:
//       "Portions Copyright (C) Steve Rabin, 2000"
//
// Copyright (C) Steve Rabin, 2000. 
//-----------------------------------------------------------------------------
#ifndef _PROFILE_H
#define _PROFILE_H


#include "Singleton.h"


#define NUM_PROFILE_SAMPLES 50


class CProfile : public CSingleton<CProfile>
{
public:
    CProfile();

    void Begin( char* name );
    void End( char* name );
    void DumpOutputToFile();
    void DumpOutputToBuffer();
    void CopyBufferToScreen();

private:
    typedef struct
    {
        bool bValid;                    //Whether this data is valid
        unsigned int iProfileInstances; //# of times ProfileBegin called
        int iOpenProfiles;              //# of times ProfileBegin w/o ProfileEnd
        char szName[256];               //Name of sample
        float fStartTime;               //The current open profile start time
        float fAccumulator;             //All samples this frame added together
        float fChildrenSampleTime;      //Time taken by all children
        unsigned int iNumParents;       //Number of profile parents
    } ProfileSample;

    typedef struct
    {
        bool bValid;        //Whether the data is valid
        char szName[256];   //Name of the sample
        float fAve;         //Average time per frame (percentage)
        float fMin;         //Minimum time per frame (percentage)
        float fMax;         //Maximum time per frame (percentage)
    } ProfileSampleHistory;

    ProfileSample        m_samples[NUM_PROFILE_SAMPLES];
    ProfileSampleHistory m_history[NUM_PROFILE_SAMPLES];

    void StoreInHistory( char* name, float percent );
    void GetFromHistory( char* name, float* ave, float* min, float* max );

    float m_startProfile;
    float m_endProfile;
    char m_strBuffer[256];
};


#define Profile CProfile::GetSingletonPtr()


#endif
