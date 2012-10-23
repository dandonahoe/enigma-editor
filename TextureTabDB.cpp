//----------------------------------------------------------------------------- 
// File: TextureTabDB.cpp
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#include <windows.h>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <io.h>
#include "TextureManager.h"
#include "Buffer.h"
#include "TextureTabDB.h"
#include "resource.h"
#include "main.h"
#include "CustomMsg.h"
#include "PolygonManager.h"
#include "MessageProcs.h"
#include "Settings.h"
//-----------------------------------------------------------------------------

using namespace std;

//-----------------------------------------------------------------------------
// Name: On_WMCommand()
// Desc: 
//-----------------------------------------------------------------------------
void OnTextureTab_WMCommand( HWND hWnd, WPARAM wParam )
{
    switch( LOWORD( wParam ) )
    {
//    case IDC_LOADTEXPACKAGE:    LoadTextureSet( NULL, hWnd );       break;
    case IDC_BUTTON_EDITTEX:    EditTexture( hWnd );                break;
    case IDC_BUTTON_SAVE:       SaveTexture( hWnd );                break;
    case IDC_BUTTON_IMPORTTEX:  ImportTexture( hWnd );              break;
    case IDC_BUTTON_EXPORTTEX:  ExportTexture( hWnd );              break;
    case IDC_APPLYTOCURRENT:    ApplyTexture( hWnd );               break;
    case IDC_LIST_PACKAGE:      OnPackageLBClick( hWnd, wParam );   break;
    case IDC_LIST_TEXTURENAMES: DblClickTexturesLB( hWnd, wParam ); break;
    case IDC_RADIO_TEXTURE1:
    case IDC_RADIO_TEXTURE2:
    case IDC_RADIO_LIGHTMAP:    UpdateTextureTabSettings( hWnd );   break;
    }
}


//-----------------------------------------------------------------------------
// Name: On_WMInitDialog()
// Desc: 
//-----------------------------------------------------------------------------
void OnTextureTab_WMInitDialog( HWND hWnd )
{
 //   static bool bFirstTime = true;

  //  if( bFirstTime )
 //   {
        EnumerateSubFolders( "dat\\tex\\world", hWnd );
        EnumerateTexturesInFolder( "dat\\tex\\world\\pack1", hWnd );

   //     bFirstTime = false;
  //  }
   // else
  //  {
       // VerifyCorrectPackageNames();
       // ReLoadPackageLB( hWnd );
  //  }


    SendDlgItemMessage( hWnd, IDC_LIST_PACKAGE,      LB_SETCURSEL, 0, 0 );
    SendDlgItemMessage( hWnd, IDC_LIST_TEXTURENAMES, LB_SETCURSEL, 0, 0 );

    ShowWindow( hWnd, true );
    UpdateWindow( hWnd );

    if( SendDlgItemMessage( hWnd, IDC_LIST_TEXTURENAMES, LB_GETCOUNT, 0, 0 ) > 0 )
        DisplayPreviewImage( hWnd );

    SetCheck( hWnd, IDC_RADIO_TEXTURE1, Settings->m_bTextureTab_Texture1 );
    SetCheck( hWnd, IDC_RADIO_TEXTURE2, Settings->m_bTextureTab_Texture2 );
    SetCheck( hWnd, IDC_RADIO_LIGHTMAP, Settings->m_bTextureTab_Lightmap );
}


//-----------------------------------------------------------------------------
// Name: On_WMInitDialog()
// Desc: 
//-----------------------------------------------------------------------------
void OnTextureTab_WMDestroy( HWND hWnd )
{
    UpdateTextureTabSettings( hWnd );
    EndDialog( hWnd, 0 );
}


//-----------------------------------------------------------------------------
// Name: EditTexture()
// Desc: 
//-----------------------------------------------------------------------------
void EditTexture( HWND hWnd )
{
    Log( "EditTexture()\n" );

    MessageBox( hWnd, "Not Implemented\n", "Error", MB_OK );
}


//-----------------------------------------------------------------------------
// Name: SaveTexture()
// Desc: 
//-----------------------------------------------------------------------------
void SaveTexture( HWND hWnd )
{
    Log( "SaveTexture()\n" );

    MessageBox( hWnd, "Not Implemented\n", "Error", MB_OK );
}


