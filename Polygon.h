//----------------------------------------------------------------------------- 
// File: Polygon.h
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _POLYGON_H_
#define _POLYGON_H_


//-----------------------------------------------------------------------------
// Includes
#include <fstream>
#include "PolygonManager.h"
#include "Face.h"
#include "enum.h"
#include "Vertex.h"
//-----------------------------------------------------------------------------

using namespace std;
//-----------------------------------------------------------------------------
// Defines
//#define MAX_FACES 6
//-----------------------------------------------------------------------------


class CPolygon  
{
public:
    CPolygon::~CPolygon();
    CPolygon::CPolygon();
    
    
    //void NewQuadFace( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, const D3DXVECTOR3 &v3, const D3DXVECTOR3 &v4 );
    void NewTriangleFace( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, const D3DXVECTOR3 &v3 );
    void SetName( const char* str, bool bAddUniqueID = false );
    virtual bool Save( const char* File, ofstream& out );
    virtual bool Open( const char* File, ifstream& in );
    void FrameMove( const float fElapsedTime );
    void TranslateX( const float amount );
    void TranslateY( const float amount );
    void TranslateZ( const float amount );
    void NextFace();
    void Release();
    bool RemoveFace( int face );
    bool ReSortFaceList();
    bool AllocateFaceSpace( const int uiTotalFaces );
    bool MoveAllMatching( const VECT vFrom, const VECT vTo );
    int  GetTotalFaces() const { return m_iTotalFaces; }
    void SetTotalFaces( const int iFaces ) { m_iTotalFaces = iFaces; }
    int  GetLoadedFaces() const { return m_iLoadedFaces; }
    void SetLoadedFaces( const int iFaces ) { m_iLoadedFaces = iFaces; }
    bool AlignFaceTextures();
    HTREEITEM GetTreeItem() const { return m_hItem; }
    void SetTreeItem( const HTREEITEM &hItem ) { m_hItem = hItem; }
    VECT GetCenterVect();

    CLASSTYPE GetClassType() const { return m_eClassType; }
    void SetClassType( const CLASSTYPE eType ) { m_eClassType = eType; }

    CFace* F( const int iFace );
    CVertex* V( const int iFace, const int iVert );
    
    bool SetF( const int iIndex, CFace* pFace );

    char m_strName[256];
    
    unsigned int GetUniqueID() const { return m_uiUniqueID; }

    bool Rebuild();

    float* GetVertexArray();
    float* GetColorArray();
    float* GetTexCoordArray();
    int*   GetTexSwitchArray();
    unsigned int* GetUniqueTextureArray();
    float* GetLightmapCoordArray();
    int* GetVertexBeginArray();
    int* GetVertexEndArray();
    int GetNumTexSwitches()    const { return m_iNumTexSwitches; }
    int GetNumVerts()          const { return m_iNumVerts; }

private:
    bool DeleteArrays();
    bool AllocateNewArrays();
    bool SortArraysByTexture();
    bool FillArrays();

    static unsigned int m_uiUniqueID;
    int       m_iTotalFaces;  
    int       m_iLoadedFaces;
    HTREEITEM m_hItem;
    CLASSTYPE m_eClassType;
    CFace**   m_pFaces;
    float*    m_fpVertexArray;
    float*    m_fpColorArray;
    float*    m_fpTexCoordArray;
    float*    m_pfLightmapCoords;
    int*      m_ipTexSwitchArray;
    int*      m_ipVertexBeginArray;
    int*      m_ipVertexEndArray;

    unsigned int* m_ipUniqueTexArray;

    int       m_iNumTexSwitches;
    int       m_iNumVerts;

protected:
    void SetVertexArray( float* pVerts ) { m_fpVertexArray = pVerts; }
    void SetColorArray(  float* pColor ) { m_fpColorArray  = pColor; }
};


#endif
