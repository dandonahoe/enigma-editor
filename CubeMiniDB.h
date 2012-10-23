//----------------------------------------------------------------------------- 
// File: CubeMiniDB.h
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _CUBEMINIDB_H_
#define _CUBEMINIDB_H_


void OnCubeMini_WMInitDialog( HWND hWnd );
void OnCubeMini_WMDestroy( HWND hWnd );
void OnCubeMini_WMCommand( HWND hWnd, WPARAM wParam );
void UpdateCubeData( HWND hWnd );
void OnCubeMini_UpdateBrush( HWND hWnd );

#endif