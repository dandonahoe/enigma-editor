//----------------------------------------------------------------------------- 
// File: 
//
// Desc: 
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include <math.h>
#include <stdio.h>
#include "light.h"
#include "main.h"
#include "enum.h"
#include "util.h"
#include "tracer.h"
//-----------------------------------------------------------------------------


unsigned int CLight::m_UniqueID = 0;


//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
CLight::CLight()
{
    Log( "CLight::CLight()\n" );

    sprintf( m_strName, "Light:%d", m_UniqueID );
    m_UniqueID++;

    m_pQuadricObj = gluNewQuadric();
    gluQuadricTexture( m_pQuadricObj, false );
    gluQuadricDrawStyle( m_pQuadricObj, GLU_FILL );

    m_eClassType = LIGHTCLASS;

    m_fBrightness = 1.0f;
}

CLight::~CLight()
{
    Log( "CLight::~CLight()\n" );

    if( m_pQuadricObj != NULL )
        gluDeleteQuadric( m_pQuadricObj );
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the image that represents the light
//-----------------------------------------------------------------------------
bool CLight::Render()
{
    glColor3f( m_vColor.x, m_vColor.y, m_vColor.z );

    glPushMatrix();
    glTranslatef( m_vPos.x, m_vPos.y, m_vPos.z );
    gluSphere( m_pQuadricObj, 0.1f, 16, 16 );
    glPopMatrix();

    return true;
}


//-----------------------------------------------------------------------------
// Name: SetColor()
// Desc: Sets the color of the light
//-----------------------------------------------------------------------------
void CLight::SetColor( const float r, const float g, const float b )
{
    m_vColor.x = r;
    m_vColor.y = g;
    m_vColor.z = b;
}


//-----------------------------------------------------------------------------
// Name: SetPos()
// Desc:Puts the light somewhere in space
//-----------------------------------------------------------------------------
void CLight::SetPos( const float x, const float y, const float z )
{
    m_vPos.x = x;
    m_vPos.y = y;
    m_vPos.z = z;
}


//-----------------------------------------------------------------------------
// Name: TranslateX()
// Desc: Changes the position of the light by 'fAmount' in the X direction
//-----------------------------------------------------------------------------
void CLight::TranslateX( const float fAmount )
{
    TRACE_ENTER_FN( CLight::TranslateX )

    m_fVirt.x += fAmount;

    if( m_fVirt.x < -m_fPrecision || m_fVirt.x > m_fPrecision )
    {
        m_vPos.x += (float)( (int)( m_fVirt.x * 10.0f ) ) / 10.0f;
        m_fVirt.x = 0.0f;
    }

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: TranslateY()
// Desc: Changes the position of the light by 'fAmount' in the Y direction
//-----------------------------------------------------------------------------
void CLight::TranslateY( const float fAmount )
{
    TRACE_ENTER_FN( CLight::TranslateY )

    m_fVirt.y += fAmount;

    if( m_fVirt.y < -m_fPrecision || m_fVirt.y > m_fPrecision )
    {
        m_vPos.y += (float)( (int)( m_fVirt.y * 10.0f ) ) / 10.0f;
        m_fVirt.y = 0.0f;
    }

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: TranslateZ()
// Desc: Changes the position of the light by 'fAmount' in the Z direction
//-----------------------------------------------------------------------------
void CLight::TranslateZ( const float fAmount )
{
    TRACE_ENTER_FN( CLight::TranslateZ )

    m_fVirt.z += fAmount;

    if( m_fVirt.z < -m_fPrecision || m_fVirt.z > m_fPrecision )
    {
        m_vPos.z += (float)( (int)( m_fVirt.z * 10.0f ) ) / 10.0f;
        m_fVirt.z = 0.0f;
    }

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: ResetVirtualCoords()
// Desc: Reseting the virtual coordinates sets them all to zero.
//-----------------------------------------------------------------------------
void CLight::ResetVirtualCoords()
{
    TRACE_ENTER_FN( CLight::ResetVirtualCoords )

    m_fVirt.x = m_fVirt.y = m_fVirt.z = 0;

    TRACE_LEAVE_FN()
}