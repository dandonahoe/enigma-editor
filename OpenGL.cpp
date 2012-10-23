//----------------------------------------------------------------------------- 
// File: OpenGL.h
//
// Desc: Declaration file for the OpenGL container class
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

//-----------------------------------------------------------------------------
// Includes
#include <fstream>
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include <math.h>
#include <stdio.h>
#include <commctrl.h>
#include "OpenGL.h"
#include "main.h"
#include "WindowPane.h"
#include "Window.h"
#include "resource.h"
#include "Profile.h"
#include "Tracer.h"
//-----------------------------------------------------------------------------





#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glaux.lib")
#pragma comment(lib, "comctl32.lib")

UINT g_FontListID = 0;
HFONT hOldFont;
#define MAX_CHARS   256                                    
#define FONT_HEIGHT 32
#define NUMPARTS 3


//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
COpenGL::COpenGL()
{
    TRACE_ENTER_FN( COpenGL::COpenGL )

    LOG( "COpenGL::COpenGL()" );

    m_ScreenWidth  = 1024;
    m_ScreenHeight = 768;
    m_ScreenDepth  = 16;
    m_hWnd         = NULL;
    m_hInstance    = NULL;
    m_hDC          = NULL;
    m_hRC          = NULL;
    m_hStatusBar   = NULL;
    m_fFogDensity  = 0.04f;
    m_bFog         = true;

    TRACE_LEAVE_FN()
}

COpenGL::~COpenGL()
{
    TRACE_ENTER_FN( COpenGL::~COpenGL )

    LOG( "COpenGL::~COpenGL()" );

    Release();

    TRACE_LEAVE_FN()
}


//UINT g_Texture[6];
//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initalizes OpenGL
//-----------------------------------------------------------------------------
BOOL COpenGL::Init( HINSTANCE hInstance, BOOL bFullScreen, char* WindowName, CWindow &pWindow  )
{
    TRACE_ENTER_FN( COpenGL::Init )

    LOG( "COpenGL::Init()" );

    m_bFullScreen = bFullScreen;
    m_hInstance = hInstance;

    Profile->Begin( "CreateMyWindow()" );
    if( !CreateMyWindow( WindowName, 0 ) )
    {
        TRACE_LEAVE_FN()
        return FALSE;
    }
    Profile->End( "CreateMyWindow()" );

    GetClientRect( m_hWnd, &m_rScreenRect );

    Profile->Begin( "InitializeOpenGL()" );
    InitializeOpenGL( pWindow );
    Profile->End( "InitializeOpenGL()" );

    //glCullFace( GL_FRONT );
   // glEnable( GL_CULL_FACE );

    Profile->Begin( "InitStatusCC()" );
    InitStatusCC();
    Profile->End( "InitStatusCC()" );

    g_FontListID = CreateOpenGLFont( "Arial", FONT_HEIGHT );

    TRACE_LEAVE_FN()

    return TRUE;
}


//-----------------------------------------------------------------------------
// Name: BeginScene()
// Desc: 
//-----------------------------------------------------------------------------
BOOL COpenGL::BeginScene( CWindow &pWindow )
{
    TRACE_ENTER_FN( COpenGL::BeginScene )

    glClear( pWindow.GetClearFlags() );
    glLoadIdentity();

    TRACE_LEAVE_FN()

    return TRUE;
}


//-----------------------------------------------------------------------------
// Name: EndScene()
// Desc: 
//-----------------------------------------------------------------------------
BOOL COpenGL::EndScene()
{
    TRACE_ENTER_FN( COpenGL::EndScene )

    SwapBuffers( m_hDC );    

    TRACE_LEAVE_FN()

    return TRUE;
}


//-----------------------------------------------------------------------------
// Name: Release()
// Desc: 
//-----------------------------------------------------------------------------
VOID COpenGL::Release()
{
    LOG( "COpenGL::Release()" );

    if( m_bFullScreen )
    {
        ChangeDisplaySettings( NULL, 0 );
        ShowCursor( TRUE );
    }

    glDeleteLists( g_FontListID, MAX_CHARS );                // Free the display list
    SelectObject( m_hDC, hOldFont );                        // Select the old font back in so we don't have memory leaks

    if( m_hRC )                                            
    {
        wglMakeCurrent( NULL, NULL );
        wglDeleteContext( m_hRC );
    }
    
    if( m_hDC ) 
        ReleaseDC( m_hWnd, m_hDC );

    UnregisterClass( m_strWindowName, m_hInstance );

    DeleteObject( m_hFont );

    PostQuitMessage( 0 );
}