//-----------------------------------------------------------------------------
// Name: ImportTexture()
// Desc: 
//-----------------------------------------------------------------------------
void ImportTexture( HWND hWnd )
{
    Log( "ImportTexture()\n" );

    MessageBox( hWnd, "Not Implemented\n", "Error", MB_OK );
}


//-----------------------------------------------------------------------------
// Name: ExportTexture()
// Desc: 
//-----------------------------------------------------------------------------
void ExportTexture( HWND hWnd )
{
    Log( "ExportTexture()\n" );

    MessageBox( hWnd, "Not Implemented\n", "Error", MB_OK );
}

//-----------------------------------------------------------------------------
// Name: ApplyTexture()
// Desc: 
//-----------------------------------------------------------------------------
void ApplyTexture( HWND hWnd )
{
    if( SendDlgItemMessage( hWnd, IDC_LIST_PACKAGE, LB_GETCOUNT, 0, 0 ) == 0 )
        return;

    if( Lookup->AnythingSelected() )
    {
        char* str = TextureManager->GetNameFromTextureTab();
        HTEXTURE hTexture = TextureManager->AddTextureFromFile( str );
        TextureManager->SetSelectedTexture( hTexture );
        PolygonManager->ApplyTextureToSelected( TextureManager->GetSelectedTexture() );
    }

}


//-----------------------------------------------------------------------------
// Name: OnPackageLBClick()
// Desc: 
//-----------------------------------------------------------------------------
void OnPackageLBClick( HWND hWnd, WPARAM wParam )
{
    char strPackage[256];

    if( SendDlgItemMessage( hWnd, IDC_LIST_PACKAGE, LB_GETCOUNT, 0, 0 ) == 0 )
        return;

    int iSel = SendDlgItemMessage( hWnd, IDC_LIST_PACKAGE, LB_GETCURSEL, 0, 0 );

    SendDlgItemMessage( hWnd, IDC_LIST_PACKAGE, LB_GETTEXT, iSel, (LPARAM) strPackage );

    if( strPackage == NULL )
        MessageBox( hWnd, "if( str == NULL )", "OnSelChange()", MB_OK );

    EnumerateTexturesInFolder( strPackage, hWnd );

    SendDlgItemMessage( hWnd, IDC_LIST_TEXTURENAMES, LB_SETCURSEL, 0, 0 );

    DisplayPreviewImage( hWnd );
}


//-----------------------------------------------------------------------------
// Name: DblClickTexturesLB()
// Desc: 
//-----------------------------------------------------------------------------
void DblClickTexturesLB( HWND hWnd, WPARAM wParam )
{
    if( HIWORD( wParam ) == LBN_DBLCLK )
        ApplyTexture( hWnd );
    else
        DisplayPreviewImage( hWnd );
}


//-----------------------------------------------------------------------------
// Name: ReLoadPackageLB()
// Desc: 
//-----------------------------------------------------------------------------
void DisplayPreviewImage( HWND hWnd )
{
    HDC hDC, memDC;
    HBITMAP  hBitmap = NULL;
    ifstream fp;
    char     strTexture[256];
    char     strPackage[256];
    int      iTexture;
    int      iPackage;

    // Make sure that at least one texture exists in this package
    if( SendDlgItemMessage( hWnd, IDC_LIST_TEXTURENAMES, LB_GETCOUNT, 0, 0 ) == 0 )
        return;

    // Get the index's of the currently selected texture package and texture
    iTexture = SendDlgItemMessage( hWnd, IDC_LIST_TEXTURENAMES, LB_GETCURSEL, 0, 0 );
    iPackage = SendDlgItemMessage( hWnd, IDC_LIST_PACKAGE,      LB_GETCURSEL, 0, 0 );

    // Get the text from each
    SendDlgItemMessage( hWnd, IDC_LIST_PACKAGE,      LB_GETTEXT, iPackage, (LPARAM) strPackage );
    SendDlgItemMessage( hWnd, IDC_LIST_TEXTURENAMES, LB_GETTEXT, iTexture, (LPARAM) strTexture );

    hBitmap = (HBITMAP)LoadImage( NULL, strTexture, IMAGE_BITMAP, 128, 128, LR_LOADFROMFILE | LR_CREATEDIBSECTION );

    if( hBitmap == NULL ) {
        char strError[256];
        sprintf( strError, "Failed to display preview of:\nPackage: %s\nTexture: %n", strPackage, strTexture );
        MessageBox( hWnd, strError, "if( hBitmap == NULL ) @ DisplayPreviewImage()", MB_OK );
        Log( "if( hBitmap == NULL )\n" ); return; }

    hDC   = GetDC( hWnd );
    memDC = CreateCompatibleDC( hDC );

    SelectObject( memDC, hBitmap );

    if( !BitBlt( hDC, 17, 265, 128, 128, memDC, 0, 0, SRCCOPY ) ) {
        MessageBox( hWnd, "if( !BitBlt() )", "DisplayPreviewImage()", MB_OK );
        Log( "if( !BitBlt() ) @ DisplayPreviewImage()\n" );
        return; }

    ReleaseDC( hWnd, hDC );
    DeleteDC( memDC );
    DeleteObject( hBitmap );

    TextureManager->SetNameFromTextureTab( strTexture );
}


