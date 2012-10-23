//----------------------------------------------------------------------------- 
// File: Grid.cpp
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
#include "Grid.h"
#include "Settings.h"
#include "Tracer.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
CGrid::CGrid()
{
    TRACE_ENTER_FN( CGrid::CGrid() )

    Log( "CGrid::CGrid()\n" );

    m_pfGridVerticies3D = NULL;
    m_pfGridVerticiesXZ = NULL;
    m_pfGridVerticiesXY = NULL;
    m_pfGridVerticiesYZ = NULL;

    m_pfGridColors3D = NULL;
    m_pfGridColorsXZ = NULL;
    m_pfGridColorsXY = NULL;
    m_pfGridColorsYZ = NULL;

    m_iNumVert3D = 0;
    m_iNumVertXZ = 0;
    m_iNumVertXY = 0;
    m_iNumVertYZ = 0;

    SetEnable3D( false );
    SetEnableXZ( false );
    SetEnableXY( false );
    SetEnableYZ( false );

    m_GridInfo.iMajorLineSpacing = 3;
    m_GridInfo.iIncrementAmount  = 5;

    m_GridInfo.iSizeX = 75; 
    m_GridInfo.iSizeY = 75;
    m_GridInfo.iSizeZ = 75;

    m_GridInfo.fOrgR = 0.75f;
    m_GridInfo.fOrgG = 0.0f;
    m_GridInfo.fOrgB = 0.0f;

    m_GridInfo.fMajR = 0.0f;
    m_GridInfo.fMajG = 0.0f;
    m_GridInfo.fMajB = 0.75f;

    m_GridInfo.fMinR = 0.0f;
    m_GridInfo.fMinG = 0.0f;
    m_GridInfo.fMinB = 0.375f;

    TRACE_LEAVE_FN()
}

