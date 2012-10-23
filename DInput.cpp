//----------------------------------------------------------------------------- 
// File: DInput.cpp
//
// Desc: The Main program
//
// Copyright (c) 2001 Dan
//-----------------------------------------------------------------------------
#define INITGUID

#pragma comment(lib, "dinput8.lib")


#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

//-----------------------------------------------------------------------------
//Includes
#include <dinput.h>
#include <fstream>
#include "DInput.h"
#include "Tracer.h"
#include "main.h"
//-----------------------------------------------------------------------------

//using namespace std;


//---------------------------------------------------------------------
// Contstructor / Destructor
//---------------------------------------------------------------------
CDInput::CDInput()
{
    TRACE_ENTER_FN( CDInput::CDInput )

    Log( "CDInput::CDInput()\n" );

    m_bMouseButtonRight = false;
    m_bMouseButtonLeft = false;
    m_bUseKeyboard = true;
    m_lpDIDevice = NULL;
    m_bIsFrozen = false;
    m_bUseMouse = true;
    m_lpMouse = NULL;
    m_lpDI = NULL;
    m_hWnd = NULL;
    m_lMouseX = 0;
    m_lMouseY = 0;
    m_lMouseZ = 0;

    TRACE_LEAVE_FN()
}

CDInput::~CDInput()
{
    TRACE_ENTER_FN( CDInput::CDInput )

    Log( "CDInput::~CDInput()\n" );

    Release();

    TRACE_LEAVE_FN()
}


//---------------------------------------------------------------------
//Name: Initalize()
//Desc: Initalized the DirectInput Object
//---------------------------------------------------------------------
bool CDInput::Init( HWND hWnd )
{
    TRACE_ENTER_FN( CDInput::Init )

    Log( "CDInput::Init()\n" );
    m_hWnd = hWnd;

    HRESULT hr;

    if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION,
                                        IID_IDirectInput8, (void**)&m_lpDI, NULL ) ) )
    {
        Log( "if( FAILED( hr = DirectInput8Create()\n" );
        TRACE_LEAVE_FN()
        return false;
    }

    if( m_bUseKeyboard )
    {
        if( FAILED( hr = m_lpDI->CreateDevice( GUID_SysKeyboard, &m_lpDIDevice, NULL ) ) )
            Log( "if( FAILED( hr = m_lpDI->CreateDevice()\n" );

        if( FAILED( hr = m_lpDIDevice->SetDataFormat( &c_dfDIKeyboard ) ) )
            Log( "if( FAILED( hr = m_lpDIDevice->SetDataFormat()\n" );

        if( FAILED( hr = m_lpDIDevice->SetCooperativeLevel( m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ) ) )
            Log( "if( FAILED( hr = m_lpDIDevice->SetCooperativeLevel()\n" );
    }

    if( m_bUseMouse )
    {
        if( FAILED( hr = m_lpDI->CreateDevice( GUID_SysMouse, &m_lpMouse, NULL ) ) )
        {
            Log( "if( FAILED( hr = m_lpDI->CreateDevice()\n" );
            TRACE_LEAVE_FN()
            return false;
        }

        if( FAILED( hr = m_lpMouse->SetDataFormat( &c_dfDIMouse2 ) ) )
        {
            Log( "if( FAILED( hr = m_lpMouse->SetDataFormat()\n" );
            TRACE_LEAVE_FN()
            return false;
        }

        if( FAILED( hr = m_lpMouse->SetCooperativeLevel( m_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND) ) )
        {
            Log( "if( FAILED( hr = m_lpMouse->SetCooperativeLevel()\n" );
            TRACE_LEAVE_FN()
            return false;
        }
    }

    TRACE_LEAVE_FN()

    return true;
}


