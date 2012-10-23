//----------------------------------------------------------------------------- 
// File: SaveTokens.h
//
// Desc: System of tokens used when saving levels to make all level files
//       compatable when opened in later versions of the editor.
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _SAVETOKENS_H_
#define _SAVETOKENS_H_ 

#define NEWLINE '\n'
/**** Version 1.0 Saving Tokens ****/

// Polygon Manager Tokens
#define PM_BEGIN       "PM_BEGIN"
#define PM_END         "PM_END"
#define PM_NUMPOLYGONS "PM_NUMPOLYGONS"
#define PM_CURRENTMODE "PM_CURRENTMODE" 
#define PM_CLASSTYPE   "PM_CLASSTYPE"

// Polygon Tokesns
#define P_BEGIN        "P_BEGIN"
#define P_END          "P_END"
#define P_NAME         "P_NAME"
#define P_UNIQUEID     "P_UNIQUEID"
#define P_TOTALFACES   "P_TOTALFACES"
#define P_LOADEDFACES  "P_LOADEDFACES"
#define P_CLASSTYPE    "P_CLASSTYPE"

// Face Tokens
#define F_BEGIN        "F_BEGIN"
#define F_END          "F_END"
#define F_UNIQUEID     "F_UNIQUEID"
#define F_NAME         "F_NAME"
#define F_FACETYPE     "F_FACETYPE"
#define F_NUMVERTS     "F_NUMVERTS"
#define F_PANU         "F_PANU"
#define F_PANV         "F_PANV"
#define F_TRANSLUCENT  "F_TRANSLUCENT"
#define F_TEX1PATH     "F_TEX1PATH"
#define F_TEX2PATH     "F_TEX2PATH"
//#define F_TEX3PATH     "F_TEX3PATH"
#define F_USETEX1      "F_USETEX1"
#define F_USETEX2      "F_USETEX2"
#define F_USELIGHTMAP  "F_USELIGHTMAP"
//#define F_LIGHTMAPRES  "F_LIGHTMAPRES"
#define F_UTEXCOORDSU  "F_UTEXCOORDSU"
#define F_UTEXCOORDSV  "F_UTEXCOORDSV"
#define F_LIGHTMAP     "F_LIGHTMAP"
#define F_GLFACETYPE   "F_GLFACETYPE"
#define F_LIGHTMAP     "F_LIGHTMAP"

// Vertex Tokens
#define V_BEGIN        "V_BEGIN"
#define V_END          "V_END"
#define V_COLORRGB     "V_COLORRGB"
#define V_NAME         "V_NAME"
#define V_POSXYZ       "V_POSXYZ"
#define V_U            "V_U"
#define V_V            "V_V"
#define V_UNIQUEID     "V_UNIQUEID"
#define V_VERTXYZ      "V_VERTXYZ"
#define V_PRECISION    "V_PRECISION"

// Lightmap Manager Token
#define LMM_BLOCK      "LMM_BLOCK"

bool IsValidToken( const char* strToken );
bool IsPolygonManagerToken( const char* strToken );
bool IsPolygonToken( const char* strToken );
bool IsFaceToken( const char* strToken );
bool IsVertToken( const char* strToken );
bool IsSameToken(  const char* strToken1, const char* strToken2 );

#endif