CGrid::~CGrid()
{
    TRACE_ENTER_FN( CGrid::~CGrid() )

    Log( "CGrid::~CGrid()\n" );

    if( !Release() ) { LOG_ERR() }

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: Init()
// Desc: The vertex arrays are loaded with their information. A given view's
//       rendering capability is only enables if it was loaded successfuly.
//-----------------------------------------------------------------------------
bool CGrid::Init()
{
    TRACE_ENTER_FN( CGrid::Init() )

    if( !Release() ) { LOG_ERR() }

    if( !LoadArrays3D() ) { LOG_ERR_RET( false ) } SetEnable3D( true );
    if( !LoadArraysXZ() ) { LOG_ERR_RET( false ) } SetEnableXZ( true );
    if( !LoadArraysXY() ) { LOG_ERR_RET( false ) } SetEnableXY( true );
    if( !LoadArraysYZ() ) { LOG_ERR_RET( false ) } SetEnableYZ( true );

    TRACE_LEAVE_FN() return true;
}


//-----------------------------------------------------------------------------
// Name: Release()
// Desc: 
//-----------------------------------------------------------------------------
bool CGrid::Release()
{
    TRACE_ENTER_FN( CMap::Release() )

    try
    {
        try { if( m_pfGridVerticies3D != NULL ) delete[] m_pfGridVerticies3D; } catch( ... ) { LOG_ERR() }
        try { if( m_pfGridVerticiesXZ != NULL ) delete[] m_pfGridVerticiesXZ; } catch( ... ) { LOG_ERR() }
        try { if( m_pfGridVerticiesXY != NULL ) delete[] m_pfGridVerticiesXY; } catch( ... ) { LOG_ERR() }
        try { if( m_pfGridVerticiesYZ != NULL ) delete[] m_pfGridVerticiesYZ; } catch( ... ) { LOG_ERR() }
      
        try { if( m_pfGridColors3D != NULL ) delete[] m_pfGridColors3D; } catch( ... ) { LOG_ERR() }
        try { if( m_pfGridColorsXZ != NULL ) delete[] m_pfGridColorsXZ; } catch( ... ) { LOG_ERR() }
        try { if( m_pfGridColorsXY != NULL ) delete[] m_pfGridColorsXY; } catch( ... ) { LOG_ERR() }
        try { if( m_pfGridColorsYZ != NULL ) delete[] m_pfGridColorsYZ; } catch( ... ) { LOG_ERR() }
        
        m_pfGridVerticies3D = NULL;
        m_pfGridVerticiesXZ = NULL;
        m_pfGridVerticiesXY = NULL;
        m_pfGridVerticiesYZ = NULL;

        m_pfGridColors3D = NULL;
        m_pfGridColorsXZ = NULL;
        m_pfGridColorsXY = NULL;
        m_pfGridColorsYZ = NULL;

        m_iNumVert3D = 0;
        m_iNumVertXZ = 0;
        m_iNumVertXY = 0;
        m_iNumVertYZ = 0;

        m_GridInfo.iMajorLineSpacing = 3;
        m_GridInfo.iIncrementAmount  = 5;

        m_GridInfo.iSizeX = 75; 
        m_GridInfo.iSizeY = 75;
        m_GridInfo.iSizeZ = 75;

        m_GridInfo.fOrgR = 0.75f;
        m_GridInfo.fOrgG = 0.0f;
        m_GridInfo.fOrgB = 0.0f;

        m_GridInfo.fMajR = 0.0f;
        m_GridInfo.fMajG = 0.0f;
        m_GridInfo.fMajB = 0.75f;

        m_GridInfo.fMinR = 0.0f;
        m_GridInfo.fMinG = 0.0f;
        m_GridInfo.fMinB = 0.375f;

    } catch( ... ) { LOG_ERR_RET( false ) }

    TRACE_LEAVE_FN() return true;
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: 
//-----------------------------------------------------------------------------
bool CGrid::Render( const VIEWTYPE eViewType )
{
    TRACE_ENTER_FN( CGrid::Render() )

    switch( eViewType )
    {
    case VIEW_ALL:
        if( !RenderGrid3D() ) { LOG_ERR_RET( false ) }
        if( !RenderGridXZ() ) { LOG_ERR_RET( false ) }
        if( !RenderGridXY() ) { LOG_ERR_RET( false ) }
        if( !RenderGridYZ() ) { LOG_ERR_RET( false ) }
        break;

    case VIEW_3D: if( !RenderGrid3D() ) { LOG_ERR_RET( false ) } break;
    case VIEW_XZ: if( !RenderGridXZ() ) { LOG_ERR_RET( false ) } break;
    case VIEW_XY: if( !RenderGridXY() ) { LOG_ERR_RET( false ) } break;
    case VIEW_YZ: if( !RenderGridYZ() ) { LOG_ERR_RET( false ) } break;

    default: LOG_ERR_RET( false ) break;   
    }

    TRACE_LEAVE_FN() return true;
}


//-----------------------------------------------------------------------------
// Name: LoadArrays3D()
// Desc: 
//-----------------------------------------------------------------------------
bool CGrid::LoadArrays3D()
{
    TRACE_ENTER_FN( CGrid::LoadArrays3D() )

    int iNumVerts = 0,
        iCurVert  = 0,
        iZ,
        iX;

    VECT vC;

    // Count up how many verticies there are going to be
    for( iZ = -m_GridInfo.iSizeZ; iZ < m_GridInfo.iSizeZ + 1; iZ += m_GridInfo.iIncrementAmount, iNumVerts += 2 ) { }
    for( iX = -m_GridInfo.iSizeX; iX < m_GridInfo.iSizeX + 1; iX += m_GridInfo.iIncrementAmount, iNumVerts += 2 ) { }

    // Two more for the vertical line in the origin
    iNumVerts += 2;

    m_iNumVert3D = iNumVerts;

    // Make space to put the verts
    m_pfGridVerticies3D = new float[ m_iNumVert3D * 3 ];
    m_pfGridColors3D    = new float[ m_iNumVert3D * 3 ];

    // This runs the old routine that would draw the grid, but instead not fills
    // the arrays with the vertex data
    for( iZ = -m_GridInfo.iSizeZ; iZ < m_GridInfo.iSizeZ + 1; iZ += m_GridInfo.iIncrementAmount, iCurVert += 6 )
    {
        if( iZ == 0.0f )
            vC = VECT( m_GridInfo.fOrgR, m_GridInfo.fOrgG, m_GridInfo.fOrgB );
        else if( iZ % m_GridInfo.iMajorLineSpacing == 0.0f )
            vC = VECT( m_GridInfo.fMajR, m_GridInfo.fMajG, m_GridInfo.fMajB );
        else
            vC = VECT( m_GridInfo.fMinR, m_GridInfo.fMinG, m_GridInfo.fMinB );

        m_pfGridVerticies3D[ iCurVert + 0 ] = -m_GridInfo.iSizeX;
        m_pfGridVerticies3D[ iCurVert + 1 ] = 0.0f;
        m_pfGridVerticies3D[ iCurVert + 2 ] = iZ;
        m_pfGridVerticies3D[ iCurVert + 3 ] = m_GridInfo.iSizeX;
        m_pfGridVerticies3D[ iCurVert + 4 ] = 0.0f;
        m_pfGridVerticies3D[ iCurVert + 5 ] = iZ;
        
        m_pfGridColors3D[ iCurVert + 0 ] = vC.x;
        m_pfGridColors3D[ iCurVert + 1 ] = vC.y;
        m_pfGridColors3D[ iCurVert + 2 ] = vC.z;
        m_pfGridColors3D[ iCurVert + 3 ] = vC.x;
        m_pfGridColors3D[ iCurVert + 4 ] = vC.y;
        m_pfGridColors3D[ iCurVert + 5 ] = vC.z;

    }

    for( iX = -m_GridInfo.iSizeX; iX < m_GridInfo.iSizeX + 1; iX += m_GridInfo.iIncrementAmount, iCurVert += 6 )
    {
        if( iX == 0.0f )
            vC = VECT( m_GridInfo.fOrgR, m_GridInfo.fOrgG, m_GridInfo.fOrgB );
        else if( iX % m_GridInfo.iMajorLineSpacing == 0.0f )
            vC = VECT( m_GridInfo.fMajR, m_GridInfo.fMajG, m_GridInfo.fMajB );
        else
            vC = VECT( m_GridInfo.fMinR, m_GridInfo.fMinG, m_GridInfo.fMinB );

        m_pfGridVerticies3D[ iCurVert + 0 ] = iX;
        m_pfGridVerticies3D[ iCurVert + 1 ] = 0.0f;
        m_pfGridVerticies3D[ iCurVert + 2 ] = -m_GridInfo.iSizeZ;
        m_pfGridVerticies3D[ iCurVert + 3 ] = iX;
        m_pfGridVerticies3D[ iCurVert + 4 ] = 0.0f;
        m_pfGridVerticies3D[ iCurVert + 5 ] = m_GridInfo.iSizeZ;

        m_pfGridColors3D[ iCurVert + 0 ] = vC.x;
        m_pfGridColors3D[ iCurVert + 1 ] = vC.y;
        m_pfGridColors3D[ iCurVert + 2 ] = vC.z;
        m_pfGridColors3D[ iCurVert + 3 ] = vC.x;
        m_pfGridColors3D[ iCurVert + 4 ] = vC.y;
        m_pfGridColors3D[ iCurVert + 5 ] = vC.z;
    }

    vC = VECT( m_GridInfo.fOrgR, m_GridInfo.fOrgG, m_GridInfo.fOrgB );

    m_pfGridVerticies3D[ iCurVert + 0 ] = 0.0f;
    m_pfGridVerticies3D[ iCurVert + 1 ] = m_GridInfo.iSizeY;
    m_pfGridVerticies3D[ iCurVert + 2 ] = 0.0f;
    m_pfGridVerticies3D[ iCurVert + 3 ] = 0.0f;
    m_pfGridVerticies3D[ iCurVert + 4 ] = -m_GridInfo.iSizeY;
    m_pfGridVerticies3D[ iCurVert + 5 ] = 0.0f;

    m_pfGridColors3D[ iCurVert + 0 ] = vC.x;
    m_pfGridColors3D[ iCurVert + 1 ] = vC.y;
    m_pfGridColors3D[ iCurVert + 2 ] = vC.z;
    m_pfGridColors3D[ iCurVert + 3 ] = vC.x;
    m_pfGridColors3D[ iCurVert + 4 ] = vC.y;
    m_pfGridColors3D[ iCurVert + 5 ] = vC.z;

    TRACE_LEAVE_FN() return true;
}


//-----------------------------------------------------------------------------
// Name: LoadArraysXZ()
// Desc: 
//-----------------------------------------------------------------------------
bool CGrid::LoadArraysXZ()
{
    TRACE_ENTER_FN( CGrid::LoadArraysXZ() )

    int iNumVerts = 0,
        iCurVert  = 0,
        iZ,
        iX;

    VECT vC;

    for( iZ = -m_GridInfo.iSizeZ; iZ < m_GridInfo.iSizeZ + 1; iZ++, iNumVerts += 2 ) { }
    for( iX = -m_GridInfo.iSizeX; iX < m_GridInfo.iSizeX + 1; iX++, iNumVerts += 2 ) { }

    m_iNumVertXZ = iNumVerts;

    // Make space to put the verts
    m_pfGridVerticiesXZ = new float[ m_iNumVertXZ * 3 ];
    m_pfGridColorsXZ    = new float[ m_iNumVertXZ * 3 ];

    for( iZ = -m_GridInfo.iSizeZ; iZ < m_GridInfo.iSizeZ + 1; iZ++, iCurVert += 6  )
    {
        if( iZ == 0 )
        {
            vC = VECT( m_GridInfo.fOrgR, m_GridInfo.fOrgG, m_GridInfo.fOrgB );
        }
        else if( iZ % m_GridInfo.iMajorLineSpacing == 0 )
        {
            vC = VECT( m_GridInfo.fMajR, m_GridInfo.fMajG, m_GridInfo.fMajB );
        }
        else
        {
            vC = VECT( m_GridInfo.fMinR, m_GridInfo.fMinG, m_GridInfo.fMinB );
        }

        m_pfGridVerticiesXZ[ iCurVert + 0 ] = -m_GridInfo.iSizeX;
        m_pfGridVerticiesXZ[ iCurVert + 1 ] = 0.0f;
        m_pfGridVerticiesXZ[ iCurVert + 2 ] = iZ;
        m_pfGridVerticiesXZ[ iCurVert + 3 ] = m_GridInfo.iSizeX;
        m_pfGridVerticiesXZ[ iCurVert + 4 ] = 0.0f;
        m_pfGridVerticiesXZ[ iCurVert + 5 ] = iZ;

        m_pfGridColorsXZ[ iCurVert + 0 ] = vC.x;
        m_pfGridColorsXZ[ iCurVert + 1 ] = vC.y;
        m_pfGridColorsXZ[ iCurVert + 2 ] = vC.z;
        m_pfGridColorsXZ[ iCurVert + 3 ] = vC.x;
        m_pfGridColorsXZ[ iCurVert + 4 ] = vC.y;
        m_pfGridColorsXZ[ iCurVert + 5 ] = vC.z;
    }

    for( iX = -m_GridInfo.iSizeX; iX < m_GridInfo.iSizeX + 1; iX++, iCurVert += 6 )
    {
        if( iX == 0 )
        {
            vC = VECT( m_GridInfo.fOrgR, m_GridInfo.fOrgG, m_GridInfo.fOrgB );
        }
        else if( iX % m_GridInfo.iMajorLineSpacing == 0 )
        {
            vC = VECT( m_GridInfo.fMajR, m_GridInfo.fMajG, m_GridInfo.fMajB );
        }
        else
        {
            vC = VECT( m_GridInfo.fMinR, m_GridInfo.fMinG, m_GridInfo.fMinB );
        }

        m_pfGridVerticiesXZ[ iCurVert + 0 ] = iX;
        m_pfGridVerticiesXZ[ iCurVert + 1 ] = 0.0f;
        m_pfGridVerticiesXZ[ iCurVert + 2 ] = -m_GridInfo.iSizeZ;
        m_pfGridVerticiesXZ[ iCurVert + 3 ] = iX;
        m_pfGridVerticiesXZ[ iCurVert + 4 ] = 0.0f;
        m_pfGridVerticiesXZ[ iCurVert + 5 ] = m_GridInfo.iSizeZ;

        m_pfGridColorsXZ[ iCurVert + 0 ] = vC.x;
        m_pfGridColorsXZ[ iCurVert + 1 ] = vC.y;
        m_pfGridColorsXZ[ iCurVert + 2 ] = vC.z;
        m_pfGridColorsXZ[ iCurVert + 3 ] = vC.x;
        m_pfGridColorsXZ[ iCurVert + 4 ] = vC.y;
        m_pfGridColorsXZ[ iCurVert + 5 ] = vC.z;
    }

    TRACE_LEAVE_FN() return true;
}


//-----------------------------------------------------------------------------
// Name: LoadArraysXY()
// Desc: 
//-----------------------------------------------------------------------------
bool CGrid::LoadArraysXY()
{
    TRACE_ENTER_FN( CGrid::LoadArraysXY() )

    int iNumVerts = 0,
        iCurVert  = 0,
        iY,
        iX;

    VECT vC;

    for( iY = -m_GridInfo.iSizeY; iY < m_GridInfo.iSizeY + 1; iY++, iNumVerts += 2 ) {}
    for( iX = -m_GridInfo.iSizeX; iX < m_GridInfo.iSizeX + 1; iX++, iNumVerts += 2 ) {}

    m_iNumVertXY = iNumVerts;

    // Make space to put the verts
    m_pfGridVerticiesXY = new float[ m_iNumVertXY * 3 ];
    m_pfGridColorsXY    = new float[ m_iNumVertXY * 3 ];

    for( iY = -m_GridInfo.iSizeY; iY < m_GridInfo.iSizeY + 1; iY++, iCurVert += 6 )
    {
        if( iY == 0 )
            vC = VECT( m_GridInfo.fOrgR, m_GridInfo.fOrgG, m_GridInfo.fOrgB );
        else if( iY % m_GridInfo.iMajorLineSpacing == 0 )
            vC = VECT( m_GridInfo.fMajR, m_GridInfo.fMajG, m_GridInfo.fMajB );
        else
            vC = VECT( m_GridInfo.fMinR, m_GridInfo.fMinG, m_GridInfo.fMinB );

        m_pfGridVerticiesXY[ iCurVert + 0 ] = -m_GridInfo.iSizeY;
        m_pfGridVerticiesXY[ iCurVert + 1 ] = iY;
        m_pfGridVerticiesXY[ iCurVert + 2 ] = 0.0f;
        m_pfGridVerticiesXY[ iCurVert + 3 ] = m_GridInfo.iSizeY;
        m_pfGridVerticiesXY[ iCurVert + 4 ] = iY;
        m_pfGridVerticiesXY[ iCurVert + 5 ] = 0.0f;

        m_pfGridColorsXY[ iCurVert + 0 ] = vC.x;
        m_pfGridColorsXY[ iCurVert + 1 ] = vC.y;
        m_pfGridColorsXY[ iCurVert + 2 ] = vC.z;
        m_pfGridColorsXY[ iCurVert + 3 ] = vC.x;
        m_pfGridColorsXY[ iCurVert + 4 ] = vC.y;
        m_pfGridColorsXY[ iCurVert + 5 ] = vC.z;
    }

    for( iX = -m_GridInfo.iSizeX; iX < m_GridInfo.iSizeX + 1; iX++, iCurVert += 6 )
    {
        if( iX == 0 )
            vC = VECT( m_GridInfo.fOrgR, m_GridInfo.fOrgG, m_GridInfo.fOrgB );
        else if( iX % m_GridInfo.iMajorLineSpacing == 0 )
            vC = VECT( m_GridInfo.fMajR, m_GridInfo.fMajG, m_GridInfo.fMajB );
        else
            vC = VECT( m_GridInfo.fMinR, m_GridInfo.fMinG, m_GridInfo.fMinB );

        m_pfGridVerticiesXY[ iCurVert + 0 ] = iX;
        m_pfGridVerticiesXY[ iCurVert + 1 ] = -m_GridInfo.iSizeY;
        m_pfGridVerticiesXY[ iCurVert + 2 ] = 0.0f;
        m_pfGridVerticiesXY[ iCurVert + 3 ] = iX;
        m_pfGridVerticiesXY[ iCurVert + 4 ] = m_GridInfo.iSizeY;
        m_pfGridVerticiesXY[ iCurVert + 5 ] = 0.0f;

        m_pfGridColorsXY[ iCurVert + 0 ] = vC.x;
        m_pfGridColorsXY[ iCurVert + 1 ] = vC.y;
        m_pfGridColorsXY[ iCurVert + 2 ] = vC.z;
        m_pfGridColorsXY[ iCurVert + 3 ] = vC.x;
        m_pfGridColorsXY[ iCurVert + 4 ] = vC.y;
        m_pfGridColorsXY[ iCurVert + 5 ] = vC.z;
    }

    TRACE_LEAVE_FN() return true;
}


//-----------------------------------------------------------------------------
// Name: LoadArraysYZ()
// Desc: 
//-----------------------------------------------------------------------------
bool CGrid::LoadArraysYZ()
{
    TRACE_ENTER_FN( CGrid::LoadArraysYZ() )

    int iNumVerts = 0,
        iCurVert  = 0,
        iY,
        iZ;

    VECT vC;

    for( iY = -m_GridInfo.iSizeY; iY < m_GridInfo.iSizeY + 1; iY++, iNumVerts += 2 ) { }
    for( iZ = -m_GridInfo.iSizeZ; iZ < m_GridInfo.iSizeZ + 1; iZ++, iNumVerts += 2 ) { }

    m_iNumVertYZ = iNumVerts;

    // Make space to put the verts
    m_pfGridVerticiesYZ = new float[ m_iNumVertYZ * 3 ];
    m_pfGridColorsYZ    = new float[ m_iNumVertYZ * 3 ];

    for( iY = -m_GridInfo.iSizeY; iY < m_GridInfo.iSizeY + 1; iY++, iCurVert += 6 )
    {
        if( iY == 0 )
            vC = VECT( m_GridInfo.fOrgR, m_GridInfo.fOrgG, m_GridInfo.fOrgB );
        else if( iY % m_GridInfo.iMajorLineSpacing == 0 )
            vC = VECT( m_GridInfo.fMajR, m_GridInfo.fMajG, m_GridInfo.fMajB );
        else
            vC = VECT( m_GridInfo.fMinR, m_GridInfo.fMinG, m_GridInfo.fMinB );

        m_pfGridVerticiesYZ[ iCurVert + 0 ] = 0.0f;
        m_pfGridVerticiesYZ[ iCurVert + 1 ] = iY;
        m_pfGridVerticiesYZ[ iCurVert + 2 ] = -m_GridInfo.iSizeZ;
        m_pfGridVerticiesYZ[ iCurVert + 3 ] = 0.0f;
        m_pfGridVerticiesYZ[ iCurVert + 4 ] = iY;
        m_pfGridVerticiesYZ[ iCurVert + 5 ] = m_GridInfo.iSizeZ;

        m_pfGridColorsYZ[ iCurVert + 0 ] = vC.x;
        m_pfGridColorsYZ[ iCurVert + 1 ] = vC.y;
        m_pfGridColorsYZ[ iCurVert + 2 ] = vC.z;
        m_pfGridColorsYZ[ iCurVert + 3 ] = vC.x;
        m_pfGridColorsYZ[ iCurVert + 4 ] = vC.y;
        m_pfGridColorsYZ[ iCurVert + 5 ] = vC.z;

    }

    for( iZ = -m_GridInfo.iSizeZ; iZ < m_GridInfo.iSizeZ + 1; iZ++, iCurVert += 6 )
    {
        if( iZ == 0 )
            vC = VECT( m_GridInfo.fOrgR, m_GridInfo.fOrgG, m_GridInfo.fOrgB );
        else if( iZ % m_GridInfo.iMajorLineSpacing == 0 )
            vC = VECT( m_GridInfo.fMajR, m_GridInfo.fMajG, m_GridInfo.fMajB );
        else
            vC = VECT( m_GridInfo.fMinR, m_GridInfo.fMinG, m_GridInfo.fMinB );

        m_pfGridVerticiesYZ[ iCurVert + 0 ] = 0.0f;
        m_pfGridVerticiesYZ[ iCurVert + 1 ] = -m_GridInfo.iSizeY;
        m_pfGridVerticiesYZ[ iCurVert + 2 ] = iZ;
        m_pfGridVerticiesYZ[ iCurVert + 3 ] = 0.0f;
        m_pfGridVerticiesYZ[ iCurVert + 4 ] = m_GridInfo.iSizeY;
        m_pfGridVerticiesYZ[ iCurVert + 5 ] = iZ;

        m_pfGridColorsYZ[ iCurVert + 0 ] = vC.x;
        m_pfGridColorsYZ[ iCurVert + 1 ] = vC.y;
        m_pfGridColorsYZ[ iCurVert + 2 ] = vC.z;
        m_pfGridColorsYZ[ iCurVert + 3 ] = vC.x;
        m_pfGridColorsYZ[ iCurVert + 4 ] = vC.y;
        m_pfGridColorsYZ[ iCurVert + 5 ] = vC.z;
    }

    TRACE_LEAVE_FN() return true;
}


//-----------------------------------------------------------------------------
// Name: RenderGrid3D()
// Desc: 
//-----------------------------------------------------------------------------
bool CGrid::RenderGrid3D()
{
    TRACE_ENTER_FN( CGrid::RenderGrid3D() )

    if( !m_bEnabled3D ) { TRACE_LEAVE_FN() return true; }

    if( m_pfGridVerticies3D == NULL ) { m_bEnabled3D = false; LOG_ERR_RET( false ) }
    if( m_pfGridColors3D    == NULL ) { m_bEnabled3D = false; LOG_ERR_RET( false ) }

    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, m_pfGridVerticies3D );

    glEnableClientState( GL_COLOR_ARRAY  );
    glColorPointer( 3, GL_FLOAT, 0, m_pfGridColors3D );

    glDrawArrays( GL_LINES, 0, m_iNumVert3D );

    TRACE_LEAVE_FN() return true;
}


//-----------------------------------------------------------------------------
// Name: RenderGridXZ()
// Desc: 
//-----------------------------------------------------------------------------
bool CGrid::RenderGridXZ()
{
    TRACE_ENTER_FN( CGrid::RenderGridXZ() )

    if( !m_bEnabledXZ ) { TRACE_LEAVE_FN() return true; }

    if( m_pfGridVerticiesXZ == NULL ) { m_bEnabledXZ = false; LOG_ERR_RET( false ) }
    if( m_pfGridColorsXZ    == NULL ) { m_bEnabledXZ = false; LOG_ERR_RET( false ) }

    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, m_pfGridVerticiesXZ );

    glEnableClientState( GL_COLOR_ARRAY  );
    glColorPointer( 3, GL_FLOAT, 0, m_pfGridColorsXZ );

    glDrawArrays( GL_LINES, 0, m_iNumVertXZ );

    TRACE_LEAVE_FN() return true;
}


