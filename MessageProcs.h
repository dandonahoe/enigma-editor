//----------------------------------------------------------------------------- 
// File: MessageProcs.h
//
// Desc: 
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _MESSAGEPROCS_H_
#define _MESSAGEPROCS_H_

bool  GetCheck( HWND hWnd, const int iID );
void  SetCheck( HWND hWnd, const int iID, const bool bChecked );
void  SetEditF( HWND hWnd, const int iID, const float fNumber );
void  SetEditI( HWND hWnd, const int iID, const int iNumber );
float GetEditF( HWND hWnd, const int iID );
int   GetEditI( HWND hWnd, const int iID );
void  GetEditS( HWND hWnd, const int iID, char* str, const int iLen );
void  SetEditS( HWND hWnd, const int iID, const char* str );
int   GetTrackPos( HWND hWnd, const int iID );
void  SetTrackPos( HWND hWnd, const int iID, const int iPos );


LRESULT CALLBACK SettingsDB(    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK CameraDB(      HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK MapDB(         HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK AboutDB(       HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK TexturesTabDB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK PolygonTabDB(  HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK LevelTabDB(    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK FaceDB(        HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK PolygonDB(     HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK VertexDB(      HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK LightingTabDB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK BrushTabDB(    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK SheetDB(       HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK CubeDB(        HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK StairsDB(      HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK SpiralDB(      HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK OptionsTabDB(  HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK CylinderDB(    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK CircleDB(      HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK GeometryTabDB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK ButtonsDB(     HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

#endif