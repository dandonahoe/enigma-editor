//----------------------------------------------------------------------------- 
// File: Vertex.cpp
//
// Desc: Definition file for the vertex class
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
#include <fstream>
#include <D3DX8math.h>
#include <stdio.h>
#include <math.h>
#include "PolygonManager.h"
#include "Vertex.h"
#include "main.h"
#include "Tracer.h"
#include "Util.h"
#include "SaveTokens.h"
//-----------------------------------------------------------------------------


unsigned int CVertex::m_UniqueID = 0;

float Round(const float number, const int num_digits);

//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
CVertex::CVertex()
{
    TRACE_ENTER_FN( CVertex::CVertex )

    m_eClassType = VERTEXCLASS;
    m_vColor = m_vPos = VECT( 0, 0, 0 );
    m_fU = m_fV = m_fVirtX = m_fVirtY = m_fVirtZ = 0.0f;
    m_fPrecision = 0.1f;

    m_pConnectedVerts = NULL;
    m_iNumConnectedVerts = 0;

    sprintf( m_strName, "Vertex:%d", m_UniqueID );
    m_UniqueID++;

    TRACE_LEAVE_FN()
}

CVertex::~CVertex()
{
    TRACE_ENTER_FN( CVertex::~CVertex )

    try
    {
        if( m_pConnectedVerts )
            delete[] m_pConnectedVerts;
    }
    catch( ... ) { LOG_ERR() }

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Updates the current state of the vertex
//-----------------------------------------------------------------------------
void CVertex::FrameMove( const float fElapsedTime )
{
    TRACE_ENTER_FN( CVertex::FrameMove )
    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: TranslateX()
// Desc: Changes the position of the vertex by 'fAmount' in the X direction
//-----------------------------------------------------------------------------
void CVertex::TranslateX( const float fAmount )
{
    TRACE_ENTER_FN( CVertex::TranslateX )

    m_fVirtX += fAmount;

    if( m_fVirtX < -m_fPrecision || m_fVirtX > m_fPrecision )
    {
        m_vPos.x += (float)( (int)( m_fVirtX * 10.0f ) ) / 10.0f;
        m_fVirtX = 0.0f;

        SnapVertsToGrid();
        PM->GenerateTextureCoords();
        DragConnectedVerts();
    }

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: TranslateY()
// Desc: Changes the position of the vertex by 'fAmount' in the Y direction
//-----------------------------------------------------------------------------
void CVertex::TranslateY( const float fAmount )
{
    TRACE_ENTER_FN( CVertex::TranslateY )

    m_fVirtY += fAmount;

    if( m_fVirtY < -m_fPrecision || m_fVirtY > m_fPrecision )
    {
        m_vPos.y += (float)( (int)( m_fVirtY * 10.0f ) ) / 10.0f;
        m_fVirtY = 0.0f;

        SnapVertsToGrid();
        PM->GenerateTextureCoords();
        DragConnectedVerts();
    }

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: TranslateZ()
// Desc: Changes the position of the vertex by 'fAmount' in the Z direction
//-----------------------------------------------------------------------------
void CVertex::TranslateZ( const float fAmount )
{
    TRACE_ENTER_FN( CVertex::TranslateZ )

    m_fVirtZ += fAmount;

    if( m_fVirtZ < -m_fPrecision || m_fVirtZ > m_fPrecision )
    {
        m_vPos.z += (float)( (int)( m_fVirtZ * 10.0f ) ) / 10.0f;
        m_fVirtZ = 0.0f;

        SnapVertsToGrid();
        PM->GenerateTextureCoords();
        DragConnectedVerts();
    }

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: ResetVirtualCoords()
// Desc: Reseting the virtual coordinates sets them all to zero.
//-----------------------------------------------------------------------------
void CVertex::ResetVirtualCoords()
{
    TRACE_ENTER_FN( CVertex::ResetVirtualCoords )

    m_fVirtX = m_fVirtY = m_fVirtZ = 0;

    TRACE_LEAVE_FN()
}
//-----------------------------------------------------------------------------
// Name: Save()
// Desc: 
//-----------------------------------------------------------------------------
bool CVertex::Save( const char* File, ofstream& out )
{
    TRACE_ENTER_FN( CVertex::Save )

    out << V_BEGIN     << NEWLINE;
    out << V_COLORRGB  << ' ' << m_vColor.x << ' ' << m_vColor.y << ' ' << m_vColor.z << NEWLINE;
    out << V_NAME      << ' ' << m_strName    << NEWLINE;
    out << V_POSXYZ    << ' ' << m_vPos.x << ' ' << m_vPos.y << ' ' << m_vPos.z << NEWLINE;
    out << V_U         << ' ' << m_fU         << NEWLINE;
    out << V_V         << ' ' << m_fV         << NEWLINE;
    out << V_UNIQUEID  << ' ' << m_UniqueID   << NEWLINE;
    out << V_VERTXYZ   << ' ' << m_fVirtX << ' ' << m_fVirtY << ' ' << m_fVirtZ << NEWLINE;
    out << V_PRECISION << ' ' << m_fPrecision << NEWLINE;
    out << V_END << NEWLINE;

    TRACE_LEAVE_FN() 

    return true;
}


//-----------------------------------------------------------------------------
// Name: Open()
// Desc: 
//-----------------------------------------------------------------------------
bool CVertex::Open( const char* File, ifstream& in )
{
    TRACE_ENTER_FN( CVertex::Open )

    char strToken[256];

    in >> strToken;

    if( !IsSameToken( V_BEGIN, strToken ) ) { LOG_VAR( strToken ) LOG_ERR_RET( false ) }

    in >> strToken;

    while( !IsSameToken( V_END, strToken ) )
    {
        if( !IsVertToken( strToken ) ) { LOG_VAR( strToken ) LOG_ERR_RET( false ) }

/***********************
VERSION 1.0 TOKENS BEGIN
***********************/
        if( IsSameToken( V_COLORRGB, strToken ) )
        { in >> m_vColor.x >> m_vColor.y >> m_vColor.z; }
        else if( IsSameToken( V_NAME, strToken ) )
        { in >> m_strName; }
        else if( IsSameToken( V_POSXYZ, strToken ) )
        { in >> m_vPos.x >> m_vPos.y >> m_vPos.z; }
        else if( IsSameToken( V_U, strToken ) )
        { in >> m_fU; }
        else if( IsSameToken( V_V, strToken ) )
        { in >> m_fV; }
        else if( IsSameToken( V_UNIQUEID, strToken ) )
        { in >> CVertex::m_UniqueID; }
        else if( IsSameToken( V_VERTXYZ, strToken ) )
        { in >> m_fVirtX >> m_fVirtY >> m_fVirtZ; }
        else if( IsSameToken( V_PRECISION, strToken ) )
        { in >> m_fPrecision; }
        else
        { LOG_VAR( strToken ) LOG_ERR() }

        in >> strToken;
    }
    
    m_eClassType = VERTEXCLASS;

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: AddVertexConnection()
// Desc: 
//-----------------------------------------------------------------------------
bool CVertex::AddVertexConnection( CVertex* v, const int iP, const int iF )
{
    TRACE_ENTER_FN( CVertex::AddVertexConnection )

    CONNECTEDVERTEX* pTemp;
    int iV;

    if( m_iNumConnectedVerts > 0 )
    {
        pTemp = new CONNECTEDVERTEX[m_iNumConnectedVerts];

        if( pTemp == NULL ) LOG_ERR_RET( false )

        for( iV = 0; iV < m_iNumConnectedVerts; iV++ )
            pTemp[iV] = m_pConnectedVerts[iV];
    }

    if( m_pConnectedVerts != NULL )
        delete[] m_pConnectedVerts;

    m_pConnectedVerts = new CONNECTEDVERTEX[m_iNumConnectedVerts+1];

    if( m_pConnectedVerts == NULL ) LOG_ERR_RET( false )

    for( iV = 0; iV < m_iNumConnectedVerts; iV++ )
        m_pConnectedVerts[iV] = pTemp[iV];

    m_pConnectedVerts[m_iNumConnectedVerts].pV = v;
    m_pConnectedVerts[m_iNumConnectedVerts].iP = iP;
    m_pConnectedVerts[m_iNumConnectedVerts].iF = iF;

    m_iNumConnectedVerts++;

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: DragConnectedVerts()
// Desc: 
//-----------------------------------------------------------------------------
bool CVertex::DragConnectedVerts()
{
    TRACE_ENTER_FN( CVertex::DragConnectedVerts )
/*
    int iP, iF, iV;

    try
    {
        for( iV = 0; iV < m_iNumConnectedVerts; iV++ )
        {
            if( m_pConnectedVerts[iV].pV == NULL ) {
                Log( "if( m_pConnectedVerts[iV] == NULL ) @ CVertex::DragConnectedVerts()\n" );
                Log( "iV: " ); Log( iV ); Log( "\n" );
                TRACE_LEAVE_FN() return false; }

            m_pConnectedVerts[iV].pV->SetVertPos( GetVertPos() );

            iP = m_pConnectedVerts[iV].iP;
            iF = m_pConnectedVerts[iV].iF;

            PM->F( iP, iF )->GenerateTextureCoordinates();
        }
    }
    catch( ... ) {
        Log( "catch( ... ) @ CVertex::DragConnectedVerts()\n" );
        Log( "iV: " ); Log( iV ); Log( "\n" );
        Log( "m_iNumConnectedVerts: " ); Log( m_iNumConnectedVerts ); Log( "\n" );
        TRACE_LEAVE_FN() return false;
    }
*/
    RET( true )
}


//-----------------------------------------------------------------------------
// Name: ClearAllVertexConnections()
// Desc: 
//-----------------------------------------------------------------------------
bool CVertex::ClearAllVertexConnections()
{
    TRACE_ENTER_FN( CVertex::ClearAllVertexConnections )

    try
    {
        if( m_pConnectedVerts != NULL )
        {
            delete[] m_pConnectedVerts;
            m_pConnectedVerts = NULL;
        }
    }
    catch( ... ) { LOG_VAR( m_iNumConnectedVerts ) LOG_ERR_RET( false ) }

    m_iNumConnectedVerts = 0;

    RET( true )
}




//-----------------------------------------------------------------------------
// Name: SnapVertsToGrid()
// Desc: 
//-----------------------------------------------------------------------------
void CVertex::SnapVertsToGrid()
{
    TRACE_ENTER_FN( CVertex::SnapVertsToGrid )

    m_vPos.x = Round( m_vPos.x, 1 );
    m_vPos.y = Round( m_vPos.y, 1 );
    m_vPos.z = Round( m_vPos.z, 1 );

    TRACE_LEAVE_FN()
}

float Round( const float number, const int num_digits )
{
	float doComplete5i, doComplete5( number * powf( 10.0f, (float)( num_digits + 1 ) ) );
	
	if(number < 0.0f)
		doComplete5 -= 5.0f;
	else
		doComplete5 += 5.0f;
	
	doComplete5 /= 10.0f;
	modff( doComplete5, &doComplete5i );
	
	return doComplete5i / powf( 10.0f, (float) num_digits );
}

//-----------------------------------------------------------------------------
// Name: SnapVertsToGrid()
// Desc: 
//-----------------------------------------------------------------------------
bool CVertex::SetVertPos( const VECT v )
{
    TRACE_ENTER_FN( CVertex::SnapVertsToGrid )

    m_vPos = v;

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Rebuild()
// Desc: 
//-----------------------------------------------------------------------------
bool CVertex::Rebuild()
{
    TRACE_ENTER_FN( CVertex::Rebuild() )

    m_fPos[ 0 ] = m_vPos.x;
    m_fPos[ 1 ] = m_vPos.y;
    m_fPos[ 2 ] = m_vPos.z;

    m_fCol[ 0 ] = m_vColor.x;
    m_fCol[ 1 ] = m_vColor.y;
    m_fCol[ 2 ] = m_vColor.z;

    m_fTexUV[ 0 ] = m_fU;
    m_fTexUV[ 1 ] = m_fV;

    RET( true )
}