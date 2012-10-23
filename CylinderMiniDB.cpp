//----------------------------------------------------------------------------- 
// File: CylinderMiniDB.cpp
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
#include "settings.h"
#include "CylinderMiniDB.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: OnCylinderMini_WMInitDialog()
// Desc: 
//-----------------------------------------------------------------------------
void OnCylinderMini_WMInitDialog( HWND hWnd )
{
    ShowWindow( hWnd, true );
    UpdateWindow( hWnd );

    char str[256];

    sprintf( str, "%f", Settings->m_fBrush_Cylinder_Height );
    SetDlgItemTextA( hWnd, IDC_EDIT_CYLINDER_HEIGHT, str );

    sprintf( str, "%f", Settings->m_fBrush_Cylinder_Radius );
    SetDlgItemTextA( hWnd, IDC_EDIT_CYLINDER_RADIUS, str );

    sprintf( str, "%i", Settings->m_iBrush_Cylinder_NumSides );
    SetDlgItemTextA( hWnd, IDC_EDIT_CYLINDER_SIDES, str );

    PolygonManager->NewBrush( CYLINDERBRUSH );
}


//-----------------------------------------------------------------------------
// Name: OnCylinderMini_WMCommand()
// Desc: 
//-----------------------------------------------------------------------------
void OnCylinderMini_WMCommand( HWND hWnd, WPARAM wParam )
{
}


//-----------------------------------------------------------------------------
// Name: OnCylinderMini_WMNotify()
// Desc: 
//-----------------------------------------------------------------------------
void OnCylinderMini_WMNotify( HWND hWnd, LPARAM lParam )
{
}


//-----------------------------------------------------------------------------
// Name: OnCylinderMini_WMDestroy()
// Desc: 
//-----------------------------------------------------------------------------
void OnCylinderMini_WMDestroy( HWND hWnd )
{
    UpdateCylinderData( hWnd );
    EndDialog( hWnd, 0 );
}


//-----------------------------------------------------------------------------
// Name: UpdateCylinderData()
// Desc: 
//-----------------------------------------------------------------------------
void UpdateCylinderData( HWND hWnd )
{
    char str[256];

    GetDlgItemTextA( hWnd, IDC_EDIT_CYLINDER_HEIGHT, str, 256 );
    Settings->m_fBrush_Cylinder_Height = (float)atof( str );

    GetDlgItemTextA( hWnd, IDC_EDIT_CYLINDER_RADIUS, str, 256 );
    Settings->m_fBrush_Cylinder_Radius = (float)atof( str );

    GetDlgItemTextA( hWnd, IDC_EDIT_CYLINDER_SIDES, str, 256 );
    Settings->m_iBrush_Cylinder_NumSides = (int)atof( str );

    if( Settings->m_iBrush_Cylinder_NumSides <= 0 )
        Settings->m_iBrush_Cylinder_NumSides = 1;
}


//-----------------------------------------------------------------------------
// Name: OnSheetMini_UpdateBrush()
// Desc: 
//-----------------------------------------------------------------------------
void OnCylinderMini_UpdateBrush( HWND hWnd )
{
    UpdateCylinderData( hWnd );
    PolygonManager->NewBrush( CYLINDERBRUSH );
}