//----------------------------------------------------------------------------- 
// File: Map.cpp
//
// Desc: Holds the map data
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Include
#include <windows.h>
#include <fstream>
#include <stdio.h>
#include "Map.h"
#include "Tracer.h"
#include "SaveTokens.h"
#include "enum.h"
#include "Camera.h"
#include "Engine.h"
#include "PolygonManager.h"
//-----------------------------------------------------------------------------

using namespace std;
//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
CMap::CMap()
{
    TRACE_ENTER_FN( CMap::CMap )
/*
    Log( "CMap::CMap()\n" );

    m_pfVerts      = NULL;
    m_pfTexCoords  = NULL;
    m_pfLightmapUV = NULL;
    m_pfColor      = NULL;
    m_sFaces       = NULL;
    m_iNumFaces    = 0;
    m_iNumVerts    = 0;
    */

    TRACE_LEAVE_FN()
}

CMap::~CMap()
{
    TRACE_ENTER_FN( CMap::~CMap )
/*
    Log( "CMap::~CMap()\n" );

    Release();
*/
    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: Release()
// Desc: This has been acting up and I can't figure out why, namely the
//       'delete[] m_pfLightmapUV' line has been crashing.
//-----------------------------------------------------------------------------
void CMap::Release()
{
    TRACE_ENTER_FN( CMap::Release )
/*
    int i;

    try
    {
        try
        {
            if( GetNumFaces() != 0 )
            {
                for( i = 0; i < GetNumFaces() - 1; i++ );
                    glDeleteTextures( 1, &m_sFaces[i].m_piLightmap );
            }
        }
        catch( ... ) { LOG_ERR() LOG_VAR( GetNumFaces() ) LOG_VAR( i ) }

        try { if( m_pfVerts      != NULL ) delete[] m_pfVerts;      } catch( ... ) { LOG_ERR() }
        try { if( m_pfTexCoords  != NULL ) delete[] m_pfTexCoords;  } catch( ... ) { LOG_ERR() }
        try { if( m_pfLightmapUV != NULL ) delete[] m_pfLightmapUV; } catch( ... ) { LOG_ERR() }
        try { if( m_pfColor      != NULL ) delete[] m_pfColor;      } catch( ... ) { LOG_ERR() }
        try { if( m_sFaces       != NULL ) delete[] m_sFaces;       } catch( ... ) { LOG_ERR() }

        m_iNumFaces = 0;
        m_iNumVerts = 0;

    } catch( ... ) { LOG_ERR() }
*/
    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: Open()
// Desc: 
//-----------------------------------------------------------------------------
bool CMap::Init()
{
    TRACE_ENTER_FN( CMap::Init )
/*
    try
    {
        Release();

        int iP, iF, iV,
            iTotalFaces    = 0,
            iTotalVerts    = 0,
            iCurVert       = 0,
            iCurColor      = 0,
            iCurTexCoord   = 0,
            iCurLightmapUV = 0,
            iCurFace       = 0;

        const int iNumPolys  = PM->GetLoadedPolygons();

        for( iP = 0; iP < iNumPolys; iP++ )
        {
            int iNumFaces = PM->P( iP )->GetLoadedFaces();

            iTotalFaces += iNumFaces;

            for( iF = 0; iF < iNumFaces; iF++ )
            {
                iTotalVerts += PM->F( iP, iF )->GetLoadedVerts();
            }
        }

        SetNumFaces( iTotalFaces );
        SetNumVerts( iTotalVerts );

        m_pfVerts          = new float[ GetNumVerts() * 3 ];
        m_pfColor          = new float[ GetNumVerts() * 3 ];
        m_pfTexCoords      = new float[ GetNumVerts() * 2 ];
        m_pfLightmapUV     = new float[ GetNumVerts() * 2 ];
        m_sFaces           = new FACE[  GetNumFaces()     ];

        for( int a = 0; a < GetNumFaces(); a++ )
            m_sFaces[ a ].m_bRender = false;

        // Make sure they were created alright
        if( m_pfVerts      == NULL ) { LOG_ERR_RET( false ) }
        if( m_pfColor      == NULL ) { LOG_ERR_RET( false ) }
        if( m_pfTexCoords  == NULL ) { LOG_ERR_RET( false ) }
        if( m_pfLightmapUV == NULL ) { LOG_ERR_RET( false ) }
        if( m_sFaces       == NULL ) { LOG_ERR_RET( false ) }

        for( a = 0; a < GetNumVerts() * 2; a++ )
            m_pfTexCoords[a] = -999;

        for( iP = 0; iP < iNumPolys; iP++ )
        {
            int iNumFaces = PM->P( iP )->GetLoadedFaces();

            for( iF = 0; iF < iNumFaces; iF++ )
            {
                CFace* pF = PM->F( iP, iF );

                m_sFaces[ iCurFace ].m_phTexture  = pF->GetTexture1();
                m_sFaces[ iCurFace ].m_piLightmap = pF->GetLightmap().id;
                m_sFaces[ iCurFace ].m_eFaceType  = GL_TRIANGLES;

                m_sFaces[ iCurFace ].m_iBeginIndex = iCurVert;
                m_sFaces[ iCurFace ].m_iEndIndex = 9;

                m_sFaces[ iCurFace ].m_bRender = true;

                for( iV = 0; iV < pF->GetLoadedVerts(); iV++ )
                {
                    CVertex* pV = PM->V( iP, iF, iV );

                    m_pfVerts[ iCurVert + 0 ] = pV->GetVertPos().x;
                    m_pfVerts[ iCurVert + 1 ] = pV->GetVertPos().y;
                    m_pfVerts[ iCurVert + 2 ] = pV->GetVertPos().z;

                    m_pfColor[ iCurColor + 0 ] = pV->GetColor().x;
                    m_pfColor[ iCurColor + 1 ] = pV->GetColor().y;
                    m_pfColor[ iCurColor + 2 ] = pV->GetColor().z;

                    m_pfTexCoords[ iCurTexCoord + 0 ] = pV->GetTexU();
                    m_pfTexCoords[ iCurTexCoord + 1 ] = pV->GetTexV();

                    iCurVert     += 3;
                    iCurColor    += 3;
                    iCurTexCoord += 2;
                }

                iCurFace++;
            }
        }
    }
    catch( ... ) { MB( "Rebuild Failed" ) LOG_ERR_RET( false ) }
*/
    RET( true )
}

