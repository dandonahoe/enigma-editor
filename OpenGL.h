//----------------------------------------------------------------------------- 
// File: OpenGL.h
//
// Desc: Declaration file for the OpenGL container class
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#ifndef _OPENGL_H
#define _OPENGL_H
//-----------------------------------------------------------------------------

#include <windows.h>
#include <gl\gl.h>
#include <commctrl.h>
#include "WindowPane.h"
#include "Window.h"
#include "Singleton.h"


#define RED 0
#define YELLOW 1
#define BLUE 2
#define GREEN 3
#define MASK 4

#define IDD_TRIANGLE   3000
#define IDD_CUBE       3001
#define IDD_SHEET      3002
#define IDD_ADDBRUSH   3003
#define IDD_NEXTVERTEX 3004
#define IDD_LBTEXTURES 3005
#define IDD_ADDSTAIRS  3006


class COpenGL : public CSingleton<COpenGL>
{
public:
    COpenGL();
    virtual ~COpenGL();
    BOOL Init( HINSTANCE hInstance, BOOL bFullScreen, char* WindowName, CWindow &pWindow );
    VOID Release();

    BOOL BeginScene( CWindow &pWindow );
    BOOL EndScene();
    VOID ChangeToFullScreen();
    VOID SizeOpenGLScreen( CWindow &pWindow  );
    VOID Repaint( WPARAM wParam, LPARAM lParam );
    HWND GetWnd() const;
    HINSTANCE GetInstance();
    VOID DrawGLText(int x, int y, const char *strString, ...);
    BOOL SelectTexture( UINT TextureID );

    // Fog controls
    BOOL GetFogEnabled();
    BOOL SetFogColor( float fR, float fG, float fB, float fA );
    BOOL GetFogColor( float *lpR, float *lpG, float *lpB, float *lpA );
    BOOL SetFogDensity( float fDensity );
    BOOL GetFogDensity( float *fDensity );
    BOOL SetFogEnable( BOOL bEnable );
    BOOL SetFogRange( float fMinDistance, float fMaxDistance );
    BOOL GetFogRange( float *fMinDistance, float *fMaxDistance );
    BOOL SetFogQuality( GLenum flag );
    BOOL SetFogMode( GLenum flag );

    BOOL SetClearColor( float fR, float fG, float fB, float fA );

    RECT GetRect() { return m_rScreenRect; }

    bool DisplayFPS();
    HWND CreateAnimationCtrl( HWND hwndDlg, int nIDCtl );
    bool InitStatusCC();
    bool LoadTextureLB( char* FileName, bool bFullPath );
    int  GetSelectedTextureIndexLB();
    void SetupOrtho(int cx, int cy);
    void SetupRenderWindow( CWindow &pWindow );
    void FocusOnWindow( CWindow &pWindow );
//private:
    UINT CreateOpenGLFont(LPSTR strFontName, int height);
    VOID PositionText( int x, int y );
    BOOL SetupPixelFormat( HDC hdc, CWindow &pWindow );
    VOID InitializeOpenGL( CWindow &pWindow );
    BOOL CreateMyWindow( LPSTR strWindowName, DWORD dwStyle );
    VOID CreateTexture( UINT textureArray[], LPSTR strFileName, INT textureID );

    INT       m_ScreenWidth,
              m_ScreenHeight,
              m_ScreenDepth;
    BOOL      m_bFullScreen,
              m_bFog;
    HINSTANCE m_hInstance;
    HDC       m_hDC;
    HGLRC     m_hRC;
    HWND      m_hWnd,
              m_hStatusBar;
    RECT      m_rScreenRect;
    float     m_fFogR,
              m_fFogG,
              m_fFogB,
              m_fFogA,
              m_fMinDistance,
              m_fMaxDistance,
              m_fFogDensity;
    char*     m_strWindowName;



    HFONT     m_hFont;

    INITCOMMONCONTROLSEX m_CommCtrls;
};


#define OpenGL COpenGL::GetSingletonPtr()
#define GL COpenGL::GetSingletonPtr()


#endif