//-----------------------------------------------------------------------------
// Name: SetAcquire()
// Desc: Acquire or unacquire the mouse, depending on if the app is active
//       Input device must be acquired before the GetDeviceState is called
//-----------------------------------------------------------------------------
bool CDInput::SetAcquire( bool bActive )
{
    if( m_lpMouse == NULL && m_bUseMouse == true )
    {
        LOG( "if( m_lpMouse == NULL && m_bUseMouse == true ) @ CDInput::SetAcquire()\n" );
        return false;
    }

    if( bActive ) 
    {
        m_lpMouse->Acquire();
        m_lpDIDevice->Acquire();
        
    }
    else 
    {
        m_lpMouse->Unacquire();
        m_lpDIDevice->Unacquire();
    }

    return true;
}


//---------------------------------------------------------------------
//Name: SaveMouseData()
//Desc: Once Every Frame, the mouse data muct be locked before any
//      GetMouse- Functions are called. This dumps the current state
//      of the mouse into a struct and allows it to be accessed
//---------------------------------------------------------------------
bool CDInput::UpdateInput()
{
    TRACE_ENTER_FN( CDInput::UpdateInput )

    HRESULT    hr;
    int tries = 0;
    DIMOUSESTATE2 dims2;

    SetFreeze( false );

    while( tries < 10 )
    {
        if( FAILED( m_lpDIDevice->GetDeviceState( sizeof( m_KeyBuffer ), ( LPVOID ) &m_KeyBuffer ) ) )
            SetAcquire( true );
        else break;

        tries++;
    }

    if( NULL == m_lpMouse )
    {
        Log( "if( NULL == m_lpMouse ) @ CDInput::UpdateInput()\n" );
        TRACE_LEAVE_FN()
        return false;
    }
    
    ZeroMemory( &dims2, sizeof( dims2 ) );

    AGAIN:

    hr = m_lpMouse->GetDeviceState( sizeof( DIMOUSESTATE2 ), &dims2 );

    if( FAILED( hr ) ) 
    {
        hr = m_lpMouse->Acquire();

        while( hr == DIERR_INPUTLOST ) 
            hr = m_lpMouse->Acquire();

        tries++;

        if( tries < 10 ) goto AGAIN;

        TRACE_LEAVE_FN()

        return false; 
    }

    if( dims2.rgbButtons[0] == 0x80 )
        m_bMouseButtonLeft = true;
    else
        m_bMouseButtonLeft = false;

    if( dims2.rgbButtons[1] == 0x80 )
        m_bMouseButtonRight = true;
    else
        m_bMouseButtonRight = false;

    m_lMouseX = dims2.lX;
    m_lMouseY = dims2.lY;
    m_lMouseZ = dims2.lZ;

    TRACE_LEAVE_FN()

    return true;
}


//---------------------------------------------------------------------
//Name: KeyPressed()
//Desc: 
//---------------------------------------------------------------------
bool CDInput::KeyPressed( int Key )
{
    if( m_KeyBuffer[Key] & 0x80 )
        return true;

    return false;
}


//---------------------------------------------------------------------
//Name: Release()
//Desc: Releases all input components
//---------------------------------------------------------------------
bool CDInput::Release()
{
    LOG( "CDInput::Release()" );

    if( m_lpDI )
    {
        if( m_lpDIDevice )
        {
            m_lpDIDevice->Unacquire();
            m_lpDIDevice->Release();
            m_lpDIDevice = NULL;
        }

        if( m_lpMouse )
        {
            m_lpMouse->Unacquire();
        }

        SAFE_RELEASE( m_lpMouse );
        SAFE_RELEASE( m_lpDI );
    }

    return true;
}


//-----------------------------------------------------------------------------
// Name: GetData()
// Desc: 
//-----------------------------------------------------------------------------
bool CDInput::GetData( LPINPUTDATA data )
{
    if( !UpdateInput() )
        return false;

    data->lButton = GetMouseButtonRight();
    data->rButton = GetMouseButtonLeft();
    data->x       = GetMouseX();
    data->y       = GetMouseY();
    data->z       = GetMouseZ();
    data->lArrow  = KeyPressed( DIK_LEFT );
    data->rArrow  = KeyPressed( DIK_RIGHT );
    data->dArrow  = KeyPressed( DIK_DOWN );
    data->uArrow  = KeyPressed( DIK_UP );

    for( int a = 0; a < 256; a++ )
        data->Buffer[a] = m_KeyBuffer[a];

    return true;
}

