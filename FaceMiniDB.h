//----------------------------------------------------------------------------- 
// File: FaceMiniDB.h
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _FACEMINIDB_H_
#define _FACEMINIDB_H_


void OnFaceMini_WMInitDialog( HWND hWnd );
void OnFaceMini_WMCommand( HWND hWnd, WPARAM wParam );
void OnFaceMini_WMCommand( HWND hWnd, bool u, bool d, bool l, bool r );
void OnFaceMini_Pan( HWND hWnd, bool u, bool d, bool l, bool r );
void ScaleChange( HWND hWnd, WPARAM wParam );
void UpdateFaceDBData( HWND hWnd );


#endif