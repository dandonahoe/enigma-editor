//----------------------------------------------------------------------------- 
// File: Brush.h
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _BRUSH_H_
#define _BRUSH_H_


//-----------------------------------------------------------------------------
// Includes
#include "Polygon.h"
#include "enum.h"
#include "Face.h"
//-----------------------------------------------------------------------------



class CBrush : public CPolygon
{
public:
    CBrush();
    virtual ~CBrush();
    bool SetType( const BRUSHTYPE type, CFace* pF = NULL );
    CPolygon* Forge();
    void Render();
    virtual bool Save( const char* File, ofstream out );
    virtual bool Open( const char* File, ifstream in );
    bool RenderSelected();
    void SetBrushOffset( const VECT v ) { m_vPos = v; }
    bool BrushRebuild();

private:
    BRUSHTYPE m_eBrushType;
    bool NewCube();
    bool NewSheet();
    bool NewStairs();
    bool NewSpiralStairs();
    bool NewCylinder();
    bool NewCircle();
    bool NewCustomBrush( CFace* pF );
    GLUquadricObj* m_pQuadricObj;
    VECT m_vPos;
};


#endif
