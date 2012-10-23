//----------------------------------------------------------------------------- 
// File: main.cpp
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <memory.h>
#include <malloc.h>
#include <crtdbg.h>
#include <windows.h>
#include <fstream>
#include <D3DX8math.h>
#include "main.h"
#include "Application.h"
#include "Tracer.h"
//-----------------------------------------------------------------------------
using namespace std;

CApplication *pApp;


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc:
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, INT ishow )
{
    try
    {
        TRACE_ENTER_FN( WinMain )

        CApplication Application;

        pApp = &Application;

        if( pApp->Init( hInstance, "EnigmaEd", 0.7 ) )
            return pApp->Run();

        TRACE_LEAVE_FN()
    }
    catch( ... )
    {
        MessageBox( HWND_DESKTOP, "Unhandled Exception, see log.txt", "Fatal Error", MB_OK );
        CallStackTrace::Dump();
    }

    return 0;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
LRESULT CALLBACK WinProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    return pApp->MsgProc( hWnd, uMsg, wParam, lParam );
}

/*
//-----------------------------------------------------------------------------
// Name: Log()
// Desc: Adds to the log file.
//-----------------------------------------------------------------------------
void Log( char *msg )
{
    ofstream fp;
    static bool FirstTime = true;

    if( FirstTime )
    {
        fp.open( LOG_FILE );
        fp.close();
        FirstTime = false;
    }

    fp.open( LOG_FILE, ios::app );
    fp << msg;
    fp.close();
}
*/

//-----------------------------------------------------------------------------
// Name: Log()
// Desc: Adds to the log file.
//-----------------------------------------------------------------------------
void Log( const char* msg )
{
    ofstream fp;
    static bool FirstTime = true;

    if( FirstTime )
    {
        fp.open( LOG_FILE );
        fp.close();
        FirstTime = false;
    }

    fp.open( LOG_FILE, ios::app );
    fp << msg;
    fp.close();
}



//-----------------------------------------------------------------------------
// Name: Log()
// Desc: Adds to the log file.
//-----------------------------------------------------------------------------
void Log( float msg )
{
    ofstream fp;
    
    fp.open( LOG_FILE, ios::app );
    fp << msg;
    fp.close();
}


//-----------------------------------------------------------------------------
// Name: Log()
// Desc: Adds to the log file.
//-----------------------------------------------------------------------------
void Log( double msg )
{
    ofstream fp;
    
    fp.open( LOG_FILE, ios::app );
    fp << msg;
    fp.close();
}


//-----------------------------------------------------------------------------
// Name: Log()
// Desc: Adds to the log file.
//-----------------------------------------------------------------------------
void Log( int msg )
{
    ofstream fp;
    
    fp.open( LOG_FILE, ios::app );

    fp << msg;
    fp.close();
}


//-----------------------------------------------------------------------------
// Name: Log()
// Desc: Adds to the log file.
//-----------------------------------------------------------------------------
void Log( unsigned long int msg )
{
    ofstream fp;
    
    fp.open( LOG_FILE, ios::app );

    fp << msg;
    fp.close();
}


//-----------------------------------------------------------------------------
// Name: Log()
// Desc: Adds to the log file.
//-----------------------------------------------------------------------------
void Log( bool msg )
{
    ofstream fp;
    
    fp.open( LOG_FILE, ios::app );

    if( msg )
        fp << "true";
    else
        fp << "false";

    fp.close();
}

//-----------------------------------------------------------------------------
// Name: Log()
// Desc: Adds to the log file.
//-----------------------------------------------------------------------------
void Log( VECT msg )
{
    ofstream fp;
    
    fp.open( LOG_FILE, ios::app );

    Log( "x: " ); Log( msg.x ); Log( "\n" );
    Log( "y: " ); Log( msg.y ); Log( "\n" );
    Log( "z: " ); Log( msg.z ); Log( "\n" );

    fp.close();
}



//-----------------------------------------------------------------------------
// Name: SetVector()
// Desc: 
//-----------------------------------------------------------------------------
void SetVector( D3DXVECTOR3 &v, float x, float y, float z )
{
    v.x = x;
    v.y = y;
    v.z = z;
}
//#ifdef _DEBUG
//    _CrtSetDbgFlag( _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG ) | _CRTDBG_LEAK_CHECK_DF );
//#endif