//----------------------------------------------------------------------------- 
// File: 
//
// Desc: 
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------

#ifndef _ENGINE_H_
#define _ENGINE_H_

#define ID_LISTVIEW   3006
#include <windows.h>
#include "PolygonManager.h"
#include "LightManager.h"
#include "Singleton.h"
#include "DInput.h"
#include "OpenGL.h"
#include "Vertex.h"
#include "Camera.h"
#include "Object.h"
#include "Audio.h"
#include "Settings.h"
#include "Map.h"
#include "Grid.h"


#define FILE_FILTER_TEXT \
    TEXT("Enigma Level File (*.elf)\0*.elf\0" )
#define DEFAULT_MEDIA_PATH  TEXT("\\\0")

struct EngineInfo
{
    char* EngineTitle;
    double Version;
};


class CEngine : public CSingleton<CEngine>
{
public:
    CEngine();
    virtual ~CEngine();
    
    bool Init( HINSTANCE hInstance, bool bFullScreen, char*strAppName, double Version );
    int GetObject3D( const int x, const int y, CWindow &pWindow );
    int GetObject2D( const int x, const int y, CWindow &pWindow );
    bool ResizeWindow( LPARAM width, WPARAM height );
    bool Repaint( WPARAM wParam, LPARAM lParam );
    bool FrameMove( const float fElapsedTime );
    void UpdateViews( float fElapsedTime );
    void RenderView( CWindow &pWindow );
    void DrawGrid( CWindow &pWindow );
    void ChangeTab( LPARAM lParam );
    bool LeftClick( int x, int y );
    bool Save( const char* File );
    bool Open( const char* File );
    bool LoadLevel( char* File );
    bool SaveLevel( char* File );
    HWND GetWnd() const;
    bool Render();
    void AlignViews();
    void ResetViews();
    void OnWMCreate( HWND hWnd );

    CWindow* GetTopView()     { return &m_3DView;      }
    CWindow* GetLeftView()    { return &m_2DLeft;      }
    CWindow* GetCenterView()  { return &m_2DCenter;    }
    CWindow* GetRightView()   { return &m_2DRight;     }
    CWindow* GetViewsWindow() { return &m_ViewsWindow; }
    
    HWND GetTabWnd() const { return m_hTabDialog; }


private:
    COpenGL         m_SingletonOpenGL;
   // CAudio          m_SingletonAudio;
    CSettings       m_SingletonSettings;
    CPolygonLookup  m_SingletonPolygonLookup;
    float           m_fElapsedTime;
    EngineInfo      m_EngineInfo;
    CGrid           m_Grid;
    
    bool CreateTabControl();

    CWindowPane m_MainHorizontal;
    CWindowPane m_BottomRight;
    CWindowPane m_BottomLeft;

    CWindow m_SideBarWindow;
    CWindow m_ViewsWindow;
    CWindow m_3DView;
    CWindow m_2DLeft;
    CWindow m_2DCenter;
    CWindow m_2DRight;

    CDInput         m_SingletonInputManager;
    CTextureManager m_SingletonTextureManager;
    CLightManager   m_SingletonLightManager;
    CPolygonManager m_SingletonPolygonManager;
    CMap            m_SingletonMap;

    HWND m_hTabControl,
         m_hTabDialog,
         m_hBtnAddStairs,
         m_hButtonsDB;

    HBITMAP m_hBMPTriangle,
            m_hBMPSheet,
            m_hBMPCube,
            m_hBMPAddBrush,
            m_hBMPNextVertex,
            m_hBMPGray,
            m_hBMPStairBrush;

    int GetObjectID( int x, int y, CWindow &pWindow );
    HFONT m_hGeorgiaFont;
};

#define Engine CEngine::GetSingletonPtr()
#define EN CEngine::GetSingletonPtr()

#endif
