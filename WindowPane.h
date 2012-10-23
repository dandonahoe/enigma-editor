//----------------------------------------------------------------------------- 
// File: WindowPane.h
//
// Desc: 
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _WINDOWPANE_H_
#define _WINDOWPANE_H_


class CWindowPane;


//-----------------------------------------------------------------------------
// Enumeration
enum WPaneStyle { CWP_HORIZONTAL = 0, CWP_VERTICAL = 1 };
enum WindowConstraints { CWP_STAYABOVE = 0, CWP_STAYBELOW = 1, CWP_STAYLEFT = 2, CWP_STAYRIGHT = 3 };
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Defines
#define NUM_OTHER_PANES 16
#define NUM_WINDOWS 16
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#include <windows.h>
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Structs
struct OtherPaneData
{
    WindowConstraints flag;
    CWindowPane* pPane;
};

typedef struct _WINDOWPANE
{
    CWindowPane* pWindowPane;
} WINDOWPANE, *LPWINDOWPANE;
//-----------------------------------------------------------------------------


class CWindowPane  
{
public:
    CWindowPane();
    virtual ~CWindowPane();
    int  GetX()            const { return m_XPos;     }
    int  GetY()            const { return m_YPos;     }
    int  GetWidth()        const { return m_Width;    }
    int  GetHeight()       const { return m_Height;   }
    HWND GetWnd()          const { return m_hWnd;     }
    HWND GetParentWnd()    const { return m_hParent;  }
    int  GetBarSize()      const { return m_BarSize;  }
    WPaneStyle GetType()   const { return m_PaneType; }
    CWindowPane* GetThis()       { return this;       }
    bool GetLockState()    const { return m_bLocked;  }

    void SetX( int x )             { if( m_bLocked ) return; m_XPos    = x;     }
    void SetY( int y )             { if( m_bLocked ) return; m_YPos    = y;     }
    void SetWidth( int w )         { if( m_bLocked ) return; m_Width   = w;     }
    void SetHeight( int h )        { if( m_bLocked ) return; m_Height  = h;     }
    void SetParentWnd( HWND hWnd ) { if( m_bLocked ) return; m_hParent = hWnd;  }
    void SetBarSize( int size )    { if( m_bLocked ) return; m_BarSize = size;  }
    void SetLock( bool bLock )     {  m_bLocked = bLock; }

    HWND Init( HWND hWndParent, int Pos, WPaneStyle Style, int BarSize = 10 );
    void OnWMPaint();
    void OnWMMouseMove( WPARAM wParam );
    void OnWMSize();
    void MoveWnd();
    void Release();
    void AddPaneToManage( CWindowPane* pPane, WindowConstraints flag );
    void AlignOtherPanes();
    void NotifyOthers();

private:
    bool Register();
    WPaneStyle m_PaneType;
    HINSTANCE m_hInst;
    HPEN m_hPenShadow, m_hPenHighLite, m_hPenBlack;
    int  m_XPos, m_YPos, m_Width, m_Height, m_BarSize;
    HWND m_hWnd, m_hParent;
    char m_WndName[256];
    OtherPaneData m_OtherPanes[NUM_OTHER_PANES];
    int m_NumOtherPanes;
    bool m_bLocked;
};


#endif
