//----------------------------------------------------------------------------- 
// File: Camera.cpp
//
// Desc: The Main program
//
// Copyright (c) 2001 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include "Camera.h"
#include "DInput.h"
#include "main.h"
#include "Tracer.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
CCamera::CCamera()
{
    m_From.x = 0.0f;
    m_From.y = 0.0f;
    m_From.z = 10.0f;

    m_To.x = 0.0f;
    m_To.y = 0.0f;
    m_To.z = 0.0f;

    m_Up.x = 0.0f;
    m_Up.y = 1.0f;
    m_Up.z = 0.0f;
}
CCamera::~CCamera()
{
}

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc:
//-----------------------------------------------------------------------------
bool CCamera::FrameMove( const float fElapsedTime, int ViewType )
{
    if( InputManager->IsFrozen() ) return false;

    switch( ViewType )
    {
    case 0:
        return Move3D( fElapsedTime );
        break;

    case 1:
        return MoveXZ( fElapsedTime );
        break;

    case 2:
        return MoveXY( fElapsedTime );
        break;

    case 3:
        return MoveYZ( fElapsedTime );
        break;
    }
    

    return true;
}


//-----------------------------------------------------------------------------
// Name: GetFromVec()
// Desc:
//-----------------------------------------------------------------------------
bool CCamera::GetFromVec( float &x, float &y, float &z ) const
{
    x = m_From.x;
    y = m_From.y;
    z = m_From.z;

    return true;
}


//-----------------------------------------------------------------------------
// Name: GetToVec()
// Desc:
//-----------------------------------------------------------------------------
bool CCamera::GetToVec( float &x, float &y, float &z ) const
{
    x = m_To.x;
    y = m_To.y;
    z = m_To.z;

    return true;
}


//-----------------------------------------------------------------------------
// Name: GetUpVec()
// Desc:
//-----------------------------------------------------------------------------
bool CCamera::GetUpVec( float &x, float &y, float &z ) const
{
    x = m_Up.x;
    y = m_Up.y;
    z = m_Up.z;

    return true;
}


//-----------------------------------------------------------------------------
// Name: SetFromVec()
// Desc:
//-----------------------------------------------------------------------------
bool CCamera::SetFromVec( const float x, const float y, const float z )
{
    m_From.x = x;
    m_From.y = y;
    m_From.z = z;

    return true;
}


//-----------------------------------------------------------------------------
// Name: SetToVec()
// Desc:
//-----------------------------------------------------------------------------
bool CCamera::SetToVec( const float x, const float y, const float z )
{
    m_To.x = x;
    m_To.y = y;
    m_To.z = z;

    return true;
}


