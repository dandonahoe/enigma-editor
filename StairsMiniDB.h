//----------------------------------------------------------------------------- 
// File: StairsMiniDB.h
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _STAIRSMINIDB_H_
#define _STAIRSMINIDB_H_


void OnStairsMini_WMInitDialog( HWND hWnd );
void OnStairsMini_WMDestroy( HWND hWnd );
void OnStairsMini_WMCommand( HWND hWnd, WPARAM wParam );
void OnStairsMini_UpdateBrush( HWND hWnd );
void UpdateStairsData( HWND hWnd );

#endif