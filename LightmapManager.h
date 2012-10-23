//----------------------------------------------------------------------------- 
// File: 
//
// Desc: 
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _LIGHTMAPMANANGER_H_
#define _LIGHTMAPMANANGER_H_

#include <fstream>
using namespace std;

typedef struct _BLOCK_SECTOR
{
    bool bLeftIsOccupied, bRightIsOccupied;
} BLOCK_SECTOR;

typedef struct _LIGHTMAP
{
    int iLightmapBlockIndex;
    int iResolution;
    BLOCK_SECTOR BlockSector;
    float fU[ 4 ];
    float fV[ 4 ];
} LIGHTMAP;

class CLightmapManager
{
public:
    CLightmapManager();
    virtual ~CLightmapManager();

    bool Init();
    bool Release();
    bool Invalidate( LIGHTMAP* lm );

    bool Open( const char* strFile, ifstream &in );
    bool Save( const char* strFile, ofstream &out );

    unsigned int GetTextureGL( const int iIndex ) { return m_pLightmapBlocks[ iIndex ]; }

private:
    int  GetNumLightmapBlocks() const           { return m_iNumLightmapBlocks; }
    void SetNumLightmapBlocks( const int iNum ) { m_iNumLightmapBlocks = iNum; }
    
    bool ReleaseLightmap( const int iIndex );

    bool CreateLightmap( const unsigned char* pLightmapData, const int iIndex );

    const int LIGHTMAP_WIDTH, LIGHTMAP_HEIGHT;
    const int BLOCK_WIDTH,    BLOCK_HEIGHT;

    int m_iNumLightmapBlocks;
    unsigned int* m_pLightmapBlocks;

    BLOCK_SECTOR m_OccupancyGrid[ 32 ][ 32 ];
};

#endif
