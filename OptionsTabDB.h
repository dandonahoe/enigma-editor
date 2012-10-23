//----------------------------------------------------------------------------- 
// File: OptionsTabDB.h
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _OPTIONSTABDB_H_
#define _OPTIONSTABDB_H_


void OnOptionsTab_WMInitDialog( HWND hWnd );
void OnOptionsTab_WMCommand( HWND hWnd, WPARAM wParam );
void OnOptionsTab_WMDestroy( HWND hWnd );
void UpdateOptionsData( HWND hWnd );


#endif