//-----------------------------------------------------------------------------
// Name: SetupPixelFormat()
// Desc: 
//-----------------------------------------------------------------------------
BOOL COpenGL::SetupPixelFormat( HDC hdc, CWindow &pWindow ) 
{
    PIXELFORMATDESCRIPTOR pfd; 
    INT pixelformat; 
 
    pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );

    pfd.nVersion = 1;
    pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; 
    pfd.dwLayerMask  = PFD_MAIN_PLANE;
    pfd.iPixelType   = PFD_TYPE_RGBA;
    pfd.cColorBits   = m_ScreenDepth;
    pfd.cDepthBits   = m_ScreenDepth;
    pfd.cAccumBits   = 0;
    pfd.cStencilBits = 0;

    Profile->Begin( "ChoosePixelFormat()" );
Log( "5\n" );
    if( ( pixelformat = ChoosePixelFormat( hdc, &pfd ) ) == FALSE ) 
    { 
        MessageBox( NULL, "ChoosePixelFormat failed", "Error", MB_OK ); 
        return FALSE; 
    }
Log( "6\n" );
    Profile->End( "ChoosePixelFormat()" );

    Profile->Begin( "SetPixelFormat()" );
Log( "7\n" );
    if( SetPixelFormat( hdc, pixelformat, &pfd) == FALSE ) 
    { 
        MessageBox( NULL, "SetPixelFormat failed", "Error", MB_OK ); 
        return FALSE; 
    }
Log( "8\n" );
    Profile->End( "SetPixelFormat()" );

    pWindow.SetPixelFormat( pixelformat );

    pWindow.SetPixelDescriptor( pfd );

    return TRUE;
}


//-----------------------------------------------------------------------------
// Name: InitializeOpenGL()
// Desc: 
//-----------------------------------------------------------------------------
VOID COpenGL::InitializeOpenGL( CWindow &pWindow ) 
{
 //   LOG( "COpenGL::InitializeOpenGL()" );

    GetClientRect( m_hWnd, &m_rScreenRect );

    Profile->Begin( "SetupRenderWindow()" );
    SetupRenderWindow( pWindow );
    Profile->End( "SetupRenderWindow()" );

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_TEXTURE_2D );

    SetClearColor( 0, 0, 0, 1 );
    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );


    Profile->Begin( "SizeOpenGLScreen()" );
    SizeOpenGLScreen( pWindow );
    Profile->End( "SizeOpenGLScreen()" );

    m_hFont = CreateFont( 14,0,0,0,FW_NORMAL, 0,0,0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                          CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Georgia" );
}


//-----------------------------------------------------------------------------
// Name: GetFogEnabled()
// Desc: 
//-----------------------------------------------------------------------------
BOOL COpenGL::GetFogEnabled()
{
    return m_bFog;
}


//-----------------------------------------------------------------------------
// Name: SetFogColor()
// Desc: Sets the fog color
//-----------------------------------------------------------------------------
BOOL COpenGL::SetFogColor( float fR,
                           float fG,
                           float fB,
                           float fA )
{
    m_fFogR = fR;
    m_fFogG = fG;
    m_fFogB = fB;
    m_fFogA = fA;

    float fogColor[4] = { fR, fG, fB, fA };
    glFogfv( GL_FOG_COLOR, fogColor );

    return TRUE;
}


//-----------------------------------------------------------------------------
// Name: GetFogColor()
// Desc: Gets the fog color
//-----------------------------------------------------------------------------
BOOL COpenGL::GetFogColor( float *lpR,
                           float *lpG,
                           float *lpB,
                           float *lpA )
{
    *lpR = m_fFogR;
    *lpG = m_fFogG;
    *lpB = m_fFogB;
    *lpA = m_fFogA;

    return TRUE;
}

//-----------------------------------------------------------------------------
// Name: SetFogDensity()
// Desc: 
//-----------------------------------------------------------------------------
BOOL COpenGL::SetFogDensity( float fDensity )
{
    m_fFogDensity = fDensity;

    glFogf( GL_FOG_DENSITY, fDensity );
    return TRUE;
}


