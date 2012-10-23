//----------------------------------------------------------------------------- 
// File: VertexMiniDB.cpp
//
// Desc: Processing for the mini vertex dialog, which appears when a single
//       vertex is selected in the polygon tree.
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#include <windows.h>
#include <commctrl.h>
#include "VertexMiniDB.h"
#include "CustomMsg.h"
#include "PolygonManager.h"
#include "resource.h"
//-----------------------------------------------------------------------------


// These globals are only used here to reduce repetitive calls to 'Get'
// functions
int g_R, g_G, g_B;


//-----------------------------------------------------------------------------
// Name: OnVertexMini_WMInitDialog()
// Desc: Creates all the UpDown  controls after getting the current vertex's
//       information.
//-----------------------------------------------------------------------------
void OnVertexMini_WMInitDialog( HWND hWnd )
{
    ShowWindow( hWnd, true );
    UpdateWindow( hWnd );

    // Get handles to each edit box
    HWND hTextureU = GetDlgItem( hWnd, IDC_EDIT_TEXTUREU );
    HWND hTextureV = GetDlgItem( hWnd, IDC_EDIT_TEXTUREV );
    HWND hRed      = GetDlgItem( hWnd, IDC_EDIT_RED );
    HWND hGreen    = GetDlgItem( hWnd, IDC_EDIT_GREEN );
    HWND hBlue     = GetDlgItem( hWnd, IDC_EDIT_BLUE );


    // Gather the current vertex information
    int r, g, b, u, v;
    PolygonManager->GetVertexInfo( r, g, b, u, v );
    

    // Create all the controls
    CreateUpDownControl( WS_CHILD | WS_BORDER | WS_VISIBLE |
                         UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_HOTTRACK | UDS_WRAP,
                         10, 10, 50, 50, hWnd, ID_TEXTUREUUPDOWN, NULL, hTextureU, 10, 0, u );

    CreateUpDownControl( WS_CHILD | WS_BORDER | WS_VISIBLE |
                         UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_HOTTRACK | UDS_WRAP,
                         10, 10, 50, 50, hWnd, ID_TEXTUREUUPDOWN, NULL, hTextureV, 10, 0, v );

    CreateUpDownControl( WS_CHILD | WS_BORDER | WS_VISIBLE |
                         UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_HOTTRACK | UDS_WRAP,
                         10, 10, 50, 50, hWnd, ID_COLORRED, NULL, hRed, 255, 0, r );

    CreateUpDownControl( WS_CHILD | WS_BORDER | WS_VISIBLE |
                         UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_HOTTRACK | UDS_WRAP,
                         10, 10, 50, 50, hWnd, ID_COLORGREEN, NULL, hGreen, 255, 0, g );

    CreateUpDownControl( WS_CHILD | WS_BORDER | WS_VISIBLE |
                         UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_HOTTRACK | UDS_WRAP,
                         10, 10, 50, 50, hWnd, ID_COLORBLUE, NULL, hBlue, 255, 0, b );
}


//-----------------------------------------------------------------------------
// Name: OnVertexMini_WMDestroy()
// Desc: Updates the polygon with the dialog info
//-----------------------------------------------------------------------------
void OnVertexMini_WMDestroy( HWND hWnd )
{
    BOOL bBool;

    // Get all the values
    int r = GetDlgItemInt( hWnd, IDC_EDIT_RED,      &bBool, true );
    int g = GetDlgItemInt( hWnd, IDC_EDIT_GREEN,    &bBool, true );
    int b = GetDlgItemInt( hWnd, IDC_EDIT_BLUE,     &bBool, true );
    int u = GetDlgItemInt( hWnd, IDC_EDIT_TEXTUREU, &bBool, true );
    int v = GetDlgItemInt( hWnd, IDC_EDIT_TEXTUREV, &bBool, true );

    // Set them all before exiting
    PolygonManager->SetVertexInfo( r, g, b, u, v );

    EndDialog( hWnd, 0 );
}


//-----------------------------------------------------------------------------
// Name: OnVertexMini_WMPaint()
// Desc: Fills the color box to the correct color
//-----------------------------------------------------------------------------
void OnVertexMini_WMPaint( HWND hWnd )
{
    BOOL bBool; // Not reall used
    HDC hDC = GetDC( hWnd );

    // Get the current values
    g_R = GetDlgItemInt( hWnd, IDC_EDIT_RED,   &bBool, true );
    g_G = GetDlgItemInt( hWnd, IDC_EDIT_GREEN, &bBool, true );
    g_B = GetDlgItemInt( hWnd, IDC_EDIT_BLUE,  &bBool, true );

    // Create a color brush
    HBRUSH hBrush = CreateSolidBrush( RGB( g_R, g_G, g_B ) );

    // Set thedrawing box
    RECT rcBox = { 8, 43, 177, 72 };

    // Draw it
    FillRect( hDC, &rcBox, hBrush );

    // Not sure if the hDC has to be deleted, but it is
    ReleaseDC( hWnd, hDC );
    DeleteObject( hBrush ); 
    DeleteObject( hDC ); 
}


//-----------------------------------------------------------------------------
// Name: OnVertexMini_WMCommand()
// Desc: Process system messages
//-----------------------------------------------------------------------------
void OnVertexMini_WMCommand( HWND hWnd, WPARAM wParam )
{
    BOOL bBool;

    // If the color Edit-Box's have changed, update the color box
    if( HIWORD( wParam ) == EN_UPDATE && LOWORD( wParam ) == IDC_EDIT_RED ||
        LOWORD( wParam ) == IDC_EDIT_GREEN || LOWORD( wParam ) == IDC_EDIT_BLUE )
    {
        // Updates the current color
        OnVertexMini_WMPaint( hWnd );
        
        // To keep things correct, just get the tex coords too
        int u = GetDlgItemInt( hWnd, IDC_EDIT_TEXTUREU, &bBool, true );
        int v = GetDlgItemInt( hWnd, IDC_EDIT_TEXTUREV, &bBool, true );

        // Sets the vertex to the dialog values
        PolygonManager->SetVertexInfo( g_R, g_G, g_B, u, v );
    }
}