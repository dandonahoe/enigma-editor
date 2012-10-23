//----------------------------------------------------------------------------- 
// File: LightingTabDB.h
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _LIGHTINGTABDB_H_
#define _LIGHTINGTABDB_H_


void OnLightingTab_WMInitDialog( HWND hWnd );
void OnLightingTab_WMCommand( HWND hWnd, WPARAM wParam );
void OnLightingTab_WMDestroy( HWND hWnd );
void UpdateLightingData( HWND hWnd );
void SetLightingBarPos( HWND hWnd, const int iPos );
bool DisplayHemiCube( HWND hWnd );
bool DisplayImage( HWND hWnd,        const char* strFileName,
                   const int iX,     const int iY,
                   const int iWidth, const int iHeight );
void UpdateWindowStats( HWND hWnd,
                        const int iCurrentPolygon,
                        const int iCurrentFace,
                        const int iTotalPolygons,
                        const int iTotalFaces,
                        const int iCoordU,
                        const int iCoordV,
                        const int iRes,
                        const int iCurrentStep,
                        const int iTotalSteps,
                        const VECT vColor );
void UpdateSelectedLightData( HWND hWnd );
void SetLightName( HWND hWnd );
void UpdateLightColor( HWND hWnd );
void OnCalculateLightmaps( HWND hWnd );


#endif