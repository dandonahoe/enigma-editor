//----------------------------------------------------------------------------- 
// File: SpiralMiniDB.cpp
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#include <windows.h>
#include <commctrl.h>
#include "PolygonManager.h"
#include "CustomMsg.h"
#include "main.h"
#include "resource.h"
#include "MessageProcs.h"
#include "SpiralMiniDB.h"
#include "Settings.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: OnSpiralMini_WMInitDialog()
// Desc: 
//-----------------------------------------------------------------------------
void OnSpiralMini_WMInitDialog( HWND hWnd )
{
    ShowWindow( hWnd, true );
    UpdateWindow( hWnd );

    char str[256];

    sprintf( str, "%f", Settings->m_fBrush_Spiral_Width );
    SetDlgItemTextA( hWnd, IDC_EDIT_SPIRAL_WIDTH, str );

    sprintf( str, "%f", Settings->m_fBrush_Spiral_Height );
    SetDlgItemTextA( hWnd, IDC_EDIT_SPIRAL_HEIGHT, str );

    sprintf( str, "%i", Settings->m_iBrush_Spiral_NumSteps );
    SetDlgItemTextA( hWnd, IDC_EDIT_SPIRAL_NUMSTEPS, str );

    sprintf( str, "%f", Settings->m_fBrush_Spiral_AddToFirst );
    SetDlgItemTextA( hWnd, IDC_EDIT_SPIRAL_ADDTOFIRST, str );

    sprintf( str, "%f", Settings->m_fBrush_Spiral_Degrees );
    SetDlgItemTextA( hWnd, IDC_EDIT_SPIRAL_DEGREES, str );

    sprintf( str, "%f", Settings->m_fBrush_Spiral_InnerRadius );
    SetDlgItemTextA( hWnd, IDC_EDIT_SPIRAL_INNERRADIUS, str );

    PolygonManager->NewBrush( SPIRALBRUSH );
}


//-----------------------------------------------------------------------------
// Name: OnSpiralMini_WMCommand()
// Desc: 
//-----------------------------------------------------------------------------
void OnSpiralMini_WMCommand( HWND hWnd, WPARAM wParam )
{
}


//-----------------------------------------------------------------------------
// Name: OnSpiralMini_WMDestroy()
// Desc: 
//-----------------------------------------------------------------------------
void OnSpiralMini_WMDestroy( HWND hWnd )
{
    UpdateSpiralData( hWnd );
    EndDialog( hWnd, 0 );
}


//-----------------------------------------------------------------------------
// Name: UpdateCubeData()
// Desc: 
//-----------------------------------------------------------------------------
void UpdateSpiralData( HWND hWnd )
{
    char str[256];

    GetDlgItemTextA( hWnd, IDC_EDIT_SPIRAL_WIDTH, str, 256 );
    Settings->m_fBrush_Spiral_Width = (float)atof( str );

    GetDlgItemTextA( hWnd, IDC_EDIT_SPIRAL_HEIGHT, str, 256 );
    Settings->m_fBrush_Spiral_Height = (float)atof( str );

    GetDlgItemTextA( hWnd, IDC_EDIT_SPIRAL_NUMSTEPS, str, 256 );
    Settings->m_iBrush_Spiral_NumSteps = (int)atof( str );

    GetDlgItemTextA( hWnd, IDC_EDIT_SPIRAL_ADDTOFIRST, str, 256 );
    Settings->m_fBrush_Spiral_AddToFirst = (float)atof( str );

    GetDlgItemTextA( hWnd, IDC_EDIT_SPIRAL_DEGREES, str, 256 );
    Settings->m_fBrush_Spiral_Degrees = (float)atof( str );

    GetDlgItemTextA( hWnd, IDC_EDIT_SPIRAL_INNERRADIUS, str, 256 );
    Settings->m_fBrush_Spiral_InnerRadius = (float)atof( str );

    if( Settings->m_iBrush_Spiral_NumSteps <= 0 )
        Settings->m_iBrush_Spiral_NumSteps = 1;
}

//-----------------------------------------------------------------------------
// Name: OnSheetMini_UpdateBrush()
// Desc: 
//-----------------------------------------------------------------------------
void OnSpiralMini_UpdateBrush( HWND hWnd )
{
    UpdateSpiralData( hWnd );
    PolygonManager->NewBrush( SPIRALBRUSH );
}
