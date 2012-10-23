// Window.h: interface for the CWindow class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _WINDOW_H_
#define _WINDOW_H_



class CWindow;

enum VIEWTYPE { VIEW_3D = 0, VIEW_XZ = 1, VIEW_XY = 2, VIEW_YZ = 3, VIEW_OTHER = 4, VIEW_ALL = 5 };

#include <windows.h>
#include <gl\gl.h>
#include "WindowPane.h"
#include "Camera.h"


typedef struct _WINDOW
{
    CWindow* pWindow;
} WINDOW, *LPWINDOW;

class CWindow  
{
public:
    CWindow();
    virtual ~CWindow();

    PIXELFORMATDESCRIPTOR GetPixelDescriptor() const { return m_pxDesc; }
    GLbitfield GetClearFlags() const { return m_ClearFlags; }
    bool GetDrawObjects() const { return m_bDrawObjects; }
    int GetPixelFormat() const { return m_PixelFormat; }
    VIEWTYPE GetViewType() const { return m_ViewType; }
    HWND GetParentWnd() const { return m_hParent; }
    bool IsActive() const { return m_bIsActive; }
    HINSTANCE GetInst() const { return m_hInst; }
    int GetHeight() const { return m_Height; }
    CCamera* GetCamera() { return &m_Camera; }
    int GetWidth() const { return m_Width; }
    HWND GetWnd() const { return m_hWnd; }
    HGLRC GetRC() const { return m_hRC; }
    CWindow* GetThis() { return this; }
    int GetX() const { return m_XPos; }
    int GetY() const { return m_YPos; }
    HDC GetDC() const { return m_hDC; }
    
    void SetRequiresUpdate( const bool bNeedsUpdate ) { m_bNeedsUpdate = bNeedsUpdate; }
    bool RequiresUpdate() const { return m_bNeedsUpdate; }

    void SetActiveState( const bool bState ) { m_bIsActive = bState; }
    void SetX( const int x )                 { m_XPos    = x;    m_bNeedsUpdate = true; }
    void SetY( const int y )                 { m_YPos    = y;    m_bNeedsUpdate = true; }
    void SetWidth( const int w )             { m_Width   = w;    m_bNeedsUpdate = true; }
    void SetHeight( const int h )            { m_Height  = h;    m_bNeedsUpdate = true; }
    void SetParentWnd( const HWND hWnd )     { m_hParent = hWnd; m_bNeedsUpdate = true; }
    void SetDC( const HDC dc )               { m_hDC     = dc;   m_bNeedsUpdate = true; }
    void SetRC( const HGLRC rc )             { m_hRC     = rc;   m_bNeedsUpdate = true; }
    void SetPixelFormat( const int pixelformat ) { m_PixelFormat = pixelformat; }
    void SetPixelDescriptor( const PIXELFORMATDESCRIPTOR pd ) { m_pxDesc = pd; }
    void SetDrawObjects( const bool bDraw ) { m_bDrawObjects = bDraw; }
    void SetViewType( const VIEWTYPE type ) { m_ViewType = type; }
    void SetClearFlags( const GLbitfield flags ) { m_ClearFlags = flags; }
    void Release();
    bool Init( HWND hParentWnd, int x, int y, int width, int height, unsigned long ulFlags, char* strMenuName = NULL );

    void OnWMPaint();
    void OnWMSize();
    void MoveWnd();

private:
    bool Register();
    HINSTANCE m_hInst;
    PIXELFORMATDESCRIPTOR m_pxDesc;
    int    m_XPos, m_YPos, m_Width, m_Height;
    HWND   m_hWnd, m_hParent;
    char   m_WndName[256];
    HDC    m_hDC;
    HGLRC  m_hRC;
    int    m_PixelFormat;
    bool   m_bDrawObjects;
    bool   m_bIsActive;
    bool   m_bNeedsUpdate;
    CCamera m_Camera;
    VIEWTYPE m_ViewType;
    GLbitfield m_ClearFlags;
    char m_strMenuName[256];
};

#endif
