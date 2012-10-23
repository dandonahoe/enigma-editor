//----------------------------------------------------------------------------- 
// File: Face.cpp
//
// Desc: Manages all information and data for faces
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#include <D3DX8math.h>
#include <stdio.h>
#include "PolygonManager.h"
#include "Face.h"
#include "main.h"
#include "Vertex.h"
#include "Tracer.h"
#include "TextureManager.h"
#include "Util.h"
#include "SaveTokens.h"
//-----------------------------------------------------------------------------


// Sets the initial value for the face class
unsigned int CFace::m_UniqueID = 0;


//-----------------------------------------------------------------------------
// Construction/Destruction
// Sets the class type, face type, default texture handle, number of verticies
// and creates a unique ID for this face. Faces are not referred to by their
// ID number, but by their 'm_strName' member.
//-----------------------------------------------------------------------------
CFace::CFace()
{
    TRACE_ENTER_FN( CFace::CFace )

    m_eClassType   = FACECLASS;
    m_eFaceType    = NOTHINGFACE;
    m_iNumVerts    = 0;
    m_bPanU        = false;
    m_bPanV        = false;
    m_bTranslucent = false;

    TM->Invalidate( m_hTexture1 );
    TM->Invalidate( m_hTexture2 );
    TM->Invalidate( &m_hLightmap );

    m_bUseTexture1 = true;
    m_bUseTexture2 = true;
    m_bUseLightmap = false;

    sprintf( m_strName, "Face:%d", m_UniqueID );
    m_UniqueID++;

  //  m_iLightmapRes = 8;
  //  m_uiLightmap = 0;

    m_pfFaceColors    = NULL;
    m_pfFaceVerts     = NULL;
    m_pfFaceTexCoords = NULL;

    m_iU[0] = 0;
    m_iV[0] = 0;
    m_iU[1] = 1;
    m_iV[1] = 0;
    m_iU[2] = 1;
    m_iV[2] = 1;
    m_iU[3] = 0;
    m_iV[3] = 1;

    m_pfLightmapCoords = new float[ 8 ];

    m_pfLightmapCoords[ 0 ] = m_iU[ 0 ];
    m_pfLightmapCoords[ 1 ] = m_iV[ 0 ];
    m_pfLightmapCoords[ 2 ] = m_iU[ 1 ];
    m_pfLightmapCoords[ 3 ] = m_iV[ 1 ];
    m_pfLightmapCoords[ 4 ] = m_iU[ 2 ];
    m_pfLightmapCoords[ 5 ] = m_iV[ 2 ];
    m_pfLightmapCoords[ 6 ] = m_iU[ 3 ];
    m_pfLightmapCoords[ 7 ] = m_iV[ 3 ];

    if( !m_pfLightmapCoords ) LOG_ERR()

    m_eGLFaceType = 0;

    // Creates a white lightmap for this face lightmap
    // NOTE: This stuff should not be in the constructor, it has
    //       a possibility of failure
    /*
    try
    {
        const int iLen = GetLightmapRes() * GetLightmapRes() * 3;
        unsigned char* pLightmap = new unsigned char[iLen];

        if( pLightmap != NULL )
        {
            for( int a = 0; a < iLen; a++ )
                pLightmap[a] = (unsigned char)255;

            if( !CreateLightmap( GetLightmapRes(), pLightmap ) )
                Log( "if( !CreateLightmap( GetLightmapRes(), pLightmap ) ) @ CFace::CFace()\n" );

            if( pLightmap != NULL )
            {
                delete[] pLightmap;
                pLightmap = NULL;
            }
        }
    }
    catch( ... )
    {
        Log( "catch( ... ) @ CFace::CFace()\n" );
    }
*/
    TRACE_LEAVE_FN()
}

