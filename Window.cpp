//----------------------------------------------------------------------------- 
// File: WindowPane.cpp
//
// Desc: 
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#include <windows.h>
#include "Window.h"
#include "OpenGL.h"
#include "main.h"
#include "CustomMsg.h"
//-----------------------------------------------------------------------------
 

LRESULT CALLBACK CWindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
int g_CWindowInstances = 0;


//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
CWindow::CWindow()
{
   // Log( "CWindow::CWindow()\n" );

    m_hInst        = NULL;
    m_hWnd         = NULL;
    m_hParent      = NULL;
    m_XPos         = 0;
    m_YPos         = 0;
    m_Width        = 0;
    m_Height       = 0;
    m_hDC          = NULL;
    m_hRC          = NULL;
    m_PixelFormat  = 0;
    m_bDrawObjects = true;
    m_bIsActive    = false;
    m_bNeedsUpdate = true;
    m_ViewType     = VIEW_OTHER;
}

CWindow::~CWindow()
{
    //Log( "CWindow::~CWindow()\n" );
    Release();
}


//-----------------------------------------------------------------------------
// Name: Init()
// Desc:
//-----------------------------------------------------------------------------
bool CWindow::Init( HWND hParentWnd, int iX, int iY, int iWidth, int iHeight,
                    unsigned long ulFlags, char* strMenuName )
{
    m_XPos    = iX;
    m_YPos    = iY;
    m_Width   = iWidth;
    m_Height  = iHeight;
    m_hParent = hParentWnd;

    if( strMenuName != NULL )
        strcpy( m_strMenuName, strMenuName );

    g_CWindowInstances++;

    sprintf( m_WndName, "CWindow%d", g_CWindowInstances );
    m_hInst = GetModuleHandle( NULL );

    if( !m_hInst )
    {
        Log( "if( !GetInst() )\n" );
        return false;
    }

    if( !Register() )
    {
        Log( "if( !Register() )\n" );
        return false;
    }

    m_hWnd = CreateWindow( m_WndName, m_WndName, ulFlags,
                           m_XPos, m_YPos, m_Width, m_Height,
                           m_hParent, (HMENU)NULL, m_hInst, (LPVOID)this );

    if( !m_hWnd )
    {
        Log( "if( !m_hWnd )\n" );
        return false;
    }

    ShowWindow( m_hWnd, SW_NORMAL );
    UpdateWindow( m_hWnd );

    m_bNeedsUpdate = true; 

    return true;
}


//-----------------------------------------------------------------------------
// Name: Release()
// Desc:
//-----------------------------------------------------------------------------
void CWindow::Release()
{
   // Log( "CWindow::Release()\n" );

    wglDeleteContext( GetRC() );
    ReleaseDC( GetWnd(), GetDC() );
    delete (LPVOID)GetWindowLong( GetWnd(), GWL_USERDATA );
    DestroyWindow( GetWnd() );
    UnregisterClass( m_WndName, GetInst() );

    m_hRC     = NULL;
    m_hInst   = NULL;
    m_hWnd    = NULL;
    m_hParent = NULL;
    m_XPos    = 0;
    m_YPos    = 0;
    m_Width   = 0;
    m_Height  = 0;
}


//-----------------------------------------------------------------------------
// Name: Register()
// Desc:
//-----------------------------------------------------------------------------
bool CWindow::Register()
{
   // Log( "CWindow::Register()\n" );

    WNDCLASS wc;

    memset( &wc, 0, sizeof( WNDCLASS ) );

    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = (WNDPROC)CWindowProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = m_hInst;
    wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 4);
    wc.lpszMenuName  = m_strMenuName;
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor( NULL, IDC_CROSS );
    wc.lpszClassName = m_WndName;

    if( !RegisterClass( &wc ) )
    {
        Log( "RegisterClass()\n" );
        return false;
    }

    return true;
}


//-----------------------------------------------------------------------------
// Name: OnWMPaint()
// Desc:
//-----------------------------------------------------------------------------
void CWindow::OnWMPaint()
{
    PAINTSTRUCT ps;
    
    BeginPaint( m_hWnd, &ps );
    EndPaint( m_hWnd, &ps );
}


