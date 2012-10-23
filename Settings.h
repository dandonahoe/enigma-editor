//----------------------------------------------------------------------------- 
// File: Settings.h
//
// Desc: Definition file for the CSettings class
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "Singleton.h"
#include "main.h"

#include "enum.h"


//-----------------------------------------------------------------------------
// Name: class CSettings 
// Desc: This class stores information about the current settings of the
//       application.
//-----------------------------------------------------------------------------
class CSettings : public CSingleton<CSettings>
{
public:
    CSettings();
    virtual ~CSettings();
    bool Save( const char* strFile = NULL );
    bool Open( const char* strFile = NULL  );

    // Brush Settings
    BRUSHTYPE m_eCurrentBrush;

    VECT  m_vBrush_Location;

    float m_fBrush_Sheet_USize;
    float m_fBrush_Sheet_VSize;
    int   m_iBrush_Sheet_Axis;

    float m_fBrush_Cube_Depth;
    float m_fBrush_Cube_Width;
    float m_fBrush_Cube_Height;

    float m_fBrush_Stairs_Height;
    float m_fBrush_Stairs_Depth;
    float m_fBrush_Stairs_Width;
    int   m_iBrush_Stairs_NumSteps;
    float m_fBrush_Stairs_AddToFirst;

    float m_fBrush_Spiral_Height;
    float m_fBrush_Spiral_Width;
    float m_fBrush_Spiral_InnerRadius;
    float m_fBrush_Spiral_AddToFirst;
    float m_fBrush_Spiral_Degrees;
    int   m_iBrush_Spiral_NumSteps;

    float m_fBrush_Cylinder_Height;
    float m_fBrush_Cylinder_Radius;
    int   m_iBrush_Cylinder_NumSides;

    float m_fBrush_Circle_Radius;
    float m_fBrush_Circle_Degrees;
    int   m_iBrush_Circle_NumSides;

    bool  m_bTextureTab_Texture1;
    bool  m_bTextureTab_Texture2;
    bool  m_bTextureTab_Lightmap;

    bool m_bEditor_UpdatePolys;
    bool m_bEditor_UpdateFaces;
    bool m_bEditor_UpdateVerts;
    bool m_bEditor_RenderNormals;
    bool m_bEditor_RenderBrush;
    bool m_bEditor_UseTexturing;
    bool m_bEditor_UseLighting;
    bool m_bEditor_RenderSelected;
    bool m_bEditor_RenderPolygons;
    bool m_bEditor_3DViewActive;
    bool m_bEditor_LeftViewActive;
    bool m_bEditor_CenterViewActive;
    bool m_bEditor_RightViewActive;
    bool m_bEditor_RenderLights;

    int   m_iLighting_LightmapLOD;
    float m_fLighting_LightIntensity;
    float m_fLighting_ReflectionIntensity;
    bool  m_bLighting_ShowHemicube;
    bool  m_bLighting_BestQuality;
    bool  m_bLighting_NormalQuality;
    int   m_fLighting_Threshold;
    VECT vF, vL, vR, vU, vD, vB;

    float m_fGeometry_MoveFactor;
    float m_fGeometry_ExpandAmount;
    int   m_iGeometry_ApplyTo;

    bool m_bSystem_DisplayErrorMessageBoxes;
};


#define Settings CSettings::GetSingletonPtr()
#define SE CSettings::GetSingletonPtr()



#endif
