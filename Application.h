//----------------------------------------------------------------------------- 
// File: Application.h
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------

#ifndef _APPLICATION_H_
#define _APPLICATION_H_


#include <windows.h>
#include "OpenGL.h"
#include "PolygonManager.h"
#include "TextureManager.h"
#include "DInput.h"
#include "Engine.h"
//#include "Window.h"
#include "Profile.h"


class CApplication  
{
public:
    BOOL Release();
    CApplication();
    virtual ~CApplication();
    bool Init( HINSTANCE hInstance, char* strAppName, double Version );
    int Run();
    LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    void OnWMCommand( WPARAM wParam, HWND hWnd );
    void OnWMKeydown( WPARAM wParam, HWND hWnd );
    void OnWMCreate( HWND hWnd );
    void OnWMPaint( WPARAM wParam, LPARAM lParam );

private:
    bool RenderScene();

    CEngine            m_Engine;
    CProfile           m_SingletonProfile;
    float              m_fElapsedTime,
                       m_fAppTime;
    HINSTANCE          m_hInstance;
    HWND               m_hWndTCtrl;
};

#endif
