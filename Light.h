//----------------------------------------------------------------------------- 
// File: 
//
// Desc: 
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _LIGHT_H_
#define _LIGHT_H_


#include "enum.h"
#include "main.h"


class CLight  
{
public:
    CLight();
    virtual ~CLight();
    bool Render();

    void SetBrightness( const float fBrightness ) { m_fBrightness = fBrightness; }
    void SetName( const char* str ) { strcpy( m_strName, str ); }
    void SetColor( const float r, const float g, const float b );
    void SetPos( const float x, const float y, const float z );
    void SetColor( const VECT vC ) { m_vColor = vC; }
    void SetPos( const VECT vP ) { m_vPos = vP; }
    
    float GetBrightness() const { return m_fBrightness; }
    VECT GetColor()       const { return m_vColor;      }
    VECT GetPos()         const { return m_vPos;        }
    
    CLASSTYPE GetClassType() const { return m_eClassType; }
    char* GetName() { return m_strName; }
    void TranslateX( const float amount );
    void TranslateY( const float amount );
    void TranslateZ( const float amount );
    CLight* GetThis() { return this; }
    void ResetVirtualCoords();

private:
    static unsigned int m_UniqueID;
    CLASSTYPE m_eClassType;
    VECT  m_fVirt;
    float m_fPrecision,
          m_fBrightness;
    VECT  m_vColor;
    VECT  m_vPos;
    char  m_strName[256];
    GLUquadricObj *m_pQuadricObj;
};

#endif
