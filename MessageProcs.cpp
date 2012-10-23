//----------------------------------------------------------------------------- 
// File: main.cpp
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#include <windows.h>
#include "MessageProcs.h"
#include "main.h"
#include "resource.h"
#include "TextureTabDB.h"
#include "PolygonTabDB.h"
#include "CustomMsg.h"
#include "VertexMiniDB.h"
#include "FaceMiniDB.h"
#include "PolygonMiniDB.h"
#include "BrushTabDB.h"
#include "SheetMiniDB.h"
#include "CubeMiniDB.h"
#include "StairsMiniDB.h"
#include "SpiralMiniDB.h"
#include "OptionsTabDB.h"
#include "CylinderMiniDB.h"
#include "CircleMiniDB.h"
#include "LightingTabDB.h"
#include "GeometryTabDB.h"
#include "ButtonsDB.h"
//-----------------------------------------------------------------------------

#define BUFFER_LEN 64


//-----------------------------------------------------------------------------
// Name: GetTrackPos()
// Desc: 
//-----------------------------------------------------------------------------
int GetTrackPos( HWND hWnd, const int iID )
{
    return SendDlgItemMessage( hWnd, iID, TBM_GETPOS, 0, 0 );
}


//-----------------------------------------------------------------------------
// Name: SetTrackPos()
// Desc: 
//-----------------------------------------------------------------------------
void SetTrackPos( HWND hWnd, const int iID, const int iPos )
{
    SendDlgItemMessage( hWnd, iID, TBM_SETPOS, 1, iPos );
}


//-----------------------------------------------------------------------------
// Name: GetEditS()
// Desc: 
//-----------------------------------------------------------------------------
void GetEditS( HWND hWnd, const int iID, char* str, const int iLen )
{
    GetDlgItemTextA( hWnd, iID, str, iLen );
}

//-----------------------------------------------------------------------------
// Name: SetEditS()
// Desc: 
//-----------------------------------------------------------------------------
void SetEditS( HWND hWnd, const int iID, const char* str )
{
    SetDlgItemText( hWnd, iID, str );
}


//-----------------------------------------------------------------------------
// Name: GetCheck()
// Desc: 
//-----------------------------------------------------------------------------
bool GetCheck( HWND hWnd, const int iID )
{ return ( BST_CHECKED == SendDlgItemMessage( hWnd, iID, BM_GETCHECK, 0, 0 ) ); }


//-----------------------------------------------------------------------------
// Name: SetCheck()
// Desc: 
//-----------------------------------------------------------------------------
void SetCheck( HWND hWnd, const int iID, const bool bChecked )
{ SendDlgItemMessage( hWnd, iID, BM_SETCHECK, bChecked, 0 ); }



//-----------------------------------------------------------------------------
// Name: SetEditF()
// Desc: 
//-----------------------------------------------------------------------------
void SetEditF( HWND hWnd, const int iID, const float fNumber )
{
    char str[BUFFER_LEN];
    sprintf( str, "%f", fNumber );
    SetDlgItemTextA( hWnd, iID, str );
}


//-----------------------------------------------------------------------------
// Name: SetEditI()
// Desc: 
//-----------------------------------------------------------------------------
void SetEditI( HWND hWnd, const int iID, const int iNumber )
{
    char str[BUFFER_LEN];
    sprintf( str, "%i", iNumber );
    SetDlgItemTextA( hWnd, iID, str );
}


//-----------------------------------------------------------------------------
// Name: GetEditF()
// Desc: 
//-----------------------------------------------------------------------------
float GetEditF( HWND hWnd, const int iID )
{
    char str[BUFFER_LEN];
    GetDlgItemTextA( hWnd, iID, str, BUFFER_LEN );
    return (float)atof( str );
}


//-----------------------------------------------------------------------------
// Name: GetEditI()
// Desc: 
//-----------------------------------------------------------------------------
int GetEditI( HWND hWnd, const int iID )
{
    char str[BUFFER_LEN];
    GetDlgItemTextA( hWnd, iID, str, BUFFER_LEN );
    return atoi( str );
}


