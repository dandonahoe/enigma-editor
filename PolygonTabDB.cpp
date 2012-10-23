//----------------------------------------------------------------------------- 
// File: PolygonTabDB.cpp
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#include <windows.h>
#include <commctrl.h>
#include "PolygonTabDB.h"
#include "PolygonManager.h"
#include "CustomMsg.h"
#include "main.h"
#include "resource.h"
#include "MessageProcs.h"
#include "shlwapi.h"
//-----------------------------------------------------------------------------


HTREEITEM g_hCurrentLeaf;
HWND g_hPolygonMiniDialog;
char g_strLeafText[256];
bool g_bAutoSelect = true;
CLASSTYPE g_eType = NOTHINGCLASS;

//-----------------------------------------------------------------------------
// Name: OnPolygonTab_WMInitDialog()
// Desc: 
//-----------------------------------------------------------------------------
void OnPolygonTab_WMInitDialog( HWND hWnd )
{
    ShowWindow( hWnd, true );
    UpdateWindow( hWnd );

    PolygonManager->FillPolygonTreeView( hWnd );

    SendDlgItemMessage( hWnd, IDC_CHECK_AUTOUPDATE, BM_SETCHECK, g_bAutoSelect, 0 );

    g_hPolygonMiniDialog = NULL;
}


//-----------------------------------------------------------------------------
// Name: OnPolygonTab_WMInitDialog()
// Desc: 
//-----------------------------------------------------------------------------
void OnPolygonTab_WMCommand( HWND hWnd, WPARAM wParam )
{
    switch( LOWORD( wParam ) )
    {
    case IDC_BUTTON_APPLYNAME: ApplyName( hWnd ); break;
    }
}


//-----------------------------------------------------------------------------
// Name: OnPolygonTab_WMNotify()
// Desc: 
//-----------------------------------------------------------------------------
void OnPolygonTab_WMNotify( HWND hWnd, LPARAM lParam )
{
    NMTREEVIEW* pTreeView = (NMTREEVIEW*)lParam;


    if( pTreeView->hdr.code == TVN_SELCHANGED )
    {
        g_hCurrentLeaf = pTreeView->itemNew.hItem;
        GetCurrentItemData( hWnd );
    }
}


//-----------------------------------------------------------------------------
// Name: OnPolygonTab_WMDestroy()
// Desc: 
//-----------------------------------------------------------------------------
void OnPolygonTab_WMDestroy( HWND hWnd )
{
    if( BST_CHECKED == SendDlgItemMessage( hWnd, IDC_CHECK_AUTOUPDATE, BM_GETCHECK, 0, 0 ) )
        g_bAutoSelect = true; else g_bAutoSelect = false;

    DestroyWindow( g_hPolygonMiniDialog );
    EndDialog( hWnd, 0 );
}


//-----------------------------------------------------------------------------
// Name: GetCurrentItemText()
// Desc: 
//-----------------------------------------------------------------------------
void GetCurrentItemData( HWND hWnd )
{
    TVITEM item;

    item.hItem      = g_hCurrentLeaf;
    item.mask       = TVIF_TEXT;
    item.pszText    = new char[256];
    item.cchTextMax = 256;

    SendDlgItemMessage( hWnd, IDC_TREE_POLYGONTREE, (UINT) TVM_GETITEM, 0, (LPARAM)(LPTVITEM) &item );

    strcpy( g_strLeafText, item.pszText );

    delete item.pszText;

    DestroyWindow( g_hPolygonMiniDialog );

    g_eType = PolygonManager->GetAssociatedClassType( g_strLeafText );

    switch( g_eType )
    {
    case POLYGONCLASS: g_hPolygonMiniDialog = CreateDialog( NULL, "PolygonDB", hWnd, (DLGPROC) PolygonDB ); break;
    case FACECLASS:    g_hPolygonMiniDialog = CreateDialog( NULL, "FaceDB",    hWnd, (DLGPROC) FaceDB );    break;
    case VERTEXCLASS:  g_hPolygonMiniDialog = CreateDialog( NULL, "VertexDB",  hWnd, (DLGPROC) VertexDB );  break;
    case NOTHINGCLASS: return;                                                                       break;
    }

    SetDlgItemText( hWnd, IDC_EDIT_NAME, g_strLeafText );

    return;
}


//-----------------------------------------------------------------------------
// Name: OnNewSelection()
// Desc: 
//-----------------------------------------------------------------------------
void OnNewSelection( HWND hWnd )
{
    if( BST_CHECKED == SendDlgItemMessage( hWnd, IDC_CHECK_AUTOUPDATE, BM_GETCHECK, 0, 0 ) )
        PolygonManager->UpdateTreeSelection( hWnd );
}


//-----------------------------------------------------------------------------
// Name: OnNewSelection()
// Desc: 
//-----------------------------------------------------------------------------
void ApplyName( HWND hWnd )
{
    char str[256];
    TVITEM item;

    GetDlgItemText( hWnd, IDC_EDIT_NAME, str, 256 );


    item.hItem      = g_hCurrentLeaf;
    item.mask       = TVIF_TEXT;
    item.pszText    = str;
    item.cchTextMax = 256;

    SendDlgItemMessage( hWnd, IDC_TREE_POLYGONTREE, (UINT) TVM_SETITEM, 0, (LPARAM)(LPTVITEM) &item );


    PolygonManager->ApplyNewName( str, g_eType );
}