//-----------------------------------------------------------------------------
// Name: RenderGridXY()
// Desc: 
//-----------------------------------------------------------------------------
bool CGrid::RenderGridXY()
{
    TRACE_ENTER_FN( CGrid::RenderGridXY() )

    if( !m_bEnabledXY ) { TRACE_LEAVE_FN() return true; }

    if( m_pfGridVerticiesXY == NULL ) { m_bEnabledXY = false; LOG_ERR_RET( true ) }
    if( m_pfGridColorsXY    == NULL ) { m_bEnabledXY = false; LOG_ERR_RET( true ) }

    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, m_pfGridVerticiesXY );

    glEnableClientState( GL_COLOR_ARRAY  );
    glColorPointer( 3, GL_FLOAT, 0, m_pfGridColorsXY );

    glDrawArrays( GL_LINES, 0, m_iNumVertXY );

    TRACE_LEAVE_FN() return true;
}


//-----------------------------------------------------------------------------
// Name: RenderGridYZ()
// Desc: 
//-----------------------------------------------------------------------------
bool CGrid::RenderGridYZ()
{
    TRACE_ENTER_FN( CGrid::RenderGridYZ() )

    if( !m_bEnabledYZ ) { TRACE_LEAVE_FN() return true; }

    if( m_pfGridVerticiesYZ == NULL ) { m_bEnabledYZ = false; LOG_ERR_RET( true ) }
    if( m_pfGridColorsYZ    == NULL ) { m_bEnabledYZ = false; LOG_ERR_RET( true ) }

    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, m_pfGridVerticiesYZ );

    glEnableClientState( GL_COLOR_ARRAY  );
    glColorPointer( 3, GL_FLOAT, 0, m_pfGridColorsYZ );

    glDrawArrays( GL_LINES, 0, m_iNumVertYZ );

    TRACE_LEAVE_FN() return true;
}