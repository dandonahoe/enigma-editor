//----------------------------------------------------------------------------- 
// File: SpiralMiniDB.h
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _SPIRALMINIDB_H_
#define _SPIRALMINIDB_H_


void OnSpiralMini_WMInitDialog( HWND hWnd );
void OnSpiralMini_WMDestroy( HWND hWnd );
void OnSpiralMini_WMCommand( HWND hWnd, WPARAM wParam );
void UpdateSpiralData( HWND hWnd );
void OnSpiralMini_UpdateBrush( HWND hWnd );

#endif