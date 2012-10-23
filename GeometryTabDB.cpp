//----------------------------------------------------------------------------- 
// File: GeometryTabDB.cpp
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#include <windows.h>
#include "PolygonManager.h"
#include "CustomMsg.h"
#include "main.h"
#include "resource.h"
#include "MessageProcs.h"
#include "GeometryTabDB.h"
#include "Settings.h"
#include "Tracer.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: OnGeometryTab_WMInitDialog()
// Desc: 
//-----------------------------------------------------------------------------
void OnGeometryTab_WMInitDialog( HWND hWnd )
{
    SetEditF( hWnd, IDC_EDIT_GEOMETRY_MOVE_SCALE,    SE->m_fGeometry_MoveFactor   );
    SetEditF( hWnd, IDC_EDIT_GEOMETRY_EXPAND_AMOUNT, SE->m_fGeometry_ExpandAmount );

    if( SE->m_iGeometry_ApplyTo == 0 )
        SetCheck( hWnd, IDC_RADIO_GEOMETRY_POLYGON, true );
    else if( SE->m_iGeometry_ApplyTo == 1 )
        SetCheck( hWnd, IDC_RADIO_GEOMETRY_FACE, true );
    else if( SE->m_iGeometry_ApplyTo == 2 )
        SetCheck( hWnd, IDC_RADIO_GEOMETRY_VERTEX, true );

    ShowWindow( hWnd, true );
    UpdateWindow( hWnd );
}

//-----------------------------------------------------------------------------
// Name: OnGeometryTab_WMCommand()
// Desc: 
//-----------------------------------------------------------------------------
void OnGeometryTab_WMCommand( HWND hWnd, WPARAM wParam )
{
    switch( LOWORD( wParam ) )
    {
        case IDC_BUTTON_EXPAND_FACE:             OnExpandFace( hWnd );    break;
        case IDC_BUTTON_GEOMETRY_SLIDE_FORWARD:  OnSlideForward( hWnd );  break;
        case IDC_BUTTON_GEOMETRY_SLIDE_BACKWARD: OnSlideBackward( hWnd ); break;
        case IDC_BUTTON_GEOMETRY_SLIDE_INFLATE:  OnInflate( hWnd );       break;
        case IDC_BUTTON_GEOMETRY_SLIDE_DEFLATE:  OnDeflate( hWnd );       break;
        case IDC_BUTTON_GEOMETRY_ROTATE_LEFT:    OnRotateLeft( hWnd );    break;
        case IDC_BUTTON_GEOMETRY_ROTATE_RIGHT:   OnRotateRight( hWnd );   break;
    }
}

//-----------------------------------------------------------------------------
// Name: OnGeometryTab_WMDestroy()
// Desc: 
//-----------------------------------------------------------------------------
void OnGeometryTab_WMDestroy( HWND hWnd )
{
    UpdateGeometryData( hWnd );
    EndDialog( hWnd, 0 );
}


//-----------------------------------------------------------------------------
// Name: UpdateGeometryData()
// Desc: 
//-----------------------------------------------------------------------------
void UpdateGeometryData( HWND hWnd )
{
    SE->m_fGeometry_MoveFactor   = GetEditF( hWnd, IDC_EDIT_GEOMETRY_MOVE_SCALE );
    SE->m_fGeometry_ExpandAmount = GetEditF( hWnd, IDC_EDIT_GEOMETRY_EXPAND_AMOUNT );

    if( GetCheck( hWnd, IDC_RADIO_GEOMETRY_POLYGON ) )
        SE->m_iGeometry_ApplyTo = 0;
    else if( GetCheck( hWnd, IDC_RADIO_GEOMETRY_FACE ) )
        SE->m_iGeometry_ApplyTo = 1;
    else if( GetCheck( hWnd, IDC_RADIO_GEOMETRY_VERTEX ) )
        SE->m_iGeometry_ApplyTo = 2;
}


//-----------------------------------------------------------------------------
// Name: OnExpandFace()
// Desc: 
//-----------------------------------------------------------------------------
void OnExpandFace( HWND hWnd )
{
    UpdateGeometryData( hWnd );

    if( !PolygonManager->ExpandFace() )
        Log( "if( !PolygonManager->ExpandFace() ) @ OnExpandFace()\n" );
}

//-----------------------------------------------------------------------------
// Name: OnSlideForward()
// Desc: 
//-----------------------------------------------------------------------------
void OnSlideForward( HWND hWnd )
{
    UpdateGeometryData( hWnd );

    if( !PolygonManager->SlideFaceForward() )
        Log( "if( !PolygonManager->SlideFaceForward() ) @ OnSlideForward()\n" );
}


//-----------------------------------------------------------------------------
// Name: OnSlideBackward()
// Desc: 
//-----------------------------------------------------------------------------
void OnSlideBackward( HWND hWnd )
{
    UpdateGeometryData( hWnd );

    if( !PolygonManager->SlideFaceBackward() )
         Log( "if( !PolygonManager->SlideFaceBackward() ) @ OnSlideBackward()\n" );
}


//-----------------------------------------------------------------------------
// Name: OnInflate()
// Desc: 
//-----------------------------------------------------------------------------
void OnInflate( HWND hWnd )
{
    UpdateGeometryData( hWnd );

    if( !PolygonManager->Inflate() )
        Log( "if( !PolygonManager->Inflate() ) @ OnInflate()\n" );
}


//-----------------------------------------------------------------------------
// Name: OnDeflate()
// Desc: 
//-----------------------------------------------------------------------------
void OnDeflate( HWND hWnd )
{
    UpdateGeometryData( hWnd );

    if( !PolygonManager->Deflate() )
        Log( "if( !PolygonManager->Deflate() ) @ OnDeflate()\n" );
}


//-----------------------------------------------------------------------------
// Name: OnDeflate()
// Desc: 
//-----------------------------------------------------------------------------
void OnRotateLeft( HWND hWnd )
{
    UpdateGeometryData( hWnd );

    if( !PolygonManager->RotateLeft() )
        Log( "if( !PolygonManager->RotateLeft() ) @ OnRotateLeft()\n" );
}


//-----------------------------------------------------------------------------
// Name: OnDeflate()
// Desc: 
//-----------------------------------------------------------------------------
void OnRotateRight( HWND hWnd )
{
    UpdateGeometryData( hWnd );

    if( !PolygonManager->RotateRight() )
        Log( "if( !PolygonManager->RotateRight() ) @ OnRotateRight()\n" );
}
