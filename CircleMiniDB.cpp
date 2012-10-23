//----------------------------------------------------------------------------- 
// File: CircleMiniDB.cpp
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
#include "CircleMiniDB.h"
#include "Settings.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: OnCubeMini_WMInitDialog()
// Desc: 
//-----------------------------------------------------------------------------
void OnCircleMini_WMInitDialog( HWND hWnd )
{
    ShowWindow( hWnd, true );
    UpdateWindow( hWnd );

    PolygonManager->NewBrush( CIRCLEBRUSH );

    SetEditI( hWnd, IDC_EDIT_CIRCLE_SIDES,   Settings->m_iBrush_Circle_NumSides );
    SetEditF( hWnd, IDC_EDIT_CIRCLE_RADIUS,  Settings->m_fBrush_Circle_Radius   );
    SetEditF( hWnd, IDC_EDIT_CIRCLE_DEGREES, Settings->m_fBrush_Circle_Degrees  );
}

//-----------------------------------------------------------------------------
// Name: OnCubeMini_WMDestroy()
// Desc: 
//-----------------------------------------------------------------------------
void OnCircleMini_WMDestroy( HWND hWnd )
{
    UpdateCircleData( hWnd );
    EndDialog( hWnd, 0 );
}


//-----------------------------------------------------------------------------
// Name: UpdateCircleData()
// Desc: 
//-----------------------------------------------------------------------------
void UpdateCircleData( HWND hWnd )
{
    Settings->m_iBrush_Circle_NumSides = GetEditI( hWnd, IDC_EDIT_CIRCLE_SIDES   );
    Settings->m_fBrush_Circle_Radius   = GetEditF( hWnd, IDC_EDIT_CIRCLE_RADIUS  );
    Settings->m_fBrush_Circle_Degrees  = GetEditF( hWnd, IDC_EDIT_CIRCLE_DEGREES );
}

//-----------------------------------------------------------------------------
// Name: OnSheetMini_UpdateBrush()
// Desc: 
//-----------------------------------------------------------------------------
void OnCircleMini_UpdateBrush( HWND hWnd )
{
    UpdateCircleData( hWnd );
    PolygonManager->NewBrush( CIRCLEBRUSH );
}