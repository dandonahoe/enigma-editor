//----------------------------------------------------------------------------- 
// File: Face.cpp
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _FACE_H_
#define _FACE_H_


//-----------------------------------------------------------------------------
// Includes
#include <string>
#include <fstream>
#include "TextureManager.h"
#include "PolygonManager.h"
#include "Vertex.h"
#include "enum.h"
#include "Buffer.h"
#include "LightmapManager.h"
//-----------------------------------------------------------------------------

using namespace std;
//-----------------------------------------------------------------------------
// Max Verts
#define MAX_VERTS 4
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: class CFace
// Desc: All face information is stored in this class
//-----------------------------------------------------------------------------
class CFace  
{
public:
    CFace();
    virtual ~CFace();
    void Release();

    void SetQuadVerts( const VECT &v1, const VECT &v2, const VECT &v3, const VECT &v4 );
    void SetTriangleVerts( const VECT &v1, const VECT &v2, const VECT &v3 );
    VECT GetCrossProduct( VECT vVector1, VECT vVector2 );
    VECT Vector( VECT vPoint1, VECT vPoint2 );
    void SetName( const char* str, bool bAddUniqueID = false );
    
    bool IsTransparent() const { return m_bTranslucent; }
    bool Save( const char* File, ofstream& out );
    bool Open( const char* File, ifstream& in );
    void SetPanU( const bool bPan ) { m_bPanU = bPan; }
    void SetPanV( const bool bPan ) { m_bPanV = bPan; }
    void SetTranslucent( const bool bTranslucent );
    VECT Normal( VECT v1, VECT v2, VECT v3 );
    VECT Normalize( VECT vNormal );
    bool FrameMove( const float fElapsedTime );
    void TranslateTexU( const float amount );
    void TranslateTexV( const float amount );
    bool GetPanU() const { return m_bPanU; }
    bool GetPanV() const { return m_bPanV; }
    float Magnitude( VECT vNormal );
    bool TranslateX( const float amount );
    bool TranslateY( const float amount );
    bool TranslateZ( const float amount );
    bool SetScale( const float factor );
    void GenerateTextureCoordinates();
    void ResetVirtualCoords();
    void FlipTexU();
    void FlipTexV();
    void RotateTex90();
    bool RotateLightmap90();
    VECT GetFaceNormal();
    VECT GetFaceCenter();

    bool IsCoenciding( CFace* pF );
    FACETYPE GetFaceType() const { return m_eFaceType; }
    void SetFaceType( const FACETYPE eType ) { m_eFaceType = eType; }

    CLASSTYPE GetClassType() const { return m_eClassType; }
    void SetClassType( const CLASSTYPE eType ) { m_eClassType = eType; }

    HTREEITEM GetTreeItem() const { return m_hItem; }
    void SetTreeItem( const HTREEITEM &hItem ) { m_hItem = hItem; }

    bool operator == ( const CFace &face )
    {
        if( m_iNumVerts != face.m_iNumVerts )
            return false;

        bool bMatch;

        for( int a = 0; a < m_iNumVerts; a++ )
        {
            bMatch = false;

            for( int b = 0; b < m_iNumVerts; b++ )
            {
                if( m_Verts[a] == face.m_Verts[b] )
                {
                    bMatch = true;
                    break;
                }
            }

            if( !bMatch )
                return false;
        }

        return true;
    }

    int GetLoadedVerts() const { return m_iNumVerts; }

    void SetTexture1( const HTEXTURE hTexture  ) { m_hTexture1 = hTexture;  }
    void SetTexture2( const HTEXTURE hTexture  ) { m_hTexture2 = hTexture;  }
    void SetLightmap( const LIGHTMAP hLightmap ) { m_hLightmap = hLightmap; }

    HTEXTURE GetTexture1() const { return m_hTexture1; }
    HTEXTURE GetTexture2() const { return m_hTexture2; }
    LIGHTMAP GetLightmap() const { return m_hLightmap; }

    void UseTexture1( const bool b ) { m_bUseTexture1 = b; }
    void UseTexture2( const bool b ) { m_bUseTexture2 = b; }
    void UseLightmap( const bool b ) { m_bUseLightmap = b; }

    bool UsingTexture1() const { return m_bUseTexture1; }
    bool UsingTexture2() const { return m_bUseTexture2; }
    bool UsingLightmap() const { return m_bUseLightmap; }

//    bool CreateLightmap( const int iRes, const unsigned char* pData );
    int GetLightmapRes() const { return m_hLightmap.iResolution; }
//    bool ReleaseLightmap();
    bool PrepareLightmap();
    void DisableLightmap();

    int GetCoordU( const int iIndex );
    int GetCoordV( const int iIndex );

    CVertex* V( const int iV );
    char* GetName() { return m_strName; }

    VECT GetCenter() { return m_vCenter; }
    VECT GetNormal() { return m_vNormal; }

    bool Rebuild();

    float* GetVertexArray();
    float* GetColorArray();
    float* GetTexCoordArray();
    float* GetLightmapCoordArray() { return m_pfLightmapCoords; }

private:
    bool AllocateVertexArrays();

    bool m_bPanU;
    bool m_bPanV;
    bool m_bTranslucent;
    CLASSTYPE m_eClassType;

    HTEXTURE m_hTexture1;
    HTEXTURE m_hTexture2;
    LIGHTMAP m_hLightmap;

    bool m_bUseTexture1;
    bool m_bUseTexture2;
    bool m_bUseLightmap;

    int m_iU[4];
    int m_iV[4];

    GLenum m_eGLFaceType;

    static unsigned int m_UniqueID;
    CVertex m_Verts[MAX_VERTS];

    VECT m_vCenter;
    VECT m_vNormal;
    char m_strName[256];
    
    FACETYPE m_eFaceType;
    HTREEITEM m_hItem;
    int m_iNumVerts;

    float* m_pfFaceVerts;
    float* m_pfFaceColors;
    float* m_pfFaceTexCoords;
    float* m_pfLightmapCoords;
};


#endif
