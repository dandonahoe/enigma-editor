//----------------------------------------------------------------------------- 
// File: StairsMiniDB.cpp
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
#include "StairsMiniDB.h"
#include "settings.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: OnStairsMini_WMInitDialog()
// Desc: 
//-----------------------------------------------------------------------------
void OnStairsMini_WMInitDialog( HWND hWnd )
{
    ShowWindow( hWnd, true );
    UpdateWindow( hWnd );

    char str[256];

    sprintf( str, "%f", Settings->m_fBrush_Stairs_Depth );
    SetDlgItemTextA( hWnd, IDC_EDIT_STEP_DEPTH, str );

    sprintf( str, "%f", Settings->m_fBrush_Stairs_Width );
    SetDlgItemTextA( hWnd, IDC_EDIT_STEP_WIDTH, str );

    sprintf( str, "%f", Settings->m_fBrush_Stairs_Height );
    SetDlgItemTextA( hWnd, IDC_EDIT_STEP_HEIGHT, str );

    sprintf( str, "%i", Settings->m_iBrush_Stairs_NumSteps );
    SetDlgItemTextA( hWnd, IDC_EDIT_STEP_NUMSTEPS, str );

    sprintf( str, "%f", Settings->m_fBrush_Stairs_AddToFirst );
    SetDlgItemTextA( hWnd, IDC_EDIT_STEP_ADDTOFIRST, str );

    PolygonManager->NewBrush( STAIRSBRUSH );
}


//-----------------------------------------------------------------------------
// Name: OnStairsMini_WMCommand()
// Desc: 
//-----------------------------------------------------------------------------
void OnStairsMini_WMCommand( HWND hWnd, WPARAM wParam )
{
}


//-----------------------------------------------------------------------------
// Name: OnStairsMini_WMNotify()
// Desc: 
//-----------------------------------------------------------------------------
void OnStairsMini_WMNotify( HWND hWnd, LPARAM lParam )
{
}


//-----------------------------------------------------------------------------
// Name: OnStairsMini_WMDestroy()
// Desc: 
//-----------------------------------------------------------------------------
void OnStairsMini_WMDestroy( HWND hWnd )
{
    UpdateStairsData( hWnd );
    EndDialog( hWnd, 0 );
}


//-----------------------------------------------------------------------------
// Name: UpdateCubeData()
// Desc: 
//-----------------------------------------------------------------------------
void UpdateStairsData( HWND hWnd )
{
    char str[256];

    GetDlgItemTextA( hWnd, IDC_EDIT_STEP_DEPTH, str, 256 );
    Settings->m_fBrush_Stairs_Depth = (float)atof( str );

    GetDlgItemTextA( hWnd, IDC_EDIT_STEP_WIDTH, str, 256 );
    Settings->m_fBrush_Stairs_Width = (float)atof( str );

    GetDlgItemTextA( hWnd, IDC_EDIT_STEP_HEIGHT, str, 256 );
    Settings->m_fBrush_Stairs_Height = (float)atof( str );

    GetDlgItemTextA( hWnd, IDC_EDIT_STEP_NUMSTEPS, str, 256 );
    Settings->m_iBrush_Stairs_NumSteps = (int)atof( str );

    GetDlgItemTextA( hWnd, IDC_EDIT_STEP_ADDTOFIRST, str, 256 );
    Settings->m_fBrush_Stairs_AddToFirst = (float)atof( str );

    if( Settings->m_iBrush_Stairs_NumSteps <= 0 )
        Settings->m_iBrush_Stairs_NumSteps = 1;
}


//-----------------------------------------------------------------------------
// Name: OnSheetMini_UpdateBrush()
// Desc: 
//-----------------------------------------------------------------------------
void OnStairsMini_UpdateBrush( HWND hWnd )
{
    UpdateStairsData( hWnd );
    PolygonManager->NewBrush( STAIRSBRUSH );
}