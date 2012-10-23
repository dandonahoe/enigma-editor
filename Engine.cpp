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
#include <fstream>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include <commctrl.h>
#include "Application.h"
#include "MessageProcs.h"
#include "Engine.h"
#include "Vertex.h"
#include "main.h"
#include "dinput.h"
#include "object.h"
#include "skybox.h"
#include "Profile.h"
#include "Audio.h"
#include "resource.h"
#include "Tracer.h"
#include "Map.h"
//-----------------------------------------------------------------------------
using namespace std;

//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
CEngine::CEngine()
{
    LOG( "CEngine::CEngine()" )

    m_fElapsedTime = 0.0f;

    m_hTabControl    = NULL;
    m_hTabDialog     = NULL;

    m_hBMPTriangle   = NULL;
    m_hBMPSheet      = NULL;
    m_hBMPCube       = NULL;
    m_hBMPAddBrush   = NULL;
    m_hBMPNextVertex = NULL;
    m_hBMPGray       = NULL;
    m_hBMPStairBrush = NULL;

    m_hGeorgiaFont   = NULL;
}

CEngine::~CEngine()
{
    LOG( "CEngine::~CEngine()" )

    // Kill windows
    DestroyWindow( m_hTabDialog );
    DestroyWindow( m_hButtonsDB );


    // Free Bitmaps
    DeleteObject( m_hBMPTriangle   );
    DeleteObject( m_hBMPSheet      );
    DeleteObject( m_hBMPCube       );
    DeleteObject( m_hBMPAddBrush   );
    DeleteObject( m_hBMPNextVertex );
    DeleteObject( m_hBMPGray       );
    DeleteObject( m_hBMPStairBrush );

    // Fonts
    DeleteObject( m_hGeorgiaFont );
}