//-----------------------------------------------------------------------------
// Name: GetFogDensity()
// Desc: 
//-----------------------------------------------------------------------------
BOOL COpenGL::GetFogDensity( float *fDensity )
{
    *fDensity = m_fFogDensity;

    return TRUE;
}


//-----------------------------------------------------------------------------
// Name: SetFogEnable()
// Desc: 
//-----------------------------------------------------------------------------
BOOL COpenGL::SetFogEnable( BOOL bEnable )
{
    m_bFog = bEnable;

    if( bEnable )
        glEnable( GL_FOG );
    else
        glDisable( GL_FOG );

    return TRUE;
}


//-----------------------------------------------------------------------------
// Name: SetFogMinMax()
// Desc: 
//-----------------------------------------------------------------------------
BOOL COpenGL::SetFogRange( float fMinDistance, float fMaxDistance )
{
    glFogf( GL_FOG_START, fMinDistance );
    glFogf( GL_FOG_END, fMaxDistance );

    return TRUE;
}


//-----------------------------------------------------------------------------
// Name: GetFogRange()
// Desc: 
//-----------------------------------------------------------------------------
BOOL COpenGL::GetFogRange( float *fMinDistance, float *fMaxDistance )
{    
    *fMinDistance = m_fMinDistance;
    *fMaxDistance = m_fMaxDistance;

    return TRUE;
}


//-----------------------------------------------------------------------------
// Name: SetFogQuality()
// Desc: 
//-----------------------------------------------------------------------------
BOOL COpenGL::SetFogQuality( GLenum flag )
{
    glHint( GL_FOG_HINT, flag );

    return TRUE;
}


//-----------------------------------------------------------------------------
// Name: SetFogQuality()
// Desc: 
//-----------------------------------------------------------------------------
BOOL COpenGL::SetFogMode( GLenum flag )
{
    glFogi( GL_FOG_MODE, flag );

    return TRUE;
}


