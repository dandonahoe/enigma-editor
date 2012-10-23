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
#include <fstream>
#include <stdlib.h>
#include "LightmapManager.h"
#include "main.h"
#include "enum.h"
#include "util.h"
#include "tracer.h"
#include "SaveTokens.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
CLightmapManager::CLightmapManager() :

LIGHTMAP_WIDTH( 8 ), LIGHTMAP_HEIGHT( 8 ),
BLOCK_WIDTH( 256 ),  BLOCK_HEIGHT( 256 )

{
    TRACE_ENTER_FN( CLightmapManager::CLightmapManager() )

    Log( "CLightmapManager::CLightmapManager()\n" );

    m_iNumLightmapBlocks = 0;
    m_pLightmapBlocks    = NULL;

    for( int iW = 0; iW < 32; iW++ )
    {
        for( int iH = 0; iH < 32; iH++ )
        {
            m_OccupancyGrid[ iW ][ iH ].bLeftIsOccupied = false;
            m_OccupancyGrid[ iW ][ iH ].bRightIsOccupied = false;
        }
    }

    TRACE_LEAVE_FN()
}

CLightmapManager::~CLightmapManager()
{
    TRACE_ENTER_FN( CLightmapManager::~CLightmapManager() )

    Log( "CLightmapManager::~CLightmapManager()\n" );

    if( !Release() ) LOG_ERR()

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: Init()
// Desc:
//-----------------------------------------------------------------------------
bool CLightmapManager::Init()
{
    TRACE_ENTER_FN( CLightmapManager::Init() )

    Log( "CLightmapManager::Init()\n" );

    if( !Release() ) LOG_ERR_RET( false )

    const int iLen = BLOCK_WIDTH * BLOCK_HEIGHT * 3;
    unsigned char* pBlock;
    
    int iA;

    m_pLightmapBlocks = new unsigned int[ 1 ];

    m_pLightmapBlocks[ 0 ] = 0;

    if( m_pLightmapBlocks == NULL ) LOG_ERR_RET( false )

    try
    {
        pBlock = new unsigned char[ iLen ];
        
        if( pBlock == NULL ) LOG_ERR_RET( false )

        for( iA = 0; iA < iLen; iA += 3 )
        {
            pBlock[ iA + 0 ] = (unsigned char) 255;//( ( iA / 3 ) % BLOCK_WIDTH );//(unsigned char)255;
            pBlock[ iA + 1 ] = (unsigned char) 255;//( ( iA / 3 ) % BLOCK_WIDTH );
            pBlock[ iA + 2 ] = (unsigned char) 255;//( ( iA / 3 ) % BLOCK_WIDTH );
        }
        pBlock[ 0 ] = 255;
        pBlock[ 1 ] = 0;
        pBlock[ 2 ] = 0;

        pBlock[ iLen - 3 ] = 255;
        pBlock[ iLen - 2 ] = 0;
        pBlock[ iLen - 1 ] = 0;

        if( !CreateLightmap( pBlock, 0 ) )
        {
            SAFE_DELETE( pBlock )
            LOG_ERR_RET( false )
        }

        SAFE_DELETE( pBlock )

    }
    catch( ... )
    {
        SAFE_DELETE( pBlock )
        LOG_ERR_RET( false )
    }

    m_iNumLightmapBlocks++;

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Release()
// Desc:
//-----------------------------------------------------------------------------
bool CLightmapManager::Release()
{
    TRACE_ENTER_FN( CLightmapManager::Release() )

    Log( "CLightmapManager::Release()\n" );

    int iIndex;

    for( iIndex = 0; iIndex < GetNumLightmapBlocks(); iIndex++ )
        if( !ReleaseLightmap( iIndex ) ) LOG_ERR()

    if( m_pLightmapBlocks != NULL )
        delete[] m_pLightmapBlocks;

    m_pLightmapBlocks    = NULL;
    m_iNumLightmapBlocks = 0;

    for( int iW = 0; iW < 32; iW++ )
    {
        for( int iH = 0; iH < 32; iH++ )
        {
            m_OccupancyGrid[ iW ][ iH ].bLeftIsOccupied  = false;
            m_OccupancyGrid[ iW ][ iH ].bRightIsOccupied = false;
        }
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Invalidate()
// Desc:
//-----------------------------------------------------------------------------
bool CLightmapManager::Invalidate( LIGHTMAP* pLightmap )
{
    TRACE_ENTER_FN( CLightmapManager::Invalidate() )

    int iIndex;

    pLightmap->BlockSector.bLeftIsOccupied  = false;
    pLightmap->BlockSector.bRightIsOccupied = false;
    pLightmap->iLightmapBlockIndex          = -1;
    pLightmap->iResolution                  = 0;

    for( iIndex = 0; iIndex < 4; iIndex++ )
        pLightmap->fU[ iIndex ] = pLightmap->fV[ iIndex ] = 0.0f;

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Open()
// Desc:
//-----------------------------------------------------------------------------
bool CLightmapManager::Open( const char* strFile, ifstream &in )
{
    TRACE_ENTER_FN( CLightmapManager::Open() )

    char strToken[ 256 ];

    in >> strToken;

    if( strcmp( LMM_BLOCK, strToken ) == 0 )
    {
        int iTemp;

        for( int a = 0; a < BLOCK_WIDTH * BLOCK_HEIGHT * 3; a++ )
        {
            in >> iTemp;
        }
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Save()
// Desc:
//-----------------------------------------------------------------------------
bool CLightmapManager::Save( const char* strFile, ofstream &out )
{
    TRACE_ENTER_FN( CLightmapManager::Save() )

    const int iLen = BLOCK_WIDTH * BLOCK_HEIGHT * 3;
    unsigned char* pPic;
    pPic = new unsigned char[ iLen ];

    if( pPic == NULL ) LOG_ERR_RET( false )

    for( int e = 0; e < GetNumLightmapBlocks(); e++ )
    {
        glBindTexture( GL_TEXTURE_2D, m_pLightmapBlocks[ e ] );                
        glGetTexImage( GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pPic ); 

        out << LMM_BLOCK << ' ';

        for( int a = 0; a < iLen; a++ )
        {
            out << (int)pPic[ a ];

            if( a != BLOCK_WIDTH * BLOCK_HEIGHT * 3 - 1 )
                out << ' ';
        }
    
        out << NEWLINE;
    }

    delete[] pPic;

    RET( true )
}

//-----------------------------------------------------------------------------
// Name: CreateLightmap()
// Desc:
//-----------------------------------------------------------------------------
bool CLightmapManager::CreateLightmap( const unsigned char* pData, const int iIndex )
{
    TRACE_ENTER_FN( CLightmapManager::CreateLightmap )

    // Free whatever may be in use
   // if( !ReleaseLightmap( iIndex ) ) LOG_ERR_RET( false )

    unsigned char *pTexture = NULL;
    const int iLen = BLOCK_WIDTH * BLOCK_HEIGHT * 3;

    // Must be a power of 2
    //if( BLOCK_WIDTH & ( BLOCK_WIDTH - 1 ) ) LOG_ERR_RET( false )

    // Data cant be null
    if( pData == NULL ) LOG_ERR_RET( false )

    // Create memory to hole the texture
    pTexture = new unsigned char[ iLen ];

    // Be sure it was created
    if( pTexture == NULL ) LOG_ERR_RET( false )

    // Copy the data passed in into the new data holder
    memcpy( pTexture, pData, iLen * sizeof( unsigned char ) );

    // Create the texture
    glGenTextures( 1, &m_pLightmapBlocks[ iIndex ] );    

    // Bind it
    glBindTexture( GL_TEXTURE_2D, m_pLightmapBlocks[ iIndex ] );
    
  //  glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 0, 0, BLOCK_WIDTH, BLOCK_HEIGHT, 0 );

    // COpy the pixel data to the texture
    glTexImage2D( GL_TEXTURE_2D, 0, 3, BLOCK_WIDTH, BLOCK_HEIGHT,
                  0, GL_RGB, GL_UNSIGNED_BYTE, pTexture );     
    
    // Set stuff for this texture
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // Clean up
    if( pTexture != NULL ) delete[] pTexture;

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: ReleaseLightmap()
// Desc:
//-----------------------------------------------------------------------------
bool CLightmapManager::ReleaseLightmap( const int iIndex )
{
    TRACE_ENTER_FN( CLightmapManager::ReleaseLightmap )

    glDeleteTextures( 1, &m_pLightmapBlocks[ iIndex ] );

    RET( true )
}