//-----------------------------------------------------------------------------
// Name: UpdateTextureTabSettings()
// Desc: 
//-----------------------------------------------------------------------------
void UpdateTextureTabSettings( HWND hWnd )
{
    Settings->m_bTextureTab_Texture1 = GetCheck( hWnd, IDC_RADIO_TEXTURE1 );
    Settings->m_bTextureTab_Texture2 = GetCheck( hWnd, IDC_RADIO_TEXTURE2 );
    Settings->m_bTextureTab_Lightmap = GetCheck( hWnd, IDC_RADIO_LIGHTMAP );
}


//---------------------------------------------------------------------
//Name: Browse()
//Desc: Searches the strDir and all sub folders for bmp files
//---------------------------------------------------------------------
void EnumerateSubFolders( const char* strDir, HWND hWindow )
{
   char buffer[_MAX_PATH * 2];
   int len;
   long find;
   struct _finddata_t finddata;

   strcpy( buffer, strDir );

   len = strlen( buffer );
   if( buffer[len - 1] != '\\' )
   {
      buffer[len] = '\\';
      buffer[len + 1] = '\0';
   }

   strcat( buffer, "*.*" );

   find = _findfirst( buffer, &finddata );

   if( find != -1 )
   {
      do
      {
         if( ( finddata.attrib & _A_SUBDIR ) == _A_SUBDIR )
         {
            if( ( strcmpi( finddata.name, "." ) != 0 ) && ( strcmpi( finddata.name, ".." ) ) != 0 )
            {
               sprintf( buffer, "%s\\%s", strDir, finddata.name );

               SendDlgItemMessage( hWindow, IDC_LIST_PACKAGE, LB_ADDSTRING, 0, (LPARAM) buffer );
               EnumerateSubFolders( buffer, hWindow );
            }
         }
      } while( _findnext( find, &finddata ) != -1 );

      _findclose( find );
   }
}


//---------------------------------------------------------------------
//Name: ()
//Desc: 
//---------------------------------------------------------------------
void EnumerateTexturesInFolder( const char* strDir, HWND hWindow )
{
    char buffer[_MAX_PATH * 2];
    int len;
    long find;
    struct _finddata_t finddata;

    strcpy( buffer, strDir );

    len = strlen( buffer );
    if( buffer[len - 1] != '\\' )
    {
        buffer[len] = '\\';
        buffer[len + 1] = '\0';
    }

    strcat( buffer, "*.*" );

    find = _findfirst( buffer, &finddata );

    SendDlgItemMessage( hWindow, IDC_LIST_TEXTURENAMES, LB_RESETCONTENT, 0, 0 );

    if( find != -1 )
    {
        do
        {
            sprintf( buffer, "%s\\%s", strDir, finddata.name );

		    if( ( finddata.name[strlen(finddata.name) - 1] == 'p' ) &&
			    ( finddata.name[strlen(finddata.name) - 2] == 'm' ) &&
			    ( finddata.name[strlen(finddata.name) - 3] == 'b' ) )
		    {
                SendDlgItemMessage( hWindow, IDC_LIST_TEXTURENAMES, LB_ADDSTRING, 0, (LPARAM) buffer );
		    }

      } while( _findnext( find, &finddata ) != -1 );

      _findclose( find );
   }
}