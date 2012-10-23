//----------------------------------------------------------------------------- 
// File: CubeMiniDB.cpp
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
#include "CubeMiniDB.h"
#include "Settings.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: OnCubeMini_WMInitDialog()
// Desc: 
//-----------------------------------------------------------------------------
void OnCubeMini_WMInitDialog( HWND hWnd )
{
    ShowWindow( hWnd, true );
    UpdateWindow( hWnd );

    PolygonManager->NewBrush( CUBEBRUSH );
    
    char str[256];

    sprintf( str, "%f", Settings->m_fBrush_Cube_Depth );
    SetDlgItemTextA( hWnd, IDC_EDIT_CUBE_DEPTH, str );

    sprintf( str, "%f", Settings->m_fBrush_Cube_Width );
    SetDlgItemTextA( hWnd, IDC_EDIT_CUBE_WIDTH, str );

    sprintf( str, "%f", Settings->m_fBrush_Cube_Height );
    SetDlgItemTextA( hWnd, IDC_EDIT_CUBE_HEIGHT, str );
}


//-----------------------------------------------------------------------------
// Name: OnCubeMini_WMCommand()
// Desc: 
//-----------------------------------------------------------------------------
void OnCubeMini_WMCommand( HWND hWnd, WPARAM wParam )
{
}


//-----------------------------------------------------------------------------
// Name: OnCubeMini_WMNotify()
// Desc: 
//-----------------------------------------------------------------------------
void OnCubeMini_WMNotify( HWND hWnd, LPARAM lParam )
{
}


//-----------------------------------------------------------------------------
// Name: OnCubeMini_WMDestroy()
// Desc: 
//-----------------------------------------------------------------------------
void OnCubeMini_WMDestroy( HWND hWnd )
{
    UpdateCubeData( hWnd );
    EndDialog( hWnd, 0 );
}


//-----------------------------------------------------------------------------
// Name: UpdateCubeData()
// Desc: 
//-----------------------------------------------------------------------------
void UpdateCubeData( HWND hWnd )
{
    char str[256];

    GetDlgItemTextA( hWnd, IDC_EDIT_CUBE_DEPTH, str, 256 );
    Settings->m_fBrush_Cube_Depth = (float)atof( str );

    GetDlgItemTextA( hWnd, IDC_EDIT_CUBE_WIDTH, str, 256 );
    Settings->m_fBrush_Cube_Width = (float)atof( str );

    GetDlgItemTextA( hWnd, IDC_EDIT_CUBE_HEIGHT, str, 256 );
    Settings->m_fBrush_Cube_Height = (float)atof( str );
}

//-----------------------------------------------------------------------------
// Name: OnSheetMini_UpdateBrush()
// Desc: 
//-----------------------------------------------------------------------------
void OnCubeMini_UpdateBrush( HWND hWnd )
{
    UpdateCubeData( hWnd );
    PolygonManager->NewBrush( CUBEBRUSH );
}