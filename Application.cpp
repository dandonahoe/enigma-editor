//----------------------------------------------------------------------------- 
// File: Application.cpp
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#include <windows.h>
#include <math.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include "Application.h"
#include "main.h"
#include "dxutil.h"
#include "dinput.h"
#include "TextureManager.h"
#include "PolygonManager.h"
#include "Vertex.h"
#include "MessageProcs.h"
#include "resource.h"
#include "Window.h"
#include "Util.h"
#include "CustomMsg.h"
#include "Tracer.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
CApplication::CApplication()
{
    TRACE_ENTER_FN( CApplication::CApplication() )

    Log( "CApplication::CApplication()\n" );

    m_hInstance = NULL;
    m_hWndTCtrl = NULL;
// this is a test

    TRACE_LEAVE_FN()
}

CApplication::~CApplication()
{
    TRACE_ENTER_FN( CApplication::~CApplication() )

    Log( "CApplication::~CApplication()\n" );

    Release();

    TRACE_LEAVE_FN()
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc:
//-----------------------------------------------------------------------------
bool CApplication::Init( HINSTANCE hInstance, char* strAppName, double Version )
{
    TRACE_ENTER_FN( CApplication::Init )

    Log( "CApplication::Init()\n" );

    Log( "CPU Speed: " ); Log( (int)GetCpuSpeed() ); Log( "MHz\n" );

    Settings->Open();

    m_hInstance = hInstance;

    Profile->Begin( "m_Engine.Init()" );

    m_Engine.Init( hInstance, false, strAppName, Version );

    Profile->End( "m_Engine.Init()" );

    Profile->DumpOutputToBuffer();

    m_Engine.AlignViews();

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Run()
// Desc: Application's main loop Renders the scene when all messages have been
//       dealt with
//-----------------------------------------------------------------------------
int CApplication::Run()
{
    TRACE_ENTER_FN( CApplication::Run() )

    Log( "CApplication::Run()\n" );

    MSG msg;

    DXUtil_Timer( TIMER_RESET );

    while( true )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) 
        { 
            if( msg.message == WM_QUIT )
                break;

            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
            RenderScene();
    }

    RET( msg.wParam )
}


//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc:
//-----------------------------------------------------------------------------
LRESULT CApplication::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{

    switch( uMsg )
    {
    case WM_CREATE:         OnWMCreate( hWnd );                                          break;
    case WM_COMMAND:     OnWMCommand( wParam, hWnd );                                 break;
    case WM_SIZE:           m_Engine.ResizeWindow( LOWORD( lParam ), HIWORD( lParam ) ); break;
    case WM_PAINT:         OnWMPaint( wParam, lParam );                                 break;
/*
        case WM_RBUTTONDOWN:
        {
            POINT pt;
            HMENU hMenu, hSubMenu;
            ClientToScreen( hWnd, &pt );

            hMenu = LoadMenu( m_hInstance, "IDR_MENU_FACE" );
            //hSubMenu = GetSubMenu( hMenu, 0 );

            TrackPopupMenuEx( hMenu, 0, pt.x, pt.y, hWnd, NULL );
        }
        break;
*/
        case WM_RBUTTONDOWN:
        case WM_LBUTTONDOWN:
        {
            m_Engine.LeftClick( LOWORD( lParam ), HIWORD( lParam ) );
        }
        break;

    case WM_KEYDOWN:     OnWMKeydown( wParam, hWnd );                                 break;
    case WM_CLOSE:         PostQuitMessage( 0 );                                        break;

    case CMSG_ALIGNVIEWS: m_Engine.AlignViews();                                      break;
    case CMSG_CHANGE_TAB: m_Engine.ChangeTab( lParam );                               break;

    default:             return DefWindowProc( hWnd, uMsg, wParam, lParam ); 
    }
 
    return 0;
}


//-----------------------------------------------------------------------------
// Name: RenderScene()
// Desc:
//-----------------------------------------------------------------------------
bool CApplication::RenderScene()
{
    TRACE_ENTER_FN( CApplication::RenderScene )

    static float t1, t2, t3, t4, t5, t6, t7, t8, t9, t10;
    static float fSecond = 0.0f;
	static int iFrame = 0;

    m_fAppTime     = DXUtil_Timer( TIMER_GETAPPTIME );
    m_fElapsedTime = DXUtil_Timer( TIMER_GETELAPSEDTIME );

    iFrame++;
	fSecond += m_fElapsedTime;

    if( fSecond >= 1.0f )
	{
        Log( "FPS: " ); Log( iFrame ); Log( "\n" );
		fSecond = 0.0f;
		iFrame = 0;
	}

    // Averages time elapsed in the last 10 frames
    t1 = t2; t2 = t3; t3 = t4; t4 = t5;  t5 = t6;
    t6 = t7; t7 = t8; t8 = t9; t9 = t10; t10 = m_fElapsedTime;
    m_fElapsedTime = ( t1 + t2 + t3 + t4 + t5 + t6 + t7 + t8 + t9 + t10 ) / 10.0f;

    m_Engine.FrameMove( m_fElapsedTime );

    m_Engine.Render();

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Release()
// Desc:
//-----------------------------------------------------------------------------
BOOL CApplication::Release()
{
    TRACE_ENTER_FN( CApplication::Release )

    Log( "CApplication::Release()\n" );

    Settings->Save();

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: OnWMCommand()
// Desc:
//-----------------------------------------------------------------------------
void CApplication::OnWMCommand( WPARAM wParam, HWND hWnd )
{
    switch( LOWORD( wParam ) )
    {
    /* Menu */

    // File
    case ID_NEW:         m_Engine.LoadLevel( "newlevel" ); break;
    case ID_OPEN:        m_Engine.LoadLevel( NULL );       break;
    case ID_CLOSE:                                         break;
    case ID_FILE_SAVEMAPAS:                                break;
    case ID_SAVE_MAP:    m_Engine.SaveLevel( NULL );       break;
    case ID_EXIT:        PostQuitMessage( 0 );             break;
    case ID_FILE_RUNLEVEL:
    {
        ShellExecute( hWnd, "open", "D:\\Apps\\OpenGL\\EnigmaEngine\\Release\\EnigmaEngine.exe",
                      NULL, NULL, SW_SHOW );
    }
    break;

       // PROCESS_INFORMATION 

    // Edit
    case ID_CUT:        break;
    case ID_COPY:       break;
    case ID_PASTE:      break;
    case ID_REDO:       break;
    case ID_UNDO:       break;
    case ID_SELECT_ALL: break;

    // Textures
    case ID_TEXTURES_ALIGNALLTEXTURES: PM->AlignAllTextures(); break;

    // Vertex
    case ID_ADD_VERTEX:                                                      break;
    case ID_DELETE_VERTEX:                                                   break;
    case ID_VERTEX_PROPERTIES:                                               break;
    case ID_VERTEX_COMPUTEVERTEXCONNECTIONS: PM->ComputeVertexConnections(); break;
    case ID_VERTEX_SNAPVERTSTOGRID:          PM->SnapVertsToGrid();          break;

    // Face
    case ID_FACE_REMOVECOENCIDINGFACES: PM->RemoveCoencidingFaces(); break;

    // Polygon
    case ID_DELETE_POLYGON: break;
    case ID_ADD_POLYGON:    break;
    case ID_LOAD_POLYGON:   break;
    case ID_SAVE_POLYGON:   break;
    case ID_SAVE_AS:        break;
    
    // Camera
    case ID_CAMERA_PROPERTIES: DialogBox( m_hInstance, "CameraDB", hWnd, (DLGPROC) CameraDB );     break;

    // Options
    case ID_MAP_PROPERTIES:    DialogBox( m_hInstance, "MapDB", hWnd, (DLGPROC) MapDB );           break;                                         break;
    case ID_EDITOR_PROPERTIES: DialogBox( m_hInstance, "SettingsDB", hWnd, (DLGPROC) SettingsDB ); break;

    // Window
    case ID_WINDOW_RESETVIEWS: m_Engine.ResetViews(); break;
    case ID_WINDOW_TOGGLEACTIVESTATE_3DWINDOW:       Settings->m_bEditor_3DViewActive     = !Settings->m_bEditor_3DViewActive;     break;
    case ID_WINDOW_TOGGLEACTIVESTATE_2DLEFTWINDOW:   Settings->m_bEditor_LeftViewActive   = !Settings->m_bEditor_LeftViewActive;   break;
    case ID_WINDOW_TOGGLEACTIVESTATE_2DCENTERWINDOW: Settings->m_bEditor_CenterViewActive = !Settings->m_bEditor_CenterViewActive; break;
    case ID_WINDOW_TOGGLEACTIVESTATE_2DRIGHTWINDOW:  Settings->m_bEditor_RightViewActive  = !Settings->m_bEditor_RightViewActive;  break;

    // Help
    case ID_ABOUT: DialogBox( m_hInstance, "AboutDB", hWnd, (DLGPROC) AboutDB ); break;
    }
}


//-----------------------------------------------------------------------------
// Name: OnWMKeydown()
// Desc:
//-----------------------------------------------------------------------------
void CApplication::OnWMKeydown( WPARAM wParam, HWND hWnd )
{
    switch( wParam )
    {    
        case VK_ESCAPE: PostQuitMessage( 0 ); break;
    }
}


//-----------------------------------------------------------------------------
// Name: OnWMCreate()
// Desc:
//-----------------------------------------------------------------------------
void CApplication::OnWMCreate( HWND hWnd )
{
    TRACE_ENTER_FN( CApplication::OnWMCreate )

    m_Engine.OnWMCreate( hWnd );

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: OnWMPaint()
// Desc:
//-----------------------------------------------------------------------------
void CApplication::OnWMPaint( WPARAM wParam, LPARAM lParam )
{
    TRACE_ENTER_FN( CApplication::OnWMPaint )

    m_Engine.Repaint( wParam, lParam );   
    
    TRACE_LEAVE_FN()
}