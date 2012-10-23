//----------------------------------------------------------------------------- 
// File: PolygonTabDB.h
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _POLYGONTABDB_H_
#define _POLYGONTABDB_H_


//-----------------------------------------------------------------------------
// Includes
#include <windows.h>
#include "PolygonManager.h"
#include "Polygon.h"
#include "Face.h"
#include "Vertex.h"
#include "enum.h"
//-----------------------------------------------------------------------------


void OnPolygonTab_WMInitDialog( HWND hWnd );
void OnPolygonTab_WMCommand( HWND hWnd, WPARAM wParam );
void OnPolygonTab_WMNotify( HWND hWnd, LPARAM lParam );
void OnPolygonTab_WMDestroy( HWND hWnd );
void OnNewSelection( HWND hWnd );
void GetCurrentItemData( HWND hWnd );
void ApplyName( HWND hWnd );


#endif