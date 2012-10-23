//----------------------------------------------------------------------------- 
// File: GeometryTabDB.h
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _GEOMETRYTABDB_H_
#define _GEOMETRYTABDB_H_


void OnGeometryTab_WMInitDialog( HWND hWnd );
void OnGeometryTab_WMCommand( HWND hWnd, WPARAM wParam );
void OnGeometryTab_WMDestroy( HWND hWnd );
void OnExpandFace( HWND hWnd );
void UpdateGeometryData( HWND hWnd );
void OnSlideForward( HWND hWnd );
void OnSlideBackward( HWND hWnd );
void OnInflate( HWND hWnd );
void OnDeflate( HWND hWnd );
void OnRotateLeft( HWND hWnd );
void OnRotateRight( HWND hWnd );

#endif