//-----------------------------------------------------------------------------
// Name: SettingsDB()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK SettingsDB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:
            break;

        case WM_COMMAND:
            switch( LOWORD( wParam ) )
            {
            case IDCANCEL: EndDialog( hWnd, 0 ); break;
            case IDOK:     EndDialog( hWnd, 0 ); break;
            }
            break;

        case WM_DESTROY:
            EndDialog( hWnd, 0 );
            break;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: CameraDB()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK CameraDB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:
            break;

        case WM_COMMAND:
            switch( LOWORD( wParam ) )
            {
            case IDCANCEL: EndDialog( hWnd, 0 ); break;
            case IDOK:     EndDialog( hWnd, 0 ); break;

            }
            break;

        case WM_DESTROY:
            EndDialog( hWnd, 0 );
            break;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: MapDB()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK MapDB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:
            break;

        case WM_COMMAND:
            switch( LOWORD( wParam ) )
            {
            case IDCANCEL: EndDialog( hWnd, 0 ); break;
            case IDOK:     EndDialog( hWnd, 0 ); break;
            }
            break;

        case WM_DESTROY:
            EndDialog( hWnd, 0 );
            break;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: MapDB()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK AboutDB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:
            break;

        case WM_COMMAND:
            switch( LOWORD( wParam ) )
            {
            case IDCANCEL: EndDialog( hWnd, 0 ); break;
            case IDOK:     EndDialog( hWnd, 0 ); break;
            }
            break;

        case WM_DESTROY:
            EndDialog( hWnd, 0 );
            break;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: MapDB()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK TexturesTabDB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG: OnTextureTab_WMInitDialog( hWnd );       break;
        case WM_COMMAND:    OnTextureTab_WMCommand( hWnd, wParam );  break;
        case WM_DESTROY:    OnTextureTab_WMDestroy( hWnd );          break;
        case CMSG_UPDATETEXTUREPREVIEW: DisplayPreviewImage( hWnd ); break;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: MapDB()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK PolygonTabDB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:     OnPolygonTab_WMInitDialog( hWnd );      break;
        case WM_COMMAND:        OnPolygonTab_WMCommand( hWnd, wParam ); break;
        case WM_NOTIFY:         OnPolygonTab_WMNotify( hWnd, lParam );  break;
        case WM_DESTROY:        OnPolygonTab_WMDestroy( hWnd );         break;
        case CMSG_NEWSELECTION: OnNewSelection( hWnd );                 break;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: MapDB()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK LevelTabDB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:
            ShowWindow( hWnd, true );
            UpdateWindow( hWnd );
            break;

        case WM_COMMAND:
            switch( LOWORD( wParam ) )
            {
            case IDCANCEL: EndDialog( hWnd, 0 ); break;
            case IDOK:     EndDialog( hWnd, 0 ); break;
            }
            break;

        case WM_DESTROY:
            EndDialog( hWnd, 0 );
            break;
    }

    return 0;
}



