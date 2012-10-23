//----------------------------------------------------------------------------- 
// File: Vertex.h
//
// Desc: Declaration file for vertex information
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _VERTEX_H_
#define _VERTEX_H_


//-----------------------------------------------------------------------------
// Includes
#include <D3DX8math.h>
#include "PolygonManager.h"
#include "enum.h"
#include "main.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: class CVertex 
// Desc: The vertex class contains all information about a vertex. (position,
//       color, texture coords, name, and a handle to its leaf in the polygon
//       view. Most verts are handled by the face class, because faces are
//       composed of verticies. Every vertex declared is attomatically given
//       a unique string name in construction. (m_strName)
//-----------------------------------------------------------------------------
class CVertex  
{
public:
    CVertex();
    virtual ~CVertex();

    // Utility Stuff
    bool AddVertexConnection( CVertex* v, const int iP, const int iF );
    void SetName( const char* str ) { strcpy( m_strName, str ); }
    CLASSTYPE GetClassType() const { return m_eClassType; }
    void FrameMove( const float fElapsedTime );
    char* GetName() { return m_strName; }
    CVertex* GetThis() { return this; }
    bool ClearAllVertexConnections();
    void ResetVirtualCoords();
    bool DragConnectedVerts();
    void SnapVertsToGrid();


    // Polygon tree window info
    HTREEITEM GetTreeItem() { return m_hItem; }
    void SetTreeItem( const HTREEITEM hItem ) { m_hItem = hItem; }
    

    // Vertex Positions
    bool SetVertPos( const VECT v );
    void TranslateX( const float amount );
    void TranslateY( const float amount );
    void TranslateZ( const float amount );
    VECT GetVertPos() { return m_vPos; }


    // Vertex Color
    void SetColor( const VECT v ) { m_vColor = v; }
    VECT GetColor() { return m_vColor; }


    // Operators
    bool operator == ( const CVertex &CVertex )
    { if( m_vPos == CVertex.m_vPos ) return true; }


    // Saving and loading
    bool Save( const char* File, ofstream& out );
    bool Open( const char* File, ifstream& in );


    // Texture Coordinates
    void  SetTexU( const float fU ) { m_fU = fU; }
    void  SetTexV( const float fV ) { m_fV = fV; }
    float GetTexU() { return m_fU; }
    float GetTexV() { return m_fV; }

    float m_fPos[3];
    float m_fCol[3];
    float m_fTexUV[2];

    bool Rebuild();
    float* GetVertexArray()   { return &m_fPos[0]; }
    float* GetColorArray()    { return &m_fCol[0]; }
    float* GetTexCoordArray() { return &m_fTexUV[0]; }

private:
    typedef struct _CONNECTEDVERT
    {
        CVertex* pV;
        int iP, iF;
    } CONNECTEDVERTEX;
    
    CONNECTEDVERTEX* m_pConnectedVerts;
    static unsigned int m_UniqueID;
    int m_iNumConnectedVerts;
    CLASSTYPE m_eClassType;
    VECT m_vColor;
    char m_strName[256];
    VECT m_vPos;
    float m_fPrecision;
    HTREEITEM m_hItem;
    float m_fVirtX;
    float m_fVirtY;
    float m_fVirtZ;
    float m_fU;
    float m_fV;
};


#endif
