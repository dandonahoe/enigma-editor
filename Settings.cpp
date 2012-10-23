//----------------------------------------------------------------------------- 
// File: Settings.cpp
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Include
#include <fstream>
#include "Settings.h"
#include "enum.h"
#include "Tracer.h"
//-----------------------------------------------------------------------------

using namespace std;
//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
CSettings::CSettings()
{
    // Window settings
    m_bEditor_3DViewActive     = true;
    m_bEditor_LeftViewActive   = false;
    m_bEditor_CenterViewActive = false;
    m_bEditor_RightViewActive  = false;

    // Brush settings
    m_eCurrentBrush             = NOTHINGBRUSH;

    // Brushes location
    m_vBrush_Location.x         = 0.0f;
    m_vBrush_Location.y         = 0.0f;
    m_vBrush_Location.z         = 0.0f;

    // Sheet size
    m_fBrush_Sheet_USize        = 1.0f;
    m_fBrush_Sheet_VSize        = 1.0f;
    m_iBrush_Sheet_Axis         = 1; // 1 = x, 2 = y, 3 = z

    // Cube size
    m_fBrush_Cube_Depth         = 1.0f;
    m_fBrush_Cube_Width         = 1.0f;
    m_fBrush_Cube_Height        = 1.0f;

    // Stair settings
    m_fBrush_Stairs_Height      = 0.25f;
    m_fBrush_Stairs_Depth       = 2.50f;
    m_fBrush_Stairs_Width       = 10.0f;
    m_fBrush_Stairs_AddToFirst  = 0.0f;
    m_iBrush_Stairs_NumSteps    = 20;

    // Spiral settings
    m_fBrush_Spiral_Height      = 0.25f;
    m_fBrush_Spiral_Width       = 10.0f;
    m_fBrush_Spiral_InnerRadius = 3.0f;
    m_fBrush_Spiral_AddToFirst  = 0.0f;
    m_fBrush_Spiral_Degrees     = 360.0f;
    m_iBrush_Spiral_NumSteps    = 20;

    // Cylinder settings
    m_fBrush_Cylinder_Height    = 2.0f;
    m_fBrush_Cylinder_Radius    = 1.0f;
    m_iBrush_Cylinder_NumSides  = 8;

    // Circle settings
    m_fBrush_Circle_Radius      = 1.0f;
    m_fBrush_Circle_Degrees     = 360.0f;
    m_iBrush_Circle_NumSides    = 8;

    // Texture Tab settings
    m_bTextureTab_Texture1      = true;
    m_bTextureTab_Texture2      = false;
    m_bTextureTab_Lightmap      = false;

    // Editor settings
    m_bEditor_UpdatePolys       = true;
    m_bEditor_UpdateFaces       = true;
    m_bEditor_UpdateVerts       = true;
    m_bEditor_RenderNormals     = true;
    m_bEditor_RenderBrush       = true;
    m_bEditor_UseTexturing      = true;
    m_bEditor_UseLighting       = true;
    m_bEditor_RenderSelected    = true;
    m_bEditor_RenderPolygons    = true;
    m_bEditor_RenderLights      = true;

    // Lighting settings
    m_iLighting_LightmapLOD         = 1;
    m_fLighting_LightIntensity      = 750.0f;
    m_fLighting_ReflectionIntensity = 3.0f;
    m_bLighting_ShowHemicube        = false;
    m_bLighting_BestQuality         = false;
    m_bLighting_NormalQuality       = true;
    m_fLighting_Threshold           = 32.0f;

    m_fGeometry_MoveFactor   = 0.05f;
    m_fGeometry_ExpandAmount = -1.0f;
    m_iGeometry_ApplyTo      = 1;

    m_bSystem_DisplayErrorMessageBoxes = true;
}

CSettings::~CSettings()
{
}


//-----------------------------------------------------------------------------
// Name: Save()
// Desc: Save this class to a file
//-----------------------------------------------------------------------------
bool CSettings::Save( const char* strFile )
{
//    Log( "CSettings::Save()\n" );

    FILE* fileDescriptor;

    if( strFile == NULL )
        fileDescriptor = fopen( "settings", "wb" );
    else
        fileDescriptor = fopen( strFile, "wb" );

    if( fileDescriptor )
    {
        fwrite( this, sizeof( CSettings ), 1, fileDescriptor );

        fclose( fileDescriptor );

        return true;
    }

    return false;
}


//-----------------------------------------------------------------------------
// Name: Open()
// Desc: Load this class from the settings file
//-----------------------------------------------------------------------------
bool CSettings::Open( const char* strFile )
{
    TRACE_ENTER_FN( CSettings::Open )

//    Log( "CSettings::Open()\n" );
/*
    FILE* fileDescriptor;

    if( strFile == NULL )
        fileDescriptor = fopen( "settings", "rb" );
    else
        fileDescriptor = fopen( strFile, "rb" );

    if( fileDescriptor )
    {
        fread( this, sizeof( CSettings ), 1, fileDescriptor );

        fclose( fileDescriptor );

        return true;
    }
*/
    TRACE_LEAVE_FN()

    return false;
}