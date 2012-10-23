//----------------------------------------------------------------------------- 
// File: BrushTabDB.cpp
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
#include "SheetMiniDB.h"
//-----------------------------------------------------------------------------


HWND g_hBrushMiniDialog = NULL;


//-----------------------------------------------------------------------------
// Name: OnBrushTab_WMInitDialog()
// Desc: 
//-----------------------------------------------------------------------------
void OnBrushTab_WMInitDialog( HWND hWnd )
{
    ShowWindow( hWnd, true );
    UpdateWindow( hWnd );

//    g_hBrushMiniDialog = NULL;
}


//-----------------------------------------------------------------------------
// Name: OnBrushTab_WMCommand()
// Desc: 
//-----------------------------------------------------------------------------
void OnBrushTab_WMCommand( HWND hWnd, WPARAM wParam )
{
    switch( LOWORD( wParam ) )
    {
    case IDC_RADIO_SHEET:
        DestroyWindow( g_hBrushMiniDialog );
        g_hBrushMiniDialog = CreateDialog( NULL, "SheetDB", hWnd, (DLGPROC) SheetDB );
        break;

    case IDC_RADIO_CUBE:
        DestroyWindow( g_hBrushMiniDialog );
        g_hBrushMiniDialog = CreateDialog( NULL, "CubeDB", hWnd, (DLGPROC) CubeDB );
        break;

    case IDC_RADIO_STAIRS:
        DestroyWindow( g_hBrushMiniDialog );
        g_hBrushMiniDialog = CreateDialog( NULL, "StairsDB", hWnd, (DLGPROC) StairsDB );
        break;

    case IDC_RADIO_SPIRAL:
        DestroyWindow( g_hBrushMiniDialog );
        g_hBrushMiniDialog = CreateDialog( NULL, "SpiralDB", hWnd, (DLGPROC) SpiralDB );
        break;

    case IDC_RADIO_CONE:
        break;

    case IDC_RADIO_SPHERE:
        break;

    case IDC_RADIO_CIRCLE:
        DestroyWindow( g_hBrushMiniDialog );
        g_hBrushMiniDialog = CreateDialog( NULL, "CircleDB", hWnd, (DLGPROC) CircleDB );
        break;

    case IDC_RADIO_CYLINDER:
        DestroyWindow( g_hBrushMiniDialog );
        g_hBrushMiniDialog = CreateDialog( NULL, "CylinderDB", hWnd, (DLGPROC) CylinderDB );
        break;

    case IDC_BUTTON_UPDATEBRUSH:
        PostMessage( g_hBrushMiniDialog, CMSG_UPDATEBRUSH, 0, 0 );
        break;
    }
}


//-----------------------------------------------------------------------------
// Name: OnBrushTab_WMNotify()
// Desc: 
//-----------------------------------------------------------------------------
void OnBrushTab_WMNotify( HWND hWnd, LPARAM lParam )
{
    ShowWindow( g_hBrushMiniDialog, true );
    UpdateWindow( g_hBrushMiniDialog );
}


//-----------------------------------------------------------------------------
// Name: OnBrushTab_WMDestroy()
// Desc: 
//-----------------------------------------------------------------------------
void OnBrushTab_WMDestroy( HWND hWnd )
{
    DestroyWindow( g_hBrushMiniDialog );

    EndDialog( hWnd, 0 );
}