CFace::~CFace()
{
    TRACE_ENTER_FN( CFace::~CFace )

    Release();

    TRACE_LEAVE_FN()
}
//-----------------------------------------------------------------------------
// Name: Release()
// Desc: 
//-----------------------------------------------------------------------------
void CFace::Release()
{
    TRACE_ENTER_FN( CFace::Release )

//    if( !ReleaseLightmap() ) LOG_ERR()

    try { if( m_pfFaceVerts     != NULL ) delete[] m_pfFaceVerts;     } catch( ... ) { LOG_ERR() }
    try { if( m_pfFaceColors    != NULL ) delete[] m_pfFaceColors;    } catch( ... ) { LOG_ERR() }
    try { if( m_pfFaceTexCoords != NULL ) delete[] m_pfFaceTexCoords; } catch( ... ) { LOG_ERR() }
    
    m_pfFaceVerts     = NULL;
    m_pfFaceColors    = NULL;
    m_pfFaceTexCoords = NULL;

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: SetName()
// Desc: 
//-----------------------------------------------------------------------------
void CFace::SetName( const char* str, bool bAddUniqueID )
{
    TRACE_ENTER_FN( CFace::SetName )

    if( str == NULL ) { LOG_ERR() TRACE_LEAVE_FN() return; }

    if( bAddUniqueID )
    {
        sprintf( m_strName, "%s:%d", m_UniqueID );
        m_UniqueID++;

        TRACE_LEAVE_FN() return;
    }

    strcpy( m_strName, str );

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: SetQuadVerts()
// Desc: Changes the face type to quad, updates the number of verts, and
//       replaces all the old coords with the new ones
//       NOTE: default color is white
//-----------------------------------------------------------------------------
void CFace::SetQuadVerts( const VECT &v1, const VECT &v2,
                          const VECT &v3, const VECT &v4 )
{
    TRACE_ENTER_FN( CFace::SetQuadVerts )

    VECT c( 255, 255, 255);

    m_eFaceType = QUADFACE;
    m_iNumVerts = 4;

    // Set each position
    m_Verts[0].SetVertPos( v1 );
    m_Verts[1].SetVertPos( v2 );
    m_Verts[2].SetVertPos( v3 );
    m_Verts[3].SetVertPos( v4 );

    // Set each color
    m_Verts[0].SetColor( c );
    m_Verts[1].SetColor( c );
    m_Verts[2].SetColor( c );
    m_Verts[3].SetColor( c );

    m_eGLFaceType = GL_QUADS;/**/

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: SetTriangleVerts()
// Desc: Changes the face type to triangle, updates the number of verts, and
//       replaces the old coords with new ones
//       NOTE: default color is white
//-----------------------------------------------------------------------------
void CFace::SetTriangleVerts( const VECT &v1, const VECT &v2, const VECT &v3 )
{
    TRACE_ENTER_FN( CFace::SetTriangleVerts )

    VECT c( 255, 255, 255);

    m_eFaceType = TRIANGLEFACE;
    m_iNumVerts = 3;

    // Set the coords
    // NOTE: when a face is a triangle, the last index of the m_Vert array
    //       is simply left empty
    m_Verts[0].SetVertPos( v1 );
    m_Verts[1].SetVertPos( v2 );
    m_Verts[2].SetVertPos( v3 );

    m_Verts[0].SetColor( c );
    m_Verts[1].SetColor( c );
    m_Verts[2].SetColor( c );

    m_eGLFaceType = GL_TRIANGLES;/**/

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: TranslateTexU()
// Desc: adds 'amount' to all U texture coords
//-----------------------------------------------------------------------------
void CFace::TranslateTexU( const float amount )
{
    TRACE_ENTER_FN( CFace::TranslateTexU )

    for( int iV = 0; iV < GetLoadedVerts(); iV++ )
        m_Verts[iV].SetTexU( m_Verts[iV].GetTexU() + amount );

    //m_Verts[iV].m_fU += amount;

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: TranslateTexV()
// Desc: adds 'amount' to all V texture coords
//-----------------------------------------------------------------------------
void CFace::TranslateTexV( const float amount )
{
    TRACE_ENTER_FN( CFace::TranslateTexV )

    for( int iV = 0; iV < GetLoadedVerts(); iV++ )
        m_Verts[iV].SetTexV( m_Verts[iV].GetTexV() + amount );
    
    //m_Verts[iV].m_fV += amount;

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: FlipTexU()
// Desc: Flips the corresponding U values
//-----------------------------------------------------------------------------
void CFace::FlipTexU()
{
    TRACE_ENTER_FN( CFace::FlipTexU )

    float temp;

    temp = m_Verts[0].GetTexU();
    m_Verts[0].SetTexU( m_Verts[2].GetTexU() );
    m_Verts[2].SetTexU( temp );

    temp = m_Verts[1].GetTexU();
    m_Verts[1].SetTexU( m_Verts[3].GetTexU() );
    m_Verts[3].SetTexU( temp );

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: FlipTexV()
// Desc: Swaps the corresponding V coords
//-----------------------------------------------------------------------------
void CFace::FlipTexV()
{
    TRACE_ENTER_FN( CFace::FlipTexV )

    float temp;

    temp = m_Verts[0].GetTexV();
    m_Verts[0].SetTexV( m_Verts[2].GetTexV() );
    m_Verts[2].SetTexV( temp );

    temp = m_Verts[1].GetTexV();
    m_Verts[1].SetTexV( m_Verts[3].GetTexV() );
    m_Verts[3].SetTexV( temp );

    /*
    temp = m_Verts[0].m_fV;
    m_Verts[0].m_fV = m_Verts[2].m_fV;
    m_Verts[2].m_fV = temp;

    temp = m_Verts[1].m_fV;
    m_Verts[1].m_fV = m_Verts[3].m_fV;
    m_Verts[3].m_fV = temp;
    */

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: RotateTex90()
// Desc: Rotetes the texture 90 degrees
//-----------------------------------------------------------------------------
void CFace::RotateTex90()
{
    TRACE_ENTER_FN( CFace::RotateTex90 )

    float fTempU0, fTempV0;
    float fTempU1, fTempV1;
    float fTempU2, fTempV2;
    float fTempU3, fTempV3;

    fTempU0 = m_Verts[ 0 ].GetTexU();
    fTempV0 = m_Verts[ 0 ].GetTexV();

    fTempU1 = m_Verts[ 1 ].GetTexU();
    fTempV1 = m_Verts[ 1 ].GetTexV();

    fTempU2 = m_Verts[ 2 ].GetTexU();
    fTempV2 = m_Verts[ 2 ].GetTexV();

    fTempU3 = m_Verts[ 3 ].GetTexU();
    fTempV3 = m_Verts[ 3 ].GetTexV();

    m_Verts[ 0 ].SetTexU( fTempU1 );
    m_Verts[ 0 ].SetTexV( fTempV1 );

    m_Verts[ 1 ].SetTexU( fTempU2 );
    m_Verts[ 1 ].SetTexV( fTempV2 );

    m_Verts[ 2 ].SetTexU( fTempU3 );
    m_Verts[ 2 ].SetTexV( fTempV3 );

    m_Verts[ 3 ].SetTexU( fTempU0 );
    m_Verts[ 3 ].SetTexV( fTempV0 );

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: RotateLightmap90()
// Desc: Rotetes the lightmap 90 degrees
//-----------------------------------------------------------------------------
bool CFace::RotateLightmap90()
{
    TRACE_ENTER_FN( CFace::RotateLightmap90 )

    float fTempU0, fTempV0;
    float fTempU1, fTempV1;
    float fTempU2, fTempV2;
    float fTempU3, fTempV3;

    fTempU0 = m_iU[ 0 ];
    fTempV0 = m_iV[ 0 ];

    fTempU1 = m_iU[ 1 ];
    fTempV1 = m_iV[ 1 ];

    fTempU2 = m_iU[ 2 ];
    fTempV2 = m_iV[ 2 ];

    fTempU3 = m_iU[ 3 ];
    fTempV3 = m_iV[ 3 ];

    m_hLightmap.fU[ 0 ] = fTempU1;
    m_hLightmap.fV[ 0 ] = fTempV1;

    m_hLightmap.fU[ 1 ] = fTempU2;
    m_hLightmap.fV[ 1 ] = fTempV2;

    m_hLightmap.fU[ 2 ] = fTempU3;
    m_hLightmap.fV[ 2 ] = fTempV3;

    m_hLightmap.fU[ 3 ] = fTempU0;
    m_hLightmap.fV[ 3 ] = fTempV0;

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: SetScale()
// Desc: Scales the texture coordinates to the factor
//-----------------------------------------------------------------------------
bool CFace::SetScale( float factor )
{
    TRACE_ENTER_FN( CFace::SetScale )

    if( factor == 0.0f ) LOG_ERR_RET( false )

    for( int iV = 0; iV < GetLoadedVerts(); iV++ )
    {
        m_Verts[ iV ].SetTexU( m_Verts[ iV ].GetTexU() * factor );
        m_Verts[ iV ].SetTexV( m_Verts[ iV ].GetTexV() * factor );
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Updates the face for this frame
//-----------------------------------------------------------------------------
bool CFace::FrameMove( const float fElapsedTime )
{
    TRACE_ENTER_FN( CFace::FrameMove )

    float fTotal = 0;
    int iV;

    // Change U texture coords if enabled
    if( m_bPanU )
        for( int iV = 0; iV < GetLoadedVerts(); iV++ )
            m_Verts[ iV ].SetTexU( m_Verts[ iV ].GetTexU() + fElapsedTime );

    // Change V texture coords if enabled
    if( m_bPanV )
        for( int iV = 0; iV < GetLoadedVerts(); iV++ )
            m_Verts[ iV ].SetTexV( m_Verts[ iV ].GetTexV() + fElapsedTime );

    // Make sure there are enough verticies to obtain a normal
    if( GetLoadedVerts() < 3 ) LOG_ERR_RET( false )

    // Get the polygon's normal
    m_vNormal = Normal( m_Verts[ 0 ].GetVertPos(),
                        m_Verts[ 1 ].GetVertPos(),
                        m_Verts[ 2 ].GetVertPos() );

    // Computes the center of the face
    for( iV = 0; iV < GetLoadedVerts(); iV++ )
        fTotal += m_Verts[ iV ].GetVertPos().x;
    m_vCenter.x = fTotal / GetLoadedVerts(); 
    fTotal = 0;

    for( iV = 0; iV < GetLoadedVerts(); iV++ )
        fTotal += m_Verts[ iV ].GetVertPos().y;
    m_vCenter.y = fTotal / GetLoadedVerts(); 
    fTotal = 0;

    for( iV = 0; iV < GetLoadedVerts(); iV++ )
        fTotal += m_Verts[ iV ].GetVertPos().z;
    m_vCenter.z = fTotal / GetLoadedVerts(); 
    fTotal = 0;

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: TranslateX()
// Desc: 
//-----------------------------------------------------------------------------
bool CFace::TranslateX( const float amount )
{
    TRACE_ENTER_FN( CFace::TranslateX )

    for( int iA = 0; iA < m_iNumVerts; iA++ )
        m_Verts[ iA ].TranslateX( amount );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: TranslateY()
// Desc: 
//-----------------------------------------------------------------------------
bool CFace::TranslateY( const float amount )
{
    TRACE_ENTER_FN( CFace::TranslateY )

    for( int iA = 0; iA < m_iNumVerts; iA++ )
        m_Verts[ iA ].TranslateY( amount );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: TranslateZ()
// Desc: 
//-----------------------------------------------------------------------------
bool CFace::TranslateZ( const float amount )
{
    TRACE_ENTER_FN( CFace::TranslateZ )

    for( int iA = 0; iA < m_iNumVerts; iA++ )
        m_Verts[ iA ].TranslateZ( amount );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Save()
// Desc: 
//-----------------------------------------------------------------------------
bool CFace::Save( const char* File, ofstream& out )
{
    TRACE_ENTER_FN( CFace::Save )

    out << F_BEGIN       << NEWLINE;
    out << F_UNIQUEID    << ' ' << m_UniqueID              << NEWLINE;
    out << F_NAME        << ' ' << m_strName               << NEWLINE;
    out << F_FACETYPE    << ' ' << m_eFaceType             << NEWLINE;
    out << F_NUMVERTS    << ' ' << m_iNumVerts             << NEWLINE;

    out << F_PANU        << ' ' << m_bPanU                 << NEWLINE;
    out << F_PANV        << ' ' << m_bPanV                 << NEWLINE;
    out << F_TRANSLUCENT << ' ' << m_bTranslucent          << NEWLINE;

    out << F_TEX1PATH    << ' ' << m_hTexture1.path        << NEWLINE;
    out << F_TEX2PATH    << ' ' << m_hTexture2.path        << NEWLINE;
//    out << F_TEX3PATH    << ' ' << m_hLightmap.path << NEWLINE;

    out << F_USETEX1     << ' ' << m_bUseTexture1          << NEWLINE;
    out << F_USETEX2     << ' ' << m_bUseTexture2          << NEWLINE;
    out << F_USELIGHTMAP << ' ' << m_bUseLightmap          << NEWLINE;
 //   out << F_LIGHTMAPRES << ' ' << m_hLightmap.iResolution << NEWLINE;

    out << F_UTEXCOORDSU << ' ' << m_iU[ 0 ] << ' ' << m_iU[ 1 ] << ' ' << m_iU[ 2 ] << ' ' << m_iU[ 3 ] << NEWLINE;
    out << F_UTEXCOORDSV << ' ' << m_iV[ 0 ] << ' ' << m_iV[ 1 ] << ' ' << m_iV[ 2 ] << ' ' << m_iV[ 3 ] << NEWLINE;

    out << F_GLFACETYPE  << ' ' << m_eGLFaceType << NEWLINE;

    out << F_LIGHTMAP    << ' ' << m_hLightmap.iLightmapBlockIndex
                         << ' ' << m_hLightmap.iResolution
                         << ' ' << (int)m_hLightmap.BlockSector.bLeftIsOccupied
                         << ' ' << (int)m_hLightmap.BlockSector.bRightIsOccupied
                         << ' ' << m_hLightmap.fU[ 0 ]
                         << ' ' << m_hLightmap.fU[ 1 ]
                         << ' ' << m_hLightmap.fU[ 2 ]
                         << ' ' << m_hLightmap.fU[ 3 ]
                         << ' ' << m_hLightmap.fV[ 0 ]
                         << ' ' << m_hLightmap.fV[ 1 ]
                         << ' ' << m_hLightmap.fV[ 2 ]
                         << ' ' << m_hLightmap.fV[ 3 ]
                         << ' ' << NEWLINE;

/*
    out << F_LIGHTMAP << ' ';

    unsigned char *ucBuf = new unsigned char[ m_iLightmapRes * m_iLightmapRes * 3 ];

    glBindTexture( GL_TEXTURE_2D, m_uiLightmap );                
    glGetTexImage( GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, ucBuf );

    for( int iA = 0; iA < m_iLightmapRes * m_iLightmapRes * 3; iA++ )
        out << (int)ucBuf[iA] << ' ';

    out << NEWLINE;

    delete[] ucBuf;
*/
    out << F_END << NEWLINE;

    for( int iV = 0; iV < GetLoadedVerts(); iV++ )
        m_Verts[iV].Save( File, out );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Open()
// Desc: 
//-----------------------------------------------------------------------------
bool CFace::Open( const char* strFile, ifstream& in )
{
    TRACE_ENTER_FN( CFace::Open )

    int iTemp;
    char strToken[256];

    in >> strToken;

    if( strcmp( F_BEGIN, strToken ) != 0 ) { LOG_VAR( strToken ) LOG_ERR_RET( false ) }

    in >> strToken;

    while( strcmp( F_END, strToken ) != 0 )
    {
        if( !IsFaceToken( strToken ) ) { LOG_VAR( strToken ) LOG_ERR_RET( false ) }

/***********************
VERSION 1.0 TOKENS BEGIN
***********************/
        if( strcmp( F_UNIQUEID, strToken ) == 0 )
        {
            in >> CFace::m_UniqueID;
        }
        else if( strcmp( F_NAME, strToken ) == 0 )
        {
            in >> m_strName;
        }
        else if( strcmp( F_FACETYPE, strToken ) == 0 )
        {
            in >> iTemp;
            m_eFaceType = GetFaceEnumType( iTemp );
        }
        else if( strcmp( F_NUMVERTS, strToken ) == 0 )
        {
            in >> m_iNumVerts;
        }
        else if( strcmp( F_PANU, strToken ) == 0 )
        {
            in >> iTemp;
            m_bPanU = iTemp != 0;
        }
        else if( strcmp( F_PANV, strToken ) == 0 )
        {
            in >> iTemp;
            m_bPanV = iTemp != 0;
        }
        else if( strcmp( F_TRANSLUCENT, strToken ) == 0 )
        {
            in >> iTemp;
            m_bTranslucent = iTemp != 0;
        }
        else if( strcmp( F_TEX1PATH, strToken ) == 0 )
        {
            in >> m_hTexture1.path;
        }
        else if( strcmp( F_TEX2PATH, strToken ) == 0 )
        {
            in >> m_hTexture2.path;
        }
        else if( strcmp( F_USETEX1, strToken ) == 0 )
        {
            in >> iTemp;
            m_bUseTexture1 = iTemp != 0;
        }
        else if( strcmp( F_USETEX2, strToken ) == 0 )
        {
            in >> iTemp;
            m_bUseTexture2 = iTemp != 0;
        }
        else if( strcmp( F_USELIGHTMAP, strToken ) == 0 )
        {
            in >> iTemp;
            m_bUseLightmap = iTemp != 0;
        }
        else if( strcmp( F_UTEXCOORDSU, strToken ) == 0 )
        {
            in >> m_iU[ 0 ] >> m_iU[ 1 ] >> m_iU[ 2 ] >> m_iU[ 3 ];
        }
        else if( strcmp( F_UTEXCOORDSV, strToken ) == 0 )
        {
            in >> m_iV[ 0 ] >> m_iV[ 1 ] >> m_iV[ 2 ] >> m_iV[ 3 ];
        }
        else if( IsSameToken( F_LIGHTMAP, strToken ) )
        {
            int i1, i2;

            in >> m_hLightmap.iLightmapBlockIndex
               >> m_hLightmap.iResolution
               >> i1 >> i2
               >> m_hLightmap.fU[ 0 ] >> m_hLightmap.fU[ 1 ]
               >> m_hLightmap.fU[ 2 ] >> m_hLightmap.fU[ 3 ]
               >> m_hLightmap.fV[ 0 ] >> m_hLightmap.fV[ 1 ]
               >> m_hLightmap.fV[ 2 ] >> m_hLightmap.fV[ 3 ];

            m_hLightmap.BlockSector.bLeftIsOccupied  = i1 != 0;
            m_hLightmap.BlockSector.bRightIsOccupied = i2 != 0;
        }

        else if( IsSameToken( F_GLFACETYPE, strToken ) )
        {
            in >> m_eGLFaceType;
        }
        else
        {
            LOG_ERR() LOG_VAR( strToken )
        }

        in >> strToken;
    }

    m_eClassType = FACECLASS;

    m_hTexture1 = TM->AddTextureFromFile( m_hTexture1.path );
    m_hTexture2 = TM->AddTextureFromFile( m_hTexture2.path );

    for( int iV = 0; iV < GetLoadedVerts(); iV++ )
        m_Verts[ iV ].Open( strFile, in );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: ()
// Desc: 
//-----------------------------------------------------------------------------
VECT CFace::Normal( VECT v1, VECT v2, VECT v3  )
{
    TRACE_ENTER_FN( CFace::Normal )

    VECT vVector1 = v3 - v1;
    VECT vVector2 = v2 - v1;

    VECT vNormal = GetCrossProduct( vVector1, vVector2 );
    vNormal = Normalize( vNormal );

    RET( vNormal );
}


//-----------------------------------------------------------------------------
// Name: ()
// Desc: 
//-----------------------------------------------------------------------------
VECT CFace::GetCrossProduct( VECT vVector1, VECT vVector2 )
{
    TRACE_ENTER_FN( CFace::GetCrossProduct )

    VECT vNormal;

    vNormal.x = ( ( vVector1.y * vVector2.z ) - ( vVector1.z * vVector2.y ) );
    vNormal.y = ( ( vVector1.z * vVector2.x ) - ( vVector1.x * vVector2.z ) );
    vNormal.z = ( ( vVector1.x * vVector2.y ) - ( vVector1.y * vVector2.x ) );

    TRACE_LEAVE_FN()

    return vNormal;
}


//-----------------------------------------------------------------------------
// Name: ()
// Desc: 
//-----------------------------------------------------------------------------
VECT CFace::Normalize( VECT vNormal )
{
    TRACE_ENTER_FN( CFace::Normalize )

    float magnitude = Magnitude( vNormal );

    if( magnitude == 0 ) {
        TRACE_LEAVE_FN() return VECT( 0.1f, 0.1f, 0.1f ); }

    vNormal /= magnitude;
    
    TRACE_LEAVE_FN()

    return vNormal;
}


//-----------------------------------------------------------------------------
// Name: ()
// Desc: 
//-----------------------------------------------------------------------------
VECT CFace::Vector( VECT vPoint1, VECT vPoint2 )
{
    TRACE_ENTER_FN( CFace::Vector )

    VECT vVector( 0, 0, 0);

    vVector.x = vPoint1.x - vPoint2.x;
    vVector.y = vPoint1.y - vPoint2.y;
    vVector.z = vPoint1.z - vPoint2.z;


    TRACE_LEAVE_FN()

    return vVector;
}


//-----------------------------------------------------------------------------
// Name: ()
// Desc: 
//-----------------------------------------------------------------------------
float CFace::Magnitude( VECT vNormal )
{
    TRACE_ENTER_FN( CFace::Magnitude )

    TRACE_LEAVE_FN()

    return (float)sqrt( ( vNormal.x * vNormal.x ) + 
                        ( vNormal.y * vNormal.y ) + 
                        ( vNormal.z * vNormal.z ) );
}


//-----------------------------------------------------------------------------
// Name: SetTranslucent()
// Desc: 
//-----------------------------------------------------------------------------
void CFace::SetTranslucent( const bool bTranslucent )
{ m_bTranslucent = bTranslucent; }




//-----------------------------------------------------------------------------
// Name: GenerateTextureCoordinates()
// Desc: 
// Auth: http://uk.geocities.com/mentalmantle.
// Mail: mentalmantle@hotmail.com
//       Original Language: Visual Basic
//-----------------------------------------------------------------------------
void CFace::GenerateTextureCoordinates()
{
    TRACE_ENTER_FN( CFace::GenerateTextureCoordinates )

    VECT vS(   0.0f, 0.0f, 0.0f ),
         vT(   0.0f, 0.0f, 0.0f ),
         vRef( 0.0f, 0.0f, 0.0f );

    m_vNormal = Normal( m_Verts[0].GetVertPos(),
                        m_Verts[1].GetVertPos(),
                        m_Verts[2].GetVertPos() );

    if( ( m_vNormal.x == 0.0f && m_vNormal.y ==  1.0f && m_vNormal.z == 0.0f ) ||
        ( m_vNormal.x == 0.0f && m_vNormal.y == -1.0f && m_vNormal.z == 0.0f ) )
        vRef.x = 1.0f;
    else
        vRef.y = -1.0f;

    vS = Normalize( GetCrossProduct( m_vNormal, vRef ) ) / 2.0f;
    vT = Normalize( GetCrossProduct( m_vNormal, vS   ) ) / 2.0f;

    for( int y = 0; y < 4; y++ )
    {
        m_Verts[y].SetTexU( ( m_Verts[y].GetVertPos().x * vS.x ) +
                            ( m_Verts[y].GetVertPos().y * vS.y ) +
                            ( m_Verts[y].GetVertPos().z * vS.z ) );

        m_Verts[y].SetTexV( ( m_Verts[y].GetVertPos().x * vT.x ) +
                            ( m_Verts[y].GetVertPos().y * vT.y ) +
                            ( m_Verts[y].GetVertPos().z * vT.z ) );
    }

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: ResetVirtualCoords()
// Desc:
//-----------------------------------------------------------------------------
void CFace::ResetVirtualCoords()
{
    TRACE_ENTER_FN( CFace::ResetVirtualCoords )

    for( int iV = 0; iV < GetLoadedVerts(); iV++ )
        m_Verts[iV].ResetVirtualCoords();

    TRACE_LEAVE_FN()
}

/*
//-----------------------------------------------------------------------------
// Name: CreateLightmap()
// Desc:
//-----------------------------------------------------------------------------
bool CFace::CreateLightmap( const int iRes, const unsigned char* pData )
{
    TRACE_ENTER_FN( CFace::CreateLightmap )

    // Free whatever may be in use
    ReleaseLightmap();

    unsigned char *pTexture = NULL;
    const int iLen = iRes * iRes * 3;

    // Must be a power of 2
    if( iRes & ( iRes - 1 ) ) LOG_ERR_RET( false )

    // Data cant be null
    if( pData == NULL ) LOG_ERR_RET( false )

    // Create memory to hole the texture
    pTexture = new unsigned char[iLen];

    // Be sure it was created
    if( pTexture == NULL ) LOG_ERR_RET( false )

    // Copy the data passed in into the new data holder
    memcpy( pTexture, pData, iLen * sizeof( unsigned char ) );

    // Create the texture
    glGenTextures( 1, &m_uiLightmap );    

    // Bind it
    glBindTexture( GL_TEXTURE_2D, m_uiLightmap );
    
    // COpy the pixel data to the texture
    glTexImage2D( GL_TEXTURE_2D, 0, 3, iRes, iRes,
                  0, GL_RGB, GL_UNSIGNED_BYTE, pTexture );    
    
    // Set stuff for this texture
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // Clean up
    if( pTexture != NULL ) delete[] pTexture;

    RET( true )
}
*/
/*
//-----------------------------------------------------------------------------
// Name: ReleaseLightmap()
// Desc:
//-----------------------------------------------------------------------------
bool CFace::ReleaseLightmap()
{
    TRACE_ENTER_FN( CFace::ReleaseLightmap )

    glDeleteTextures( 1, &m_uiLightmap );

    RET( true )
}
*/

//-----------------------------------------------------------------------------
// Name: PrepareLightmap()
// Desc:
//-----------------------------------------------------------------------------
bool CFace::PrepareLightmap()
{
    TRACE_ENTER_FN( CFace::PrepareLightmap )
/*
    TM->glActiveTextureARB( GL_TEXTURE1_ARB );
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, m_uiLightmap );
*/
    RET( true )
}


//-----------------------------------------------------------------------------
// Name: DisableLightmap()
// Desc:
//-----------------------------------------------------------------------------
void CFace::DisableLightmap()
{
    TRACE_ENTER_FN( CFace::DisableLightmap )

    TM->glActiveTextureARB( GL_TEXTURE1_ARB );
    glDisable( GL_TEXTURE_2D );

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: DisableLightmap()
// Desc:
//-----------------------------------------------------------------------------
int CFace::GetCoordU( const int iIndex )
{
    TRACE_ENTER_FN( CFace::GetCoordU )

    int iU;

    try { iU = m_iU[iIndex]; }

    catch( ... )
    { Log( "catch( ... ) @ CFace::GetCoordU()\n" ); }

    TRACE_LEAVE_FN()

    return iU;
}


//-----------------------------------------------------------------------------
// Name: DisableLightmap()
// Desc:
//-----------------------------------------------------------------------------
int CFace::GetCoordV( const int iIndex )
{
    TRACE_ENTER_FN( CFace::GetCoordV )

    int iV;

    try { iV = m_iV[iIndex]; }

    catch( ... ) { Log( "catch( ... ) @ CFace::GetCoordV()\n" ); }

    TRACE_LEAVE_FN()

    return iV;
}


//-----------------------------------------------------------------------------
// Name: GetFaceNormal()
// Desc:
//-----------------------------------------------------------------------------
VECT CFace::GetFaceNormal()
{
    TRACE_ENTER_FN( CFace::GetFaceNormal )

    VECT vN = Normal( m_Verts[0].GetVertPos(),
                      m_Verts[1].GetVertPos(),
                      m_Verts[2].GetVertPos() );

    TRACE_LEAVE_FN()

    return vN;
}


//-----------------------------------------------------------------------------
// Name: GetFaceCenter()
// Desc:
//-----------------------------------------------------------------------------
VECT CFace::GetFaceCenter()
{
    TRACE_ENTER_FN( CFace::GetFaceCenter )

    VECT vC( 0.0f, 0.0f, 0.0f );

    if( GetLoadedVerts() <= 0 ) LOG_ERR_RET( VECT( 0.0f, 0.0f, 0.0f ) )

    for( int iV = 0; iV < GetLoadedVerts(); iV++ )
        vC += m_Verts[iV].GetVertPos();

    vC /= GetLoadedVerts();

    TRACE_LEAVE_FN()

    return vC;
}


//-----------------------------------------------------------------------------
// Name: IsCoenciding()
// Desc:
//-----------------------------------------------------------------------------
bool CFace::IsCoenciding( CFace* pF )
{
    TRACE_ENTER_FN( CFace::IsCoenciding )

    int iV1, iV2;
    VECT v1, v2;
    bool bMatch;

    if( pF == NULL ) LOG_ERR_RET( false )

    if( GetLoadedVerts() != pF->GetLoadedVerts() ) RET( false )

    if( strcmp( pF->m_strName, m_strName ) == 0 ) RET( false )

    for( iV1 = 0; iV1 < GetLoadedVerts(); iV1++ )
    {
        v1 = m_Verts[iV1].GetVertPos();
        bMatch = false;

        for( iV2 = 0; iV2 < GetLoadedVerts(); iV2++ )
        {
            v2 = pF->m_Verts[iV2].GetVertPos();

            if( v1 == v2 )
            {
                bMatch = true;
                break;
            }
        }

        if( !bMatch ) RET( false )
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: V()
// Desc:
//-----------------------------------------------------------------------------
CVertex* CFace::V( const int iV )
{
    TRACE_ENTER_FN( CFace::V )

    try{ RET( &m_Verts[ iV ] ) }
    catch( ... ) { LOG_VAR( iV ) LOG_ERR_RET( NULL ) }
}


//-----------------------------------------------------------------------------
// Name: AllocateVertexArrays()
// Desc: 
//-----------------------------------------------------------------------------
bool CFace::AllocateVertexArrays()
{
    TRACE_ENTER_FN( CFace::AllocateVertexArrays() )

    try { if( m_pfFaceVerts     != NULL ) delete[] m_pfFaceVerts;     } catch( ... ) { LOG_ERR() }
    try { if( m_pfFaceColors    != NULL ) delete[] m_pfFaceColors;    } catch( ... ) { LOG_ERR() }
    try { if( m_pfFaceTexCoords != NULL ) delete[] m_pfFaceTexCoords; } catch( ... ) { LOG_ERR() }

    m_pfFaceVerts     = new float[ GetLoadedVerts() * 3 ];
    m_pfFaceColors    = new float[ GetLoadedVerts() * 3 ];
    m_pfFaceTexCoords = new float[ GetLoadedVerts() * 2 ];

    if( m_pfFaceVerts     == NULL ) { LOG_VAR( GetLoadedVerts() ) LOG_ERR_RET( false ) }
    if( m_pfFaceColors    == NULL ) { LOG_VAR( GetLoadedVerts() ) LOG_ERR_RET( false ) }
    if( m_pfFaceTexCoords == NULL ) { LOG_VAR( GetLoadedVerts() ) LOG_ERR_RET( false ) }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Rebuild()
// Desc: 
//-----------------------------------------------------------------------------
bool CFace::Rebuild()
{
    TRACE_ENTER_FN( CFace::Rebuild() )

    int iV = -1;

    if( !AllocateVertexArrays() ) LOG_ERR_RET( false )

    for( iV = 0; iV < GetLoadedVerts(); iV++ )
    {
        if( !V( iV )->Rebuild() ) LOG_ERR_RET( false )

        m_pfFaceVerts[ iV * 3 + 0 ] = V( iV )->m_fPos[ 0 ];
        m_pfFaceVerts[ iV * 3 + 1 ] = V( iV )->m_fPos[ 1 ];
        m_pfFaceVerts[ iV * 3 + 2 ] = V( iV )->m_fPos[ 2 ];

        m_pfFaceColors[ iV * 3 + 0 ] = V( iV )->m_fCol[ 0 ];
        m_pfFaceColors[ iV * 3 + 1 ] = V( iV )->m_fCol[ 1 ];
        m_pfFaceColors[ iV * 3 + 2 ] = V( iV )->m_fCol[ 2 ];

        m_pfFaceTexCoords[ iV * 2 + 0 ] = V( iV )->m_fTexUV[ 0 ];
        m_pfFaceTexCoords[ iV * 2 + 1 ] = V( iV )->m_fTexUV[ 1 ];
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: GetVertexArray()
// Desc: 
//-----------------------------------------------------------------------------
float* CFace::GetVertexArray()
{
    TRACE_ENTER_FN( CFace::GetVertexArray() )

    if( m_pfFaceVerts == NULL )
    {
        LOG_ERR()

        if( !Rebuild() ) { LOG_ERR() RET( NULL ) }
    }

    RET( m_pfFaceVerts )
}


//-----------------------------------------------------------------------------
// Name: GetColorArray()
// Desc: 
//-----------------------------------------------------------------------------
float* CFace::GetColorArray()
{
    TRACE_ENTER_FN( CFace::GetColorArray() )

    if( m_pfFaceColors == NULL )
    {
        LOG_ERR()

        if( !Rebuild() ) { LOG_ERR() RET( NULL ) }
    }

    RET( m_pfFaceColors )
}


//-----------------------------------------------------------------------------
// Name: GetTexCoordArray()
// Desc: 
//-----------------------------------------------------------------------------
float* CFace::GetTexCoordArray()
{
    TRACE_ENTER_FN( CFace::GetTexCoordArray() )

    if( m_pfFaceTexCoords == NULL )
    {
        LOG_ERR()

        if( !Rebuild() ) { LOG_ERR() RET( NULL ) }
    }

    RET( m_pfFaceTexCoords )
}
