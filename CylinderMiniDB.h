//----------------------------------------------------------------------------- 
// File: CylinderMiniDB.h
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _CYLINDERMINIDB_H_
#define _CYLINDERMINIDB_H_


void OnCylinderMini_WMInitDialog( HWND hWnd );
void OnCylinderMini_WMDestroy( HWND hWnd );
void OnCylinderMini_WMCommand( HWND hWnd, WPARAM wParam );
void OnCylinderMini_UpdateBrush( HWND hWnd );
void UpdateCylinderData( HWND hWnd );

#endif