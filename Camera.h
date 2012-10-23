//----------------------------------------------------------------------------- 
// File: Camera.h
//
// Desc: 
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _CAMERA_H_
#define _CAMERA_H_



#include <D3DX8math.h>
#include "dinput.h"
#include "main.h"

class CCamera  
{
public:
    CCamera();
    virtual ~CCamera();

    bool FrameMove( const float fElapsedTime, int viewType );

    bool GetFromVec( float &x, float &y, float &z ) const;
    bool GetToVec( float &x, float &y, float &z ) const;
    bool GetUpVec( float &x, float &y, float &z ) const;

    VECT GetFromVec() const { return m_From; }
    VECT GetToVec() const   { return m_To;   }
    VECT GetUpVec() const   { return m_Up;   }

    bool SetFromVec( const float x, const float y, const float z );
    bool SetToVec( const float x, const float y, const float z );
    bool SetUpVec( const float x, const float y, const float z );
    void SetFromVec( VECT vVec ) { m_From = vVec; }
    void SetToVec( VECT vVec )   { m_To   = vVec; }
    void SetUpVec( VECT vVec )   { m_Up   = vVec; }

    bool Render();
    bool RotateView( float X, float Y, float Z );

    bool Move3D( float fElapsedTime );
    bool MoveXZ( float fElapsedTime );
    bool MoveXY( float fElapsedTime );
    bool MoveYZ( float fElapsedTime );

private:

    VECT m_From, m_To, m_Up;
};


#endif