//-----------------------------------------------------------------------------
// Name: FaceDB()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK FaceDB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG: OnFaceMini_WMInitDialog( hWnd );       break;
        case WM_COMMAND:    OnFaceMini_WMCommand( hWnd, wParam );  break;
        case WM_DESTROY:    EndDialog( hWnd, 0 );                  break;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: FaceDB()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK PolygonDB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG: OnPolygonMini_WMInitDialog( hWnd );      break;
        case WM_COMMAND:    OnPolygonMini_WMCommand( hWnd, wParam ); break;
        case WM_DESTROY:    EndDialog( hWnd, 0 );                    break;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: VertexDB()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK VertexDB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG: OnVertexMini_WMInitDialog( hWnd );      break;
        case WM_PAINT:      OnVertexMini_WMPaint( hWnd );           break;
        case WM_DESTROY:    OnVertexMini_WMDestroy( hWnd );         break;
        case WM_COMMAND:    OnVertexMini_WMCommand( hWnd, wParam ); break;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: BrushTabDB()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK OptionsTabDB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG: OnOptionsTab_WMInitDialog( hWnd );      break;
        case WM_COMMAND:    OnOptionsTab_WMCommand( hWnd, wParam ); break;
        case WM_DESTROY:    OnOptionsTab_WMDestroy( hWnd );         break;
        default: DefWindowProc( hWnd, uMsg, wParam, lParam );       break;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: GeometryTabDB()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK GeometryTabDB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG: OnGeometryTab_WMInitDialog( hWnd );      break;
        case WM_COMMAND:    OnGeometryTab_WMCommand( hWnd, wParam ); break;
        case WM_DESTROY:    OnGeometryTab_WMDestroy( hWnd );         break;
        default: DefWindowProc( hWnd, uMsg, wParam, lParam );        break;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: BrushTabDB()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK BrushTabDB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG: OnBrushTab_WMInitDialog( hWnd );      break;
        case WM_NOTIFY:     OnBrushTab_WMNotify( hWnd, lParam );  break;
        case WM_COMMAND:    OnBrushTab_WMCommand( hWnd, wParam ); break;
        case WM_DESTROY:    OnBrushTab_WMDestroy( hWnd );         break;
        default: DefWindowProc( hWnd, uMsg, wParam, lParam );     break;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: LightingTabDB()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK LightingTabDB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG: OnLightingTab_WMInitDialog( hWnd );      break;
        case WM_COMMAND:    OnLightingTab_WMCommand( hWnd, wParam ); break;
        case WM_DESTROY:    OnLightingTab_WMDestroy( hWnd );         break;
        case WM_HSCROLL:    UpdateLightColor( hWnd );                break;
        case CMSG_UPDATELIGHTDATA: UpdateSelectedLightData( hWnd );  break;
        default: DefWindowProc( hWnd, uMsg, wParam, lParam );        break;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: SheetDB()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK SheetDB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:    OnSheetMini_WMInitDialog( hWnd );      break;
        case WM_KEYDOWN:       OnSheetMini_WMKeydown( wParam, hWnd ); break;
        case WM_DESTROY:       OnSheetMini_WMDestroy( hWnd );         break;
        case WM_COMMAND:       OnSheetMini_WMCommand( hWnd, wParam ); break;
        case CMSG_UPDATEBRUSH: OnSheetMini_UpdateBrush( hWnd );       break;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: CubeDB()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK CubeDB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:    OnCubeMini_WMInitDialog( hWnd );      break;
        case WM_DESTROY:       OnCubeMini_WMDestroy( hWnd );         break;
        case WM_COMMAND:       OnCubeMini_WMCommand( hWnd, wParam ); break;
        case CMSG_UPDATEBRUSH: OnCubeMini_UpdateBrush( hWnd );       break;
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Name: StairsDB()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK StairsDB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:    OnStairsMini_WMInitDialog( hWnd );      break;
        case WM_DESTROY:       OnStairsMini_WMDestroy( hWnd );         break;
        case WM_COMMAND:       OnStairsMini_WMCommand( hWnd, wParam ); break;
        case CMSG_UPDATEBRUSH: OnStairsMini_UpdateBrush( hWnd );         break;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: SpiralDB()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK SpiralDB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:    OnSpiralMini_WMInitDialog( hWnd );      break;
        case WM_COMMAND:       OnSpiralMini_WMCommand( hWnd, wParam ); break;
        case WM_DESTROY:       EndDialog( hWnd, 0 );                   break;
        case CMSG_UPDATEBRUSH: OnSpiralMini_UpdateBrush( hWnd );       break;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: SpiralDB()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK CylinderDB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:    OnCylinderMini_WMInitDialog( hWnd );      break;
        case WM_COMMAND:       OnCylinderMini_WMCommand( hWnd, wParam ); break;
        case WM_DESTROY:       EndDialog( hWnd, 0 );                     break;
        case CMSG_UPDATEBRUSH: OnCylinderMini_UpdateBrush( hWnd );       break;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: CircleDB()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK CircleDB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:    OnCircleMini_WMInitDialog( hWnd );      break;
        case WM_DESTROY:       OnCircleMini_WMDestroy( hWnd );         break;
        case CMSG_UPDATEBRUSH: OnCircleMini_UpdateBrush( hWnd );       break;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: CircleDB()
// Desc: 
//-----------------------------------------------------------------------------
LRESULT CALLBACK ButtonsDB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:    OnButtonsDB_WMInitDialog( hWnd );      break;
        case WM_DESTROY:       OnButtonsDB_WMDestroy( hWnd );         break;
        case WM_COMMAND:       OnButtonsDB_WMCommand( hWnd, wParam ); break;
    }

    return 0;
}
