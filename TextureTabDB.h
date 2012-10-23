//----------------------------------------------------------------------------- 
// File: TextureTabDB.h
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _TEXTURETABDB_H_
#define _TEXTURETABDB_H_

//-----------------------------------------------------------------------------
// Defines
#define FILE_FILTER_TEXT \
    TEXT("Texture Set File (*.tex)\0*.tex\0" )
#define DEFAULT_MEDIA_PATH  TEXT("\\\0")
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#include <windows.h>
//-----------------------------------------------------------------------------


void EnumerateTexturesInFolder( const char* strDir, HWND hWindow );
void EnumerateSubFolders( const char* strDir, HWND hWindow );
bool LoadTexturesFromPackage( HWND hWnd, const char* File );
void BrowseSubFolders( const char* strDir, HWND hWindow );
void OnTextureTab_WMCommand( HWND hWnd, WPARAM lParam );
void DblClickTexturesLB( HWND hWnd, WPARAM wParam );
void OnPackageLBClick( HWND hWnd, WPARAM wParam );
void OnTextureTab_WMInitDialog( HWND hWnd );
void UpdateTextureTabSettings( HWND hWnd );
void OnTextureTab_WMDestroy( HWND hWnd );
void DisplayPreviewImage( HWND hWnd );
void ReLoadPackageLB( HWND hWnd );
void ImportTexture( HWND hWnd );
void ExportTexture( HWND hWnd );
void ApplyTexture( HWND hWnd );
void EditTexture( HWND hWnd ); 
void SaveTexture( HWND hWnd );


#endif