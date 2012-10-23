//----------------------------------------------------------------------------- 
// File: PolygonManager.cpp
//
// Desc: Manages all polygon stuff in the scene
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#include <windows.h>
#include <commctrl.h>
#include <gl\glaux.h>
#include <fstream>
#include <gl\glu.h>
#include <string.h>
#include <gl\gl.h>
#include <stdio.h>
#include <cmath>
#include "PolygonManager.h"
#include "TextureManager.h"
#include "PolygonTabDB.h"
#include "LightManager.h"
#include "Application.h"
#include "SaveTokens.h"
#include "CustomMsg.h"
#include "Settings.h"
#include "resource.h"
#include "Polygon.h"
#include "OpenGL.h"
#include "dinput.h"
#include "Vertex.h"
#include "Window.h"
#include "Tracer.h"
#include "main.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
CPolygonManager::CPolygonManager()
{
    TRACE_ENTER_FN( CPolygonManager::CPolygonManager )

    Log( "CPolygonManager::CPolygonManager()\n" );

    m_iLoadedPolygons = 0;
    m_pPolygons = NULL;

    TRACE_LEAVE_FN()
}

CPolygonManager::~CPolygonManager()
{
    TRACE_ENTER_FN( CPolygonManager::~CPolygonManager )

    Log( "CPolygonManager::~CPolygonManager()\n" );
    
    if( !Release() ) LOG_ERR()

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: Init()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::Init()
{
    TRACE_ENTER_FN( CPolygonManager::Init )

    Log( "CPolygonManager::Init()\n" );

    m_eClassType         = POLYGONMANAGER;
    m_iLoadedPolygons    = 0;
    m_pPolygons          = NULL;
    glMultiTexCoord2fARB = NULL;

    // Used to select polygons through mouse clicks
    LU->ResetLookupTable();

    m_Tree.bSelected       = false;
    m_Tree.hPolygonManager = NULL;
    m_Tree.hZone           = NULL;

    m_pQuadricObj = gluNewQuadric();
    gluQuadricTexture( m_pQuadricObj, false );
    gluQuadricDrawStyle( m_pQuadricObj, GLU_FILL );

    glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC) wglGetProcAddress( "glMultiTexCoord2fARB" );

    if( glMultiTexCoord2fARB == NULL ) LOG_ERR_RET( false )

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Frees resources and memory
//-----------------------------------------------------------------------------
bool CPolygonManager::Release()
{
    TRACE_ENTER_FN( CPolygonManager::Release )

    Log( "CPolygonManager::Release()\n" );

    int iA;

    try
    {
        if( m_pPolygons )
        {
            for( iA = 0; iA < m_iLoadedPolygons; iA++ )
            {
                // If the polygon isnt NULL, then delete it and set it to NULL
                if( P( iA ) != NULL )
                {
                    delete P( iA );
                    SetP( iA, NULL );
                }
            }

            delete[] m_pPolygons;
        }

        m_iLoadedPolygons = 0;

        LU->ResetLookupTable();

        if( m_pQuadricObj != NULL ) gluDeleteQuadric( m_pQuadricObj );

        m_Brush.Release();
    }
    catch( ... ) { LOG_VAR( iA ) LOG_VAR( m_iLoadedPolygons ) LOG_ERR_RET( false ) }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Reset()
// Desc: 
//-----------------------------------------------------------------------------
void CPolygonManager::Reset()
{
    TRACE_ENTER_FN( CPolygonManager::Reset )

    Release();
    Init();

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: NewBrush()
// Desc: Selects a new brush type
//-----------------------------------------------------------------------------
void CPolygonManager::NewBrush( const BRUSHTYPE type )
{
    TRACE_ENTER_FN( CPolygonManager::NewBrush )

    m_Brush.SetType( type );
    
    LU->ResetLookupTable();

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: ForgeBrush()
// Desc: Takes the current brush and uses it as a template to create a new
//       polygon. The Polygon Tree is then updated, and the lookup table is
//       reset because the scene has changed.
//-----------------------------------------------------------------------------
bool CPolygonManager::ForgeBrush()
{
    TRACE_ENTER_FN( CPolygonManager::ForgeBrush )

    TVINSERTSTRUCT tvs;
    HTREEITEM hPolygon;
    int iIndex, iA;

    // Create a temporary holder array for the polygon pointers
    CPolygon** pTemp = NULL;

    // Initalize the holder
    pTemp = new CPolygon*[ m_iLoadedPolygons + 1 ];

    // Make sure it was created
    if( pTemp == NULL ) LOG_ERR()

    // Copy all polygons into the temp
    for( iA = 0; iA < m_iLoadedPolygons; iA++ ) pTemp[ iA ] = P( iA );

    // Delete the old polygon holder if any polygons were ever in it
    if( m_pPolygons != NULL ) { delete[] m_pPolygons; m_pPolygons = NULL; }

    // Increase the size of the polygon array to 1 larger
    m_pPolygons = new CPolygon*[ m_iLoadedPolygons + 1 ];

    // Make sure it was created
    if( m_pPolygons == NULL ) LOG_ERR_RET( false )

    // Copy the temp back into the real one
    for( iA = 0; iA < m_iLoadedPolygons; iA++ ) SetP( iA, pTemp[ iA ] );

    // Delete the temp holder
    if( pTemp != NULL ) delete[] pTemp;

    CPolygon* pPoly = m_Brush.Forge();

    if( pPoly == NULL ) LOG_ERR_RET( false )

    // Put the newly created polygon at the end of the list
    SetP( m_iLoadedPolygons, pPoly );

    // We have one more now
    m_iLoadedPolygons++;

    // This is just for convienence
    iIndex = m_iLoadedPolygons - 1;

    // Make sure it was actually created
    if( P( iIndex ) == NULL ) LOG_ERR_RET( false )

    // Add a new leaf to the tree at the end. (After the other polygons)
    tvs.hInsertAfter = TVI_LAST;
    tvs.item.mask    = TVIF_TEXT;
    tvs.hParent      = m_Tree.hZone;
    tvs.item.pszText = P( iIndex )->m_strName;

    // Insert it
    hPolygon = (HTREEITEM)SendDlgItemMessage( m_Tree.hTreeWnd, IDC_TREE_POLYGONTREE, TVM_INSERTITEM, 0, (LPARAM)(LPTV_INSERTSTRUCT) &tvs );
    tvs.hParent = hPolygon;

    // Give the polygon a handle to its leaf
    P( iIndex )->SetTreeItem( hPolygon );

    // Update all the other handles for all the other polygons, faces, and verts
    for( int iF = 0; iF < P( iIndex )->GetLoadedFaces(); iF++ )
    {
        tvs.item.pszText = F( iIndex, iF )->GetName();
        tvs.hParent = (HTREEITEM)SendDlgItemMessage( m_Tree.hTreeWnd, IDC_TREE_POLYGONTREE, TVM_INSERTITEM, 0, (LPARAM)(LPTV_INSERTSTRUCT) &tvs );

        F( iIndex, iF )->SetTreeItem( tvs.hParent );

        for( int iV = 0; iV < F( iIndex, iF )->GetLoadedVerts(); iV++ )
        {
            tvs.item.pszText = V( iIndex, iF, iV )->GetName();
            V( iIndex, iF, iV )->SetTreeItem( (HTREEITEM)SendDlgItemMessage( m_Tree.hTreeWnd, IDC_TREE_POLYGONTREE,TVM_INSERTITEM, 0, (LPARAM)(LPTV_INSERTSTRUCT) &tvs ) );
        }

        tvs.hParent = hPolygon;
    }

    LU->ResetLookupTable();

    RecalculateGeometry();

    if( !Map->Init() ) LOG_ERR_RET( false )

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: FillPolygonTreeView()
// Desc: Runs through all zones, polygons, faces, and verticies to create the
//       polygon tree in the polygon tab. Gives each instance of every class a
//       handle to its corresponding leaf.
//-----------------------------------------------------------------------------
void CPolygonManager::FillPolygonTreeView( HWND hWnd )
{
    TRACE_ENTER_FN( CPolygonManager::FillPolygonTreeView )

    char* str = NULL;

    TVINSERTSTRUCT tvs;
    HTREEITEM hPolygon,
              hFace,
              hVert;

    m_Tree.hTreeWnd = hWnd;

    // Clear the tree
    HTREEITEM m_hItem = TVI_ROOT;
    SendDlgItemMessage( hWnd, IDC_TREE_POLYGONTREE, TVM_DELETEITEM, 0,
                        (LPARAM) (HTREEITEM) m_hItem);

    tvs.hInsertAfter = TVI_LAST;
    tvs.item.mask    = TVIF_TEXT;
    tvs.hParent      = TVI_ROOT;

    tvs.item.pszText = "Polygon Manager";

    m_Tree.hPolygonManager = (HTREEITEM)SendDlgItemMessage( hWnd, IDC_TREE_POLYGONTREE,
                                                     TVM_INSERTITEM, 0,
                                                     (LPARAM)(LPTV_INSERTSTRUCT) &tvs );

    tvs.hParent = m_Tree.hPolygonManager;
    tvs.item.pszText = "Zone 1";

    m_Tree.hZone = (HTREEITEM)SendDlgItemMessage( hWnd, IDC_TREE_POLYGONTREE, TVM_INSERTITEM,
                                                      0, (LPARAM)(LPTV_INSERTSTRUCT) &tvs );

    tvs.hParent = m_Tree.hZone;

    for( int iP = 0; iP < GetLoadedPolygons(); iP++ )
    {
        str = P( iP )->m_strName;

        tvs.item.pszText = str;
        hPolygon = (HTREEITEM)SendDlgItemMessage( hWnd, IDC_TREE_POLYGONTREE, TVM_INSERTITEM,
                                                  0, (LPARAM)(LPTV_INSERTSTRUCT) &tvs );
        tvs.hParent = hPolygon;

        P( iP )->SetTreeItem( hPolygon );

        for( int iF = 0; iF < P( iP )->GetLoadedFaces(); iF++ )
        {
            tvs.item.pszText = F( iP, iF )->GetName();
            hFace = (HTREEITEM)SendDlgItemMessage( hWnd, IDC_TREE_POLYGONTREE,TVM_INSERTITEM, 0, (LPARAM)(LPTV_INSERTSTRUCT) &tvs );
            tvs.hParent = hFace;

            F( iP, iF )->SetTreeItem( hFace );

            for( int iV = 0; iV < F( iP, iF )->GetLoadedVerts(); iV++ )
            {
                tvs.item.pszText = V( iP, iF, iV )->GetName();
                hVert = (HTREEITEM)SendDlgItemMessage( hWnd, IDC_TREE_POLYGONTREE, TVM_INSERTITEM, 0, (LPARAM)(LPTV_INSERTSTRUCT) &tvs );
                V( iP, iF, iV )->SetTreeItem( hVert );
            }

            tvs.hParent = hPolygon;
        }

        tvs.hParent = m_Tree.hZone;
    }

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: ApplyTextureToSelected()
// Desc: Applys the gives texture to the selected face
//-----------------------------------------------------------------------------
void CPolygonManager::ApplyTextureToSelected( const HTEXTURE hTexture )
{
    TRACE_ENTER_FN( CPolygonManager::ApplyTextureToSelected )

    int iP = LU->GetPoly();
    int iF = LU->GetFace();

    if( LU->PolySelected() && LU->FaceSelected() )
    {
        if( SE->m_bTextureTab_Texture1 )
            F( iP, iF )->SetTexture1( hTexture );
        else if( SE->m_bTextureTab_Texture2 )
            F( iP, iF )->SetTexture2( hTexture );
        else if( SE->m_bTextureTab_Lightmap )
            LOG_ERR()
    }

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: GetAssociatedClassType()
// Desc: Given a string, usually from the polygon tree, returns what kind of
//       object it represents. See 'enum.h' for class types.
//-----------------------------------------------------------------------------
CLASSTYPE CPolygonManager::GetAssociatedClassType( char* str )
{
    TRACE_ENTER_FN( CPolygonManager::GetAssociatedClassType )

    if( str == NULL ) {
        m_Tree.bSelected = false;
        RET( NOTHINGCLASS ) }

    m_Tree.bSelected = true;

    // Compares the string to all polys, faces, and verts until a match is found
    // When found, information about the selected object is recorded in the
    // 'm_Tree' struct. The name is the attribute compared
    for( int iP = 0; iP < GetLoadedPolygons(); iP++ )
    {
        if( strcmp( P( iP )->m_strName, str ) == 0 )
        {
            m_Tree.eCurrentType = P( iP )->GetClassType();
            m_Tree.iPoly = iP;

            RET( P( iP )->GetClassType() )
        }

        for( int iF = 0; iF < P( iP )->GetLoadedFaces(); iF++ )
        {
            if( strcmp( F( iP, iF )->GetName(), str ) == 0 )
            {
                m_Tree.eCurrentType = F( iP, iF )->GetClassType();
                m_Tree.iPoly = iP;
                m_Tree.iFace = iF;

                RET( F( iP, iF )->GetClassType() )
            }

            for( int iV = 0; iV < F( iP, iF )->GetLoadedVerts(); iV++ )
            {
                if( strcmp( V( iP, iF, iV )->GetName(), str ) == 0 )
                {
                    m_Tree.eCurrentType = V( iP, iF, iV )->GetClassType();
                    m_Tree.iPoly = iP;
                    m_Tree.iFace = iF;
                    m_Tree.iVert = iV;

                    RET( V( iP, iF, iV )->GetClassType() )
                }
            }
        }
    }

    // Nothing should be selected
    m_Tree.bSelected = false;

    // If it couldnt fina a match, its unknown

    RET( NOTHINGCLASS )
}


//-----------------------------------------------------------------------------
// Name: GetVertexInfo()
// Desc: Gets all the information about the selected vertex
//-----------------------------------------------------------------------------
bool CPolygonManager::GetVertexInfo( int &r, int &g, int &b, int &u, int &v )
{
    TRACE_ENTER_FN( CPolygonManager::GetVertexInfo )

    int iP = m_Tree.iPoly;
    int iF = m_Tree.iFace;
    int iV = m_Tree.iVert;

    r = (int)V( iP, iF, iV )->GetColor().x;
    g = (int)V( iP, iF, iV )->GetColor().y;
    b = (int)V( iP, iF, iV )->GetColor().z;
    u = (int)V( iP, iF, iV )->GetTexU();
    v = (int)V( iP, iF, iV )->GetTexV();

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: UpdateTreeSelection()
// Desc: Selects and insures that the given selection is visible.
//-----------------------------------------------------------------------------
bool CPolygonManager::UpdateTreeSelection( HWND hWnd )
{
    TRACE_ENTER_FN( CPolygonManager::UpdateTreeSelection )

    if( !LU->PolySelected() || !LU->FaceSelected() ) RET( true )

    if( hWnd == NULL ) LOG_ERR_RET( false )

    int iP = LU->GetPoly();
    int iF = LU->GetFace();

    SendDlgItemMessage(hWnd, IDC_TREE_POLYGONTREE, TVM_SELECTITEM, (WPARAM)TVGN_CARET,
        (LPARAM)(HTREEITEM)F( iP, iF )->GetTreeItem() );

     RET( true )
}


//-----------------------------------------------------------------------------
// Name: ApplyNewName()
// Desc: Sets the new name of the object. Only updates the internal name, not
//       the name that appears in the polygon tree. (done elsewhere)
//-----------------------------------------------------------------------------
bool CPolygonManager::ApplyNewName( char* str, CLASSTYPE eType )
{
    TRACE_ENTER_FN( CPolygonManager::ApplyNewName )

    if( str == NULL ) LOG_ERR_RET( false )

    int iP = m_Tree.iPoly;
    int iF = m_Tree.iFace;
    int iV = m_Tree.iVert;

    switch( eType )
    {
    case VERTEXCLASS:  strcpy( V( iP, iF, iV )->GetName(), str ); break;
    case FACECLASS:    strcpy( F( iP, iF )->GetName(),     str ); break;
    case POLYGONCLASS: strcpy( P( iP )->m_strName,         str ); break;
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: SetVertexInfo()
// Desc: Sets information about the selected vertex
//-----------------------------------------------------------------------------
bool CPolygonManager::SetVertexInfo( int iR, int iG, int iB, int iU, int iV )
{
    TRACE_ENTER_FN( CPolygonManager::SetVertexInfo )

    CVertex* pV = V( m_Tree.iPoly, m_Tree.iFace, m_Tree.iVert );

    VECT vC( (float)iR, (float)iG, (float)iB );

    pV->SetColor( vC   );
    pV->SetTexU(  (float)iU );
    pV->SetTexV(  (float)iV );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: TranslateTexU()
// Desc: Tells the face to slide the U coords an 'amount'
//-----------------------------------------------------------------------------
void CPolygonManager::TranslateTexU( const float amount )
{ F( m_Tree.iPoly, m_Tree.iFace )->TranslateTexU( amount ); }


//-----------------------------------------------------------------------------
// Name: TranslateTexV()
// Desc: Tells the face to slide the V coords an 'amount'
//-----------------------------------------------------------------------------
void CPolygonManager::TranslateTexV( const float amount )
{ F( m_Tree.iPoly, m_Tree.iFace )->TranslateTexV( amount ); }


//-----------------------------------------------------------------------------
// Name: FlipTexU()
// Desc: Tells the face to flip th texture in terms of U
//-----------------------------------------------------------------------------
void CPolygonManager::FlipTexU()
{ F( m_Tree.iPoly, m_Tree.iFace )->FlipTexU(); }


//-----------------------------------------------------------------------------
// Name: RotateTex90()
// Desc: Tells the face to flip th texture in terms of U
//-----------------------------------------------------------------------------
void CPolygonManager::RotateTex90()
{ F( m_Tree.iPoly, m_Tree.iFace )->RotateTex90(); }


//-----------------------------------------------------------------------------
// Name: RotateTex90()
// Desc: Tells the face to flip th texture in terms of U
//-----------------------------------------------------------------------------
void CPolygonManager::RotateLightmap90()
{ F( m_Tree.iPoly, m_Tree.iFace )->RotateLightmap90(); }


//-----------------------------------------------------------------------------
// Name: FlipTexV()
// Desc: Tells the face to flip th texture in terms of V
//-----------------------------------------------------------------------------
void CPolygonManager::FlipTexV()
{ F( m_Tree.iPoly, m_Tree.iFace )->FlipTexV(); }


//-----------------------------------------------------------------------------
// Name: SetScale()
// Desc: Tells face to sclae the texture by the 'factor'
//-----------------------------------------------------------------------------
void CPolygonManager::SetScale( const float factor )
{ F( m_Tree.iPoly, m_Tree.iFace )->SetScale( factor ); }


//-----------------------------------------------------------------------------
// Name: SetPanU()
// Desc: Sets the pan state of the face to 'bPan'
//-----------------------------------------------------------------------------
void CPolygonManager::SetPanU( const bool bPan )
{ F( m_Tree.iPoly, m_Tree.iFace )->SetPanU( bPan ); }


//-----------------------------------------------------------------------------
// Name: SetPanV()
// Desc: Sets the pan state of the face to 'bPan'
//-----------------------------------------------------------------------------
void CPolygonManager::SetPanV( const bool bPan )
{ F( m_Tree.iPoly, m_Tree.iFace )->SetPanV( bPan ); }


//-----------------------------------------------------------------------------
// Name: GetPanU()
// Desc: Gets the U pan state of the face
//-----------------------------------------------------------------------------
bool CPolygonManager::GetPanU()
{ return F( m_Tree.iPoly, m_Tree.iFace )->GetPanU(); }


//-----------------------------------------------------------------------------
// Name: SetPanV()
// Desc: Gets the V pan state of the face
//-----------------------------------------------------------------------------
bool CPolygonManager::GetPanV()
{ return F( m_Tree.iPoly, m_Tree.iFace )->GetPanV(); }


//-----------------------------------------------------------------------------
// Name: SetTranslucent()
// Desc: 
//-----------------------------------------------------------------------------
void CPolygonManager::SetTranslucent( const bool bTranslucent )
{ F( m_Tree.iPoly, m_Tree.iFace )->SetTranslucent( bTranslucent ); }


//-----------------------------------------------------------------------------
// Name: GetUseTexture1()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::GetUseTexture1()
{
    TRACE_ENTER_FN( CPolygonManager::GetUseTexture1 )

    bool b = F( m_Tree.iPoly, m_Tree.iFace )->UsingTexture1();

    RET( b )
}


//-----------------------------------------------------------------------------
// Name: GetUseTexture2()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::GetUseTexture2()
{
    TRACE_ENTER_FN( CPolygonManager::GetUseTexture2 )

    bool b = F( m_Tree.iPoly, m_Tree.iFace )->UsingTexture2();

    RET( b )
}


//-----------------------------------------------------------------------------
// Name: GetUseLightmap()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::GetUseLightmap()
{
    TRACE_ENTER_FN( CPolygonManager::GetUseLightmap )

    bool b = F( m_Tree.iPoly, m_Tree.iFace )->UsingLightmap();

    RET( b )
}


//-----------------------------------------------------------------------------
// Name: SetUseTexture1()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::SetUseTexture1( const bool b )
{
    TRACE_ENTER_FN( CPolygonManager::SetUseTexture1 )

    F( m_Tree.iPoly, m_Tree.iFace )->UseTexture1( b );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: SetUseTexture2()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::SetUseTexture2( const bool b )
{
    TRACE_ENTER_FN( CPolygonManager::SetUseTexture2 )

    F( m_Tree.iPoly, m_Tree.iFace )->UseTexture2( b );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: SetUseLightmap()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::SetUseLightmap( const bool b )
{
    TRACE_ENTER_FN( CPolygonManager::SetUseLightmap )

    F( m_Tree.iPoly, m_Tree.iFace )->UseLightmap( b );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: NextFace()
// Desc: Selects the next face of the polygon
//-----------------------------------------------------------------------------
int CPolygonManager::NextFace()
{
    TRACE_ENTER_FN( CPolygonManager::NextFace )

    if( !LU->FaceSelected() || ( !LU->BrushSelected() && !LU->PolySelected() ) ) RET( 0 )

    CPolygon* pPoly = NULL;

    if( LU->BrushSelected() )
        pPoly = (CPolygon*)&m_Brush;
    else
        pPoly = P( LU->GetPoly() );

    if( pPoly == NULL ) { LOG_VAR( LU->BrushSelected() ) LOG_VAR( LU->GetPoly() ) LOG_ERR_RET( 0 ) }

    pPoly->NextFace();

    RET( 0 )
}

//-----------------------------------------------------------------------------
// Name: GenerateTextureCoords()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::GenerateTextureCoords()
{
    TRACE_ENTER_FN( CPolygonManager::GenerateTextureCoords )

    if( !LU->FaceSelected() || LU->BrushSelected() ) RET( true )

    F( LU->GetPoly(), LU->GetFace() )->GenerateTextureCoordinates();

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: TranslateVertX()
// Desc: Changes the position of the vertex by 'amount' in the X direction. If
//       any other points in the polygon share the vertex, they are
//       automatically moved
//-----------------------------------------------------------------------------
bool CPolygonManager::TranslateVertX( const float amount )
{
    TRACE_ENTER_FN( CPolygonManager::TranslateVertX )

    if( LU->GetVert() == NA ) LOG_ERR_RET( false )

    CPolygon* pPoly = NULL;

    if( LU->BrushSelected() )
        pPoly = (CPolygon*)&m_Brush;
    else
        pPoly = P( LU->GetPoly() );

    if( pPoly == NULL ) LOG_ERR_RET( false )

    // The point that is being moved
    VECT v = pPoly->V( LU->GetFace(), LU->GetVert() )->GetVertPos();

    // Cycle through every vertex in the polygon
    // looking for a twin. If found, it is moved
    for( int iF = 0; iF < pPoly->GetLoadedFaces(); iF++ )
        for( int iV = 0; iV < pPoly->F( iF )->GetLoadedVerts(); iV++ )
            if( v == pPoly->V( iF, iV )->GetVertPos() )
                pPoly->V( iF, iV )->TranslateX( amount );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: TranslateVertY()
// Desc: Changes the position of the vertex by 'amount' in the Y direction. If
//       any other points in the polygon share the vertex, they are
//       automatically moved
//-----------------------------------------------------------------------------
bool CPolygonManager::TranslateVertY( const float amount )
{
    TRACE_ENTER_FN( CPolygonManager::TranslateVertY )

    // If no point has been selected, then fail
    if( LU->GetVert() == NA ) LOG_ERR_RET( false )

    CPolygon* pPoly = NULL;

    if( LU->BrushSelected() )
        pPoly = (CPolygon*)&m_Brush;
    else
        pPoly = P( LU->GetPoly() );

    if( pPoly == NULL ) LOG_ERR_RET( false )

    // The point that is being moved
    VECT v = pPoly->V( LU->GetFace(), LU->GetVert() )->GetVertPos();

    // Cycle through every vertex in the polygon looking for a twin. If found, it is moved
    for( int iF = 0; iF < pPoly->GetLoadedFaces(); iF++ )
        for( int iV = 0; iV < pPoly->F( iF )->GetLoadedVerts(); iV++ )
            if( v == pPoly->V( iF, iV )->GetVertPos() )
                pPoly->V( iF, iV )->TranslateY( amount );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: TranslateVertZ()
// Desc: Changes the position of the vertex by 'amount' in the Z direction. If
//       any other points in the polygon share the vertex, they are
//       automatically moved
//-----------------------------------------------------------------------------
bool CPolygonManager::TranslateVertZ( const float amount )
{
    TRACE_ENTER_FN( CPolygonManager::TranslateVertZ )

    // If no point has been selected, then fail
    if( LU->GetVert() == NA ) LOG_ERR_RET( false )

    CPolygon* pPoly = NULL;

    if( LU->BrushSelected() )
        pPoly = (CPolygon*)&m_Brush;
    else
        pPoly = P( LU->GetPoly() );

    if( pPoly == NULL ) LOG_ERR_RET( false )

    // The point that is being moved
    VECT v = pPoly->V( LU->GetFace(), LU->GetVert() )->GetVertPos();

    // Cycle through every vertex in the polygon looking for a twin. If found, it is moved
    for( int iF = 0; iF < pPoly->GetLoadedFaces(); iF++ )
        for( int iV = 0; iV < pPoly->F( iF )->GetLoadedVerts(); iV++ )
            if( v == pPoly->V( iF, iV )->GetVertPos() )
                pPoly->V( iF, iV )->TranslateZ( amount );

    RET( true )
}

//-----------------------------------------------------------------------------
// Name: TranslateFaceX()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::TranslateFaceX( const float amount )
{
    TRACE_ENTER_FN( CPolygonManager::TranslateFaceX )

    // Make sure the face is selected
    if( !LU->FaceSelected() ) LOG_ERR_RET( false )

    //Remember which vert was selected
    int iTemp = LU->GetVert();
    int iVerts;

    // Get the number of verts in this face
    if( LU->BrushSelected() )
        iVerts = m_Brush.F( LU->GetFace() )->GetLoadedVerts();
    else
        iVerts = F( LU->GetPoly(), LU->GetFace() )->GetLoadedVerts();

    // Iterate through all the verts of the face, moving each one
    for( int iV = 0; iV < iVerts; iV++ )
    {
        LU->SetVert( iV );
        TranslateVertX( amount );
    }

    // Set the vert that was selected before
    LU->SetVert( iTemp );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: TranslateFaceY()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::TranslateFaceY( const float amount )
{
    TRACE_ENTER_FN( CPolygonManager::TranslateFaceY )

    if( !LU->FaceSelected() ) LOG_ERR_RET( false )

        //Remember which vert was selected
    int iTemp = LU->GetVert();
    int iVerts;

    // Get the number of verts in this face
    if( LU->BrushSelected() )
        iVerts = m_Brush.F( LU->GetFace() )->GetLoadedVerts();
    else
        iVerts = F( LU->GetPoly(), LU->GetFace() )->GetLoadedVerts();

    // Iterate through all the verts of the face, moving each one
    for( int iV = 0; iV < iVerts; iV++ )
    {
        LU->SetVert( iV );
        TranslateVertY( amount );
    }

    LU->SetVert( iTemp );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: TranslateFaceZ()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::TranslateFaceZ( const float amount )
{
    TRACE_ENTER_FN( CPolygonManager::TranslateFaceZ )

    if( !LU->FaceSelected() ) LOG_ERR_RET( false )

    //Remember which vert was selected
    int iTemp = LU->GetVert();
    int iVerts;

    // Get the number of verts in this face
    if( LU->BrushSelected() )
        iVerts = m_Brush.F( LU->GetFace() )->GetLoadedVerts();
    else
        iVerts = F( LU->GetPoly(), LU->GetFace() )->GetLoadedVerts();

    // Iterate through all the verts of the face, moving each one
    for( int iV = 0; iV < iVerts; iV++ )
    {
        LU->SetVert( iV );
        TranslateVertZ( amount );
    }

    LU->SetVert( iTemp );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: TranslatePolyX()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::TranslatePolyX( const float amount )
{
    TRACE_ENTER_FN( CPolygonManager::TranslatePolyX )

    if( LU->BrushSelected() && LU->FaceSelected() )
    {
        m_Brush.TranslateX( amount );
        m_Brush.SetBrushOffset( m_Brush.GetCenterVect() );
        RET( true )
    }

    if( !LU->PolySelected() ) RET( false )

    P( LU->GetPoly() )->TranslateX( amount );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: TranslatePolyY()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::TranslatePolyY( const float amount )
{
    TRACE_ENTER_FN( CPolygonManager::TranslatePolyY )

    if( LU->BrushSelected() && LU->FaceSelected() )
    {
        m_Brush.TranslateY( amount );
        m_Brush.SetBrushOffset( m_Brush.GetCenterVect() );
        RET( true )
    }

    if( !LU->PolySelected() )  RET( false )

    P( LU->GetPoly() )->TranslateY( amount );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: TranslatePolyZ()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::TranslatePolyZ( const float amount )
{
    TRACE_ENTER_FN( CPolygonManager::TranslatePolyZ )

    if( LU->BrushSelected() && LU->FaceSelected() )
    {
        m_Brush.TranslateZ( amount );
        m_Brush.SetBrushOffset( m_Brush.GetCenterVect() );
        RET( true )
    }

    if( !LU->PolySelected() ) RET( false )

    P( LU->GetPoly() )->TranslateZ( amount );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: SelectNextVertex()
// Desc: Selects the next vertex
//-----------------------------------------------------------------------------
bool CPolygonManager::SelectNextVertex()
{
    TRACE_ENTER_FN( CPolygonManager::SelectNextVertex )

    if( !LU->VertSelected() ) RET( true )

    int iVert = LU->GetVert();

    iVert++;

    if( iVert >= 4 ) iVert = 0;

    LU->SetVert( iVert );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: DeleteSelectedFace()
// Desc: Deletes the selected face. If it was the last face of the selected
//       polygon, the polygon is deleted as well
//-----------------------------------------------------------------------------
bool CPolygonManager::DeleteSelectedFace()
{
    TRACE_ENTER_FN( CPolygonManager::DeleteSelectedFace )

    if( !LU->FaceSelected() || !LU->PolySelected() || LU->BrushSelected() ) RET( false )

    int iPoly = LU->GetPoly();
    int iFace = LU->GetFace();

    // Remove the selected face from the 
    P( iPoly )->RemoveFace( LU->GetFace() );
   
    // If there are no faces left in the polygon, then delete the polygon
    if( P( iPoly )->GetLoadedFaces() == 0 )
    {
        DeleteSelectedPolygon();

        // Geometry has changed
        LU->ResetLookupTable();

        // If there are still polygons left in the scene
        if( iPoly < m_iLoadedPolygons )
        {
            // Select the first face of the next polygon
            LU->SetPoly( iPoly );
            LU->SetFace( 0 );
        }
        else if( m_iLoadedPolygons > 0 )
        {
            // If any polygons are left, but the current iPoly
            // is out of range, (usually because it was the last in the list)
            // select the first face of the first polygon
            LU->SetPoly( 0 );
            LU->SetFace( 0 );
        }
    }
    else
    {
        // If the polygon was not deleted
        LU->ResetLookupTable();

        // Select the next face of the polygon
        if( P( iPoly )->GetLoadedFaces() > 0 )
        {
            LU->SetPoly( iPoly );

            if( iFace < P( iPoly )->GetLoadedFaces() )
                LU->SetFace( iFace );
            else
                LU->SetFace( 0 );
        }
    }

    // Notify the tree window that a selection was made
    NewTreeSelection();

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: AlignPolygonTextures()
// Desc: /***/
//-----------------------------------------------------------------------------
bool CPolygonManager::AlignPolygonTextures()
{
    TRACE_ENTER_FN( CPolygonManager::AlignPolygonTextures )

    if( !LU->PolySelected() || !LU->FaceSelected() ) RET( true )

    int iP = LU->GetPoly();

    for( int iF = 0; iF < P( iP )->GetLoadedFaces(); iF++ )
        F( iP, iF )->GenerateTextureCoordinates();

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: ResetVirtualCoords()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::ResetVirtualCoords()
{
    TRACE_ENTER_FN( CPolygonManager::ResetVirtualCoords )

    if( !LU->PolySelected() ) RET( true )

    int iP = LU->GetPoly();

    for( int iF = 0; iF < P( iP )->GetLoadedFaces(); iF++ )
        F( iP, iF )->ResetVirtualCoords();

    RET( true )
}

//-----------------------------------------------------------------------------
// Name: NewTreeSelection()
// Desc: 
//-----------------------------------------------------------------------------
void CPolygonManager::NewTreeSelection()
{
    PostMessage( m_Tree.hTreeWnd, CMSG_NEWSELECTION, 0, 0 );

    m_Tree.bSelected = true;
}

//-----------------------------------------------------------------------------
// Name: ResetVirtualCoords()
// Desc: 
//-----------------------------------------------------------------------------
CPolygon* CPolygonManager::P( const int iP )
{
    TRACE_ENTER_FN( CPolygonManager::P )

    try { RET( m_pPolygons[iP] ) }

    catch( ... ) { LOG_VAR( m_iLoadedPolygons ) LOG_VAR( iP ) LOG_ERR_RET( NULL ) }
}

//-----------------------------------------------------------------------------
// Name: ResetVirtualCoords()
// Desc: 
//-----------------------------------------------------------------------------
CFace* CPolygonManager::F( const int iP, const int iF )
{
    TRACE_ENTER_FN( CPolygonManager::F )

    try { RET( P( iP )->F( iF ) ) }

    catch( ... ) { LOG_VAR( m_iLoadedPolygons ) LOG_VAR( iP ) LOG_VAR( iF ) LOG_ERR_RET( NULL ) }
}


//-----------------------------------------------------------------------------
// Name: ResetVirtualCoords()
// Desc: 
//-----------------------------------------------------------------------------
CVertex* CPolygonManager::V( const int iP, const int iF, const int iV )
{
    TRACE_ENTER_FN( CPolygonManager::V )

    try { RET( P( iP )->V( iF, iV ) ) }

    catch( ... ) { LOG_VAR( m_iLoadedPolygons ) LOG_VAR( iP ) LOG_VAR( iF ) LOG_VAR( iV ) LOG_ERR_RET( NULL ) }
}


//-----------------------------------------------------------------------------
// Name: SetV()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::SetV( const int iP, const int iF, const int iV, CVertex* pV )
{
    TRACE_ENTER_FN( CPolygonManager::SetV )

    Log( "CPolygonManager::SetV()\n" );

    TRACE_LEAVE_FN() return true;
}


//-----------------------------------------------------------------------------
// Name: SetF()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::SetF( const int iP, const int iF, CFace* pF )
{
    TRACE_ENTER_FN( CPolygonManager::SetF )

    if( !P( iP )->SetF( iF, pF ) ) LOG_ERR_RET( false )

    TRACE_LEAVE_FN() return true;
}


//-----------------------------------------------------------------------------
// Name: SetP()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::SetP( const int iPolygon, CPolygon* pPoly )
{
    TRACE_ENTER_FN( CPolygonManager::SetP )

    m_pPolygons[iPolygon] = pPoly;

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: DeleteSelectedPolygon()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::DeleteSelectedPolygon()
{
    TRACE_ENTER_FN( CPolygonManager::DeleteSelectedPolygon )

    int iIndex = 0;

    if( m_iLoadedPolygons <= 0 ) LOG_ERR_RET( true )

    // Make sure a polygon is selected
    if( !LU->PolySelected() ) LOG_ERR_RET( true )

    // Get the index of the polygon to remove
    int iP = LU->GetPoly();

    // If its already gone, skip deletion
    if( P( iP ) != NULL )
    {
        delete P( iP );
        SetP( iP, NULL );
    }

    CPolygon** pTemp = new CPolygon*[m_iLoadedPolygons];

    // See if our temporary list was created alright
    if( pTemp == NULL ) LOG_ERR_RET( false )

    for( iIndex = 0; iIndex < m_iLoadedPolygons; iIndex++ )
        pTemp[iIndex] = NULL;

    iIndex = 0;

    // Copy the non null polygons into a temporary list, without gaps
    for( iP = 0; iP < m_iLoadedPolygons; iP++ )
    {
        if( P( iP ) == NULL ) continue;

        pTemp[iIndex] = P( iP );
        iIndex++;
    }

    // Delete the origional list
    if( m_pPolygons != NULL ) { delete[] m_pPolygons; m_pPolygons = NULL; }

    m_iLoadedPolygons--;

    if( m_iLoadedPolygons <= 0 ) { LU->ResetLookupTable(); RET( true ) }

    // Create a new list, one less in length
    m_pPolygons = new CPolygon*[m_iLoadedPolygons];

    if( m_pPolygons == NULL ) LOG_ERR_RET( false )

    // Copy the temporary list to the real one
    for( iP = 0; iP < m_iLoadedPolygons; iP++ )
        SetP( iP, pTemp[iP] );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: GetTotalFaces()
// Desc: Returns the total number of faces in all normal polygons
//-----------------------------------------------------------------------------
int CPolygonManager::GetTotalFaces()
{
    TRACE_ENTER_FN( CPolygonManager::GetTotalFaces )

    int iTotal = 0;

    for( int a = 0; a < m_iLoadedPolygons; a++ )
        iTotal += P( a )->GetLoadedFaces();

    RET( iTotal );
}


//-----------------------------------------------------------------------------
// Name: Save()
// Desc: Saves the state of the polygon manager to the 'File'
//-----------------------------------------------------------------------------
bool CPolygonManager::Save( const char* File, ofstream &out )
{
    TRACE_ENTER_FN( CPolygonManager::Save )

    out << PM_BEGIN       << NEWLINE;
    out << PM_NUMPOLYGONS << ' ' << m_iLoadedPolygons << NEWLINE;
    out << PM_CURRENTMODE << ' ' << m_CurrentMode     << NEWLINE;
    out << PM_CLASSTYPE   << ' ' << (int)m_eClassType << NEWLINE;
    out << PM_END << NEWLINE;

    for( int iP = 0; iP < m_iLoadedPolygons; iP++ )
        P( iP )->Save( File, out );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Open()
// Desc: /***/
//-----------------------------------------------------------------------------
bool CPolygonManager::Open( const char* File, ifstream& in )
{
    TRACE_ENTER_FN( CPolygonManager::Open )

    int iTemp;
    char strToken[256];

    Reset();
    
    in >> strToken;

    if( strcmp( PM_BEGIN, strToken ) != 0 ) LOG_ERR_RET( false )

    in >> strToken;

    while( strcmp( PM_END, strToken ) != 0 )
    {
        if( !IsPolygonManagerToken( strToken ) ) LOG_ERR_RET( false )

/***********************
VERSION 1.0 TOKENS BEGIN
***********************/
        if( strcmp( PM_NUMPOLYGONS, strToken ) == 0 )
        {
            in >> m_iLoadedPolygons;
        }
        else if( strcmp( PM_CURRENTMODE, strToken ) == 0 )
        {
            in >> m_CurrentMode;
        }
        else if( strcmp( PM_CLASSTYPE, strToken ) == 0 )
        {
            in >> iTemp;
            m_eClassType = GetClassEnumType( iTemp );
        }
        else
        {
            Log( "Unknown PolygonManager Token: " ); Log( strToken ); Log( "\n" );
        }

        in >> strToken;
    }

    m_pPolygons = new CPolygon*[m_iLoadedPolygons];

    if( m_pPolygons == NULL ) LOG_ERR_RET( false )

    for( int iP = 0; iP < GetLoadedPolygons(); iP++ )
    {
        SetP( iP, new CPolygon() );

        if( P( iP ) == NULL ) LOG_ERR_RET( false )

        P( iP )->Open( File, in );
    }

    LU->ResetLookupTable();

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: ExpandFace()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::ExpandFace()
{
    TRACE_ENTER_FN( CPolygonManager::ExpandFace )

    if( LU->PolySelected() && LU->FaceSelected() )
    {
        CFace* pF = F( LU->GetPoly(), LU->GetFace() );

        if( pF == NULL ) LOG_ERR_RET( false )

        m_Brush.SetType( CUSTOMBRUSH, pF );
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: SlideFaceForward()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::SlideFaceForward()
{
    TRACE_ENTER_FN( CPolygonManager::SlideFaceForward )

    if( LU->PolySelected() && LU->FaceSelected() )
    {
        CFace* pF = F( LU->GetPoly(), LU->GetFace() );
        CPolygon* pP = P( LU->GetPoly() );
        int iV;
        VECT vOld;

        if( pF == NULL ) LOG_ERR_RET( false )
        if( pP == NULL ) LOG_ERR_RET( false )

        const int iNumVerts = pF->GetLoadedVerts();
        const VECT vN       = pF->GetFaceNormal();

        for( iV = 0; iV < iNumVerts; iV++ )
        {
            vOld = pF->V( iV )->GetVertPos();
            pF->V( iV )->GetVertPos() += vN * SE->m_fGeometry_MoveFactor;
            pP->MoveAllMatching( vOld, pF->V( iV )->GetVertPos() );
        }
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: SlideFaceBackward()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::SlideFaceBackward()
{
    TRACE_ENTER_FN( CPolygonManager::SlideFaceBackward )

    if( LU->PolySelected() && LU->FaceSelected() )
    {
        CFace* pF = F( LU->GetPoly(), LU->GetFace() );
        CPolygon* pP = P( LU->GetPoly() );
        int iV;
        VECT vOld;

        if( pF == NULL ) LOG_ERR_RET( false )
        if( pP == NULL ) LOG_ERR_RET( false )

        const int iNumVerts = pF->GetLoadedVerts();
        const VECT vN       = pF->GetFaceNormal();

        for( iV = 0; iV < iNumVerts; iV++ )
        {
            vOld = pF->V( iV )->GetVertPos();
            pF->V( iV )->GetVertPos() += vN * -SE->m_fGeometry_MoveFactor;
            pP->MoveAllMatching( vOld, pF->V( iV )->GetVertPos() );
        }
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Inflate()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::Inflate()
{
    TRACE_ENTER_FN( CPolygonManager::Inflate )

    if( LU->PolySelected() && LU->FaceSelected() )
    {
        CFace* pF = F( LU->GetPoly(), LU->GetFace() );
        CPolygon* pP = P( LU->GetPoly() );
        int iV;
        VECT vOld;

        if( pF == NULL ) LOG_ERR_RET( false )
        if( pP == NULL ) LOG_ERR_RET( false )

        const int iNumVerts = pF->GetLoadedVerts();
        const VECT vC       = pF->GetFaceCenter();

        for( iV = 0; iV < iNumVerts; iV++ )
        {
            vOld = pF->V( iV )->GetVertPos();
            pF->V( iV )->GetVertPos() += ( pF->V( iV )->GetVertPos() - vC ) *  SE->m_fGeometry_MoveFactor;
            pP->MoveAllMatching( vOld, pF->V( iV )->GetVertPos() );
        }
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Deflate()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::Deflate()
{
    TRACE_ENTER_FN( CPolygonManager::Deflate )

    if( LU->PolySelected() && LU->FaceSelected() )
    {
        CFace*    pF = F( LU->GetPoly(), LU->GetFace() );
        CPolygon* pP = P( LU->GetPoly() );
        int iV;
        VECT vOld;

        if( pF == NULL ) LOG_ERR_RET( false )
        if( pP == NULL ) LOG_ERR_RET( false )

        const int iNumVerts = pF->GetLoadedVerts();
        const VECT vC       = pF->GetFaceCenter();

        for( iV = 0; iV < iNumVerts; iV++ )
        {
            vOld = pF->V( iV )->GetVertPos();
            pF->V( iV )->GetVertPos() -= ( pF->V( iV )->GetVertPos() - vC ) * SE->m_fGeometry_MoveFactor;
            pP->MoveAllMatching( vOld, pF->V( iV )->GetVertPos() );
        }
    }

    RET( true )
}



//-----------------------------------------------------------------------------
// Name: RotateLeft()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::RotateLeft()
{
    TRACE_ENTER_FN( CPolygonManager::RotateLeft )

    if( LU->PolySelected() && LU->FaceSelected() )
    {
        CFace*    pF = F( LU->GetPoly(), LU->GetFace() );
        CPolygon* pP = P( LU->GetPoly() );
        int iV;
        VECT vOld;

        if( pF == NULL ) LOG_ERR_RET( false )
        if( pP == NULL ) LOG_ERR_RET( false )

        const int iNumVerts = pF->GetLoadedVerts();
        const VECT vC       = pF->GetFaceCenter();
        const VECT vN       = pF->GetFaceNormal();

        for( iV = 0; iV < iNumVerts; iV++ )
        {
            vOld = pF->V( iV )->GetVertPos();

            pF->V( iV )->GetVertPos() = RotPtByLn( pF->V( iV )->GetVertPos(), SE->m_fGeometry_MoveFactor, vC, vN );

            pP->MoveAllMatching( vOld, pF->V( iV )->GetVertPos() );
        }
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: RotateRight()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::RotateRight()
{
    TRACE_ENTER_FN( CPolygonManager::RotateRight )

    if( LU->PolySelected() && LU->FaceSelected() )
    {
        CFace*    pF = F( LU->GetPoly(), LU->GetFace() );
        CPolygon* pP = P( LU->GetPoly() );
        int iV;
        VECT vOld;

        if( pF == NULL ) LOG_ERR_RET( false )
        if( pP == NULL ) LOG_ERR_RET( false )

        const int iNumVerts = pF->GetLoadedVerts();
        const VECT vC       = pF->GetFaceCenter();
        const VECT vN       = pF->GetFaceNormal();

        for( iV = 0; iV < iNumVerts; iV++ )
        {
            vOld = pF->V( iV )->GetVertPos();
            pF->V( iV )->GetVertPos() = RotPtByLn( pF->V( iV )->GetVertPos(), -SE->m_fGeometry_MoveFactor, vC, vN );
            pP->MoveAllMatching( vOld, pF->V( iV )->GetVertPos() );
        }
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: AlignAllTextures()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::AlignAllTextures()
{
    TRACE_ENTER_FN( CPolygonManager::AlignAllTextures )

    int iP, iF;

    for( iP = 0; iP < GetLoadedPolygons(); iP++ )
        for( iF = 0; iF < P( iP )->GetLoadedFaces(); iF++ )
            F( iP, iF )->GenerateTextureCoordinates();

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: ComputeVertexConnections()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::ComputeVertexConnections()
{
    TRACE_ENTER_FN( CPolygonManager::ComputeVertexConnections )

    SnapVertsToGrid();

    for( int iP = 0; iP < GetLoadedPolygons(); iP++ ) {
        for( int iF = 0; iF < P( iP )->GetLoadedFaces(); iF++ ) {
            for( int iV = 0; iV < F( iP, iF )->GetLoadedVerts(); iV++ ) {
                CVertex* pV = V( iP, iF, iV );
                SearchAndConnectVerts( pV, iP );
            }
        }
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: SearchAndConnectVerts()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::SearchAndConnectVerts( CVertex* pV, const int iP1 )
{
    TRACE_ENTER_FN( CPolygonManager::SearchAndConnectVerts )

    VECT v1 = pV->GetVertPos();
    VECT v2;

    pV->ClearAllVertexConnections();

    for( int iP = 0; iP < GetLoadedPolygons(); iP++ )
    {
        if( iP == iP1 ) continue;

        for( int iF = 0; iF < P( iP )->GetLoadedFaces(); iF++ )
        {
            for( int iV = 0; iV < F( iP, iF )->GetLoadedVerts(); iV++ )
            {
                v2 = V( iP, iF, iV )->GetVertPos();
                
                if( v1 == v2 )
                {
                    if( strcmp( pV->GetName(), V( iP, iF, iV )->GetName() ) != 0 )
                        pV->AddVertexConnection( V( iP, iF, iV ), iP, iF );
                }
            }
        }
    }

    RET( false )
}


//-----------------------------------------------------------------------------
// Name: RemoveCoencidingFaces()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::RemoveCoencidingFaces()
{
    TRACE_ENTER_FN( CPolygonManager::RemoveCoencidingFaces )

    int iP, iF;

    SnapVertsToGrid();

    try
    {
        for( iP = 0; iP < GetLoadedPolygons(); iP++ )
        {
            for( iF = 0; iF < P( iP )->GetLoadedFaces(); iF++ )
            {
                SearchAndRemoveFaces( F( iP, iF ), iP, iF );
            }
        }
    } 
    catch( ... ) { LOG_VAR( iP ) LOG_VAR( iF ) LOG_ERR_RET( false ) }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: SearchAndRemoveFaces()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::SearchAndRemoveFaces( CFace* pF,
                                            const int iCurPoly,
                                            const int iCurFace )
{
    TRACE_ENTER_FN( CPolygonManager::SearchAndRemoveFaces )
    
    int iP = 0, iF = 0;

    if( pF == NULL ) { LOG_VAR( iP ) LOG_VAR( iF ) LOG_ERR_RET( false ) }

    try
    {
        for( iP = 0; iP < GetLoadedPolygons(); iP++ )
        {
            CPolygon* pP = P( iP );

            if( pP == NULL ) { LOG_VAR( iP ) LOG_VAR( iF ) LOG_ERR_RET( false ) }

            for( iF = 0; iF < pP->GetLoadedFaces(); iF++ )
            {
                CFace* pF = F( iP, iF );

                if( pF == NULL ) { LOG_VAR( iP ) LOG_VAR( iF ) LOG_ERR_RET( false ) }

                if( pF->IsCoenciding( pF ) )
                {
                    const int iPoly = LU->GetPoly();
                    const int iFace = LU->GetFace();

                    LU->SetPoly( iP );
                    LU->SetFace( iF );

                    DeleteSelectedFace();

                    LU->SetPoly( iCurPoly );
                    LU->SetFace( iCurFace );

                    DeleteSelectedFace();

                    LU->SetPoly( iPoly );
                    LU->SetFace( iFace );
                }
            }
        }
    } 
    catch( ... ) { LOG_VAR( iP ) LOG_VAR( iF ) LOG_ERR_RET( false ) }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: SnapVertsToGrid()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::SnapVertsToGrid()
{
    TRACE_ENTER_FN( CPolygonManager::SnapVertsToGrid )

    int iP, iF, iV;

    try
    {
        for( iP = 0; iP < GetLoadedPolygons(); iP++ )
        {
            for( iF = 0; iF < P( iP )->GetLoadedFaces(); iF++ )
            {
                for( iV = 0; iV < F( iP, iF )->GetLoadedVerts(); iV++ )
                {
                    CVertex* pV = V( iP, iF, iV );

                    if( pV == NULL ) LOG_ERR_RET( false )

                    pV->SnapVertsToGrid();
                }
            }
        }
    }
    catch( ... ) { LOG_VAR( iP ) LOG_VAR( iF ) LOG_VAR( iV ) LOG_ERR_RET( false ) }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: RecalculateGeometry()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::RecalculateGeometry()
{
    TRACE_ENTER_FN( CPolygonManager::RecalculateGeometry )

    if( !RemoveCoencidingFaces() )    LOG_ERR_RET( false )
    if( !ComputeVertexConnections() ) LOG_ERR_RET( false )
    if( !AlignAllTextures() )         LOG_ERR_RET( false )

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: RebuildGeometry()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::RebuildGeometry()
{
    TRACE_ENTER_FN( CPolygonManager::RebuildGeometry )

    for( int iP = 0; iP < GetLoadedPolygons(); iP++ )
    {
        P( iP )->Rebuild();
    }

    RET( true )
}