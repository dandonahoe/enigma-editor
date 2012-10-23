//----------------------------------------------------------------------------- 
// File: SheetMiniDB.cpp
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "PolygonManager.h"
#include "CustomMsg.h"
#include "main.h"
#include "resource.h"
#include "MessageProcs.h"
#include "SheetMiniDB.h"
#include "Settings.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: OnSheetMini_WMInitDialog()
// Desc: 
//-----------------------------------------------------------------------------
void OnSheetMini_WMInitDialog( HWND hWnd )
{
    char str[256];

    ShowWindow( hWnd, true );
    UpdateWindow( hWnd );

    PolygonManager->NewBrush( SHEETBRUSH );

    sprintf( str, "%f", Settings->m_fBrush_Sheet_USize );
    SetDlgItemTextA( hWnd, IDC_EDIT_USIZE, str );

    sprintf( str, "%f", Settings->m_fBrush_Sheet_VSize );
    SetDlgItemTextA( hWnd, IDC_EDIT_VSIZE, str );

    switch( Settings->m_iBrush_Sheet_Axis )
    {
    case 1: SendDlgItemMessage( hWnd, IDC_RADIO_SHEET_X_AXIS, BM_SETCHECK, TRUE, 0 ); break;
    case 2: SendDlgItemMessage( hWnd, IDC_RADIO_SHEET_Y_AXIS, BM_SETCHECK, TRUE, 0 ); break;
    case 3: SendDlgItemMessage( hWnd, IDC_RADIO_SHEET_Z_AXIS, BM_SETCHECK, TRUE, 0 ); break;
    default: Settings->m_iBrush_Sheet_Axis = 1; break;
    }
}


//-----------------------------------------------------------------------------
// Name: OnSheetMini_WMCommand()
// Desc: 
//-----------------------------------------------------------------------------
void OnSheetMini_WMCommand( HWND hWnd, WPARAM wParam )
{
    switch( LOWORD( wParam ) )
    {
    case IDC_RADIO_SHEET_X_AXIS:
        Settings->m_iBrush_Sheet_Axis = 1;
        PolygonManager->NewBrush( SHEETBRUSH );
        break;

    case IDC_RADIO_SHEET_Y_AXIS:
        Settings->m_iBrush_Sheet_Axis = 2;
        PolygonManager->NewBrush( SHEETBRUSH );
        break;

    case IDC_RADIO_SHEET_Z_AXIS:
        Settings->m_iBrush_Sheet_Axis = 3;
        PolygonManager->NewBrush( SHEETBRUSH );
        break;
    }
}


//-----------------------------------------------------------------------------
// Name: OnSheetMini_WMDestroy()
// Desc: 
//-----------------------------------------------------------------------------
void OnSheetMini_WMDestroy( HWND hWnd )
{
    UpdateSheetData( hWnd );

    EndDialog( hWnd, 0 );
}


//-----------------------------------------------------------------------------
// Name: UpdateSheetData()
// Desc: 
//-----------------------------------------------------------------------------
void UpdateSheetData( HWND hWnd )
{
    char str[256];

    GetDlgItemTextA( hWnd, IDC_EDIT_USIZE, str, 256 );
    Settings->m_fBrush_Sheet_USize = (float)atof( str );

    GetDlgItemTextA( hWnd, IDC_EDIT_VSIZE, str, 256 );
    Settings->m_fBrush_Sheet_VSize = (float)atof( str );

    if( BST_CHECKED == SendDlgItemMessage( hWnd, IDC_RADIO_SHEET_X_AXIS, BM_GETCHECK, 0, 0 ) )
        Settings->m_iBrush_Sheet_Axis = 1;
    else if( BST_CHECKED == SendDlgItemMessage( hWnd, IDC_RADIO_SHEET_Y_AXIS, BM_GETCHECK, 0, 0 ) )
        Settings->m_iBrush_Sheet_Axis = 2;
    else if( BST_CHECKED == SendDlgItemMessage( hWnd, IDC_RADIO_SHEET_Z_AXIS, BM_GETCHECK, 0, 0 ) )
        Settings->m_iBrush_Sheet_Axis = 3;
}


//-----------------------------------------------------------------------------
// Name: OnSheetMini_WMKeydown()
// Desc: 
//-----------------------------------------------------------------------------
void OnSheetMini_WMKeydown( WPARAM wParam, HWND hWnd )
{
    switch( wParam )
    {
        case VK_RETURN: PolygonManager->NewBrush( SHEETBRUSH ); break;
    }
}


//-----------------------------------------------------------------------------
// Name: OnSheetMini_UpdateBrush()
// Desc: 
//-----------------------------------------------------------------------------
void OnSheetMini_UpdateBrush( HWND hWnd )
{
    UpdateSheetData( hWnd );
    PolygonManager->NewBrush( SHEETBRUSH );
}