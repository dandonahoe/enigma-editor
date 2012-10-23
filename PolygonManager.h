//----------------------------------------------------------------------------- 
// File: PolygonManager.h
//
// Desc: A singleton polygon manager that holds all geometry data for the world
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _POLYGONMANAGER_H_
#define _POLYGONMANAGER_H_
 

//-----------------------------------------------------------------------------
// Includes
#include <commctrl.h>
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include <fstream>
#include "glext.h"
#include <stdio.h>
#include "Polygon.h"
#include "Brush.h"
#include "TextureManager.h"
#include "Window.h"
#include "enum.h"
#include "Singleton.h"
#include "PolygonLookup.h"
#include "Vertex.h"
#include "PolygonLookup.h"
//-----------------------------------------------------------------------------

using namespace std;
//-----------------------------------------------------------------------------
// Name: class CPolygonManager
// Desc: Manages all polygon information about the world. Also updates the
//       polygon tree under the polygon tab on command. This is a singleton,
//       defined once in the application class's declaration file. To use it,
//       anywhere in the program, use 'PolygonManager->' after including this
//       file.
//-----------------------------------------------------------------------------
class CPolygonManager : public CSingleton<CPolygonManager>
{
private:

    typedef struct _TREEINFO
    {
        HTREEITEM hPolygonManager;
        CLASSTYPE eCurrentType;
        HWND      hDlgParent;
        bool      bSelected;
        HWND      hTreeWnd;
        int       iPoly;
        int       iVert;
        HTREEITEM hZone;
        int       iFace;
    } TREEINFO, *LPTREEINFO;

public:
    CPolygonManager();
    virtual ~CPolygonManager();
    bool Release();
    void Reset();
    bool Init();


    // Update Operations
    inline void FrameMoveCapitalPressed( const float fElapsedTime );
    inline void FrameMoveShiftPressed( const float fElapsedTime );
    inline void FrameMoveCtrlPressed( const float fElapsedTime );
    void TreeSelected( const bool b ) { m_Tree.bSelected = b; }
    int GetLoadedPolygons() const { return m_iLoadedPolygons; }
    void FrameMoveGeometry(  const float fElapsedTime );
    void FrameMoveQPressed( const float fElapsedTime );
    void FrameMoveLPressed( const float fElapsedTime );
    void FrameMoveFPressed( const float fElapsedTime );
    void FrameMove( const float fElapsedTime );
    CBrush* Brush() { return &m_Brush; }
    bool Render( CWindow* pWindow );
    void NewTreeSelection();


    // Safter means of accessing data
    CVertex*  V( const int iPolygon, const int iFace, const int iVert );
    CFace*    F( const int iPolygon, const int iFace );
    CPolygon* P( const int iPolygon );
    bool SetV( const int iPolygon, const int iFace, const int iVert, CVertex* pVert );
    bool SetF( const int iPolygon, const int iFace, CFace* pFace );
    bool SetP( const int iPolygon, CPolygon* pPoly );

    // Brush Operations
    void NewBrush( const BRUSHTYPE type );
    bool ForgeBrush();
    

    // Manager State Operations
    bool Save( const char* File, ofstream &out );
    bool Open( const char* File, ifstream &in );
    

    // Dialog Operations
    bool FillDialogs( HWND hMain, HWND hMini, CLASSTYPE eType );
    bool UpdateTreeSelection( HWND hWnd );
    void FillPolygonTreeView( HWND hWnd );
    bool FillPolygonDialog( HWND hMini );
    bool FillFaceDialog( HWND hMini );


    // Class Operations
    CLASSTYPE GetClassType() const { return m_eClassType; }
    bool ApplyNewName( char* str, CLASSTYPE eType );
    CLASSTYPE GetAssociatedClassType( char* str );
    

    // Vertex Operations
    bool GetVertexInfo( int &r, int &g, int &b, int &u, int &v );
    bool SetVertexInfo( int r, int g, int b, int u, int v );
    bool TranslateVertX( const float amount );
    bool TranslateVertY( const float amount );
    bool TranslateVertZ( const float amount );
    bool SelectNextVertex();

    // Face Operations
    void ApplyTextureToSelected( const HTEXTURE hTexture );
    void SetTranslucent( const bool bTranslucent );
    bool TranslateFaceX( const float amount );
    bool TranslateFaceY( const float amount );
    bool TranslateFaceZ( const float amount );
    void TranslateTexU( const float amount );
    void TranslateTexV( const float amount );
    void SetScale( const float factor );
    void SetPanU( const bool bPan );
    void SetPanV( const bool bPan );
    bool GenerateTextureCoords();
    bool DeleteSelectedFace();
    bool GetPanU();
    bool GetPanV();
    int GetTotalFaces();
    void FlipTexU();
    void FlipTexV();
    int NextFace();
    bool GetUseTexture1();
    bool GetUseTexture2();
    bool GetUseLightmap();
    bool SetUseTexture1( const bool b );
    bool SetUseTexture2( const bool b );
    bool SetUseLightmap( const bool b );
    void RotateTex90();
    void RotateLightmap90();
    bool ExpandFace();
    bool SlideFaceForward();
    bool SlideFaceBackward();
    bool Inflate();
    bool Deflate();
    bool RotateLeft();
    bool RotateRight();
    // Polygon Operations
    int GetNumPolygons() { return m_iLoadedPolygons; }
    bool TranslatePolyX( const float amount );
    bool TranslatePolyY( const float amount );
    bool TranslatePolyZ( const float amount ); 
    bool DeleteSelectedPolygon();
    bool AlignPolygonTextures();
    bool ResetVirtualCoords();

    bool AlignAllTextures();
    bool ComputeVertexConnections();
    bool RemoveCoencidingFaces();
    bool SnapVertsToGrid();
    bool RecalculateGeometry();

    bool RebuildGeometry();

//private:

    // Rendering Operations
    inline bool Render3DFaceTriangle( int polygon, int face );
    inline bool Render2DFaceTriangle( int polygon, int face );
    inline bool Render3DFaceQuad( int polygon, int face );
    inline bool Render2DFaceQuad( int polygon, int face );
    inline bool Render3DFace( int polygon, int face );
    inline bool Render2DFace( int polygon, int face );
    inline bool RenderSelected();
    inline bool Render3DScene();
    inline bool Render2DScene();
    inline bool Render3D();
    inline bool RenderXZ();
    inline bool RenderXY();
    inline bool RenderYZ();
    bool Render3DPolygons();
    bool Render3DPolygonNormals();
    bool SearchAndConnectVerts( CVertex* pV, const int iP );
    bool SearchAndRemoveFaces( CFace* pF, const int iP, const int iF );

    // Data members
    int            m_iLoadedPolygons;
    GLUquadricObj* m_pQuadricObj;
    GLenum         m_CurrentMode;
    CLASSTYPE      m_eClassType;
    CPolygon**     m_pPolygons;
    CBrush         m_Brush;
    TREEINFO       m_Tree;

    PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
};


// Makes things easier when using the singleton
#define PolygonManager CPolygonManager::GetSingletonPtr()
#define PM CPolygonManager::GetSingletonPtr()


#endif
