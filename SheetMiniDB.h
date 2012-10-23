//----------------------------------------------------------------------------- 
// File: SheetMiniDB.h
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _SHEETMINIDB_H_
#define _SHEETMINIDB_H_


void OnSheetMini_WMInitDialog( HWND hWnd );
void OnSheetMini_WMDestroy( HWND hWnd );
void OnSheetMini_WMCommand( HWND hWnd, WPARAM wParam );
void UpdateSheetData( HWND hWnd );
void OnSheetMini_WMKeydown( WPARAM wParam, HWND hWnd );
void OnSheetMini_UpdateBrush( HWND hWnd );

#endif