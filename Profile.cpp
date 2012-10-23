//----------------------------------------------------------------------------- 
// File: Profile.h
//
// Desc: This software is provided "as is" without express or implied
//       warranties. You may freely copy and compile this source into
//       applications you distribute provided that the copyright text
//       below is included in the resulting source code, for example:
//       "Portions Copyright (C) Steve Rabin, 2000"
//
// Copyright (C) Steve Rabin, 2000. | Modified: Dan 2002
//-----------------------------------------------------------------------------
#include "profile.h"
#include "custom_time.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "main.h"
#include "OpenGL.h"


//-----------------------------------------------------------------------------
// Name: CProfile()
// Desc: 
//-----------------------------------------------------------------------------
CProfile::CProfile()
{
    Log( "CProfile::CProfile()\n" );

    unsigned int i;

    for( i = 0; i < NUM_PROFILE_SAMPLES; i++ )
    {
        m_samples[i].bValid = false;
        m_history[i].bValid = false;
    }

    m_startProfile = GetExactTime();
    m_endProfile   = 0.0f;
//    m_strBuffer    = "";
}


//-----------------------------------------------------------------------------
// Name: Begin()
// Desc: 
//-----------------------------------------------------------------------------
void CProfile::Begin( char* name )
{
    unsigned int i = 0;

    while( i < NUM_PROFILE_SAMPLES && m_samples[i].bValid == true )
    {
        if( strcmp( m_samples[i].szName, name ) == 0 )
        {
            //Found the sample
            m_samples[i].iOpenProfiles++;
            m_samples[i].iProfileInstances++;
            m_samples[i].fStartTime = GetExactTime();
            assert( m_samples[i].iOpenProfiles == 1 ); //max 1 open at once
            return;
        }

        i++;    
    }

    if( i >= NUM_PROFILE_SAMPLES )
    {
        assert( !"Exceeded Max Available Profile Samples" );
        return;
    }

    strcpy( m_samples[i].szName, name );

    m_samples[i].bValid = true;
    m_samples[i].iOpenProfiles = 1;
    m_samples[i].iProfileInstances = 1;
    m_samples[i].fAccumulator = 0.0f;
    m_samples[i].fStartTime = GetExactTime();
    m_samples[i].fChildrenSampleTime = 0.0f;
}


//-----------------------------------------------------------------------------
// Name: End()
// Desc: 
//-----------------------------------------------------------------------------
void CProfile::End( char* name )
{
    unsigned int i = 0;
    unsigned int numParents = 0;

    while( i < NUM_PROFILE_SAMPLES && m_samples[i].bValid == true )
    {
        if( strcmp( m_samples[i].szName, name ) == 0 )
        {
            //Found the sample
            unsigned int inner = 0;
            int parent = -1;
            float fEndTime = GetExactTime();
            m_samples[i].iOpenProfiles--;

            //Count all parents and find the immediate parent
            while( m_samples[inner].bValid == true )
            {
                if( m_samples[inner].iOpenProfiles > 0 )
                {
                    //Found a parent (any open profiles are parents)
                    numParents++;

                    if( parent < 0 )
                    {
                        //Replace invalid parent (index)
                        parent = inner;
                    }
                    else if( m_samples[inner].fStartTime >= m_samples[parent].fStartTime )
                    {
                        //Replace with more immediate parent
                        parent = inner;
                    }
                }

                inner++;
            }

            //Remember the current number of parents of the sample
            m_samples[i].iNumParents = numParents;

            if( parent >= 0 )
            { 
                //Record this time in fChildrenSampleTime (add it in)
                m_samples[parent].fChildrenSampleTime += fEndTime -
                                                     m_samples[i].fStartTime;
            }

            //Save sample time in accumulator
            m_samples[i].fAccumulator += fEndTime - m_samples[i].fStartTime;

            return;
        }

        i++;    
    }
}


//-----------------------------------------------------------------------------
// Name: DumpOutputToBuffer()
// Desc: 
//-----------------------------------------------------------------------------
void CProfile::DumpOutputToFile()
{
    unsigned int i = 0;

    m_endProfile = GetExactTime();

    Log( "  Ave :   Min :   Max :   # : Profile Name\n" );
    Log( "--------------------------------------------\n" );

    while( i < NUM_PROFILE_SAMPLES && m_samples[i].bValid == true )
    {        
        unsigned int indent = 0;
        float sampleTime, percentTime, aveTime, minTime, maxTime;
        char line[256], name[256], indentedName[256];
        char ave[16], min[16], max[16], num[16];
            
        if( m_samples[i].iOpenProfiles < 0 )
        {
            assert( !"ProfileEnd() called without a ProfileBegin()" );    
        }
        else if( m_samples[i].iOpenProfiles > 0 )
        {
            assert( !"ProfileBegin() called without a ProfileEnd()" );
        }

        sampleTime = m_samples[i].fAccumulator - m_samples[i].fChildrenSampleTime;
        percentTime = ( sampleTime / (m_endProfile - m_startProfile ) ) * 100.0f;

        aveTime = minTime = maxTime = percentTime;

        //Add new measurement into the history and get the ave, min, and max
        StoreInHistory( m_samples[i].szName, percentTime );
        GetFromHistory( m_samples[i].szName, &aveTime, &minTime, &maxTime );

        //Format the data
        sprintf( ave, "%3.1f", aveTime );
        sprintf( min, "%3.1f", minTime );
        sprintf( max, "%3.1f", maxTime );
        sprintf( num, "%3d", m_samples[i].iProfileInstances );

        strcpy( indentedName, m_samples[i].szName );

        for( indent=0; indent < m_samples[i].iNumParents; indent++ )
        {
            sprintf( name, "   %s", indentedName );
            strcpy( indentedName, name );
        }

        sprintf( line,"%5s : %5s : %5s : %3s : %s\n", ave, min, max, num, indentedName );
        Log( line );    //Send the line to text buffer
        i++;
    }

    {  
        //Reset samples for next frame
        unsigned int i;

        for( i = 0; i < NUM_PROFILE_SAMPLES; i++ )
            m_samples[i].bValid = false;

        m_startProfile = GetExactTime();
    }
}



