//----------------------------------------------------------------------------- 
// File: Map.h
//
// Desc: 
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _MAP_H_
#define _MAP_H_

//-----------------------------------------------------------------------------
// Includes
#include "Singleton.h"
#include "TextureManager.h"
#include "enum.h"
//-----------------------------------------------------------------------------


typedef struct _FACE
{
    float        m_pfNormal;
    float        m_fPanU;
    float        m_fPanV;
	HTEXTURE     m_phTexture;
    unsigned int m_piLightmap;
    int          m_iBeginIndex;
    int          m_iEndIndex;
    GLenum       m_eFaceType;
    bool         m_bRender;

} FACE, *LPFACE;


//-----------------------------------------------------------------------------
// Name: class CMap
// Desc: 
//-----------------------------------------------------------------------------
class CMap : public CSingleton<CMap>
{
public:
    CMap();
    virtual ~CMap();

    bool Init();
    void Release();

    LPFACE GetFace( const int iFace ) { return &m_sFaces[iFace]; }
    float* GetVerts()           const { return m_pfVerts;        }
    float* GetTexCoords()       const { return m_pfTexCoords;    }
    float* GetColors()          const { return m_pfColor;        }
    float* GetLightmapCoords()  const { return m_pfLightmapUV;   }
    int GetNumFaces()           const { return m_iNumFaces;      }
    int GetNumVerts()           const { return m_iNumVerts;      }

private:

    void SetNumVerts( const int iNumVerts ) { m_iNumVerts = iNumVerts; }
    void SetNumFaces( const int iNumFaces ) { m_iNumFaces = iNumFaces; }
    float* m_pfLightmapUV;
    float* m_pfTexCoords;
    float* m_pfVerts;
    float* m_pfColor;
    LPFACE m_sFaces;
    int m_iNumFaces;
    int m_iNumVerts;
};


#define Map CMap::GetSingletonPtr()

#endif