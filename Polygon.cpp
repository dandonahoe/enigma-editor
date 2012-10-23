//----------------------------------------------------------------------------- 
// File: Polygon.cpp
//
// Desc: 
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
#include <D3DX8math.h>
#include <windows.h>
#include <gl\glaux.h>
#include <fstream>
#include <gl\glu.h>
#include <gl\gl.h>
#include <stdio.h>
#include "PolygonManager.h"
#include "SaveTokens.h"
#include "Polygon.h"
#include "Vertex.h"
#include "Tracer.h"
#include "main.h"
#include "Util.h"
//-----------------------------------------------------------------------------


unsigned int CPolygon::m_uiUniqueID = 0;


//-----------------------------------------------------------------------------
// Construction/Destruction
// Sets the class type, and sets all the face pointers to NULL. Faces are
// created dynamically. Then gives the polygon a unique name.
//-----------------------------------------------------------------------------
CPolygon::CPolygon()
{
    TRACE_ENTER_FN( CPolygon::CPolygon )

    m_eClassType      = POLYGONCLASS;
    m_iTotalFaces     = 0;
    m_iLoadedFaces    = 0;
    m_iNumVerts       = 0;
    m_iNumTexSwitches = 0;

    m_pFaces             = NULL;
    m_fpVertexArray      = NULL;
    m_fpColorArray       = NULL;
    m_fpTexCoordArray    = NULL;
    m_pfLightmapCoords   = NULL;
    m_ipTexSwitchArray   = NULL;
    m_ipUniqueTexArray   = NULL;
    m_ipVertexBeginArray = NULL;
    m_ipVertexEndArray   = NULL;

    // Assign a unique name
    sprintf( m_strName, "Polygon:%d", m_uiUniqueID );
    m_uiUniqueID++;

    TRACE_LEAVE_FN()
}

