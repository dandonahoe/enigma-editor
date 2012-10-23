//----------------------------------------------------------------------------- 
// File: DINPUT.h
//
// Desc: The Main program
//
// Copyright (c) 2001 Dan
//-----------------------------------------------------------------------------


#ifndef _DINPUT_H
#define _DINPUT_H


#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include <dinput.h>
#include "Singleton.h"

typedef struct _INPUTDATA
{
    LONG x, y, z;
    BOOL lButton, rButton, lArrow, rArrow, dArrow, uArrow;
    char Buffer[256];
} INPUTDATA, *LPINPUTDATA;

class CDInput : public CSingleton<CDInput>
{
public:
    //---------------------------------------------------------------------
    // Contstructor / Destructor
    CDInput();
    virtual ~CDInput();
    //---------------------------------------------------------------------


    //---------------------------------------------------------------------
    //Functions
    
    bool GetMouseButtonRight() const { return m_bMouseButtonRight; }
    void SetFreeze( const bool bFreeze ) { m_bIsFrozen = bFreeze; }
    bool GetMouseButtonLeft() const { return m_bMouseButtonLeft; }
    bool IsFrozen() const { return m_bIsFrozen; }
    long GetMouseX() const { return m_lMouseX; }
    long GetMouseY() const { return m_lMouseY; }
    long GetMouseZ() const { return m_lMouseZ; }
    bool GetData( LPINPUTDATA data );
    bool SetAcquire( bool bActive );
    bool KeyPressed( int Key );
    bool Init( HWND hWnd );
    bool UpdateInput();
    bool Release();
    //---------------------------------------------------------------------

private:
    //---------------------------------------------------------------------
    //Data Members
    LPDIRECTINPUTDEVICE8 m_lpDIDevice;
    LPDIRECTINPUTDEVICE8 m_lpMouse;
    bool m_bMouseButtonRight;
    bool m_bMouseButtonLeft;
    LPDIRECTINPUT8 m_lpDI;
    char m_KeyBuffer[256];
    bool m_bUseKeyboard;
    bool m_bUseMouse;
    bool m_bIsFrozen;
    long m_lMouseX;
    long m_lMouseY;
    long m_lMouseZ;
    HWND m_hWnd;
    //---------------------------------------------------------------------
};


#define InputManager CDInput::GetSingletonPtr()
#define IM CDInput::GetSingletonPtr()


#endif
