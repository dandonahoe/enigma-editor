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
#include "OpenGL.h"
#include "WindowPane.h"
#include "main.h"
#include "CustomMsg.h"
//-----------------------------------------------------------------------------

 
LRESULT CALLBACK WindowPaneProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
int g_CWindowPaneInstances = 0;

//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
CWindowPane::CWindowPane()
{
 //   Log( "CWindowPane::CWindowPane()\n" );

    m_hInst         = NULL;
    m_hPenShadow    = NULL;
    m_hPenHighLite  = NULL;
    m_hPenBlack     = NULL;
    m_hWnd          = NULL;
    m_hParent       = NULL;
    m_XPos          = 0;
    m_YPos          = 0;
    m_Width         = 0;
    m_Height        = 0;
    m_BarSize       = 0;
    m_NumOtherPanes = 0;
    m_bLocked       = false;

    for( int a = 0; a < NUM_OTHER_PANES; a++ )
        m_OtherPanes[a].pPane = NULL;
}
CWindowPane::~CWindowPane()
{
   // Log( "CWindowPane::~CWindowPane()\n" );

    Release();
}


//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Registers the bar window, and displays it
//-----------------------------------------------------------------------------
HWND CWindowPane::Init( HWND hWndParent, int Pos, WPaneStyle Style, int BarSize )
{
   // Log( "CWindowPane::Init()\n" );

    RECT rc;

    g_CWindowPaneInstances++;

    sprintf( m_WndName, "CWindowPane%d", g_CWindowPaneInstances );

    m_hInst    = GetModuleHandle( NULL );
    m_hParent  = hWndParent;
    m_PaneType = Style;
    m_BarSize  = BarSize;

    if( !m_hParent )
    {
        Log( "if( !m_hParent )\n" );
        return NULL;
    }

    if( !m_hInst )
    {
        Log( "if( !m_hInst )\n" );
        return NULL;
    }

    if( !Register() )
    {
        Log( "if( !Register() )\n" );
        return NULL;
    }
   
    GetClientRect( m_hParent, &rc );

    if( CWP_HORIZONTAL == m_PaneType )
    {
        m_XPos   = rc.left;
        m_YPos   = Pos;
        m_Width  = rc.right;
        m_Height = BarSize;
    }
    else
    {
        m_XPos   = Pos;
        m_YPos   = rc.top;
        m_Width  = BarSize;
        m_Height = rc.bottom;
    }

    m_hWnd = CreateWindow( m_WndName, m_WndName, WS_CHILD, 0, 0, 0, 0,
                           m_hParent, NULL, m_hInst, (LPVOID)this );

    if( !m_hWnd )
    {
        Log( "if( !m_hWnd )\n" );
        return NULL;
    }

    m_hPenShadow   = CreatePen( PS_SOLID, 1, GetSysColor( COLOR_3DSHADOW ) );
    m_hPenHighLite = CreatePen( PS_SOLID, 1, GetSysColor( COLOR_3DHIGHLIGHT ) );
    m_hPenBlack    = CreatePen( PS_SOLID, 1, RGB( 0, 0, 0 ) );

    if( m_hPenShadow == NULL )
        Log( "if( m_hPenShadow == NULL )\n" );

    if( m_hPenHighLite == NULL )
        Log( "if( m_hPenHighLite == NULL )\n" );

    if( m_hPenBlack == NULL )
        Log( "if( m_hPenBlack == NULL )\n" );

    ShowWindow( m_hWnd, SW_NORMAL );
    UpdateWindow( m_hWnd );

    return m_hWnd;
}


//-----------------------------------------------------------------------------
// Name: DrawBar()
// Desc: Draws all the lines around the borders to make it look 3d-ish
//-----------------------------------------------------------------------------
void CWindowPane::OnWMPaint()
{
    PAINTSTRUCT ps;
    
    BeginPaint( m_hWnd, &ps );

    if( m_PaneType == CWP_VERTICAL )
    {
        SelectObject( ps.hdc, m_hPenHighLite );
        MoveToEx( ps.hdc, ps.rcPaint.left + 1, ps.rcPaint.top, NULL );
        LineTo( ps.hdc, ps.rcPaint.left + 1, ps.rcPaint.bottom );
        SelectObject( ps.hdc, m_hPenShadow );
        MoveToEx( ps.hdc, ps.rcPaint.right - 2, ps.rcPaint.top, NULL );
        LineTo( ps.hdc, ps.rcPaint.right - 2, ps.rcPaint.bottom );
        SelectObject( ps.hdc, m_hPenBlack );
        MoveToEx( ps.hdc, ps.rcPaint.right - 1, ps.rcPaint.top, NULL );
        LineTo( ps.hdc, ps.rcPaint.right - 1, ps.rcPaint.bottom );
    }
    else
    {
        SelectObject( ps.hdc, m_hPenHighLite );
        MoveToEx( ps.hdc, ps.rcPaint.left, ps.rcPaint.top + 1, NULL );
        LineTo( ps.hdc, ps.rcPaint.right, ps.rcPaint.top + 1 );
        SelectObject( ps.hdc, m_hPenShadow );
        MoveToEx( ps.hdc, ps.rcPaint.left, ps.rcPaint.bottom - 2, NULL );
        LineTo( ps.hdc, ps.rcPaint.right, ps.rcPaint.bottom - 2 );
        SelectObject( ps.hdc, m_hPenBlack );
        MoveToEx( ps.hdc, ps.rcPaint.left, ps.rcPaint.bottom - 1, NULL );
        LineTo( ps.hdc, ps.rcPaint.right, ps.rcPaint.bottom - 1 );
    }

    EndPaint( m_hWnd, &ps );
}