//-----------------------------------------------------------------------------
// Name: Init()
// Desc: 
//-----------------------------------------------------------------------------
bool CEngine::Init( HINSTANCE hInstance, bool bFullScreen, char* strAppName, double Version )
{
    TRACE_ENTER_FN( CEngine::Init )

    if( strAppName != NULL )
        sprintf( strAppName, "%s %f", strAppName, Version );

    strAppName[12] = '\0';

    m_EngineInfo.EngineTitle = strAppName;
    m_EngineInfo.Version = Version;

    if( !m_Grid.Init() ) LOG_ERR_RET( false )

    GL->Init( hInstance, bFullScreen, strAppName, m_3DView );
    PM->Init();
    IM->Init( GL->GetWnd() );
  //  AM->Init();
    LM->Init();
    TM->Init();

    TRACE_LEAVE_FN()

    return TRUE;
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: 
//-----------------------------------------------------------------------------
bool CEngine::Render()
{
    TRACE_ENTER_FN( CEngine::Render )

    // This probably shouldne be here, but the following 'if' statement is run
    // the first time the scene is going to be rendered
    static bool FirstTime = true;

    if( FirstTime )
    {
        FirstTime = false;
        ResetViews();
        CreateTabControl();

        // Prepares each window to be drawn in. This allows fast switching between
        // windows
        GL->SetupRenderWindow( m_2DLeft   );
        GL->SetupRenderWindow( m_2DCenter );
        GL->SetupRenderWindow( m_2DRight  );

        // Start out with a cube brush
        PM->NewBrush( CUBEBRUSH );
    }

    // Handle to the currently active window
    HWND hActiveWindow = GetCapture();

    // Update the window who's handle matches the active one
    if( Settings->m_bEditor_3DViewActive     || m_3DView.GetWnd()   == hActiveWindow )
        RenderView( m_3DView );

    if( Settings->m_bEditor_LeftViewActive   || m_2DLeft.GetWnd()   == hActiveWindow  )
        RenderView( m_2DLeft );

    if( Settings->m_bEditor_CenterViewActive || m_2DCenter.GetWnd() == hActiveWindow  )
        RenderView( m_2DCenter );

    if( Settings->m_bEditor_RightViewActive  || m_2DRight.GetWnd()  == hActiveWindow  )
        RenderView( m_2DRight );

    GL->FocusOnWindow( m_3DView );

//    AM->SetListenerPos( m_3DView.GetCamera(), m_3DView.GetCamera()->GetUpVec() );

//    AM->Render();

    TRACE_LEAVE_FN() return true;
}


//-----------------------------------------------------------------------------
// Name: ResizeWindow()
// Desc: 
//-----------------------------------------------------------------------------
bool CEngine::ResizeWindow( LPARAM width, WPARAM height )
{
    AlignViews();

    OpenGL->SizeOpenGLScreen( m_3DView );

    m_MainHorizontal.AlignOtherPanes();
    m_BottomRight.AlignOtherPanes();
    m_BottomLeft.AlignOtherPanes();

    return TRUE;
}


//-----------------------------------------------------------------------------
// Name: Repaint()
// Desc: 
//-----------------------------------------------------------------------------
bool CEngine::Repaint( WPARAM wParam, LPARAM lParam )
{
    OpenGL->Repaint( wParam, lParam );

    return true;
}


//-----------------------------------------------------------------------------
// Name: Save()
// Desc: 
//-----------------------------------------------------------------------------
bool CEngine::Save( const char* strFile )
{
    ofstream out;

    out.open( strFile );

    LM->Save( strFile, out );
    PM->Save( strFile, out );

    out.close();

    return true;
}


//-----------------------------------------------------------------------------
// Name: Save()
// Desc: 
//-----------------------------------------------------------------------------
bool CEngine::Open( const char* strFile )
{
    ifstream in;

    in.open( strFile );

    TM->Release();
    TM->Init();

    LM->Open( strFile, in );
    PM->Open( strFile, in );

	in.close();

    Log( "LEVEL LOAD COMPLETE\n" );
    return true;
}

//-----------------------------------------------------------------------------
// Name: LeftClick()
// Desc: 
//-----------------------------------------------------------------------------
bool CEngine::LeftClick( int x, int y )
{
    int Selected = 0; 

    if( IM->KeyPressed( DIK_LSHIFT ) )
        return false;

    HWND hActiveWindow = GetCapture();

    if( m_3DView.GetWnd() == hActiveWindow )
        LU->SetSelectedPolygon( GetObjectID( x, y, m_3DView ) );

    if( m_2DLeft.GetWnd() == hActiveWindow )
        LU->SetSelectedPolygon( GetObjectID( x, y, m_2DLeft ) );

    if( m_2DCenter.GetWnd() == hActiveWindow )
        LU->SetSelectedPolygon( GetObjectID( x, y, m_2DCenter ) );

    if( m_2DRight.GetWnd() == hActiveWindow )
        LU->SetSelectedPolygon( GetObjectID( x, y, m_2DRight ) );

    return true;
}


//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Updates all the data of the engine
//-----------------------------------------------------------------------------
bool CEngine::FrameMove( const float fElapsedTime )
{
    TRACE_ENTER_FN( CEngine::FrameMove )

    // Save the elapsed time so that it doesnt have to be passed to member
    // functions of the CEngine class
    m_fElapsedTime = fElapsedTime;

    // Update the state of all input devices
    IM->UpdateInput();

    // Update the polygon manager
    PM->FrameMove( m_fElapsedTime );

    // Update each view's camera
    UpdateViews( m_fElapsedTime );

    TRACE_LEAVE_FN()

    return true;
}


//-----------------------------------------------------------------------------
// Name: UpdateViews()
// Desc: Moves the cameras for the currently active view if there is one
//-----------------------------------------------------------------------------
void CEngine::UpdateViews( float fElapsedTime )
{
    // If both mouse buttons are up, then no view is selected
    if( !InputManager->GetMouseButtonLeft() && !InputManager->GetMouseButtonRight() )
    {
        ReleaseCapture();
        ShowCursor( true );
        return;
    }

    // Handle to the window thats active right now
    HWND hActiveWindow = GetCapture();

    // Update the view who's handle matches the active handle
    if( m_3DView.GetWnd() == hActiveWindow )
    {
        m_3DView.GetCamera()->FrameMove( fElapsedTime, m_3DView.GetViewType() );
    }
    else if( m_2DLeft.GetWnd() == hActiveWindow  )
    {
        if( m_2DLeft.GetCamera()->FrameMove( fElapsedTime, m_2DLeft.GetViewType() ) )
            m_2DLeft.SetRequiresUpdate( true );
    }
    else if( m_2DCenter.GetWnd() == hActiveWindow  )
    {
        if( m_2DCenter.GetCamera()->FrameMove( fElapsedTime, m_2DCenter.GetViewType() ) )
            m_2DCenter.SetRequiresUpdate( true );
    }
    else if( m_2DRight.GetWnd() == hActiveWindow  )
    {
        if( m_2DRight.GetCamera()->FrameMove( fElapsedTime, m_2DRight.GetViewType() ) )
            m_2DRight.SetRequiresUpdate( true );
    }
}


//-----------------------------------------------------------------------------
// Name: GetHWND()
// Desc: 
//-----------------------------------------------------------------------------
HWND CEngine::GetWnd() const
{
    return GL->GetWnd();
}


//---------------------------------------------------------------------
//Name: OnWMCreate()
//Desc: 
//---------------------------------------------------------------------
void CEngine::OnWMCreate( HWND hWnd )
{
    RECT rc;
    GetClientRect( hWnd , &rc );

    m_SideBarWindow.Init( hWnd, 0, 0, 250, rc.bottom - 20, WS_CHILD );
    m_SideBarWindow.SetViewType( VIEW_OTHER );

    m_ViewsWindow.Init( hWnd, 250, 0, rc.right - 250, rc.bottom - 20, WS_CHILD );
    m_ViewsWindow.SetViewType( VIEW_OTHER );

    HWND hViews = m_ViewsWindow.GetWnd();
    GetClientRect( hViews , &rc );

    // Initialize Each Bar
    m_MainHorizontal.Init( hViews, rc.bottom / 2,        CWP_HORIZONTAL );
    m_BottomLeft.Init(     hViews, rc.right / 3,         CWP_VERTICAL );
    m_BottomRight.Init(    hViews, ( rc.right / 3 ) * 2, CWP_VERTICAL );

    //Set their constraints to be done automatically
    m_MainHorizontal.AddPaneToManage( m_BottomLeft.GetThis(), CWP_STAYBELOW );
    m_MainHorizontal.AddPaneToManage( m_BottomRight.GetThis(), CWP_STAYBELOW );

    m_BottomLeft.AddPaneToManage( m_BottomRight.GetThis(), CWP_STAYRIGHT );
    m_BottomRight.AddPaneToManage( m_BottomLeft.GetThis(), CWP_STAYLEFT );

    m_3DView.Init( hViews, 0, 0, m_ViewsWindow.GetWidth(), m_ViewsWindow.GetHeight() / 2, WS_CHILD );

    m_2DLeft.Init( hViews, 0, m_MainHorizontal.GetY() + m_MainHorizontal.GetHeight(),
                   m_BottomLeft.GetX(), rc.bottom - m_MainHorizontal.GetY() + m_MainHorizontal.GetHeight(),
                   WS_CHILD );

    m_2DCenter.Init( hViews, m_BottomLeft.GetX() + m_BottomLeft.GetWidth(), m_MainHorizontal.GetY() + m_MainHorizontal.GetHeight(),
                     m_BottomRight.GetX() - m_BottomLeft.GetX() - m_BottomRight.GetWidth(), rc.bottom - m_MainHorizontal.GetY() + m_MainHorizontal.GetHeight(),
                     WS_CHILD );

    m_2DRight.Init( hViews, m_BottomRight.GetX() + m_BottomRight.GetWidth(), m_MainHorizontal.GetY() + m_MainHorizontal.GetHeight(),
                             rc.right - m_BottomRight.GetX(), rc.bottom - m_MainHorizontal.GetY() + m_MainHorizontal.GetHeight(),
                             WS_CHILD );


    m_3DView.SetDrawObjects( false );
    m_3DView.SetViewType( VIEW_3D );
    m_3DView.SetClearFlags( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_2DLeft.SetDrawObjects( false );
    m_2DLeft.SetViewType( VIEW_XZ );
    m_2DLeft.GetCamera()->SetFromVec( 0, 10, 0 );
    m_2DLeft.GetCamera()->SetToVec( 0, 0, 0 );
    m_2DLeft.GetCamera()->SetUpVec( 0, 0, -1 );
    m_2DLeft.SetClearFlags( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_2DCenter.SetDrawObjects( false );
    m_2DCenter.SetViewType( VIEW_XY );
    m_2DCenter.GetCamera()->SetFromVec( 0, 0, 10 );
    m_2DCenter.GetCamera()->SetToVec( 0, 0, 0 );
    m_2DCenter.GetCamera()->SetUpVec( 0, 1, 0 );
    m_2DCenter.SetClearFlags( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_2DRight.SetDrawObjects( false );
    m_2DRight.SetViewType( VIEW_YZ );
    m_2DRight.GetCamera()->SetFromVec( 10, 0, 0 );
    m_2DRight.GetCamera()->SetToVec( 0, 0, 0 );
    m_2DRight.GetCamera()->SetUpVec( 0, 1, 0 );
    m_2DRight.SetClearFlags( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}


//---------------------------------------------------------------------
//Name: ResetViews()
//Desc: 
//---------------------------------------------------------------------
void CEngine::ResetViews()
{
    RECT rc;
    GetClientRect( m_ViewsWindow.GetWnd(), &rc );

    m_MainHorizontal.SetY( rc.bottom / 2 );
    m_MainHorizontal.SetWidth( rc.right );
    m_MainHorizontal.MoveWnd();
    m_MainHorizontal.AlignOtherPanes();

    m_BottomLeft.SetX( ( rc.right - rc.left ) / 3 );
    m_BottomLeft.MoveWnd();

    m_BottomRight.SetX( ( ( rc.right - rc.left ) / 3 ) * 2 );
    m_BottomRight.MoveWnd();

    AlignViews();
}


//---------------------------------------------------------------------
//Name: AlignViews()
//Desc: 
//---------------------------------------------------------------------
void CEngine::AlignViews()
{
    TRACE_ENTER_FN( CEngine::AlignViews )

//    Log( "COpenGL::AlignViews()\n" );

    RECT rc;

    GetClientRect( GL->GetWnd(), &rc );


    m_ViewsWindow.SetWidth( rc.right - m_SideBarWindow.GetWidth() );
    m_ViewsWindow.SetHeight( rc.bottom - 20 );
    m_ViewsWindow.MoveWnd();


    m_3DView.SetWidth( m_ViewsWindow.GetWidth() );
    m_3DView.SetHeight( m_MainHorizontal.GetY() - 1 );
    m_3DView.MoveWnd();

    GetClientRect( m_ViewsWindow.GetWnd(), &rc );

    m_2DLeft.SetY( m_MainHorizontal.GetY() + m_MainHorizontal.GetHeight() );
    m_2DLeft.SetWidth( m_BottomLeft.GetX() );
    m_2DLeft.SetHeight( rc.bottom - m_MainHorizontal.GetY() + m_MainHorizontal.GetHeight() - 20 );
    m_2DLeft.MoveWnd();


    m_2DCenter.SetX( m_BottomLeft.GetX() + m_BottomLeft.GetWidth() );
    m_2DCenter.SetY( m_MainHorizontal.GetY() + m_MainHorizontal.GetHeight() );
    m_2DCenter.SetWidth( m_BottomRight.GetX() - m_BottomLeft.GetX() - m_BottomRight.GetWidth() );
    m_2DCenter.SetHeight( rc.bottom - m_MainHorizontal.GetY() + m_MainHorizontal.GetHeight() - 20 );
    m_2DCenter.MoveWnd();


    m_2DRight.SetX( m_BottomRight.GetX() + m_BottomRight.GetWidth() );
    m_2DRight.SetY( m_MainHorizontal.GetY() + m_MainHorizontal.GetHeight() );
    m_2DRight.SetWidth( rc.right - m_BottomRight.GetX() - m_BottomRight.GetWidth() );
    m_2DRight.SetHeight( rc.bottom - m_MainHorizontal.GetY() + m_MainHorizontal.GetHeight() - 20 );
    m_2DRight.MoveWnd();


    TRACE_LEAVE_FN()
}

/*
//-----------------------------------------------------------------------------
// Name: CreateSideBar()
// Desc: 
//-----------------------------------------------------------------------------
bool CEngine::CreateSideBar()
{
    HINSTANCE m_hInstance = OpenGL->GetInstance();

    m_hBMPTriangle   = LoadBitmap( m_hInstance, "IDB_TRIANGLE"   );
    m_hBMPSheet      = LoadBitmap( m_hInstance, "IDB_SHEET"      );
    m_hBMPCube       = LoadBitmap( m_hInstance, "IDB_CUBE"       );
    m_hBMPAddBrush   = LoadBitmap( m_hInstance, "IDB_ADDBRUSH"   );
    m_hBMPNextVertex = LoadBitmap( m_hInstance, "IDB_NEXTVERTEX" );
    m_hBMPGray       = LoadBitmap( m_hInstance, "IDB_GRAY"       );
    m_hBMPStairBrush = LoadBitmap( m_hInstance, "IDB_STAIRBRUSH" );


    SendMessage( (HWND) m_hBtnSheet,      BM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM)m_hBMPSheet      );
    SendMessage( (HWND) m_hBtnCube,       BM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM)m_hBMPCube       );
    SendMessage( (HWND) m_hBtnAddBrush,   BM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM)m_hBMPAddBrush   );
    SendMessage( (HWND) m_hBtnNextVertex, BM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM)m_hBMPNextVertex );
    SendMessage( (HWND) m_hBtnAddStairs,  BM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM)m_hBMPStairBrush );

    SendDlgItemMessage( m_hButtonsDB, IDC_BUTTONSDB_NEXTTRIANGLE,  BM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM)m_hBMPTriangle );

    SendDlgItemMessage( m_hButtonsDB, IDC_BUTTONSDB_NEXTFACE,      BM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) );
    SendDlgItemMessage( m_hButtonsDB, IDC_BUTTONSDB_NEXTVERTEX,    BM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) );
    SendDlgItemMessage( m_hButtonsDB, IDC_BUTTONSDB_NEXTSEGMENT,   BM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) );
    SendDlgItemMessage( m_hButtonsDB, IDC_BUTTONSDB_SUBTRACTBRUSH, BM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) );
    SendDlgItemMessage( m_hButtonsDB, IDC_BUTTONSDB_ADDBRUSH,      BM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) );
    SendDlgItemMessage( m_hButtonsDB, IDC_BUTTONSDB_EXPANDFACE,    BM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) );



    return true;
}

*/
//-----------------------------------------------------------------------------
// Name: RenderView()
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::RenderView( CWindow &pWindow )
{
    TRACE_ENTER_FN( CEngine::RenderView )

    GL->FocusOnWindow( pWindow );

    GL->BeginScene( pWindow );

    pWindow.GetCamera()->Render();

    glDisable( GL_TEXTURE_2D );
    m_Grid.Render( pWindow.GetViewType() );
    glEnable( GL_TEXTURE_2D );

    PM->Render( &pWindow );
            
    GL->EndScene();

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: GetObjectID()
// Desc: Returns the ID of
//-----------------------------------------------------------------------------
int CEngine::GetObjectID( int x, int y, CWindow &pWindow )
{
    switch( pWindow.GetViewType() )
    {
    case VIEW_3D: return GetObject3D( x, y, pWindow ); break;
    case VIEW_XZ: 
    case VIEW_XY: 
    case VIEW_YZ: return GetObject2D( x, y, pWindow ); break;
    default: Log( "Unknown View type @ CEngine::GetObjectID()\n" ); break;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: GetObject3D()
// Desc: 
//-----------------------------------------------------------------------------
int CEngine::GetObject3D( const int x, const int y, CWindow &pWindow )
{
    OpenGL->FocusOnWindow( pWindow );

    int objectsFound = 0;
    int viewportCoords[4] = {0};
    unsigned int selectBuffer[32] = {0};

    glSelectBuffer( 32, selectBuffer );
    glGetIntegerv( GL_VIEWPORT, viewportCoords );
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glRenderMode( GL_SELECT );
    glLoadIdentity();
    gluPickMatrix( x, viewportCoords[3] - y, 2, 2, viewportCoords );

    gluPerspective( 45.0f, (float)pWindow.GetWidth() / (float)pWindow.GetHeight(), 0.1f, 250.0f );
    glMatrixMode( GL_MODELVIEW );
    
    //Render the polygons for this view
    OpenGL->BeginScene( pWindow );
    pWindow.GetCamera()->Render();
    PolygonManager->Render( &pWindow );
    OpenGL->EndScene();
    //
    objectsFound = glRenderMode( GL_RENDER );
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );

    if( objectsFound > 0 )
    {
        unsigned int lowestDepth = selectBuffer[1];
        int selectedObject       = selectBuffer[3];

        for( int i = 1; i < objectsFound; i++ )
        {
            if( selectBuffer[(i * 4) + 1] < lowestDepth )
            {
                lowestDepth = selectBuffer[( i * 4 ) + 1];
                selectedObject = selectBuffer[( i * 4 ) + 3];
            }
        }
        
        return selectedObject;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: GetObject2D()
// Desc: 
//-----------------------------------------------------------------------------
int CEngine::GetObject2D( const int x, const int y, CWindow &pWindow )
{
    int objectsFound = 0;
    int viewportCoords[4] = {0};
    unsigned int selectBuffer[32] = {0};
    VECT vFrom = pWindow.GetCamera()->GetFromVec();

    OpenGL->FocusOnWindow( pWindow );

    glSelectBuffer( 32, selectBuffer );
    glGetIntegerv( GL_VIEWPORT, viewportCoords );
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glRenderMode( GL_SELECT );
    glLoadIdentity();
    gluPickMatrix( x, viewportCoords[3] - y, 2, 2, viewportCoords );
    glOrtho( -vFrom.y, vFrom.y, -vFrom.y, vFrom.y, -250.0f, 250.0f );
    glMatrixMode( GL_MODELVIEW );
    
    // Render the polygons for this view
    OpenGL->BeginScene( pWindow );
    pWindow.GetCamera()->Render();
    PolygonManager->Render( &pWindow );
    OpenGL->EndScene();

    // Now figure out what was clicked
    objectsFound = glRenderMode( GL_RENDER );
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );

    if( objectsFound > 0 )
    {
        unsigned int lowestDepth = selectBuffer[1];
        int selectedObject       = selectBuffer[3];

        for( int i = 1; i < objectsFound; i++ )
        {
            if( selectBuffer[(i * 4) + 1] < lowestDepth )
            {
                lowestDepth = selectBuffer[( i * 4 ) + 1];
                selectedObject = selectBuffer[( i * 4 ) + 3];
            }
        }
        
        return selectedObject;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: CreateTabControl()
// Desc: 
//-----------------------------------------------------------------------------
bool CEngine::CreateTabControl()
{
    TCITEM tci;







    m_hGeorgiaFont = CreateFont( 14,0,0,0,FW_NORMAL, 0,0,0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                 DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,  "Andale Mono IPA" );

    if( m_hGeorgiaFont == NULL )
        Log( "if( m_hGeorgiaFont == NULL )\n" );

    int x = 0;
    int y = 0;
    int w = m_SideBarWindow.GetWidth();
    int h = m_SideBarWindow.GetHeight() - 20;

    m_hTabControl = CreateWindow( WC_TABCONTROL, "", WS_VISIBLE | WS_TABSTOP | WS_CHILD | TCS_VERTICAL | TCS_BUTTONS ,
                                  x, y, w, h, m_SideBarWindow.GetWnd(), NULL, OpenGL->GetInstance(), NULL );

    m_hButtonsDB = CreateDialog( m_SideBarWindow.GetInst(), "BUTTONSDB",
                                 m_hTabControl, (DLGPROC) ButtonsDB );

    if( m_hTabControl == NULL ) {
        Log( "if( m_hTabControl == NULL )\n" ); return false; }

    SendMessage( (HWND) m_hTabControl,  WM_SETFONT,  (WPARAM) m_hGeorgiaFont, MAKELPARAM( false, 0 )   );


    tci.mask = TCIF_TEXT;
    tci.iImage = -1;

    // Create all the tabs
    tci.pszText = "Textures";
    TabCtrl_InsertItem( m_hTabControl, 0, &tci );
    tci.pszText = "Polygons";
    TabCtrl_InsertItem( m_hTabControl, 1, &tci );
    tci.pszText = "Level Info";
    TabCtrl_InsertItem( m_hTabControl, 2, &tci );
    tci.pszText = "Brushes";
    TabCtrl_InsertItem( m_hTabControl, 3, &tci );
    tci.pszText = "Lights";
    TabCtrl_InsertItem( m_hTabControl, 4, &tci );
    tci.pszText = "Options";
    TabCtrl_InsertItem( m_hTabControl, 5, &tci );
    tci.pszText = "Geometry";
    TabCtrl_InsertItem( m_hTabControl, 6, &tci );

    m_hTabDialog = CreateDialog( m_SideBarWindow.GetInst(), "TexturesTabDB",
                                 m_hTabControl, (DLGPROC) TexturesTabDB );

    if( m_hTabDialog == NULL ) {
        Log( "if( m_hTabDialog == NULL )\n" );
        return false; }

    return true;
}

//-----------------------------------------------------------------------------
// Name: ChangeTab()
// Desc: 
//-----------------------------------------------------------------------------
void CEngine::ChangeTab( LPARAM lParam )
{
    NMHDR* nmptr = (LPNMHDR) lParam;
    int x = 0;
    int y = 150;
    int w = m_SideBarWindow.GetWidth();
    int h = 400;

    if( m_hTabDialog )
        DestroyWindow( m_hTabDialog );

    int TabNumber = TabCtrl_GetCurSel( m_hTabControl );

    switch( TabNumber )
    {
    case 0:
        m_hTabDialog = CreateDialog( m_SideBarWindow.GetInst(), "TexturesTabDB",
                                     m_hTabControl, (DLGPROC) TexturesTabDB );
        break;

    case 1:
        m_hTabDialog = CreateDialog( m_SideBarWindow.GetInst(), "PolygonTabDB",
                                     m_hTabControl, (DLGPROC) PolygonTabDB );
        break;

    case 2:
        m_hTabDialog = CreateDialog( m_SideBarWindow.GetInst(), "LevelTabDB",
                                     m_hTabControl, (DLGPROC) LevelTabDB );
        break;

    case 3:
        m_hTabDialog = CreateDialog( m_SideBarWindow.GetInst(), "BrushTabDB",
                                     m_hTabControl, (DLGPROC) BrushTabDB );
        break;

    case 4:
        m_hTabDialog = CreateDialog( m_SideBarWindow.GetInst(), "LightTabDB",
                                     m_hTabControl, (DLGPROC) LightingTabDB );
        break;

    case 5:
        m_hTabDialog = CreateDialog( m_SideBarWindow.GetInst(), "OptionsTabDB",
                                     m_hTabControl, (DLGPROC) OptionsTabDB );
        break;

    case 6:
        m_hTabDialog = CreateDialog( m_SideBarWindow.GetInst(), "GEOMETRYTABDB",
                                     m_hTabControl, (DLGPROC) GeometryTabDB );
        break;

    default:
        Log( "default @ CEngine::ChangeTab()\n" );
        break;
    }

    if( m_hTabDialog == NULL ) {
        MessageBox( HWND_DESKTOP, "if( m_hTabDialog == NULL )\n", "Error", MB_OK );
        return; }
}


//-----------------------------------------------------------------------------
// Name: LoadLevel()
// Desc: 
//-----------------------------------------------------------------------------
bool CEngine::LoadLevel( char* File )
{
    Log( "CEngine::LoadLevel()\n" );

    if( File == NULL )
    {
        TCHAR szFilename[MAX_PATH];
        TCHAR FileTitle[MAX_PATH];
    
        static OPENFILENAME ofn;
        static bool bSetInitialDir = true;

        *szFilename = NULL;

        ofn.lStructSize       = sizeof( OPENFILENAME );
        ofn.hwndOwner         = GL->GetWnd();
        ofn.lpstrFilter       = NULL;
        ofn.lpstrFilter       = FILE_FILTER_TEXT;
        ofn.lpstrCustomFilter = NULL;
        ofn.nFilterIndex      = 1;
        ofn.lpstrFile         = szFilename;
        ofn.nMaxFile          = MAX_PATH;
        ofn.lpstrTitle        = TEXT("Load Level File...\0");
        ofn.lpstrFileTitle    = FileTitle;
        ofn.lpstrDefExt       = TEXT("*\0");
        ofn.Flags             = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_SHOWHELP;
    
        // Remember the path of the first selected file
        if( bSetInitialDir == false )
        {
            ofn.lpstrInitialDir = DEFAULT_MEDIA_PATH;
            bSetInitialDir = true;
        }
        else 
            ofn.lpstrInitialDir = NULL;

         if( !GetOpenFileName( ( LPOPENFILENAME ) &ofn ) )
             return false;

        File = ofn.lpstrFile;
    }

    if( !Open( File ) ) {
        MessageBox( GL->GetWnd(), "if( !Open( File ) )", "CEngine::LoadLevel()", MB_OK );
        Log( "if( !Open( File ) ) @ CEngine::LoadLevel()\n" );
        return false; }

    Map->Init();

    return true;
}



//-----------------------------------------------------------------------------
// Name: SaveLevel()
// Desc: 
//-----------------------------------------------------------------------------
bool CEngine::SaveLevel( char* File )
{
    if( File == NULL )
    {
        TCHAR szFilename[MAX_PATH];
        TCHAR FileTitle[MAX_PATH];
    
        static OPENFILENAME sfn;
        static bool bSetInitialDir = true;

        *szFilename = NULL;

        sfn.lStructSize       = sizeof( OPENFILENAME );
        sfn.hwndOwner         = GL->GetWnd();
        sfn.lpstrFilter       = NULL;
        sfn.lpstrFilter       = FILE_FILTER_TEXT;
        sfn.lpstrCustomFilter = NULL;
        sfn.nFilterIndex      = 1;
        sfn.lpstrFile         = szFilename;
        sfn.nMaxFile          = MAX_PATH;
        sfn.lpstrTitle        = TEXT("Save Level File...\0");
        sfn.lpstrFileTitle    = FileTitle;
        sfn.lpstrDefExt       = TEXT("*\0");
        sfn.Flags             = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_SHOWHELP;
    
        // Remember the path of the first selected file
        if( bSetInitialDir == false )
        {
            sfn.lpstrInitialDir = DEFAULT_MEDIA_PATH;
            bSetInitialDir = true;
        }
        else 
            sfn.lpstrInitialDir = NULL;
 
         if( !GetSaveFileName( ( LPOPENFILENAME ) &sfn ) )
             return false;

        File = sfn.lpstrFile;
    }

    if( !Save( File ) ) {
        MessageBox( GL->GetWnd(), "if( !Save( File ) )", "CEngine::SaveLevel()", MB_OK );
        Log( "if( !Save( File ) ) @ CEngine::SaveLevel()\n" );
        return false; }

    return true;
}