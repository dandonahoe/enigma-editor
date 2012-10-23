//----------------------------------------------------------------------------- 
// File: SaveTokens.cpp
//
// Desc: 
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Include
#include <string.h>
#include "SaveTokens.h"
#include "Tracer.h"
#include "main.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: IsValidToken()
// Desc:
//-----------------------------------------------------------------------------
bool IsValidToken( const char* strToken )
{
    TRACE_ENTER_FN( IsValidToken )

    if( strToken == NULL ) LOG_ERR_RET( false )

    if( strlen( strToken ) <= 2 ) { LOG_VAR( strToken ) LOG_ERR_RET( false ) }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: IsPolygonManagerToken()
// Desc:
//-----------------------------------------------------------------------------
bool IsPolygonManagerToken( const char* strToken )
{
    TRACE_ENTER_FN( IsPolygonManagerToken )

//    if( !IsValidToken( strToken ) ) { TRACE_LEAVE_FN() return false; }

/***********************
VERSION 1.0 TOKENS BEGIN
***********************/
    if( IsSameToken( PM_BEGIN,       strToken ) ) RET( true )
    if( IsSameToken( PM_END,         strToken ) ) RET( true )
    if( IsSameToken( PM_NUMPOLYGONS, strToken ) ) RET( true )
    if( IsSameToken( PM_CURRENTMODE, strToken ) ) RET( true )
    if( IsSameToken( PM_CLASSTYPE,   strToken ) ) RET( true )
/***********************
VERSION 1.0 TOKENS BEGIN
***********************/

    LOG_ERR_RET( false )
}


//-----------------------------------------------------------------------------
// Name: IsPolygonToken()
// Desc:
//-----------------------------------------------------------------------------
bool IsPolygonToken( const char* strToken )
{
    TRACE_ENTER_FN( IsPolygonToken )

//    if( !IsValidToken( strToken ) ) RET( true )

/***********************
VERSION 1.0 TOKENS BEGIN
***********************/
    if( IsSameToken( P_BEGIN,       strToken ) ) RET( true )
    if( IsSameToken( P_END,         strToken ) ) RET( true )
    if( IsSameToken( P_NAME,        strToken ) ) RET( true )
    if( IsSameToken( P_UNIQUEID,    strToken ) ) RET( true )
    if( IsSameToken( P_TOTALFACES,  strToken ) ) RET( true )
    if( IsSameToken( P_LOADEDFACES, strToken ) ) RET( true )
    if( IsSameToken( P_CLASSTYPE,   strToken ) ) RET( true )
/***********************
VERSION 1.0 TOKENS END
***********************/
    LOG_ERR_RET( false )
}


//-----------------------------------------------------------------------------
// Name: IsFaceToken()
// Desc:
//-----------------------------------------------------------------------------
bool IsFaceToken( const char* strToken )
{
    TRACE_ENTER_FN( IsFaceToken )

//    if( !IsValidToken( strToken ) ) RET( true )

/***********************
VERSION 1.0 TOKENS BEGIN
***********************/
    if( IsSameToken( F_BEGIN,       strToken ) ) RET( true )
    if( IsSameToken( F_END,         strToken ) ) RET( true )
    if( IsSameToken( F_UNIQUEID,    strToken ) ) RET( true )
    if( IsSameToken( F_NAME,        strToken ) ) RET( true )
    if( IsSameToken( F_FACETYPE,    strToken ) ) RET( true )
    if( IsSameToken( F_NUMVERTS,    strToken ) ) RET( true )
    if( IsSameToken( F_PANU,        strToken ) ) RET( true )
    if( IsSameToken( F_PANV,        strToken ) ) RET( true )
    if( IsSameToken( F_TRANSLUCENT, strToken ) ) RET( true )
    if( IsSameToken( F_TEX1PATH,    strToken ) ) RET( true )
    if( IsSameToken( F_TEX2PATH,    strToken ) ) RET( true )
//    if( IsSameToken( F_TEX3PATH,    strToken ) ) RET( true )
    if( IsSameToken( F_USETEX1,     strToken ) ) RET( true )
    if( IsSameToken( F_USETEX2,     strToken ) ) RET( true )
    if( IsSameToken( F_USELIGHTMAP, strToken ) ) RET( true )
//    if( IsSameToken( F_LIGHTMAPRES, strToken ) ) RET( true )
    if( IsSameToken( F_UTEXCOORDSU, strToken ) ) RET( true )
    if( IsSameToken( F_UTEXCOORDSV, strToken ) ) RET( true )
    if( IsSameToken( F_LIGHTMAP,    strToken ) ) RET( true )
    if( IsSameToken( F_GLFACETYPE,  strToken ) ) RET( true )
/***********************
VERSION 1.0 TOKENS END
***********************/

    LOG_ERR_RET( false )
}


//-----------------------------------------------------------------------------
// Name: IsVertToken()
// Desc:
//-----------------------------------------------------------------------------
bool IsVertToken( const char* strToken )
{
    TRACE_ENTER_FN( IsVertToken )

//    if( !IsValidToken( strToken ) ) RET( true )

/***********************
VERSION 1.0 TOKENS BEGIN
***********************/
    if( IsSameToken( V_BEGIN,     strToken ) ) RET( true )
    if( IsSameToken( V_END,       strToken ) ) RET( true )
    if( IsSameToken( V_COLORRGB,  strToken ) ) RET( true )
    if( IsSameToken( V_NAME,      strToken ) ) RET( true )
    if( IsSameToken( V_POSXYZ,    strToken ) ) RET( true )
    if( IsSameToken( V_U,         strToken ) ) RET( true )
    if( IsSameToken( V_V,         strToken ) ) RET( true )
    if( IsSameToken( V_UNIQUEID,  strToken ) ) RET( true )
    if( IsSameToken( V_VERTXYZ,   strToken ) ) RET( true )
    if( IsSameToken( V_PRECISION, strToken ) ) RET( true )
/***********************
VERSION 1.0 TOKENS END
***********************/

    LOG_ERR_RET( false )
}


//-----------------------------------------------------------------------------
// Name: IsVertToken()
// Desc:
//-----------------------------------------------------------------------------
bool IsSameToken(  const char* strToken1, const char* strToken2 )
{
    return strcmp( strToken1, strToken2 ) == 0;
}