//-----------------------------------------------------------------------------
// Name: Register()
// Desc: Registers the window
//-----------------------------------------------------------------------------
bool CWindowPane::Register()
{
    //Log( "CWindowPane::Register()\n" );

    WNDCLASS wc;

    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = (WNDPROC)WindowPaneProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = m_hInst;
    wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
    wc.lpszMenuName  = NULL;
    wc.hIcon         = NULL;

    if( m_PaneType == CWP_HORIZONTAL )
        wc.hCursor       = LoadCursor( NULL, IDC_SIZENS );
    else
        wc.hCursor       = LoadCursor( NULL, IDC_SIZEWE );

    wc.lpszClassName = m_WndName;

    if( !RegisterClass( &wc ) )
        return false;

    return true;
}


//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Frees all memory
//-----------------------------------------------------------------------------
void CWindowPane::Release()
{
  //  Log( "CWindowPane::Release()\n" );

    delete (LPVOID)GetWindowLong( m_hWnd, GWL_USERDATA );
    DestroyWindow( m_hWnd );
    DeleteObject( m_hPenHighLite );
    DeleteObject( m_hPenShadow );
    DeleteObject( m_hPenBlack );
    UnregisterClass( m_WndName, m_hInst );
}


//-----------------------------------------------------------------------------
// Name: MouseMove()
// Desc: Drags the bar left, right, up or down
//-----------------------------------------------------------------------------
void CWindowPane::OnWMMouseMove( WPARAM wParam )
{
    if( m_bLocked ) return;

    RECT rc;
    POINT pt;

    GetClientRect( m_hParent, &rc );
    GetCursorPos( &pt );
    ScreenToClient( m_hParent, &pt );

    static int x = pt.x, y = pt.y;

    // If it hasn't moved, don't do anything
    if( pt.x == x && pt.y == y )
        return;

    // Update the position
    x = pt.x;
    y = pt.y;

    if( wParam == MK_LBUTTON )
    {
        if( CWP_HORIZONTAL == m_PaneType )
        { 
            m_YPos  = pt.y - ( m_BarSize / 2 );
            m_Height = m_BarSize;
            m_Width = rc.right;

            if( m_YPos < 0 )
                m_YPos = 0;
            else if( m_YPos > rc.bottom - 30 )
                m_YPos = rc.bottom - 30;

            MoveWnd();

            InvalidateRect( m_hWnd, NULL, TRUE );
        }
        else
        {
            m_XPos  = pt.x - m_BarSize / 2;
            m_Width = m_BarSize;

            if( m_XPos < 0 )
                m_XPos = 0;
            else if( m_XPos > rc.right - m_BarSize )
                m_XPos = rc.right - m_BarSize;

            MoveWnd();

            InvalidateRect( m_hWnd, NULL, TRUE );
        }
    }

    AlignOtherPanes();
}


//-----------------------------------------------------------------------------
// Name: Size()
// Desc: When the parent window has been resized, make the bar fit it
//-----------------------------------------------------------------------------
void CWindowPane::OnWMSize()
{
//    Log( "CWindowPane::OnWMSize()\n" );
}


//-----------------------------------------------------------------------------
// Name: MoveWnd()
// Desc: 
//-----------------------------------------------------------------------------
void CWindowPane::MoveWnd()
{
    MoveWindow( m_hWnd, m_XPos, m_YPos, m_Width, m_Height, TRUE );
}


//-----------------------------------------------------------------------------
// Name: WindowPaneProc()
// Desc: Message handling for all instances of CWindowPane
//-----------------------------------------------------------------------------
LRESULT CALLBACK WindowPaneProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_CREATE:
        {
            CWindowPane* pPane = (CWindowPane*)((LPCREATESTRUCT) lParam)->lpCreateParams;
            LPWINDOWPANE pWindowData = new WINDOWPANE;
            pWindowData->pWindowPane = pPane;
            SetWindowLong( hWnd, GWL_USERDATA, (long)pWindowData );
            PostMessage( hWnd, WM_SIZE, 0, 0 );
            return 0;
        }

        case WM_LBUTTONDOWN:
        {
            SetCapture( hWnd );
            return 0;
        }

        case WM_SIZE:
        {
            LPWINDOWPANE pPane = (LPWINDOWPANE)GetWindowLong( hWnd, GWL_USERDATA );
            pPane->pWindowPane->MoveWnd();
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            LPWINDOWPANE pPane = (LPWINDOWPANE)GetWindowLong( hWnd, GWL_USERDATA );
            pPane->pWindowPane->OnWMMouseMove( wParam );
            return 0;    
        }
    
        case WM_PAINT:
        {
            LPWINDOWPANE pPane = (LPWINDOWPANE)GetWindowLong( hWnd, GWL_USERDATA );
            pPane->pWindowPane->OnWMPaint();
            return 0;
        }

        case WM_LBUTTONUP:
        {

            LPWINDOWPANE pPane = (LPWINDOWPANE)GetWindowLong( hWnd, GWL_USERDATA );
            pPane->pWindowPane->AlignOtherPanes();
            pPane->pWindowPane->NotifyOthers();
            ReleaseCapture();
            return 0;
        }

        case CMSG_ALIGNOTHERPANES:
        {
            LPWINDOWPANE pPane = (LPWINDOWPANE)GetWindowLong( hWnd, GWL_USERDATA );
            pPane->pWindowPane->AlignOtherPanes();
            return 0;
        }
    }

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}