CPolygon::~CPolygon()
{
    TRACE_ENTER_FN( CPolygon::~CPolygon )

    Release();

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: SetName()
// Desc: 
//-----------------------------------------------------------------------------
void CPolygon::SetName( const char* str, bool bAddUniqueID )
{
    TRACE_ENTER_FN( CPolygon::SetName )

    if( str == NULL ) { LOG_ERR() TRACE_LEAVE_FN() return; }

    if( bAddUniqueID )
    {
        sprintf( m_strName, "%s:%d", str, m_uiUniqueID );
        m_uiUniqueID++;

        TRACE_LEAVE_FN() return;
    }

    strcpy( m_strName, str );

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: AllocateFaceSpace()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygon::AllocateFaceSpace( const int uiTotalFaces )
{
    TRACE_ENTER_FN( CPolygon::AllocateFaceSpace )

    Release();

    m_iTotalFaces = uiTotalFaces;

    m_pFaces = new CFace*[ m_iTotalFaces ];

    if( m_pFaces == NULL ) { LOG_VAR( m_iTotalFaces ) LOG_ERR_RET( false ) }

    for( int iF = 0; iF < m_iTotalFaces; iF++ )
        m_pFaces[iF] = NULL;

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Frees all faces that the polygon is controling and sets them to null
//-----------------------------------------------------------------------------
void CPolygon::Release()
{
    TRACE_ENTER_FN( CPolygon::Release )

    int iIndex;

    if( m_pFaces != NULL )
    {
        for( iIndex = 0; iIndex < m_iTotalFaces; iIndex++ )
        {
            try
            {
                // If the face isnt NULL, then delete it and set it to NULL
                if( m_pFaces[ iIndex ] != NULL )
                {
                    delete m_pFaces[ iIndex ];
                    m_pFaces[ iIndex ] = NULL;
                }
            }
            catch( ... ) { LOG_VAR( iIndex ) LOG_ERR() }
        }

        try { if( m_pFaces )             delete[] m_pFaces;             } catch( ... ) { LOG_ERR() }
        try { if( m_fpVertexArray )      delete[] m_fpVertexArray;      } catch( ... ) { LOG_ERR() }
        try { if( m_fpColorArray )       delete[] m_fpColorArray;       } catch( ... ) { LOG_ERR() }
        try { if( m_fpTexCoordArray )    delete[] m_fpTexCoordArray;    } catch( ... ) { LOG_ERR() }
        try { if( m_ipTexSwitchArray )   delete[] m_ipTexSwitchArray;   } catch( ... ) { LOG_ERR() }
        try { if( m_ipUniqueTexArray )   delete[] m_ipUniqueTexArray;   } catch( ... ) { LOG_ERR() }
        try { if( m_ipVertexBeginArray ) delete[] m_ipVertexBeginArray; } catch( ... ) { LOG_ERR() }
        try { if( m_ipVertexEndArray )   delete[] m_ipVertexEndArray;   } catch( ... ) { LOG_ERR() }
        try { if( m_pfLightmapCoords )   delete[] m_pfLightmapCoords;   } catch( ... ) { LOG_ERR() }

        m_pFaces             = NULL;
        m_fpVertexArray      = NULL;
        m_fpColorArray       = NULL;
        m_fpTexCoordArray    = NULL;
        m_ipTexSwitchArray   = NULL;
        m_ipUniqueTexArray   = NULL;
        m_ipVertexBeginArray = NULL;
        m_ipVertexEndArray   = NULL;
        m_pfLightmapCoords   = NULL;
        m_iLoadedFaces       = 0;
        m_iTotalFaces        = 0;
        m_iNumVerts          = 0;
        m_iNumTexSwitches    = 0;
    }

    TRACE_LEAVE_FN()
}

/*
//-----------------------------------------------------------------------------
// Name: NewQuadFace()
// Desc: Alcolates space for a new quad face and sets the coords for it
//       Increments the number of faces
//-----------------------------------------------------------------------------
void CPolygon::NewQuadFace( const VECT &v1, const VECT &v2,
                            const VECT &v3, const VECT &v4 )
{
    TRACE_ENTER_FN( CPolygon::NewQuadFace )

    if( m_iLoadedFaces == m_iTotalFaces )
    {
        MessageBox( HWND_DESKTOP, "if( m_iLoadedFaces == m_iTotalFaces )", "", MB_OK );
        Log( "if( m_iLoadedFaces == m_iTotalFaces ) @ CPolygon::NewQuadFace()\n" );
        Log( "m_iLoadedFaces: " ); Log( m_iLoadedFaces ); Log( "\n" );
        Log( "m_iTotalFaces:" ); Log( m_iTotalFaces );Log( "\n" );
    } 

    m_pFaces[m_iLoadedFaces] = new CFace;
    m_pFaces[m_iLoadedFaces]->SetQuadVerts( v1, v2, v3, v4 );
    m_iLoadedFaces++;

    TRACE_LEAVE_FN()
}
*/

//-----------------------------------------------------------------------------
// Name: NewTriangleFace()
// Desc: Alcolates space for a new triangle face and sets the coords for it.
//       Increments the number of faces
//-----------------------------------------------------------------------------
void CPolygon::NewTriangleFace( const VECT &v1, const VECT &v2, const VECT &v3 )
{
    TRACE_ENTER_FN( CPolygon::NewTriangleFace )

    m_pFaces[m_iLoadedFaces] = new CFace;
    m_pFaces[m_iLoadedFaces]->SetTriangleVerts( v1, v2, v3 );
    m_iLoadedFaces++;

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Updates the current state of the polygon
//-----------------------------------------------------------------------------
void CPolygon::FrameMove( const float fElapsedTime )
{
}


//-----------------------------------------------------------------------------
// Name: TranslateX()
// Desc: 
//-----------------------------------------------------------------------------
void CPolygon::TranslateX( const float fAmount )
{
    TRACE_ENTER_FN( CPolygon::TranslateX )

    for( int a = 0; a < m_iLoadedFaces; a++ )
        m_pFaces[a]->TranslateX( fAmount );

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: TranslateY()
// Desc: 
//-----------------------------------------------------------------------------
void CPolygon::TranslateY( const float fAmount )
{
    TRACE_ENTER_FN( CPolygon::TranslateY )

    for( int a = 0; a < m_iLoadedFaces; a++ )
        m_pFaces[a]->TranslateY( fAmount );

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: TranslateZ()
// Desc: 
//-----------------------------------------------------------------------------
void CPolygon::TranslateZ( const float fAmount )
{
    TRACE_ENTER_FN( CPolygon::TranslateZ )

    for( int a = 0; a < m_iLoadedFaces; a++ )
        m_pFaces[a]->TranslateZ( fAmount );

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: Save()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygon::Save( const char* File, ofstream& out )
{
    TRACE_ENTER_FN( CPolygon::Save )

    out << P_BEGIN       << NEWLINE;
    out << P_NAME        << ' ' << m_strName      << NEWLINE;
    out << P_UNIQUEID    << ' ' << m_uiUniqueID   << NEWLINE;
    out << P_TOTALFACES  << ' ' << m_iTotalFaces  << NEWLINE;
    out << P_LOADEDFACES << ' ' << m_iLoadedFaces << NEWLINE;
    out << P_CLASSTYPE   << ' ' << m_eClassType   << NEWLINE;
    out << P_END         << '\n';

    for( int a = 0; a < GetLoadedFaces(); a++ )
        m_pFaces[a]->Save( File, out );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Open()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygon::Open( const char* File, ifstream& in )
{
    TRACE_ENTER_FN( CPolygon::Open )

    int iTemp;
    char strToken[256];

    in >> strToken;

    if( strcmp( P_BEGIN, strToken ) != 0 ) LOG_ERR_RET( false )

    in >> strToken;

    while( strcmp( P_END, strToken ) != 0 )
    {
        if( !IsPolygonToken( strToken ) ) LOG_ERR_RET( false )

/***********************
VERSION 1.0 TOKENS BEGIN
***********************/
        if( strcmp( P_NAME, strToken ) == 0 )
        {
            in >> m_strName;
        }
        else if( strcmp( P_UNIQUEID, strToken ) == 0 )
        {
            in >> CPolygon::m_uiUniqueID;
        }
        else if( strcmp( P_TOTALFACES, strToken ) == 0 )
        {
            in >> m_iTotalFaces;
        }
        else if( strcmp( P_LOADEDFACES, strToken ) == 0 )
        {
            in >> m_iLoadedFaces;
        }
        else if( strcmp( P_CLASSTYPE, strToken ) == 0 )
        {
            in >> iTemp;
            m_eClassType = GetClassEnumType( iTemp );
        }
        else
        {
            LOG_ERR() LOG_VAR( strToken )
        }

        in >> strToken;
    }

    AllocateFaceSpace( GetLoadedFaces() );

    SetTotalFaces( GetLoadedFaces() );

    SetLoadedFaces( 0 );
    
    for( int a = 0; a < GetTotalFaces(); a++ )
    {
        m_pFaces[a] = new CFace();
        
        if( m_pFaces[a] == NULL ) LOG_ERR_RET( false )

        m_pFaces[a]->Open( File, in );

        m_iLoadedFaces++;
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: RemoveFace()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygon::RemoveFace( int iFace )
{
    TRACE_ENTER_FN( CPolygon::RemoveFace() )

    try
    {
        if( m_pFaces[ iFace ] != NULL )
        {
            delete m_pFaces[ iFace ];
            m_pFaces[ iFace ] = NULL;
        }
    }
    catch( ... ) { LOG_VAR( iFace ) LOG_ERR_RET( false ) }

    if( !ReSortFaceList() ) { LOG_VAR( iFace ) LOG_ERR_RET( false ) }

    m_iLoadedFaces--;

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: ReSortFaceList()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygon::ReSortFaceList()
{
    TRACE_ENTER_FN( CPolygon::ReSortFaceList() )

    int iA = 0;
    int iIndex = 0;
    CFace** pTemp;

    if( m_iLoadedFaces <= 0 ) { LOG_VAR( m_iLoadedFaces ) LOG_ERR_RET( false ) }

    try
    {
        pTemp = new CFace*[ m_iLoadedFaces ];

        if( pTemp == NULL ) LOG_ERR_RET( false )

        for( iA = 0; iA < m_iLoadedFaces; iA++ )
            pTemp[ iA ] = NULL;

        for( iA = 0; iA < m_iLoadedFaces; iA++ )
        {
            if( m_pFaces[ iA ] != NULL )
            {
                pTemp[ iIndex ] = m_pFaces[ iA ];
                iIndex++;
            }
        }

        for( iA = 0; iA < m_iLoadedFaces; iA++ )
            m_pFaces[ iA ] = pTemp[ iA ];

        delete[] pTemp;
    }
    catch( ... ) { LOG_VAR( iA ) LOG_VAR( iIndex ) LOG_VAR( m_iLoadedFaces ) LOG_ERR_RET( false ) }

    RET( true )
}

//-----------------------------------------------------------------------------
// Name: NextFace()
// Desc: Selects the next face of the polygon
//-----------------------------------------------------------------------------
void CPolygon::NextFace()
{
    TRACE_ENTER_FN( CPolygon::NextFace )

    if( !LU->FaceSelected() || ( !LU->BrushSelected() && !LU->PolySelected() ) )
    { TRACE_LEAVE_FN() return; }

    int iFace = LU->GetFace();

    iFace++;

    if( iFace >= m_iLoadedFaces ) iFace = 0;

    LU->SetFace( iFace );

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: F()
// Desc: Returns a pointer to the face 'iFace'. If it does not exist, NULL is
//       returned.
//-----------------------------------------------------------------------------
CFace* CPolygon::F( const int iFace )
{
    TRACE_ENTER_FN( CPolygon::F )

    try { RET( m_pFaces[iFace] ) }

    catch( ... ) { LOG_VAR( iFace ) LOG_VAR( GetLoadedFaces() ) LOG_ERR_RET( NULL ) }
}


//-----------------------------------------------------------------------------
// Name: V()
// Desc: 
//-----------------------------------------------------------------------------
CVertex* CPolygon::V( const int iFace, const int iVert )
{
    TRACE_ENTER_FN( CPolygon::V )
/*
    if( iFace >= GetLoadedFaces() ) {
        Log( "if( iFace >= GetLoadedFaces() ) @ CPolygon::V()\n" );
        LOG_VAR( iFace ) LOG_VAR( iVert )
        TRACE_LEAVE_FN() return NULL; }

    if( iVert >= m_pFaces[iFace]->GetLoadedVerts() ) {
        Log( "if( iVert >= m_pFaces[iFace]->m_NumVerts ) @ CPolygon::V()\n" );
        LOG_VAR( iFace ) LOG_VAR( iVert )
        TRACE_LEAVE_FN() return NULL; }

    CVertex* pVert = m_pFaces[iFace]->V( iVert );

    TRACE_LEAVE_FN()

*/
    try { RET( m_pFaces[iFace]->V( iVert ) ) }

    catch( ... ) { LOG_VAR( iFace ) LOG_VAR( iVert ) LOG_ERR_RET( NULL ) }

  //  return pVert;
}


//-----------------------------------------------------------------------------
// Name: SetF()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygon::SetF( const int iIndex, CFace* pFace )
{
    TRACE_ENTER_FN( CPolygon::SetF )
/*
    if( iIndex >= GetLoadedFaces() ) {
        Log( "if( iIndex >= GetLoadedFaces() ) @ CPolygon::SetF()\n" );
        TRACE_LEAVE_FN() return false; }

    if( pFace == NULL ) {
        Log( "if( pFace == NULL ) @ CPolygon::SetF()\n" );
        TRACE_LEAVE_FN() return false; }
*/
    try { m_pFaces[iIndex] = pFace; }

    catch( ... ) { LOG_VAR( iIndex ) LOG_VAR( GetLoadedFaces() ) LOG_ERR_RET( false ) }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: MoveAllMatching()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygon::MoveAllMatching( const VECT vFrom, const VECT vTo )
{
    TRACE_ENTER_FN( CPolygon::MoveAllMatching )

    for( int iF = 0; iF < GetLoadedFaces(); iF++ )
    {
        CFace* pTemp = F( iF );

        for( int i = 0; i < pTemp->GetLoadedVerts(); i++ )
        {
            if( pTemp->V( i )->GetVertPos() == vFrom )
            {
                pTemp->V( i )->GetVertPos() = vTo;
            }
        }
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: AlignFaceTextures()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygon::AlignFaceTextures()
{
    TRACE_ENTER_FN( CPolygon::AlignFaceTextures )

    int iF;

    for( iF = 0; iF < GetLoadedFaces(); iF++ )
        m_pFaces[iF]->GenerateTextureCoordinates();

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: GetCenterVect()
// Desc: 
//-----------------------------------------------------------------------------
VECT CPolygon::GetCenterVect()
{
    TRACE_ENTER_FN( CPolygon::GetCenterVect )

    VECT vCenter( 0.0f, 0.0f, 0.0f );
    int iF;

    if( GetLoadedFaces() <= 0 ) LOG_ERR_RET( VECT( 0.0f, 0.0f, 0.0f ) )

    for( iF = 0; iF < GetLoadedFaces(); iF++ )
        vCenter += F( iF )->GetFaceCenter();

    RET( vCenter /= GetLoadedFaces() )
}


//-----------------------------------------------------------------------------
// Name: Rebuild()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygon::Rebuild()
{
    TRACE_ENTER_FN( CPolygon::Rebuild() )

    if( !DeleteArrays() )      LOG_ERR_RET( false )

    if( !AllocateNewArrays() ) LOG_ERR_RET( false )

    if( !FillArrays() )        LOG_ERR_RET( false )

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: DeleteArrays()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygon::DeleteArrays()
{
    TRACE_ENTER_FN( CPolygon::DeleteArrays() )

    if( m_fpVertexArray != NULL )
    { try{ delete[] m_fpVertexArray;      } catch( ... ) { LOG_ERR_RET( false ) } }

    if( m_fpColorArray != NULL )
    { try{ delete[] m_fpColorArray;       } catch( ... ) { LOG_ERR_RET( false ) } }

    if( m_fpTexCoordArray != NULL )
    { try{ delete[] m_fpTexCoordArray;    } catch( ... ) { LOG_ERR_RET( false ) } }

    if( m_ipTexSwitchArray != NULL )
    { try{ delete[] m_ipTexSwitchArray;   } catch( ... ) { LOG_ERR_RET( false ) } }

    if( m_ipUniqueTexArray != NULL )
    { try{ delete[] m_ipUniqueTexArray;   } catch( ... ) { LOG_ERR_RET( false ) } }

    if( m_ipVertexBeginArray != NULL )
    { try{ delete[] m_ipVertexBeginArray; } catch( ... ) { LOG_ERR_RET( false ) } }

    if( m_ipVertexEndArray != NULL )
    { try{ delete[] m_ipVertexEndArray;   } catch( ... ) { LOG_ERR_RET( false ) } }

    if( m_pfLightmapCoords != NULL )
    { try{ delete[] m_pfLightmapCoords;   } catch( ... ) { LOG_ERR_RET( false ) } }
    
    RET( true )
}


//-----------------------------------------------------------------------------
// Name: AllocateNewArrays()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygon::AllocateNewArrays()
{
    TRACE_ENTER_FN( CPolygon::AllocateNewArrays() )

    int iF;

    for( iF = 0; iF < GetLoadedFaces(); iF++ )
    {
        if( !F( iF )->Rebuild() ) LOG_ERR_RET( false )

        m_iNumVerts += F( iF )->GetLoadedVerts();
    }

    m_fpVertexArray    = new float[ m_iNumVerts * 3 ];
    m_fpColorArray     = new float[ m_iNumVerts * 3 ];
    m_fpTexCoordArray  = new float[ m_iNumVerts * 2 ];
    m_pfLightmapCoords = new float[ m_iNumVerts * 2 ];

    if( m_fpVertexArray    == NULL ) LOG_ERR_RET( false )
    if( m_fpColorArray     == NULL ) LOG_ERR_RET( false )
    if( m_fpTexCoordArray  == NULL ) LOG_ERR_RET( false )
    if( m_pfLightmapCoords == NULL ) LOG_ERR_RET( false )

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: FillArrays()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygon::FillArrays()
{
    TRACE_ENTER_FN( CPolygon::FillArrays() )

    int iCurTexCoordIndex = 0,
        iCurIndex         = 0,
        iCurVert          = 0,
        iF                = 0;

    m_iNumVerts = 0;

    if( !SortArraysByTexture() ) LOG_ERR_RET( false )

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: GetVertexArray()
// Desc: 
//-----------------------------------------------------------------------------
float* CPolygon::GetVertexArray()
{
    TRACE_ENTER_FN( CPolygon::GetVertexArray() )
    
    if( m_fpVertexArray == NULL )
    {
        LOG_ERR()

        if( !Rebuild() ) { LOG_ERR() RET( NULL ) }
    }

    RET( m_fpVertexArray )
}


//-----------------------------------------------------------------------------
// Name: GetColorArray()
// Desc: 
//-----------------------------------------------------------------------------
float* CPolygon::GetColorArray()
{
    TRACE_ENTER_FN( CPolygon::GetColorArray() )
    
    if( m_fpColorArray == NULL )
    {
        LOG_ERR()

        if( !Rebuild() ) { LOG_ERR() RET( NULL ) }
    }

    RET( m_fpColorArray )
}


//-----------------------------------------------------------------------------
// Name: GetTexCoordArray()
// Desc: 
//-----------------------------------------------------------------------------
float* CPolygon::GetTexCoordArray()
{
    TRACE_ENTER_FN( CPolygon::GetTexCoordArray() )
    
    if( m_fpTexCoordArray == NULL )
    {
        LOG_ERR()

        if( !Rebuild() ) { LOG_ERR() RET( NULL ) }
    }

    RET( m_fpTexCoordArray )
}


//-----------------------------------------------------------------------------
// Name: GetTexSwitchArray()
// Desc: 
//-----------------------------------------------------------------------------
int* CPolygon::GetTexSwitchArray()
{
    TRACE_ENTER_FN( CPolygon::GetTexSwitchArray() )
    
    if( m_ipTexSwitchArray == NULL )
    {
        LOG_ERR()

        if( !Rebuild() ) { LOG_ERR() RET( NULL ) }
    }

    RET( m_ipTexSwitchArray )
}


//-----------------------------------------------------------------------------
// Name: GetVertexBeginArray()
// Desc: 
//-----------------------------------------------------------------------------
int* CPolygon::GetVertexBeginArray()
{
    TRACE_ENTER_FN( CPolygon::GetVertexBeginArray() )
    
    if( m_ipVertexBeginArray == NULL )
    {
        LOG_ERR()

        if( !Rebuild() ) { LOG_ERR() RET( NULL ) }
    }

    RET( m_ipVertexBeginArray )
}


//-----------------------------------------------------------------------------
// Name: GetVertexEndArray()
// Desc: 
//-----------------------------------------------------------------------------
int* CPolygon::GetVertexEndArray()
{
    TRACE_ENTER_FN( CPolygon::GetVertexEndArray() )
    
    if( m_ipVertexEndArray == NULL )
    {
        LOG_ERR()

        if( !Rebuild() ) { LOG_ERR() RET( NULL ) }
    }

    RET( m_ipVertexEndArray )
}


//-----------------------------------------------------------------------------
// Name: GetUniqueTextureArray()
// Desc: 
//-----------------------------------------------------------------------------
unsigned int* CPolygon::GetUniqueTextureArray()
{
    TRACE_ENTER_FN( CPolygon::GetUniqueTextureArray() )
    
    if( m_ipUniqueTexArray == NULL )
    {
        LOG_ERR()

        if( !Rebuild() ) { LOG_ERR() RET( NULL ) }
    }

    RET( m_ipUniqueTexArray )
}


//-----------------------------------------------------------------------------
// Name: GetLightmapCoordArray()
// Desc: 
//-----------------------------------------------------------------------------
float* CPolygon::GetLightmapCoordArray()
{
    TRACE_ENTER_FN( CPolygon::GetLightmapCoordArray() )

    if( m_pfLightmapCoords == NULL )
    {
        LOG_ERR()

        if( !Rebuild() ) { LOG_ERR() RET( NULL ) }
    }
        
    RET( m_pfLightmapCoords )
}


//-----------------------------------------------------------------------------
// Name: SortArraysByTexture()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygon::SortArraysByTexture()
{
    TRACE_ENTER_FN( CPolygon::SortArraysByTexture() )

    unsigned int* ipAllTextures = NULL;

    int* ipNumEachTexture = NULL;

    unsigned int uiTemp        = 0,
                 uiLastTexture = 0,
                 uiTexture     = 0;

    int iF                 = 0,
        iNumUniqueTextures = 0,
        iI                 = 0,
        iJ                 = 0,
        iIndex             = 0;

    m_iNumTexSwitches = 0;

    ipAllTextures = new unsigned int[ GetLoadedFaces() ];

    if( ipAllTextures == NULL ) LOG_ERR_RET( false )

    for( iF = 0; iF < GetLoadedFaces(); iF++ )
        ipAllTextures[ iF ] = F( iF )->GetTexture1().id;

    // Do a bubble sort on the texture id's
    for( int i = 0; i < GetLoadedFaces() - 1; i++ )
    {
        for( int j = 0; j < GetLoadedFaces() - 1 - i; j++ )
        {
            if( ipAllTextures[ j + 1 ] < ipAllTextures[ j ] )
            {  
                uiTemp = ipAllTextures[ j ];

                ipAllTextures[ j ] = ipAllTextures[ j + 1 ];

                ipAllTextures[ j + 1 ] = uiTemp;
            }
        }
    }


    



    // Count the number of unique textures
    uiLastTexture = NOTEXTURE;
    
    for( iF = 0; iF < GetLoadedFaces(); iF++ )
    {
        if( ipAllTextures[ iF ] != uiLastTexture )
            iNumUniqueTextures++;

        uiLastTexture = ipAllTextures[ iF ];
    }

    if( iNumUniqueTextures <= 0 ) { LOG_VAR( iNumUniqueTextures ) LOG_ERR_RET( false ) }


    m_iNumTexSwitches = iNumUniqueTextures;



    m_ipUniqueTexArray     = new unsigned int[ iNumUniqueTextures ];
    if( m_ipUniqueTexArray == NULL ) LOG_ERR_RET( false )

    m_ipTexSwitchArray     = new int[ iNumUniqueTextures ];
    if( m_ipTexSwitchArray == NULL ) LOG_ERR_RET( false )

    ipNumEachTexture       = new int[ iNumUniqueTextures ];
    if( ipNumEachTexture == NULL ) LOG_ERR_RET( false )

    m_ipVertexBeginArray   = new int[ iNumUniqueTextures ];
    if( m_ipVertexBeginArray == NULL ) LOG_ERR_RET( false )

    m_ipVertexEndArray     = new int[ iNumUniqueTextures ];
    if( m_ipVertexEndArray == NULL ) LOG_ERR_RET( false )

    uiLastTexture = NOTEXTURE;
    iIndex = 0;
    int iNumOfThisTexture = 0;

    for( iF = 0; iF < GetLoadedFaces(); iF++ )
    {
        if( ipAllTextures[ iF ] != uiLastTexture )
        {
            m_ipTexSwitchArray[ iIndex ] = iF;

            if( iNumOfThisTexture != 0 )
            ipNumEachTexture[ iIndex - 1] = iNumOfThisTexture;
            m_ipUniqueTexArray[ iIndex ] = ipAllTextures[ iF ];
            iIndex++;
            iNumOfThisTexture = 0;

            if( iIndex >= iNumUniqueTextures + 1 ) {
                LOG_VAR( iF ) LOG_VAR( iIndex )
                LOG_VAR( iNumUniqueTextures ) LOG_ERR_RET( false ) }
        }

        iNumOfThisTexture++;

        uiLastTexture = ipAllTextures[ iF ];
    }

    // Gets the last number for the ipNumEachTexture array
    int iSum = 0;

    for( int d = 0; d < iNumUniqueTextures - 1; d++ )
        iSum += ipNumEachTexture[ d ];

    ipNumEachTexture[ iNumUniqueTextures - 1 ] = GetLoadedFaces() - iSum;

    for( int g = 0; g < iNumUniqueTextures; g++ )
    {
        m_ipVertexBeginArray[ g ] = m_ipTexSwitchArray[ g ] * 3;
        m_ipVertexEndArray[   g ] = ipNumEachTexture[   g ] * 3;
    }

    int iCurIndex = 0;
    int iCurTexCoordIndex = 0;
    int iCurVert;

    for( int t = 0; t < iNumUniqueTextures; t++ )
    {
        unsigned int iFindThese = m_ipUniqueTexArray[ t ];

        int iNumToFind = m_ipTexSwitchArray[ t ]; 

        for( iF = 0; iF < GetLoadedFaces(); iF++ )
        {
            if( iFindThese == F( iF )->GetTexture1().id )
            {
                float* pfFVA  = F( iF )->GetVertexArray();
                float* pfFCA  = F( iF )->GetColorArray();
                float* pfFTA  = F( iF )->GetTexCoordArray();
                float* pfFTCA = F( iF )->GetLightmapCoordArray();

                for( iCurVert = 0; iCurVert < F( iF )->GetLoadedVerts() * 3; iCurVert++ )
                {
                    m_fpVertexArray[ iCurIndex + iCurVert ] = pfFVA[ iCurVert ];
                    m_fpColorArray[  iCurIndex + iCurVert ] = pfFCA[ iCurVert ];
                }

                for( iCurVert = 0; iCurVert < F( iF )->GetLoadedVerts() * 2; iCurVert++ )
                {
                    m_fpTexCoordArray[  iCurTexCoordIndex + iCurVert ] = pfFTA[  iCurVert ];
                    m_pfLightmapCoords[ iCurTexCoordIndex + iCurVert ] = pfFTCA[ iCurVert ];
                }

                iCurIndex         += F( iF )->GetLoadedVerts() * 3;
                iCurTexCoordIndex += F( iF )->GetLoadedVerts() * 2;
            }
        }
    }

    try{ delete[] ipAllTextures;    } catch( ... ) { LOG_ERR_RET( false ) }
    try{ delete[] ipNumEachTexture; } catch( ... ) { LOG_ERR_RET( false ) }

    ipAllTextures    = NULL;
    ipNumEachTexture = NULL;

    RET( true )
}

