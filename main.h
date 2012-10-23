//----------------------------------------------------------------------------- 
// File: main.h
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------

#ifndef _MAIN_H_
#define _MAIN_H_

#include <windows.h>
#include <stdio.h>
#include <D3DX8math.h>
typedef D3DXVECTOR3 VECT;
#include "Settings.h"

#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#define LOG_FILE "log.txt"
#define NORMAL_ERROR(p) { char str[256];\
                          sprintf( str, "    ERROR: %s\n", p );\
                          Log( str ); }
#define CRITICAL_ERROR(p) { char str[256];\
                            sprintf( str, "    ERROR: %s\n", p );\
                            Log( str );\
                            return false; }
#define LOG(p) { char str[256];\
                 sprintf( str, "%s\n", p );\
                 Log( str ); }



//void Log( char* msg );
void Log( const char* msg );
void Log( float msg );
void Log( double msg );
void Log( int msg );
void Log( bool msg );
void Log( VECT msg );
void Log( unsigned long int msg );

#define LOG_ERR() { \
    char str[256]; \
    sprintf( str, "%s @ line: %d", __FILE__ ,__LINE__ ); \
    Log( str ); \
    Log( "\n" ); \
    if( SE->m_bSystem_DisplayErrorMessageBoxes ) MessageBox( HWND_DESKTOP, str, "Error", MB_OK ); }
/*
#define LOG_ERR_RET_F() { \
    char str[256]; \
    sprintf( str, "%s @ line: %d", __FILE__ ,__LINE__ ); \
    Log( str ); \
    Log( "\n" ); \
    if( SE->m_bSystem_DisplayErrorMessageBoxes ) MessageBox( HWND_DESKTOP, str, "Error", MB_OK ); \
    TRACE_LEAVE_FN() \
    return false; }

#define LOG_ERR_RET_T() { \
    char str[256]; \
    sprintf( str, "%s @ line: %d", __FILE__ ,__LINE__ ); \
    Log( str ); \
    Log( "\n" ); \
    if( SE->m_bSystem_DisplayErrorMessageBoxes ) MessageBox( HWND_DESKTOP, str, "Error", MB_OK ); \
    TRACE_LEAVE_FN() \
    return true; }
*/
#define LOG_VAR( var ) { \
    Log( #var ); \
    Log( ": " ); \
    Log( var  ); \
    Log( "\n" ); }

#define MB( msg ) { \
    char str[256]; \
    sprintf( str, "%s @ line: %d", __FILE__ ,__LINE__ ); \
    MessageBox( HWND_DESKTOP, msg, str, MB_OK ); }


#define LOG_ERR_RET( var ) { \
    char str[256]; \
    sprintf( str, "%s @ line: %d", __FILE__ ,__LINE__ ); \
    Log( str ); \
    Log( "\n" ); \
    if( SE->m_bSystem_DisplayErrorMessageBoxes ) MessageBox( HWND_DESKTOP, str, "Error", MB_OK ); \
    TRACE_LEAVE_FN() \
    return var; }

#define RET( var ) { \
    TRACE_LEAVE_FN() \
    return var; }

LRESULT CALLBACK WinProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

float RandomNumber( float fMin, float fMax );

void SetVector( VECT &v, float x, float y, float z );



typedef struct
{
    char id_len;                 // ID Field (Number of bytes - max 255)
    char map_type;               // Colormap Field (0 or 1)
    char img_type;               // Image Type (7 options - color vs. compression)
    int  map_first;              // Color Map stuff - first entry index
    int  map_len;                // Color Map stuff - total entries in file
    char map_entry_size;         // Color Map stuff - number of bits per entry
    int  x;                      // X-coordinate of origin 
    int  y;                      // Y-coordinate of origin
    int  width;                  // Width in Pixels
    int  height;                 // Height in Pixels
    char bpp;                    // Number of bits per pixel
    char misc;                   // Other stuff - scan origin and alpha bits
} TARGAHEADER;


#endif