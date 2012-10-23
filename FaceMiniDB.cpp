//-----------------------------------------------------------------------------
// File: FaceMiniDB.cpp
//
// Desc: Controls the face mini dialog box under the polygon tree
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#include <windows.h>
#include <commctrl.h>
#include "FaceMiniDB.h"
#include "resource.h"
#include "CustomMsg.h"
#include "PolygonManager.h"
#include "MessageProcs.h"
//-----------------------------------------------------------------------------


bool g_bPanU = false, g_bPanV = false, g_bTranslucent = false;


//-----------------------------------------------------------------------------
// Name: OnFaceMini_WMInitDialog()
// Desc: Creates the UpDown controls
//-----------------------------------------------------------------------------
void OnFaceMini_WMInitDialog( HWND hWnd )
{
    ShowWindow( hWnd, true );
    UpdateWindow( hWnd );

    // Get handles to the dialog edit boxes in order to add the up down control
    HWND hScale     = GetDlgItem( hWnd, IDC_EDIT_SCALE     );
    HWND hPanAmount = GetDlgItem( hWnd, IDC_EDIT_PANAMOUNT );

    CreateUpDownControl( WS_CHILD | WS_BORDER | WS_VISIBLE |
                         UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_HOTTRACK | UDS_WRAP,
                         10, 10, 50, 50, hWnd, ID_SCALE, NULL, hScale, 10, 0, 1 );

    CreateUpDownControl( WS_CHILD | WS_BORDER | WS_VISIBLE |
                         UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_HOTTRACK | UDS_WRAP,
                         10, 10, 50, 50, hWnd, ID_PANAMOUNT, NULL, hPanAmount, 10, 0, 1 );

    SendDlgItemMessage( hWnd, IDC_CHECK_UPAN, BM_SETCHECK, PolygonManager->GetPanU(), 0 );
    SendDlgItemMessage( hWnd, IDC_CHECK_VPAN, BM_SETCHECK, PolygonManager->GetPanV(), 0 );

    SetCheck( hWnd, IDC_CHECK_USE_TEXTURE1, PolygonManager->GetUseTexture1() );
    SetCheck( hWnd, IDC_CHECK_USE_TEXTURE2, PolygonManager->GetUseTexture2() );
    SetCheck( hWnd, IDC_CHECK_USE_LIGHTMAP, PolygonManager->GetUseLightmap() );
}


//-----------------------------------------------------------------------------
// Name: OnFaceMini_WMCommand()
// Desc: Responds to messages
//-----------------------------------------------------------------------------
void OnFaceMini_WMCommand( HWND hWnd, WPARAM wParam )
{
    switch( LOWORD( wParam ) )
    {
    case IDC_BUTTON_PANUP:     OnFaceMini_Pan( hWnd, true,  false, false, false ); break;
    case IDC_BUTTON_PANLEFT:   OnFaceMini_Pan( hWnd, false, false, true,  false ); break;
    case IDC_BUTTON_PANRIGHT:  OnFaceMini_Pan( hWnd, false, false, false, true  ); break;
    case IDC_BUTTON_PANDOWN:   OnFaceMini_Pan( hWnd, false, true,  false, false ); break;
    case IDC_BUTTON_FLIPU:     PolygonManager->FlipTexU();                         break;
    case IDC_BUTTON_FLIPV:     PolygonManager->FlipTexV();                         break;
    case IDC_BUTTON_ROTATE_90: PolygonManager->RotateTex90();                      break;
    case IDC_BUTTON_ROTATE_LIGHTMAP: PolygonManager->RotateLightmap90();           break;
    case IDC_EDIT_SCALE:       ScaleChange( hWnd, wParam );                        break;
    case IDC_BUTTON_FACE_ALIGNTEXTURE: PolygonManager->GenerateTextureCoords();    break;

    case IDC_CHECK_TRANSLUCENT:
    case IDC_CHECK_UPAN:
    case IDC_CHECK_VPAN:
    case IDC_CHECK_USE_TEXTURE1:
    case IDC_CHECK_USE_TEXTURE2:
    case IDC_CHECK_USE_LIGHTMAP: UpdateFaceDBData( hWnd );                          break;
    }
}


//-----------------------------------------------------------------------------
// Name: OnFaceMini_Pan()
// Desc: The 'true' direction is updated after getting the pan factor from the
//       scale edit box.
//-----------------------------------------------------------------------------
void OnFaceMini_Pan( HWND hWnd, bool u, bool d, bool l, bool r )
{
    BOOL bBool;

    float pan = (float)GetDlgItemInt( hWnd, IDC_EDIT_PANAMOUNT, &bBool, true ) / 100.0f;

    if( u ) PolygonManager->TranslateTexV(  pan );
    else if( d ) PolygonManager->TranslateTexV( -pan );
    else if( l ) PolygonManager->TranslateTexU(  pan );
    else if( r ) PolygonManager->TranslateTexU( -pan );
    else Log( "No bool values are 'true' @ OnFaceMini_Pan()\n" );
}


//-----------------------------------------------------------------------------
// Name: ScaleChange()
// Desc: Sets the scale factor for the current face if the box has changed
//-----------------------------------------------------------------------------
void ScaleChange( HWND hWnd, WPARAM wParam )
{
    BOOL bBool;

    // If it has changed
    if( HIWORD( wParam ) == EN_UPDATE )
    {
        float factor = (float)GetDlgItemInt( hWnd, IDC_EDIT_SCALE, &bBool, true );
        PolygonManager->SetScale( factor );
    }
}


//-----------------------------------------------------------------------------
// Name: UpdateFaceDBData()
// Desc: 
//-----------------------------------------------------------------------------
void UpdateFaceDBData( HWND hWnd )
{
    PolygonManager->SetUseTexture1( GetCheck( hWnd, IDC_CHECK_USE_TEXTURE1 ) );
    PolygonManager->SetUseTexture2( GetCheck( hWnd, IDC_CHECK_USE_TEXTURE2 ) );
    PolygonManager->SetUseLightmap( GetCheck( hWnd, IDC_CHECK_USE_LIGHTMAP ) );
    PolygonManager->SetPanU(        GetCheck( hWnd, IDC_CHECK_UPAN         ) );
    PolygonManager->SetPanV(        GetCheck( hWnd, IDC_CHECK_VPAN         ) );
    PolygonManager->SetTranslucent( GetCheck( hWnd, IDC_CHECK_TRANSLUCENT  ) );
}