//-----------------------------------------------------------------------------
// Name: OnWMSize()
// Desc:
//-----------------------------------------------------------------------------
void CWindow::OnWMSize()
{
    m_bNeedsUpdate = true; 
}

//-----------------------------------------------------------------------------
// Name: MoveWnd()
// Desc:
//-----------------------------------------------------------------------------
void CWindow::MoveWnd()
{
    m_bNeedsUpdate = true; 
    MoveWindow( m_hWnd, m_XPos, m_YPos, m_Width, m_Height, TRUE );
}


//-----------------------------------------------------------------------------
// Name: WindowProc()
// Desc:
//-----------------------------------------------------------------------------
LRESULT CALLBACK CWindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_CREATE:
        {
            CWindow* pWindow = (CWindow*)((LPCREATESTRUCT) lParam)->lpCreateParams;

            LPWINDOW pWnd = new WINDOW;

            pWnd->pWindow = pWindow;

            SetWindowLong( hWnd, GWL_USERDATA, (long)pWnd );

            PostMessage( hWnd, WM_SIZE, 0, 0 );

            return 0;
        }

        case WM_NOTIFY:
        {
            NMHDR* nmptr = (LPNMHDR) lParam;

            if( nmptr->code == TCN_SELCHANGE )
                PostMessage( GetParent( hWnd ), CMSG_CHANGE_TAB, wParam, lParam );

            return 0;
        }

        case WM_COMMAND:
        {
            switch( wParam )
            {
                case IDD_CUBE:
                {
                    PostMessage( GetParent( hWnd ), WM_COMMAND, IDD_CUBE, 0 );
                    return 0;
                }

                case IDD_TRIANGLE:
                {
                    PostMessage( GetParent( hWnd ), WM_COMMAND, IDD_TRIANGLE, 0 );
                    return 0;
                }

                case IDD_SHEET:
                {
                    PostMessage( GetParent( hWnd ), WM_COMMAND, IDD_SHEET, 0 );
                    return 0;
                }
                
                case IDD_ADDBRUSH:
                {
                    PostMessage( GetParent( hWnd ), WM_COMMAND, IDD_ADDBRUSH, 0 );
                    return 0;
                }

                case IDD_NEXTVERTEX:
                {
                    PostMessage( GetParent( hWnd ), WM_COMMAND, IDD_NEXTVERTEX, 0 );
                    return 0;
                }

                case IDD_ADDSTAIRS:
                {
                    PostMessage( GetParent( hWnd ), WM_COMMAND, IDD_ADDSTAIRS, 0 );
                    return 0;
                }
            }
                
            return 0;
        }

        case WM_RBUTTONDOWN:
        {
            SetCapture( hWnd );
            ShowCursor( FALSE );

            PostMessage( GetParent( GetParent( hWnd ) ), WM_RBUTTONDOWN, wParam, lParam );
            return 0;
        }

        case WM_LBUTTONDOWN:
        {
            SetCapture( hWnd );
            ShowCursor( FALSE );

            PostMessage( GetParent( GetParent( hWnd ) ), WM_LBUTTONDOWN, wParam, lParam );
            return 0;
        }
/*
        case WM_LBUTTONDOWN:
        {
            SetCapture( hWnd );
            ShowCursor( FALSE );
            //
            
            return 0;
        }
*/
        case WM_SIZE:
        {
            LPWINDOW pWnd = (LPWINDOW)GetWindowLong( hWnd, GWL_USERDATA );
            pWnd->pWindow->MoveWnd();
            return 0;
        }
    
        case WM_PAINT:
            {
            LPWINDOW pWnd = (LPWINDOW)GetWindowLong( hWnd, GWL_USERDATA );
            pWnd->pWindow->OnWMPaint();
            return 0;
        }

        case WM_LBUTTONUP:
        {
            
            ReleaseCapture();
            return 0;
        }
        case WM_RBUTTONUP:
        {
            
            return 0;
        }
        
    }

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

