//----------------------------------------------------------------------------- 
// File: 
//
// Desc: 
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _GRID_H_
#define _GRID_H_


#include "Window.h"


class CGrid
{
public:
    CGrid();
    virtual ~CGrid();

    bool Init();
    bool FrameMove();
    bool Render( const VIEWTYPE eViewType );
    bool Release();

    void SetEnable3D( const bool bEnable ) { m_bEnabled3D = bEnable; }
    void SetEnableXZ( const bool bEnable ) { m_bEnabledXZ = bEnable; }
    void SetEnableXY( const bool bEnable ) { m_bEnabledXY = bEnable; }
    void SetEnableYZ( const bool bEnable ) { m_bEnabledYZ = bEnable; }

private:

    struct GridInfo
    {
        // How many units large the grid is
        int iSizeX,
            iSizeY,
            iSizeZ;

        // Lights up line % MajorLineSpacing to be a differet color
        int iMajorLineSpacing,
            iIncrementAmount;

    
        float fOrgR,//Color of the origin line
              fOrgG,
              fOrgB,
              fMajR,//Color of the major lines
              fMajG,
              fMajB,
              fMinR,//Color of the minor line
              fMinG,
              fMinB;
    };

    bool LoadArrays3D();
    bool LoadArraysXZ();
    bool LoadArraysXY();
    bool LoadArraysYZ();

    bool RenderGrid3D();
    bool RenderGridXZ();
    bool RenderGridXY();
    bool RenderGridYZ();

    GridInfo m_GridInfo;

    float* m_pfGridVerticies3D;
    float* m_pfGridVerticiesXZ;
    float* m_pfGridVerticiesXY;
    float* m_pfGridVerticiesYZ;

    float* m_pfGridColors3D;
    float* m_pfGridColorsXZ;
    float* m_pfGridColorsXY;
    float* m_pfGridColorsYZ;

    int m_iNumVert3D;
    int m_iNumVertXZ;
    int m_iNumVertXY;
    int m_iNumVertYZ;

    bool m_bEnabled3D;
    bool m_bEnabledXZ;
    bool m_bEnabledXY;
    bool m_bEnabledYZ;
};

#endif