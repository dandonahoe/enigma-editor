//----------------------------------------------------------------------------- 
// File: Util.h
//
// Desc: 
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _UTIL_H_
#define _UTIL_H_

#include <windows.h>
#include <D3DX8math.h>
#include "enum.h"

// These defines are used to tell us about the type of TARGA file it is
#define TGA_RGB         2        // This tells us it's a normal RGB (really BGR) file
#define TGA_A         3        // This tells us it's a ALPHA file
#define TGA_RLE        10        // This tells us that the targa is Run-Length Encoded (RLE)


DWORD GetCpuSpeed();
D3DXVECTOR3 RotPtByLn( D3DXVECTOR3 p, float theta,
                       D3DXVECTOR3 p1, D3DXVECTOR3 p2 );

typedef struct tImageTGA
{
    int iChannels;
    int iSizeX;
    int iSizeY;
    unsigned char *pData;
} TARGADATA, *PTARGADATA;

bool GenerateScreenShot( const char* strFileName,
                         const int iWidth, const int iHeight,
                         const bool bFlip,
                         const unsigned char* pData );

void WriteTargaHeader( TARGAHEADER h, FILE *tga );

TARGADATA* LoadTGA( const char *filename );

BRUSHTYPE GetBrushEnumType( const int i );
int GetBrushEnumID( const int eType );
CAMERAVIEW GetViewEnumType( const int iType );
int GetViewEnumID( const CAMERAVIEW eView );
FACETYPE GetFaceEnumType( const int i );
int GetFaceEnumID( const FACETYPE eType );
CLASSTYPE GetClassEnumType( const int i );
int GetClassEnumID( const CLASSTYPE eType );

bool NormalizeBitmap( double* pData, const iLen );

bool RemoveArrayIndex( float** pfArray, const int iLength, const int iRemoveIndex );
bool RemoveArrayIndex( float*** pfArray, const int iLength, const int iRemoveIndex );
#endif