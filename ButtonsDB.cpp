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
#include "ButtonsDB.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: OnButtonsDB_WMInitDialog()
// Desc: 
//-----------------------------------------------------------------------------
void OnButtonsDB_WMInitDialog( HWND hWnd )
{
    ShowWindow( hWnd, true );
    UpdateWindow( hWnd );
}


//-----------------------------------------------------------------------------
// Name: OnButtonsDB_WMCommand()
// Desc: 
//-----------------------------------------------------------------------------
void OnButtonsDB_WMCommand( HWND hWnd, WPARAM wParam )
{
    switch( LOWORD( wParam ) )
    {
        case IDC_BUTTONSDB_NEXTTRIANGLE:  PM->NextFace();         break;
        case IDC_BUTTONSDB_NEXTFACE:                              break;
        case IDC_BUTTONSDB_NEXTVERTEX:    PM->SelectNextVertex(); break;
        case IDC_BUTTONSDB_NEXTSEGMENT:                           break;
        case IDC_BUTTONSDB_SUBTRACTBRUSH: PM->ForgeBrush();       break;
        case IDC_BUTTONSDB_ADDBRUSH:                              break;
        case IDC_BUTTONSDB_EXPANDFACE: if( !PM->ExpandFace() ) LOG_ERR() break;
        case IDC_BUTTONSDB_REBUILD_GEOMETRY: if( !PM->RebuildGeometry() ) LOG_ERR(); break;

        default: Log( "GOT A MESSAGE\n" ); break;
    }
}


//-----------------------------------------------------------------------------
// Name: OnButtonsDB_WMNotify()
// Desc: 
//-----------------------------------------------------------------------------
void OnButtonsDB_WMNotify( HWND hWnd, LPARAM lParam )
{
}


//-----------------------------------------------------------------------------
// Name: OnButtonsDB_WMDestroy()
// Desc: 
//-----------------------------------------------------------------------------
void OnButtonsDB_WMDestroy( HWND hWnd )
{
    EndDialog( hWnd, 0 );
}