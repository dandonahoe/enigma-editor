//----------------------------------------------------------------------------- 
// File: PolygonMiniDB.cpp
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#include <windows.h>
#include "PolygonMiniDB.h"
#include "resource.h"
#include "PolygonManager.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: OnPolygonMini_WMInitDialog()
// Desc: 
//-----------------------------------------------------------------------------
void OnPolygonMini_WMInitDialog( HWND hWnd )
{
    ShowWindow( hWnd, true );
    UpdateWindow( hWnd );
}


//-----------------------------------------------------------------------------
// Name: OnFaceMini_WMCommand()
// Desc: Responds to messages
//-----------------------------------------------------------------------------
void OnPolygonMini_WMCommand( HWND hWnd, WPARAM wParam )
{
    switch( LOWORD( wParam ) )
    {
    case IDC_BUTTON_POLYGON_ALIGNTEXTURES: PolygonManager->AlignPolygonTextures(); break;
    }
}