//-----------------------------------------------------------------------------
// Name: StoreInHistory()
// Desc: 
//-----------------------------------------------------------------------------
void CProfile::StoreInHistory( char* name, float percent )
{
    unsigned int i = 0;
    float oldRatio;
    float newRatio = 0.8f * GetElapsedTime();

    if( newRatio > 1.0f )
        newRatio = 1.0f;

    oldRatio = 1.0f - newRatio;

    while( i < NUM_PROFILE_SAMPLES && m_history[i].bValid == true )
    {
        if( strcmp( m_history[i].szName, name ) == 0 )
        {
            //Found the sample
            m_history[i].fAve = ( m_history[i].fAve * oldRatio ) + ( percent * newRatio );
            if( percent < m_history[i].fMin )
            {
                m_history[i].fMin = percent;
            }
            else
            {
                m_history[i].fMin = ( m_history[i].fMin * oldRatio ) + ( percent * newRatio );
            }

            if( m_history[i].fMin < 0.0f )
                m_history[i].fMin = 0.0f;

            if( percent > m_history[i].fMax )
                m_history[i].fMax = percent;
            else
                m_history[i].fMax = ( m_history[i].fMax * oldRatio ) + ( percent * newRatio );

            return;
        }

        i++;
    }

    if( i < NUM_PROFILE_SAMPLES )
    {
        //Add to history
        strcpy( m_history[i].szName, name );
        m_history[i].bValid = true;
        m_history[i].fAve = m_history[i].fMin = m_history[i].fMax = percent;
    }
    else
    {
        assert( !"Exceeded Max Available Profile Samples!");
    }
}


//-----------------------------------------------------------------------------
// Name: GetFromHistory()
// Desc: 
//-----------------------------------------------------------------------------
void CProfile::GetFromHistory( char* name, float* ave, float* min, float* max )
{
    unsigned int i = 0;

    while( i < NUM_PROFILE_SAMPLES && m_history[i].bValid == true )
    {
        if( strcmp( m_history[i].szName, name ) == 0 )
        {
            //Found the sample
            *ave = m_history[i].fAve;
            *min = m_history[i].fMin;
            *max = m_history[i].fMax;
            return;
        }

        i++;
    }    

    *ave = *min = *max = 0.0f;
}


//-----------------------------------------------------------------------------
// Name: Draw()
// Desc: 
//-----------------------------------------------------------------------------
void CProfile::CopyBufferToScreen()
{
    OpenGL->DrawGLText( 50, 50, "  Ave :   Min :   Max :   # : Profile Name" );
    OpenGL->DrawGLText( 50, 75, "--------------------------------------------" );
    OpenGL->DrawGLText( 50, 100, m_strBuffer );
    Log( m_strBuffer );
    Log( "\n" );
}


//-----------------------------------------------------------------------------
// Name: DumpOutputToBuffer()
// Desc: 
//-----------------------------------------------------------------------------
void CProfile::DumpOutputToBuffer()
{
    unsigned int i = 0;

    m_endProfile = GetExactTime();

    while( i < NUM_PROFILE_SAMPLES && m_samples[i].bValid == true )
    {        
        unsigned int indent = 0;
        float sampleTime, percentTime, aveTime, minTime, maxTime;
        char line[256], name[256], indentedName[256];
        char ave[16], min[16], max[16], num[16];
            
        if( m_samples[i].iOpenProfiles < 0 )
        {
            assert( !"ProfileEnd() called without a ProfileBegin()" );    
        }
        else if( m_samples[i].iOpenProfiles > 0 )
        {
            assert( !"ProfileBegin() called without a ProfileEnd()" );
        }

        sampleTime = m_samples[i].fAccumulator - m_samples[i].fChildrenSampleTime;
        percentTime = ( sampleTime / (m_endProfile - m_startProfile ) ) * 100.0f;

        aveTime = minTime = maxTime = percentTime;

        //Add new measurement into the history and get the ave, min, and max
        StoreInHistory( m_samples[i].szName, percentTime );
        GetFromHistory( m_samples[i].szName, &aveTime, &minTime, &maxTime );

        //Format the data
        sprintf( ave, "%3.1f", aveTime );
        sprintf( min, "%3.1f", minTime );
        sprintf( max, "%3.1f", maxTime );
        sprintf( num, "%3d", m_samples[i].iProfileInstances );

        strcpy( indentedName, m_samples[i].szName );

        for( indent=0; indent < m_samples[i].iNumParents; indent++ )
        {
            sprintf( name, "   %s", indentedName );
            strcpy( indentedName, name );
        }

        sprintf( line,"%5s : %5s : %5s : %3s : %s\n", ave, min, max, num, indentedName );

        strcpy( m_strBuffer, line );

        i++;
    }

    {  
        //Reset samples for next frame
        unsigned int i;

        for( i = 0; i < NUM_PROFILE_SAMPLES; i++ )
            m_samples[i].bValid = false;

        m_startProfile = GetExactTime();
    }
}