//-----------------------------------------------------------------------------
// Name: SizeOpenGLScreen()
// Desc: Resizes the window
//-----------------------------------------------------------------------------
VOID COpenGL::SizeOpenGLScreen( CWindow &pWindow )
{
    TRACE_ENTER_FN( COpenGL::SizeOpenGLScreen )

    GetClientRect( m_hWnd, &m_rScreenRect );

    if( !pWindow.RequiresUpdate() ) { TRACE_LEAVE_FN() return; }

    pWindow.SetRequiresUpdate( false );

    glViewport( 0, 0, pWindow.GetWidth(), pWindow.GetHeight() );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    const float fAspectRatio = (float)pWindow.GetWidth() / (float)pWindow.GetHeight();
    const VECT vFrom = pWindow.GetCamera()->GetFromVec();

    switch( pWindow.GetViewType() )
    {
        case VIEW_3D:
            gluPerspective( 45.0f, fAspectRatio, 0.2f , 250.0f );
            break;

        case VIEW_XZ:
            glOrtho( -vFrom.y * fAspectRatio, vFrom.y * fAspectRatio, -vFrom.y, vFrom.y, -250.0f, 250.0f );
            break;

        case VIEW_XY:
            glOrtho( -vFrom.z * fAspectRatio, vFrom.z * fAspectRatio, -vFrom.z, vFrom.z, -250.0f, 250.0f );
            break;

        case VIEW_YZ:
            glOrtho( -vFrom.x * fAspectRatio, vFrom.x * fAspectRatio, -vFrom.x, vFrom.x, -250.0f, 250.0f );
            break;

        default: LOG_ERR() break;
    }

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: ChangeToFullScreen()
// Desc: Changes the display to fullscreen mode
//-----------------------------------------------------------------------------
VOID COpenGL::ChangeToFullScreen()
{
    LOG( "COpenGL::ChangeToFullScreen()" );

    DEVMODE dmSettings;

    memset( &dmSettings, 0, sizeof( dmSettings ) );

    if( !EnumDisplaySettings( NULL,ENUM_CURRENT_SETTINGS, &dmSettings ) )
    {
        MessageBox( NULL, "Could Not Enum Display Settings", "Error", MB_OK );
        return;
    }

    dmSettings.dmPelsWidth  = m_ScreenWidth;
    dmSettings.dmPelsHeight = m_ScreenHeight;    

    if( ChangeDisplaySettings( &dmSettings,CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
    {
        MessageBox( NULL, "Display Mode Not Compatible", "Error", MB_OK );
        PostQuitMessage( 0 );
    }
}


//-----------------------------------------------------------------------------
// Name: CreateMyWindow()
// Desc: Creates the main window
//-----------------------------------------------------------------------------
BOOL COpenGL::CreateMyWindow( LPSTR strWindowName, DWORD dwStyle )
{
 //   LOG( "COpenGL::CreateMyWindow()" );

    RECT rWindow;

    m_strWindowName = strWindowName;

    WNDCLASS wndclass;

    memset( &wndclass, 0, sizeof( WNDCLASS ) );

    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = WinProc;
    wndclass.hInstance     = m_hInstance;
    wndclass.hIcon         = LoadIcon( m_hInstance, "IDI_ICON" );
    wndclass.hCursor       = LoadCursor( NULL, IDC_ARROW );
    wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
    wndclass.lpszClassName = strWindowName;
    wndclass.lpszMenuName  = "IDR_MAINMENU";

    RegisterClass( &wndclass );

    if( m_bFullScreen && !dwStyle )
    {
        dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

        ChangeToFullScreen();

        ShowCursor( FALSE );
    }
    else if( !dwStyle )
        dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_MAXIMIZE;
   
    rWindow.left   = 0;
    rWindow.right  = m_ScreenWidth;
    rWindow.top    = 0;
    rWindow.bottom = m_ScreenHeight;

    //AdjustWindowRect( &rWindow, dwStyle, FALSE );

    m_hWnd = CreateWindow( strWindowName,
                           strWindowName,
                           dwStyle,
                           0, 0,
                           1024, 768, 
                           NULL,
                           NULL,
                           m_hInstance,
                           NULL);

    if( !m_hWnd )
    {
        Log( "if( !m_hWnd )\n" );
        return false;
    }

    ShowWindow( m_hWnd, SW_MAXIMIZE );
    UpdateWindow( m_hWnd );

    SetFocus( m_hWnd );
    

    //Allow for commom control use
    m_CommCtrls.dwSize = sizeof( INITCOMMONCONTROLSEX );
    m_CommCtrls.dwICC  = ICC_BAR_CLASSES | ICC_TAB_CLASSES | ICC_UPDOWN_CLASS;
    InitCommonControlsEx( &m_CommCtrls );

    return TRUE;
}


//-----------------------------------------------------------------------------
// Name: Repaint()
// Desc: Repaingt the window
//-----------------------------------------------------------------------------
VOID COpenGL::Repaint( WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;

    BeginPaint( m_hWnd, &ps );                    
    EndPaint( m_hWnd, &ps );
}


//-----------------------------------------------------------------------------
// Name: GetWnd()
// Desc: Repaingt the window
//-----------------------------------------------------------------------------
HWND COpenGL::GetWnd() const
{
    return m_hWnd;
}


//-----------------------------------------------------------------------------
// Name: CreateOpenGLFont()
// Desc: 
//-----------------------------------------------------------------------------
UINT COpenGL::CreateOpenGLFont(LPSTR strFontName, int height)    // Build Our Bitmap Font
{
 //   LOG( "COpenGL::CreateOpenGLFont()" );

    UINT    fontListID = 0;                                // This will hold the base ID for our display list
    HFONT    hFont;                                        // This will store the handle to our font

    fontListID = glGenLists(MAX_CHARS);                    // Generate the list for the font

    hFont = CreateFont( height,                            // Our desired HEIGHT of the font
                        0,                                // The WIDTH (If we leave this zero it will pick the best width depending on the height)
                        0,                                // The angle of escapement
                        0,                                // The angle of orientation
                        FW_BOLD,                        // The font's weight (We want it bold)
                        FALSE,                            // Italic - We don't want italic
                        FALSE,                            // Underline - We don't want it underlined
                        FALSE,                            // Strikeout - We don't want it strikethrough
                        ANSI_CHARSET,                    // This is the type of character set
                        OUT_TT_PRECIS,                    // The Output Precision
                        CLIP_DEFAULT_PRECIS,            // The Clipping Precision
                        ANTIALIASED_QUALITY,            // The quality of the font - We want anitaliased fonts
                        FF_DONTCARE|DEFAULT_PITCH,        // The family and pitch of the font.  We don't care.
                        strFontName);                    // The font name (Like "Arial", "Courier", etc...)

    hOldFont = (HFONT)SelectObject(m_hDC, hFont);

    wglUseFontBitmaps(m_hDC, 0, MAX_CHARS - 1, fontListID);    // Builds 255 bitmap characters

    return fontListID;                                    // Return the ID to the display list to use later
}


//-----------------------------------------------------------------------------
// Name: CreateOpenGLFont()
// Desc: 
//-----------------------------------------------------------------------------
VOID COpenGL::PositionText( int x, int y )
{
    glPushAttrib( GL_TRANSFORM_BIT | GL_VIEWPORT_BIT );
    glMatrixMode( GL_PROJECTION );                        // Set our matrix to our projection matrix
    glPushMatrix();                                        // Push on a new matrix to work with
    glLoadIdentity();                                    // reset the matrix
    glMatrixMode( GL_MODELVIEW );                        // Set our matrix to our model view matrix
    glPushMatrix();                                        // Push on a new matrix to work with
    glLoadIdentity();                                    // Reset that matrix


    y = m_ScreenHeight - FONT_HEIGHT - y;

    glViewport( x - 1, y - 1, 0, 0 );

    glRasterPos4f( 0, 0, 0, 1 );

    glPopMatrix();

    glMatrixMode( GL_PROJECTION );

    glPopMatrix();                                    

    glPopAttrib();    

}


//-----------------------------------------------------------------------------
// Name: CreateOpenGLFont()
// Desc: 
//-----------------------------------------------------------------------------
VOID COpenGL::DrawGLText(int x, int y, const char *strString, ...)
{
    char    strText[256];
    va_list argumentPtr;

    if ( strString == NULL )
        return;    

    va_start( argumentPtr, strString );

    vsprintf( strText, strString, argumentPtr );

    va_end( argumentPtr);
    
    PositionText( x, y );
    
    glPushAttrib( GL_LIST_BIT );

    glListBase( g_FontListID );

    glCallLists( strlen( strText ), GL_UNSIGNED_BYTE, strText );

    glPopAttrib();                                        
}


//-----------------------------------------------------------------------------
// Name: GetInstance()
// Desc: 
//-----------------------------------------------------------------------------
HINSTANCE COpenGL::GetInstance()
{
    return m_hInstance;
}


//-----------------------------------------------------------------------------
// Name: SetClearColor()
// Desc: 
//-----------------------------------------------------------------------------
BOOL COpenGL::SetClearColor( float fR, float fG, float fB, float fA )
{
    glClearColor( fR, fG, fB, fA );

    return TRUE;
}

/*
//-----------------------------------------------------------------------------
// Name: CreateSideBar()
// Desc: 
//-----------------------------------------------------------------------------
bool COpenGL::CreateSideBar()
{
    LOG( "COpenGL::CreateSideBar()" )

    m_hBMPTriangle   = LoadBitmap( m_hInstance, "IDB_TRIANGLE"   );
    m_hBMPSheet      = LoadBitmap( m_hInstance, "IDB_SHEET"      );
    m_hBMPCube       = LoadBitmap( m_hInstance, "IDB_CUBE"       );
    m_hBMPAddBrush   = LoadBitmap( m_hInstance, "IDB_ADDBRUSH"   );
    m_hBMPNextVertex = LoadBitmap( m_hInstance, "IDB_NEXTVERTEX" );
    m_hBMPGray       = LoadBitmap( m_hInstance, "IDB_GRAY"       );

    m_hBtnTriangle   = CreateWindow( "button", "", WS_CHILD | WS_VISIBLE | BS_BITMAP,
                                     0, 0, 30, 30,
                                     m_SideBarWindow.GetWnd(),(HMENU) IDD_TRIANGLE, m_hInstance, NULL );

    m_hBtnSheet      = CreateWindow( "button", "", WS_CHILD | WS_VISIBLE | BS_BITMAP,
                                     0, 30, 30, 30,
                                     m_SideBarWindow.GetWnd(),(HMENU) IDD_SHEET, m_hInstance, NULL );

    m_hBtnCube       = CreateWindow( "button", "", WS_CHILD | WS_VISIBLE | BS_BITMAP,
                                     0, 60, 30, 30,
                                     m_SideBarWindow.GetWnd(),(HMENU) IDD_CUBE, m_hInstance, NULL );

    m_hBtnAddBrush   = CreateWindow( "button", "", WS_CHILD | WS_VISIBLE | BS_BITMAP,
                                     0, 90, 30, 30,
                                     m_SideBarWindow.GetWnd(),(HMENU) IDD_ADDBRUSH, m_hInstance, NULL );

    m_hBtnNextVertex = CreateWindow( "button", "", WS_CHILD | WS_VISIBLE | BS_BITMAP,
                                     0, 120, 30, 30,
                                     m_SideBarWindow.GetWnd(),(HMENU) IDD_NEXTVERTEX, m_hInstance, NULL );

    m_hFont = CreateFont( 14,0,0,0,FW_NORMAL, 0,0,0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                          CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Georgia" );

    GetClientRect( m_hWnd, &m_rScreenRect );

    m_hLbTextures = CreateWindow ( "listbox", "", WS_CHILD | WS_VISIBLE | LBS_HASSTRINGS |
                                   WS_VSCROLL | LBS_NOTIFY, 10, 500, 148,
                                   100, m_SideBarWindow.GetWnd(), (HMENU) IDD_LBTEXTURES, m_hInstance, NULL );

    if( !m_hLbTextures ) CRITICAL_ERROR( "if( !m_hLbTextures )" )

    SendMessage( (HWND) m_hLbTextures,    WM_SETFONT,  (WPARAM) m_hFont,      MAKELPARAM( false, 0 )   );
    SendMessage( (HWND) m_hBtnTriangle,   BM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM)m_hBMPTriangle   );
    SendMessage( (HWND) m_hBtnSheet,      BM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM)m_hBMPSheet      );
    SendMessage( (HWND) m_hBtnCube,       BM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM)m_hBMPCube       );
    SendMessage( (HWND) m_hBtnAddBrush,   BM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM)m_hBMPAddBrush   );
    SendMessage( (HWND) m_hBtnNextVertex, BM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM)m_hBMPNextVertex );

    ShowWindow( m_hLbTextures, true );
    UpdateWindow( m_hLbTextures );

    ShowWindow( m_hBtnTriangle, true );
    UpdateWindow( m_hBtnTriangle );

    ShowWindow( m_hBtnSheet, true );
    UpdateWindow( m_hBtnSheet );

    ShowWindow( m_hBtnCube, true );
    UpdateWindow( m_hBtnCube );

    ShowWindow( m_hBtnAddBrush, true );
    UpdateWindow( m_hBtnAddBrush );

    ShowWindow( m_hBtnNextVertex, true );
    UpdateWindow( m_hBtnNextVertex );

    return true;
}
*/

//---------------------------------------------------------------------
//Name: InitStatusWindow()
//Desc: Creates that ststus bar at the bottom
//---------------------------------------------------------------------
bool COpenGL::InitStatusCC()
{
 //   LOG( "COpenGL::InitStatusCC()" )

    RECT WinDim;
    int i;
    int parts[NUMPARTS];

    GetClientRect( m_hWnd, &WinDim );

    for( i = 0; i < NUMPARTS; i++ )
        parts[i] = WinDim.right / NUMPARTS * ( i + 1 );

    m_hStatusBar = CreateWindow( STATUSCLASSNAME,
                                 "",
                                 WS_CHILD | WS_VISIBLE,
                                 0,0,0,0,
                                 m_hWnd,
                                 NULL,
                                 m_hInstance,
                                 NULL );
    if( !m_hStatusBar )
        CRITICAL_ERROR( "Status Window Creation Failed" )

    SendMessage( m_hStatusBar, SB_SETPARTS, (WPARAM) NUMPARTS, (LPARAM) parts );
    SendMessage( m_hStatusBar, SB_SETTEXT,  (WPARAM) 0,        (LPARAM) ""    );
    SendMessage( m_hStatusBar, SB_SETTEXT,  (WPARAM) 1,        (LPARAM) ""    );
    SendMessage( m_hStatusBar, SB_SETTEXT,  (WPARAM) 2,        (LPARAM) ""    );

    return true;
}


//---------------------------------------------------------------------
//Name: LoadTextureLB()
//Desc: Loads the Texture list box with a list of all loaded textures
//      available
//---------------------------------------------------------------------
bool COpenGL::LoadTextureLB( char* FileName, bool bFullPath )
{
    LOG( "COpenGL::LoadTextureLB()" )
/*
    ifstream fp;
    char Text[256];
    int Length = SendDlgItemMessage( m_SideBarWindow.GetWnd(), IDD_LBTEXTURES, LB_GETCOUNT, 0, 0 );

    //Then deletes them all
    for( int a = 0; a < Length; a++ )
        SendDlgItemMessage( m_SideBarWindow.GetWnd(), IDD_LBTEXTURES, LB_DELETESTRING, 0, 0 );

    fp.open( FileName );

    while( !fp.eof() )
    {
        fp.getline( Text, 255, '\n' );

        if( !Text ) return true;

        SendDlgItemMessage( m_SideBarWindow.GetWnd(), IDD_LBTEXTURES, LB_ADDSTRING, 0, (LPARAM) Text );
    }

    fp.close();
*/
    return true;
}


//---------------------------------------------------------------------
//Name: GetSelectedTextureIndexLB()
//Desc: 
//---------------------------------------------------------------------
int COpenGL::GetSelectedTextureIndexLB()
{
    return -1;//SendDlgItemMessage( m_SideBarWindow.GetWnd(), IDD_LBTEXTURES, LB_GETCURSEL, 0, 0 );
}




void COpenGL::SetupOrtho(int cx, int cy)
{
    
    // This function takes in rectangle coordinates for our 2D view port.
    // What happens below is, we need to change our projection matrix.  To do this,
    // we call glMatrixMode(GL_PROJECTION) to tell OpenGL the current mode we want.
    // Then we push on a new matrix so we can just call glPopMatrix() to return us
    // to our previous projection matrix, but first we will render in ortho mode.
    //  Next, we load a new identity matrix so that everything is initialize before
    // we go into ortho mode.  One we switch to ortho by calling glOrth(), we then
    // want to set our matrix mode to GL_MODELVIEW, which puts us back and ready to
    // render our world using the model matrix.  We also initialize this new model view
    // matrix for our 2D rendering before drawing anything.

    // Switch to our projection matrix so that we can change it to ortho mode, not perspective.
    glMatrixMode(GL_PROJECTION);                        

    // Push on a new matrix so that we can just pop it off to go back to perspective mode
    glPushMatrix();                                    
    
    // Reset the current matrix to our identify matrix
    glLoadIdentity();                                

    //Pass in our 2D ortho screen coordinates.like so (left, right, bottom, top).  The last
    // 2 parameters are the near and far planes.
    glOrtho( -10, 10, -10, 10, 0, 1 );    
    
    // Switch to model view so that we can render the scope image
    glMatrixMode(GL_MODELVIEW);                                

    // Initialize the current model view matrix with the identity matrix
    glLoadIdentity();

}


//---------------------------------------------------------------------
//Name: SwitchRenderWindow()
//Desc: 
//---------------------------------------------------------------------
void COpenGL::SetupRenderWindow( CWindow &pWindow )
{
    Log( "1\n" );

    m_hDC = GetDC( pWindow.GetWnd() );
Log( "2\n" );
    if( !SetupPixelFormat( m_hDC, pWindow ) )
        PostQuitMessage( 0 );
Log( "3\n" );
    m_hRC = wglCreateContext( m_hDC );
Log( "4\n" );
    wglMakeCurrent( m_hDC, m_hRC ); // THis is the crashing one
Log( "5\n" );
    pWindow.SetRC( m_hRC );
    pWindow.SetDC( m_hDC );
}


//---------------------------------------------------------------------
//Name: FocusOnWindow()
//Desc: 
//---------------------------------------------------------------------
void COpenGL::FocusOnWindow( CWindow &pWindow )
{
    TRACE_ENTER_FN( COpenGL::FocusOnWindow )

    m_hDC = GetDC( pWindow.GetWnd() );

    m_hRC = pWindow.GetRC();

    wglMakeCurrent( m_hDC, m_hRC );

    SizeOpenGLScreen( pWindow );

    TRACE_LEAVE_FN()
}