//-----------------------------------------------------------------------------
// Name: SetUpVec()
// Desc:
//-----------------------------------------------------------------------------
bool CCamera::SetUpVec( const float x, const float y, const float z )
{
    m_Up.x = x;
    m_Up.y = y;
    m_Up.z = z;

    return true;
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc:
//-----------------------------------------------------------------------------
bool CCamera::Render()
{
    TRACE_ENTER_FN( CCamera::Render )

    gluLookAt( m_From.x, m_From.y, m_From.z,
               m_To.x,   m_To.y,   m_To.z,
               m_Up.x,   m_Up.y,   m_Up.z );

    TRACE_LEAVE_FN()

    return true;
}


//-----------------------------------------------------------------------------
// Name: RotateView()
// Desc:
//-----------------------------------------------------------------------------
bool CCamera::RotateView( float X, float Y, float Z )
{
    VECT vVector;

    // Get our view vector (The direciton we are facing)
    vVector.x = m_To.x - m_From.x;        // This gets the direction of the X    
    vVector.y = m_To.y - m_From.y;        // This gets the direction of the Y
    vVector.z = m_To.z - m_From.z;        // This gets the direction of the Z

    // Rotate the view along the desired axis
    if( X )
    {
        // Rotate the view vector up or down, then add it to our position
        m_To.z = (float)( m_From.z + sin( X ) * vVector.y + cos( X ) * vVector.z );
        m_To.y = (float)( m_From.y + cos( X ) * vVector.y - sin( X ) * vVector.z );
    }

    if( Y )
    {
        // Rotate the view vector right or left, then add it to our position
        m_To.z = (float)( m_From.z + sin( Y ) * vVector.x + cos( Y ) * vVector.z );
        m_To.x = (float)( m_From.x + cos( Y ) * vVector.x - sin( Y ) * vVector.z );
    }

    if( Z )
    {
        // Rotate the view vector diagnally right or diagnally down, then add it to our position
        m_To.x = (float)( m_From.x + sin( Z ) * vVector.y + cos( Z ) * vVector.x );        
        m_To.y = (float)( m_From.y + cos( Z ) * vVector.y - sin( Z ) * vVector.x );
    }

    return true;
}


//-----------------------------------------------------------------------------
// Name: Move3D()
// Desc:
//-----------------------------------------------------------------------------
bool CCamera::Move3D( float fElapsedTime )
{
    const float DIVIDE = 1000.0f;

   // fElapsedTime

    m_From = VECT( sinf( fElapsedTime ) * 10.0f, 10.0f, cosf( fElapsedTime ) * 10.0f );

    m_To = VECT( 0.0f, 0.0f, 0.0f );


/*
    if( InputManager->GetMouseButtonLeft() &&
        InputManager->GetMouseButtonRight() )
    {
        VECT matTrans( 0, 0, 0 ), matRot( 0, 0, 0 );

        matTrans.y = ( -(float)InputManager->GetMouseY() / DIVIDE ) * fElapsedTime * DIVIDE;
        m_From += matTrans;
        m_To   += matTrans;

        VECT vCross( 0, 0, 0 );
        VECT vViewVector( m_To.x - m_From.x, m_To.y - m_From.y, m_To.z - m_From.z );                                                

        vCross.x = ( ( m_Up.y * vViewVector.z ) - ( m_Up.z * vViewVector.y ) );
        vCross.y = ( ( m_Up.z * vViewVector.x ) - ( m_Up.x * vViewVector.z ) );
        vCross.z = ( ( m_Up.x * vViewVector.y ) - ( m_Up.y * vViewVector.x ) );

        m_From.x += vCross.x * ( -(float)InputManager->GetMouseX() / DIVIDE ) * fElapsedTime * DIVIDE * 0.15f;
        m_From.z += vCross.z * ( -(float)InputManager->GetMouseX() / DIVIDE ) * fElapsedTime * DIVIDE * 0.15f;

        m_To.x   += vCross.x * ( -(float)InputManager->GetMouseX() / DIVIDE ) * fElapsedTime * DIVIDE * 0.15f;
        m_To.z   += vCross.z * ( -(float)InputManager->GetMouseX() / DIVIDE ) * fElapsedTime * DIVIDE * 0.15f;
    }
    else if( InputManager->GetMouseButtonRight() )
    {
       RotateView( 0.0f, ( (float)InputManager->GetMouseX() / DIVIDE ) * fElapsedTime * DIVIDE * 0.15f, 0.0f );

       m_To.y += ( -(float)InputManager->GetMouseY() / DIVIDE ) * fElapsedTime * ( DIVIDE * 2.0f );
    }
    else if( InputManager->GetMouseButtonLeft() )
    {
        VECT vVector;

        RotateView( 0.0f, ( (float)InputManager->GetMouseX() / DIVIDE ) * fElapsedTime * DIVIDE * 0.15f, 0.0f );

        vVector.x = m_To.x - m_From.x;    
        vVector.y = m_To.y - m_From.y;
        vVector.z = m_To.z - m_From.z;

        m_From.x += vVector.x * ( -(float)InputManager->GetMouseY() / DIVIDE ) * fElapsedTime * DIVIDE * 0.15f;
        m_From.z += vVector.z * ( -(float)InputManager->GetMouseY() / DIVIDE ) * fElapsedTime * DIVIDE * 0.15f;

        m_To.x   += vVector.x * ( -(float)InputManager->GetMouseY() / DIVIDE ) * fElapsedTime * DIVIDE * 0.15f;
        m_To.z   += vVector.z * ( -(float)InputManager->GetMouseY() / DIVIDE ) * fElapsedTime * DIVIDE * 0.15f;
    }

    */
    return true;
}


//-----------------------------------------------------------------------------
// Name: MoveXZ()
// Desc:
//-----------------------------------------------------------------------------
bool CCamera::MoveXZ( float fElapsedTime )
{
    const float DIVIDE = 1000.0f;
    bool bNeedsUpdate = false;

    if( InputManager->GetMouseButtonLeft() && InputManager->GetMouseButtonRight() )
    {
        m_From.y += ( -(float)InputManager->GetMouseY() / DIVIDE ) * fElapsedTime * DIVIDE;
        bNeedsUpdate = true;
    }
    else if( InputManager->GetMouseButtonLeft() )
    {
        m_From.x += ( (float)InputManager->GetMouseX() / DIVIDE ) * fElapsedTime * DIVIDE;
        m_From.z += ( (float)InputManager->GetMouseY() / DIVIDE ) * fElapsedTime * DIVIDE;
        bNeedsUpdate = true;
    }

    m_To = m_From;
    m_To.y -= 1;

    return bNeedsUpdate;
}


//-----------------------------------------------------------------------------
// Name: MoveXY()
// Desc:
//-----------------------------------------------------------------------------
bool CCamera::MoveXY( float fElapsedTime )
{
    const float DIVIDE = 1000.0f;
    bool bNeedsUpdate = false;

    if( InputManager->GetMouseButtonLeft() && InputManager->GetMouseButtonRight() )
    {
        m_From.z += ( -(float)InputManager->GetMouseY() / DIVIDE ) * fElapsedTime * DIVIDE;
        bNeedsUpdate = true;
    }
    else if( InputManager->GetMouseButtonLeft() )
    {
        m_From.x += ( (float)InputManager->GetMouseX() / DIVIDE ) * fElapsedTime * DIVIDE;
        m_From.y += ( -(float)InputManager->GetMouseY() / DIVIDE ) * fElapsedTime * DIVIDE;
        bNeedsUpdate = true;
    }

    m_To = m_From;
    m_To.z -= 1;

    return bNeedsUpdate;
}


//-----------------------------------------------------------------------------
// Name: MoveYZ()
// Desc:
//-----------------------------------------------------------------------------
bool CCamera::MoveYZ( float fElapsedTime )
{
    const float DIVIDE = 1000.0f;
    bool bNeedsUpdate = false;

    if( InputManager->GetMouseButtonLeft() && InputManager->GetMouseButtonRight() )
    {
        m_From.x += ( -(float)InputManager->GetMouseY() / DIVIDE ) * fElapsedTime * DIVIDE;
        bNeedsUpdate = true;
    }
    else if( InputManager->GetMouseButtonLeft() )
    {
        m_From.z += ( -(float)InputManager->GetMouseX() / DIVIDE ) * fElapsedTime * DIVIDE;
        m_From.y += ( -(float)InputManager->GetMouseY() / DIVIDE ) * fElapsedTime * DIVIDE;
        bNeedsUpdate = true;
    }

    m_To = m_From;
    m_To.x -= 1;

    return bNeedsUpdate;
}