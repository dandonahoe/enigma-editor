//----------------------------------------------------------------------------- 
// File: Hemicube.h
//
// Desc: 
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------

#ifndef _HEMICUBE_H_
#define _HEMICUBE_H_


#include "enum.h"
#include "Util.h"

class CHemicube  
{
public:
    CHemicube();
    virtual ~CHemicube();
    bool Init();
    void Release();
    bool Render( const VECT vFrom,
                 const VECT vAt,
                 const VECT vUp,
                 const CAMERAVIEW eView,
                 const bool bInPhase1 );

    VECT GetColor( const bool bInPhase1 );
    bool GenerateScreens();
    unsigned int GetTextureID( const int iIndex );

private:
    bool CreateTexture( const int iSize, const int iChannels,
                        const int iType, const int iTexID );
    bool ApplyViewCorrection( unsigned char* pData, const CAMERAVIEW eView );
    bool GetDataTGA( float* pData, const char* strFileName );
    bool RenderPhase1();
    bool RenderPhase2();

    unsigned char* m_pTexBuffer[5];
    unsigned int m_uiTextures[5];
    const int m_iTexSize;
    float* m_pForwardMap;
    float* m_pBottomMap;
    float* m_pRightMap;
    float* m_pLeftMap;
    float* m_pUpMap;
};


#endif