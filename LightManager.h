//----------------------------------------------------------------------------- 
// File: LightManager.h
//
// Desc: Manages light data for the level
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


#ifndef _LIGHTIMANAGER_H_
#define _LIGHTIMANAGER_H_


//-----------------------------------------------------------------------------
// Includes
#include <vector>
#include "PolygonManager.h"
#include "Polygon.h"
#include "Light.h"
#include "Singleton.h"
#include "Window.h"
#include "Face.h"
#include "enum.h"
#include "Hemicube.h"
#include "LightmapManager.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: class CLightManager
// Desc: Manages all lighting information about the world. This is a singleton,
//       defined once in the application class's declaration file. To use it,
//       anywhere in the program, use 'LightManager->' or 'LM->' after including 
//       this file.
//-----------------------------------------------------------------------------
class CLightManager : public CSingleton<CLightManager>
{
public:
    CLightManager();
    virtual ~CLightManager();

    bool Init();
    bool Release();
    bool Render();

    bool CalculateLighting( const RADIOSITYMODE eMode );
    CHemicube* GetHemicube() { return &m_hemiCube; }
    bool AddLight( const int iP,
                   const int iF,
                   const VECT vC );
    bool RemoveLight();
    int GetLoadedLights() const { return m_iLoadedLights; }
    void RenderSelected();
    bool GetSelectedLightName( char* str );
    VECT GetSelectedLightColor();
    float GetSelectedLightBrightness();
    bool SetSelectedLightName( const char* str );
    bool SetSelectedLightColor( const VECT vC, const float fBrightness );
    void ResetVirtualCoords();
    bool TranslateLightX( const float fAmount );
    bool TranslateLightY( const float fAmount );
    bool TranslateLightZ( const float fAmount );

    bool Invalidate( LIGHTMAP* hTexture );

    bool Save( const char* strFile, ofstream& out );
    bool Open( const char* strFile, ifstream& in );

    unsigned int GetTextureGL( const int iIndex ) { return m_cLightmapManager.GetTextureGL( iIndex ); }
private:
    
    int GetTotalSteps();

    bool IncreaseLightmapDetail( const RADIOSITYMODE eMode );

    bool GetCameraDirections( CFace* pF,
                              const VECT vCenter,
                              VECT &vForward,
                              VECT &vLeft,
                              VECT &vRight,
                              VECT &vUp,
                              VECT &vDown,
                              VECT &vBack );

    bool SetPixelData( unsigned char* pData,
                       const int iPixel,
                       const int iRes,
                       const VECT vColor );

    bool CalculateThisLightmap( const int iP,
                                const int iF,
                                const int iTotalSteps,
                                const int iLoadedFaces,
                                const int iLoadedPolygons,
                                const bool bShowHemicube,
                                const HWND hTab,
                                int &iCurrentStep,
                                const RADIOSITYMODE eMode );

    bool CalculateThisPatch( const int iU,
                             const int iV,
                             const int iP,
                             const int iF,
                             const int iRes,
                             const int iTotalSteps,
                             const int iLoadedFaces,
                             const int iLoadedPolygons,
                             const bool bDisplayHemicube,
                             const HWND hTab,
                             CFace* pF,
                             unsigned char* pLightmap,
                             int &iCurrentStep,
                             VECT* vPositions,
                             const int iOffset,
                             const int iPixel );

    bool InterpolateLightmap( int iRow,
                              int iLeft,
                              int iRight,
                              const int iRes,
                              unsigned char* pLightmap );

    bool BestLighting( const int iP,
                       const int iF,
                       const int iTotalSteps,
                       const int iLoadedFaces,
                       const int iLoadedPolygons,
                       const bool bShowHemicube,
                       const HWND hTab,
                       const int iRes,
                       CFace* pF,
                       int &iCurrentStep,
                       unsigned char* pLightmap,
                       VECT* vPositions,
                       const int iLen  );

    bool FastLighting( const int iP,
                       const int iF,
                       const int iTotalSteps,
                       const int iLoadedFaces,
                       const int iLoadedPolygons,
                       const bool bShowHemicube,
                       const HWND hTab,
                       const int iRes,
                       CFace* pF,
                       int &iCurrentStep,
                       unsigned char* pLightmap );

    bool SetPixelColor( unsigned char* pLightmap,
                        const int iU,
                        const int iV,
                        const int iRes,
                        const VECT vC );

    VECT GetPixelColor( const unsigned char* pLightmap,
                        const int iU,
                        const int iV,
                        const int iRes );

    bool ComputeFastLighting();
public:
    int  GetRequiredArrayLength( const int iRes );

    bool GetPositionArray( CFace *pF,
                           const int iRes,
                           VECT* vPositions,
                           const int iLen );
private:
    bool GetCameraPosition( const int iU,
                            const int iV,
                            const int iRes,
                            CFace *pF,
                            CAMERAVIEW eView,
                            VECT &vFrom,
                            VECT &vAt,
                            VECT &vUp );

    CHemicube m_hemiCube;
    bool m_bInPhase1;
    CLight** m_pLights;
    int m_iLoadedLights;
    GLUquadricObj *m_pQuadricObj;

    CLightmapManager m_cLightmapManager;
};


#define LightManager CLightManager::GetSingletonPtr()
#define LM CLightManager::GetSingletonPtr()


#endif