//-----------------------------------------------------------------------------
// Name: AddPane()
// Desc: 
//-----------------------------------------------------------------------------
void CWindowPane::AddPaneToManage( CWindowPane* pPane, WindowConstraints flag )
{
//    Log( "CWindowPane::AddPane()\n" );

    if( pPane == NULL )
    {
        Log( "if( pPane == NULL )\n" );
        return;
    }

    if( GetParentWnd() != pPane->GetParentWnd() )
    {
        Log( "if( GetParentWnd() != pPane->GetParentWnd() )\n" );
        return;
    }

    m_OtherPanes[m_NumOtherPanes].pPane = pPane;
    m_OtherPanes[m_NumOtherPanes].flag = flag;

    m_NumOtherPanes++;

    AlignOtherPanes();
}


//-----------------------------------------------------------------------------
// Name: AlignOtherPanes()
// Desc: 
//-----------------------------------------------------------------------------
void CWindowPane::AlignOtherPanes()
{
    WPaneStyle style;
    CWindowPane *pPane;
    RECT rc;
    GetClientRect( m_hParent, &rc );

    for( int a = 0; a < m_NumOtherPanes; a++ )
    {
        pPane = m_OtherPanes[a].pPane;
        style = pPane->GetType();

        switch( m_OtherPanes[a].flag )
        {
        case CWP_STAYABOVE:
            if( style == CWP_VERTICAL )
            {
                pPane->SetHeight( GetY() );
            }
            else
            {
                if( pPane->GetY() + pPane->GetHeight() > GetY() )
                    pPane->SetY( GetY() - pPane->GetHeight() );
            }
            break;

        case CWP_STAYBELOW:
            if( style == CWP_VERTICAL )
            {
                pPane->SetY( GetY() + GetHeight() );
            }
            else
            {
                if( pPane->GetY() < GetY() + GetHeight() )
                    pPane->SetY( GetY() + GetHeight() );

                if( pPane->GetY() + pPane->GetHeight() > rc.bottom )
                {
                    pPane->SetY( rc.bottom - ( pPane->GetHeight() ) );
                    SetY( pPane->GetY() - GetHeight() );
                }
            }
            break;

        case CWP_STAYLEFT:
            if( style == CWP_VERTICAL )
            {
                if( pPane->GetX() > GetX() - GetWidth() )
                    pPane->SetX( GetX() - GetWidth() );

                if( pPane->GetX() < 0 )
                {
                    pPane->SetX( 0 );
                    SetX( pPane->GetWidth() );
                }
            }
            else
            {
                pPane->SetWidth( GetX() );
            }
            break;

        case CWP_STAYRIGHT:
            if( style == CWP_VERTICAL )
            {
                if( pPane->GetX() < GetX() + GetWidth() )
                    pPane->SetX( GetX() + GetWidth() );
                
                if( pPane->GetX() + pPane->GetWidth() > rc.right )
                {
                    pPane->SetX( rc.right - pPane->GetWidth() );
                    SetX( rc.right - ( pPane->GetWidth() + GetWidth() ) );
                }
            }
            else
            {
                pPane->SetX( GetX() + GetWidth() );
            }
            break;

        default:
            Log( "Unknown Flag Type @ CWindowPane::AlignOtherPanes()\n" );
            continue;
            break;
        }

        if( style == CWP_VERTICAL )
        {
            if( pPane->GetY() + pPane->GetHeight() > rc.bottom )
                pPane->SetHeight( pPane->GetHeight() - ( pPane->GetY() + pPane->GetHeight() - ( rc.bottom ) ) );

            if( pPane->GetY() + pPane->GetHeight() < rc.bottom )
                pPane->SetHeight( pPane->GetHeight() + ( ( rc.bottom ) - ( pPane->GetY() + pPane->GetHeight() ) ) );
        }


        pPane->MoveWnd();

        InvalidateRect( pPane->GetWnd(), NULL, TRUE );
    }

    InvalidateRect( m_hWnd, NULL, TRUE );
}


//-----------------------------------------------------------------------------
// Name: NotifyOthers()
// Desc: 
//-----------------------------------------------------------------------------
void CWindowPane::NotifyOthers()
{
    PostMessage( GetParent( m_hParent ), CMSG_ALIGNVIEWS, 0, 0 );
}