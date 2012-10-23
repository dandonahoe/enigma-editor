//----------------------------------------------------------------------------- 
// File: LightManager.cpp
//
// Desc: 
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include "LightManager.h"
#include "main.h"
#include "Window.h"
#include "Tracer.h"
#include "PolygonManager.h"
#include "Face.h"
#include "Settings.h"
#include "enum.h"
#include "Engine.h"
#include "Hemicube.h"
#include "Util.h"
#include "LightingTabDB.h"
#include "resource.h"
#include "MessageProcs.h"
//-----------------------------------------------------------------------------

using namespace std;

//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
CLightManager::CLightManager()
{
    TRACE_ENTER_FN( CLightManager::CLightManager )

    Log( "CLightManager::CLightManager()\n" );

    m_bInPhase1 = true;
    m_pLights = NULL;
    m_iLoadedLights = 0;

    TRACE_LEAVE_FN()
}

CLightManager::~CLightManager()
{
    TRACE_ENTER_FN( CLightManager::~CLightManager )

    Log( "CLightManager::~CLightManager()\n" );
    Release();

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: Init()
// Desc:
//-----------------------------------------------------------------------------
bool CLightManager::Init()
{
    TRACE_ENTER_FN( CLightManager::Init )

    if( !m_hemiCube.Init() )         LOG_ERR_RET( false )
    if( !m_cLightmapManager.Init() ) LOG_ERR_RET( false )
    if( m_iLoadedLights != 0 )       LOG_ERR_RET( false )

    m_pLights = new CLight*[ m_iLoadedLights + 1 ];

    if( m_pLights == NULL ) LOG_ERR_RET( false )

    m_pQuadricObj = gluNewQuadric();
    gluQuadricTexture( m_pQuadricObj, false );
    gluQuadricDrawStyle( m_pQuadricObj, GLU_FILL );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Release()
// Desc:
//-----------------------------------------------------------------------------
bool CLightManager::Release()
{
    TRACE_ENTER_FN( CLightManager::Release )

    Log( "CLightManager::Release()\n" );

    if( m_pLights != NULL )
    {
        for( int iA = 0; iA < m_iLoadedLights; iA++ )
        {
            SAFE_DELETE( m_pLights[iA] )
        }
    }

    delete[] m_pLights;

    if( m_pQuadricObj != NULL )
        gluDeleteQuadric( m_pQuadricObj );

    m_pQuadricObj = NULL;

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: CalculateLighting()
// Desc: Creates lightmaps for all faces using a radiosity method
//-----------------------------------------------------------------------------
bool CLightManager::CalculateLighting( const RADIOSITYMODE eMode )
{
    TRACE_ENTER_FN( CLightManager::CalculateLighting )

    if( eMode == BESTLIGHTING )
    {
        m_bInPhase1 = true;

        // Runs Phase 1, calculating lightsources
        if( !IncreaseLightmapDetail( eMode ) ) LOG_ERR_RET( false )

        m_bInPhase1 = false;

        // Runs Phase 2, calculating reflections
        for( int iLOD = 0; iLOD < SE->m_iLighting_LightmapLOD - 1; iLOD++ )
        {
            // Every call calculates another reflection, more = better
            if( !IncreaseLightmapDetail( eMode ) ) LOG_ERR_RET( false )
        }
    }
    else if( eMode == FASTLIGHTING )
    {
        if( !ComputeFastLighting() ) LOG_ERR_RET( false )
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: IncreaseLightmapDetail()
// Desc:
//-----------------------------------------------------------------------------
bool CLightManager::IncreaseLightmapDetail( const RADIOSITYMODE eMode )
{
    TRACE_ENTER_FN( CLightManager::CreateInitialLightmaps )

    const int iLoadedPolygons = PM->GetLoadedPolygons();
    int iTotalSteps = GetTotalSteps();
    int iCurrentStep = 0;
    HWND hTab = Engine->GetTabWnd();

    if( hTab == NULL ) LOG_ERR_RET( false )

    bool bShowHemicube = GetCheck( hTab, IDC_CHECK_SHOW_HEMICUBE );

    // Cycle through all faces of all polygons and compute the
    // lightmaps for all the faces
    for( int iP = 0; iP < iLoadedPolygons; iP++ )
    {

        const int iLoadedFaces = PM->P( iP )->GetLoadedFaces();

        for( int iF = 0; iF < iLoadedFaces; iF++ )
        {
            if( !CalculateThisLightmap( iP,
                                        iF,
                                        iTotalSteps,
                                        iLoadedFaces,
                                        iLoadedPolygons,
                                        bShowHemicube,
                                        hTab,
                                        iCurrentStep,
                                        eMode ) )
            {
                LOG_VAR( iP ) LOG_VAR( iF ) LOG_VAR( iTotalSteps )
                LOG_VAR( iLoadedFaces ) LOG_VAR( iLoadedPolygons )
                LOG_VAR( bShowHemicube ) LOG_VAR( iCurrentStep )
                LOG_VAR( (int)eMode ) LOG_ERR_RET( false )
            }
        }
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: AppendLightmapData()
// Desc: Adds this color information to the correct index. iU and iV are
//       like x, and y coordinates
//-----------------------------------------------------------------------------
bool CLightManager::SetPixelData( unsigned char* pData,
                                  const int iPixel,
                                  const int iRes,
                                  const VECT vColor )
{
    TRACE_ENTER_FN( CLightManager::AppendLightmapData )
/*
    const int iOffset = iU * iRes * 3 + iV * 3;

    try
    {
        pData[iOffset + 0] = vColor.x;
        pData[iOffset + 1] = vColor.y;
        pData[iOffset + 2] = vColor.z;
    }
    catch( ... ) { LOG_VAR( iOffset ) LOG_ERR_RET( false ) }
*/
    RET( true )
}


//-----------------------------------------------------------------------------
// Name: CalculateThisPatch();
// Desc: Calculates the amount of light reaching this area on the face.
//-----------------------------------------------------------------------------
bool CLightManager::CalculateThisPatch( const int iU,
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
                                        const int iPixel )
{
    TRACE_ENTER_FN( CLightManager::CalculateThisPatch )

    CAMERAVIEW eView;

    iCurrentStep++;

    IM->UpdateInput();

    if( IM->KeyPressed( DIK_ESCAPE ) ) {
        SAFE_DELETE( pLightmap ) RET( true ) }

    for( int iSide = 0; iSide < 5; iSide++ )
    {
        eView = GetViewEnumType( iSide );

        //Render this view of the hemicube
        if( !m_hemiCube.Render( vPositions[ iOffset + iSide * 3 + 0 ],
                                vPositions[ iOffset + iSide * 3 + 1 ],
                                vPositions[ iOffset + iSide * 3 + 2 ],
                                eView,
                                m_bInPhase1 ) )LOG_ERR_RET( false )
    }

    VECT vColor = m_hemiCube.GetColor( m_bInPhase1 );

    // Add the values of this patch to the lightmap itself
    if( !SetPixelData( pLightmap, iPixel, iRes, vColor ) ) LOG_ERR_RET( false )

    // Update the lighting window with the new stats
    UpdateWindowStats( hTab, iP, iF, iLoadedPolygons, iLoadedFaces,
                       iU, iV, iRes, iCurrentStep, iTotalSteps, vColor );

    if( bDisplayHemicube )
    {
        if( !DisplayHemiCube( hTab ) ) LOG_ERR_RET( false )
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: GetTotalSteps();
// Desc: Get the total number of patches that have to be computed
//-----------------------------------------------------------------------------
int CLightManager::GetTotalSteps()
{
    TRACE_ENTER_FN( CLightManager::GetTotalSteps )

    int iTotalSteps = 0;

    try
    {
        for( int iP = 0; iP < PM->GetLoadedPolygons(); iP++ )
        {
            for( int iF = 0; iF < PM->P( iP )->GetLoadedFaces(); iF++ )
            {
                int iRes = PM->F( iP, iF )->GetLightmapRes();
                iTotalSteps += iRes * iRes;
            }
        }
    }
    catch( ... )
    {
        Log( "catch( ... ) @ CLightManager::GetTotalSteps()\n" );
        TRACE_LEAVE_FN() return -1;
    }

    TRACE_LEAVE_FN()

    return iTotalSteps;
}


//-----------------------------------------------------------------------------
// Name: CalculateThisLightmap()
// Desc: It makes a lightmap for that face by caluclating the light reaching
//       all of the face's patches
//-----------------------------------------------------------------------------
bool CLightManager::CalculateThisLightmap( const int iP,
                                           const int iF,
                                           const int iTotalSteps,
                                           const int iLoadedFaces,
                                           const int iLoadedPolygons,
                                           const bool bShowHemicube,
                                           const HWND hTab,
                                           int &iCurrentStep,
                                           const RADIOSITYMODE eMode )
{
    TRACE_ENTER_FN( CLightManager::CalculateThisLightmap )

    // Get a pointer to the current face
    CFace* pF = PM->F( iP, iF );

    // Make sure it isn't null
    if( pF == NULL ) LOG_ERR_RET( false )

    // Make sure that were using quads (triangles arent implemented yet)
    if( pF->GetFaceType() != TRIANGLEFACE ) LOG_ERR_RET( false )

    // Get the Resolution of the lightmap for this face
    const iRes = 8;//pF->GetLightmapRes();

    // iLen is the number of patches on this lightmap
    //const int iLen = iRes * iRes * 3;
    
    const int iLen = ( GetRequiredArrayLength( iRes ) / 3 ) / 5;

    const int iPosArrayLen = GetRequiredArrayLength( iRes );

    if( iPosArrayLen == 0 ) LOG_ERR_RET( false )

    VECT* vPositions = new VECT[ iPosArrayLen ];

    // Allocate space to hold the lightmap data while being computed
    unsigned char* pLightmap = new unsigned char[ iLen ];

    // Make sure it was created
    if( pLightmap == NULL ) LOG_ERR_RET( false )

    for( int iA = 0; iA < iLen; iA++ )
        pLightmap[ iA ] = 0;

    // Now compue the lightmap using whichever mode was selected
    switch( eMode )
    {
        case BESTLIGHTING:
        {
            if( !BestLighting( iP,
                               iF,
                               iTotalSteps,
                               iLoadedFaces,
                               iLoadedPolygons,
                               bShowHemicube,
                               hTab,
                               iRes,
                               pF,
                               iCurrentStep,
                               pLightmap,
                               vPositions,
                               iPosArrayLen ) )
            {
                LOG_VAR( iP ) LOG_VAR( iF ) LOG_VAR( iTotalSteps ) LOG_VAR( iLoadedFaces )
                LOG_VAR( iLoadedPolygons ) LOG_VAR( bShowHemicube ) LOG_VAR( iRes )
                LOG_VAR( iPosArrayLen ) LOG_VAR( iCurrentStep )
                SAFE_DELETE( pLightmap ) delete[] vPositions; LOG_ERR_RET( false )
            }
        }
        break;

        case FASTLIGHTING:
        {
            if( !FastLighting( iP,
                               iF,
                               iTotalSteps,
                               iLoadedFaces,
                               iLoadedPolygons,
                               bShowHemicube,
                               hTab,
                               iRes,
                               pF,
                               iCurrentStep,
                               pLightmap ) )
            {
                LOG_VAR( iP ) LOG_VAR( iF ) LOG_VAR( iTotalSteps ) LOG_VAR( iLoadedFaces )
                LOG_VAR( iLoadedPolygons ) LOG_VAR( bShowHemicube ) LOG_VAR( iRes )
                LOG_VAR( iCurrentStep ) SAFE_DELETE( pLightmap ) delete[] vPositions; LOG_ERR_RET( false )
            }
        }
        break;

        default:
        {
            delete[] vPositions;
            SAFE_DELETE( pLightmap )
            LOG_ERR_RET( false )
        }
        break;
    }

    delete[] vPositions;
    // Apply the lightmap data to the face
//    pF->CreateLightmap( iRes, pLightmap );

    SAFE_DELETE( pLightmap )

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: 
//-----------------------------------------------------------------------------
bool CLightManager::Render()
{
    TRACE_ENTER_FN( CLightManager::AddLight )

    glDisable( GL_TEXTURE_2D );

    for( int iA = 0; iA < GetLoadedLights(); iA++ )
    {
        Lookup->StartNaming();

        Lookup->Include( NA, NA, NA, iA, false, false, true );

        m_pLights[iA]->Render();

        Lookup->EndNaming();
    }

    glEnable( GL_TEXTURE_2D );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: RenderSelected()
// Desc: Draws the red rings around the selected light so that the color
//       of the origional light can always be seen
//-----------------------------------------------------------------------------
void CLightManager::RenderSelected()
{
    TRACE_ENTER_FN( CLightManager::RenderSelected )

    int iL = Lookup->GetLight();

    try
    {
        Lookup->StartNaming();

        Lookup->Include( NA, NA, NA, iL, false, false, true );

        VECT vP = m_pLights[iL]->GetPos();

        glColor3f( 1.0f, 0, 0 );

        glPushMatrix();
        glTranslatef( vP.x, vP.y, vP.z );
        gluDisk( m_pQuadricObj, 0.1f, .125f, 16, 1 );

        glRotatef( 90, 0, 1, 0 );
        gluDisk( m_pQuadricObj, 0.1f, .125f, 16, 1 );

        glRotatef( 90, 1, 0, 0 );
        gluDisk( m_pQuadricObj, 0.1f, .125f, 16, 1 );

        glPopMatrix();

        Lookup->EndNaming();
    }
    catch( ... )
    {
        Log( "catch( ... ) @ CLightManager::RenderSelected()\n" );
        Lookup->EndNaming();
        TRACE_LEAVE_FN() return;
    }

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: GetSelectedLightName()
// Desc: 
//-----------------------------------------------------------------------------
bool CLightManager::GetSelectedLightName( char* str )
{
    TRACE_ENTER_FN( CLightManager::GetSelectedLightName )

    try
    {
        strcpy( str, m_pLights[Lookup->GetLight()]->GetName() );
    }
    catch( ... )
    {
        Log( "catch( ... ) @ CLightManager::GetSelectedLightName()\n" );
        TRACE_LEAVE_FN() return false;
    }

    if( str == NULL ) {
        Log( "if( str == NULL ) @ CLightManager::GetSelectedLightName()\n" );
        TRACE_LEAVE_FN() return false; }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: GetSelectedLightColor()
// Desc: 
//-----------------------------------------------------------------------------
VECT CLightManager::GetSelectedLightColor()
{
    TRACE_ENTER_FN( CLightManager::GetSelectedLightColor )

    VECT vC( 0, 0, 0 );
    
    try
    {
        vC = m_pLights[Lookup->GetLight()]->GetColor();
    }
    catch( ... )
    {
        Log( "catch( ... ) @ CLightManager::GetSelectedLightName()\n" );
        TRACE_LEAVE_FN() return VECT( 0, 0, 0 );
    }

    TRACE_LEAVE_FN()

    return vC;
}

//-----------------------------------------------------------------------------
// Name: GetSelectedLightBrightness()
// Desc: 
//-----------------------------------------------------------------------------
float CLightManager::GetSelectedLightBrightness()
{
    TRACE_ENTER_FN( CLightManager::GetSelectedLightBrightness )

    float fBrightness;

    try
    {
        fBrightness = m_pLights[Lookup->GetLight()]->GetBrightness();
    }
    catch( ... )
    {
        Log( "catch( ... ) @ CLightManager::GetSelectedLightBrightness()\n" );
        TRACE_LEAVE_FN() return 0.0f;
    }

    TRACE_LEAVE_FN()

    return fBrightness;
}



//-----------------------------------------------------------------------------
// Name: SetSelectedLightName()
// Desc: 
//-----------------------------------------------------------------------------
bool CLightManager::SetSelectedLightName( const char* str )
{
    TRACE_ENTER_FN( CLightManager::SetSelectedLightName )

    if( str == NULL ) {
        Log( "if( str == NULL ) @ CLightManager::SetSelectedLightName()\n" );
        TRACE_LEAVE_FN() return false; }

    try
    {
        m_pLights[Lookup->GetLight()]->SetName( str );
    }
    catch( ... )
    {
        Log( "catch( ... ) @ CLightManager::GetSelectedLightName()\n" );
        TRACE_LEAVE_FN() return false;
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: SetSelectedLightColor()
// Desc: 
//-----------------------------------------------------------------------------
bool CLightManager::SetSelectedLightColor( const VECT vC, const float fBrightness )
{
    TRACE_ENTER_FN( CLightManager::SetSelectedLightColor )
    
    if( Lookup->GetLight() == -1 ) { TRACE_LEAVE_FN() return true; }

    try
    {
        m_pLights[Lookup->GetLight()]->SetColor( vC );
        m_pLights[Lookup->GetLight()]->SetBrightness( fBrightness );
    }
    catch( ... )
    {
        Log( "catch( ... ) @ CLightManager::SetSelectedLightColor()\n" );
        Log( "Lookup->GetLight(): " ); Log( Lookup->GetLight() ); Log( "\n" );
        TRACE_LEAVE_FN() return false;
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: ResetVirtualCoords()
// Desc: 
//-----------------------------------------------------------------------------
void CLightManager::ResetVirtualCoords()
{
    TRACE_ENTER_FN( CLightManager::ResetVirtualCoords )

    if( !Lookup->LightSelected() ) { TRACE_LEAVE_FN() return; }

    for( int iL = 0; iL < GetLoadedLights(); iL++ )
        m_pLights[iL]->ResetVirtualCoords();

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: TranslateLightX()
// Desc: 
//-----------------------------------------------------------------------------
bool CLightManager::TranslateLightX( const float fAmount )
{
    TRACE_ENTER_FN( CLightManager::TranslateLightX )

    if( !Lookup->LightSelected() ) { TRACE_LEAVE_FN() return false; }

    m_pLights[Lookup->GetLight()]->TranslateX( fAmount );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: TranslateLightY()
// Desc: 
//-----------------------------------------------------------------------------
bool CLightManager::TranslateLightY( const float fAmount )
{
    TRACE_ENTER_FN( CLightManager::TranslateLightY )

    if( !Lookup->LightSelected() ) { TRACE_LEAVE_FN() return false; }

    m_pLights[Lookup->GetLight()]->TranslateY( fAmount );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: TranslateLightZ()
// Desc: 
//-----------------------------------------------------------------------------
bool CLightManager::TranslateLightZ( const float fAmount )
{
    TRACE_ENTER_FN( CLightManager::TranslateLightZ )

    if( !Lookup->LightSelected() ) { TRACE_LEAVE_FN() return false; }

    m_pLights[Lookup->GetLight()]->TranslateZ( fAmount );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: AddLight()
// Desc: 
//-----------------------------------------------------------------------------
bool CLightManager::AddLight( const int iP,  const int iF, const VECT vC )
{
    TRACE_ENTER_FN( CLightManager::AddLight )

    CFace* pF = PM->F( iP, iF );

    if( pF == NULL ) {
        Log( "if( pF == NULL ) @ CLightManager::AddLight()\n" );
        TRACE_LEAVE_FN() return false; }

    // Create a temporary holder array for the polygon pointers
    CLight** pTemp = NULL;

    // Initalize the holder
    pTemp = new CLight*[m_iLoadedLights + 1];

    // Make sure it was created
    if( pTemp == NULL ) Log( "if( pTemp == NULL ) @ CLightManager::AddLight()" );

    // Copy all polygons into the temp
    for( int a = 0; a < m_iLoadedLights; a++ ) pTemp[a] = m_pLights[a];

    // Delete the old polygon holder if any polygons were ever in it
    if( m_pLights != NULL ) { delete[] m_pLights; m_pLights = NULL; }

    // Increase the size of the polygon array to 1 larger
    m_pLights = new CLight*[m_iLoadedLights + 1];

    // Make sure it was created
    if( m_pLights == NULL ) Log( "if( m_pLights == NULL ) @ CLightManager::AddLight()\n" );

    // Copy the temp back into the real one
    for( a = 0; a < m_iLoadedLights; a++ ) m_pLights[a] = pTemp[a];

    // Delete the temp holder
    if( pTemp != NULL ) delete[] pTemp;

    CLight* pLight = new CLight;

    if( pLight == NULL ) Log( "if( pLight == NULL ) @ CLightManager::AddLight()\n" );

    // Put the newly created polygon at the end of the list
    m_pLights[m_iLoadedLights] = pLight;

    m_pLights[GetLoadedLights()]->SetPos( pF->GetNormal() / 2 + pF->GetCenter() );
    m_pLights[GetLoadedLights()]->SetColor( 1.0f, 1.0f, 1.0f );

    m_iLoadedLights++;

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: AddLight()
// Desc: 
//-----------------------------------------------------------------------------
bool CLightManager::RemoveLight()
{
    TRACE_ENTER_FN( CLightManager::RemoveLight )

    int iIndex = 0;

    if( GetLoadedLights() <= 0 ) {
        Log( "if( GetLoadedLights() <= 0 ) @ CLightManager::RemoveLight()\n" );
        TRACE_LEAVE_FN() return true; }

    // Make sure a polygon is selected
    if( !Lookup->LightSelected() ) {
        Log( "if( !Lookup->LightSelected() ) @ CLightManager::RemoveLight()\n" );
        TRACE_LEAVE_FN() return true;}

    // Get the index of the polygon to remove
    int iL = Lookup->GetLight();

    // If its already gone, skip deletion
    if( m_pLights[iL] != NULL )
    {
        delete m_pLights[iL];
        m_pLights[iL] = NULL;
    }

    CLight** pTemp = new CLight*[GetLoadedLights()];

    // See if our temporary list was created alright
    if( pTemp == NULL ) {
        Log( "if( pTemp == NULL ) @ CLightManager::RemoveLight()\n" );
        TRACE_LEAVE_FN() return false; }

    for( iIndex = 0; iIndex < GetLoadedLights(); iIndex++ )
        pTemp[iIndex] = NULL;

    iIndex = 0;

    // Copy the non null polygons into a temporary list, without gaps
    for( iL = 0; iL < GetLoadedLights(); iL++ )
    {
        if( m_pLights[iL] == NULL ) continue;

        pTemp[iIndex] = m_pLights[iL];
        iIndex++;
    }

    // Delete the origional list
    if( m_pLights != NULL ) { delete[] m_pLights; m_pLights = NULL; }

    m_iLoadedLights--;

    if( GetLoadedLights() <= 0 ) {
        Lookup->ResetLookupTable();
        TRACE_LEAVE_FN() return false; }

    // Create a new list, one less in length
    m_pLights = new CLight*[GetLoadedLights()];

    if( m_pLights == NULL ) {
        Log( "if( m_pLights == NULL ) @ CLightManager::RemoveLight()\n" );
        TRACE_LEAVE_FN() return false; }

    // Copy the temporary list to the real one
    for( iL = 0; iL < GetLoadedLights(); iL++ )
        m_pLights[iL] = pTemp[iL];

    Lookup->ResetLookupTable();

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: GetPixelColor()
// Desc: 
//-----------------------------------------------------------------------------
VECT CLightManager::GetPixelColor( const unsigned char* pLightmap,
                                   const int iU,
                                   const int iV,
                                   const int iRes )
{
    VECT vC( 0, 0, 0 );

    TRACE_ENTER_FN( CLightManager::GetPixelColor )

    const int iOffset = iU * iRes * 3 + iV * 3;

    try
    {
        vC.x = pLightmap[iOffset + 0];
        vC.y = pLightmap[iOffset + 1];
        vC.z = pLightmap[iOffset + 2];
    }
    catch( ... )
    {
        Log( "catch( ... ) @ CLightManager::GetPixelColor\n" );
        TRACE_LEAVE_FN() return VECT( 0, 0, 0 );
    }

    TRACE_LEAVE_FN()

    return vC;
}


//-----------------------------------------------------------------------------
// Name: SetPixelColor()
// Desc: 
//-----------------------------------------------------------------------------
bool CLightManager::SetPixelColor( unsigned char* pLightmap,
                                   const int iU,
                                   const int iV,
                                   const int iRes,
                                   const VECT vC )
{
    TRACE_ENTER_FN( CLightManager::SetPixelColor )

    const int iOffset = iU * iRes * 3 + iV * 3;

    try
    {
        pLightmap[iOffset + 0] = vC.x;
        pLightmap[iOffset + 1] = vC.y;
        pLightmap[iOffset + 2] = vC.z;
    }
    catch( ... )
    {
        Log( "catch( ... ) @ CLightManager::SetPixelColor\n" );
        TRACE_LEAVE_FN() return false;
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: BestLighting()
// Desc: 
//-----------------------------------------------------------------------------
bool CLightManager::BestLighting( const int iP,
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
                                  const int iLen )
{
    TRACE_ENTER_FN( CLightManager::BestLighting )

    int iOffset = 0;
    int iPixel = 0;

    if( vPositions == NULL ) LOG_ERR_RET( false )

    if( !GetPositionArray( pF, iRes, vPositions, iLen ) ) LOG_ERR_RET( false )

    LOG_VAR( iLen )

    for( iOffset = 0; iOffset < iLen; iOffset += 15 )
    {
        // Cycle across the texture
        if( !CalculateThisPatch( iOffset,
                                 -1,
                                 iP,
                                 iF,
                                 iRes,
                                 iTotalSteps,
                                 iLoadedFaces,
                                 iLoadedPolygons,
                                 bShowHemicube,
                                 hTab,
                                 pF,
                                 pLightmap,
                                 iCurrentStep,
                                 vPositions,
                                 iOffset,
                                 iPixel ) )
        {
            LOG_VAR( iP ) LOG_VAR( iF )
            LOG_VAR( iRes ) LOG_VAR( iTotalSteps ) LOG_VAR( iLoadedFaces )
            LOG_VAR( iLoadedPolygons ) LOG_VAR( bShowHemicube )
            LOG_VAR( iCurrentStep ) SAFE_DELETE( pLightmap ) LOG_ERR_RET( false )
        }

        iPixel++;
    }


    RET( true )
}


//-----------------------------------------------------------------------------
// Name: FastLighting()
// Desc: 
//-----------------------------------------------------------------------------
bool CLightManager::FastLighting( const int iP,
                                  const int iF,
                                  const int iTotalSteps,
                                  const int iLoadedFaces,
                                  const int iLoadedPolygons,
                                  const bool bShowHemicube,
                                  const HWND hTab,
                                  const int iRes,
                                  CFace* pF,
                                  int &iCurrentStep,
                                  unsigned char* pLightmap )
{
    TRACE_ENTER_FN( CLightManager::FastLighting )
/*
    int iLeft = 0;
    int iRight = iRes - 1;

    for( int iRow = 0; iRow < iRes; iRow++ )
    {
        if( !CalculateThisPatch( iRow, iLeft, iP, iF, iRes, iTotalSteps, iLoadedFaces,
                                 iLoadedPolygons, bShowHemicube, hTab, pF, pLightmap, iCurrentStep ) ) {
        Log( "if( !CalculateThisPatch() @ CLightManager::FastLighting()\n" );
        SAFE_DELETE( pLightmap )
        TRACE_LEAVE_FN() return false; }

        if( !CalculateThisPatch( iRow, iRight, iP, iF, iRes, iTotalSteps, iLoadedFaces,
                                 iLoadedPolygons, bShowHemicube, hTab, pF, pLightmap, iCurrentStep ) ) {
        Log( "if( !CalculateThisPatch() @ CLightManager::FastLighting()\n" );
        SAFE_DELETE( pLightmap )
        TRACE_LEAVE_FN() return false; }

        InterpolateLightmap( iRow, iLeft, iRight, iRes, pLightmap );
    }
*/
    RET( true )
}


//-----------------------------------------------------------------------------
// Name: InterpolateLightmap()
// Desc: 
//-----------------------------------------------------------------------------
bool CLightManager::InterpolateLightmap( int iRow,
                                         int iLeft,
                                         int iRight,
                                         const int iRes,
                                         unsigned char* pLightmap )
{

    const float fThreshold = SE->m_fLighting_Threshold;
    const VECT vLC         = GetPixelColor( pLightmap, iRow, iLeft,  iRes );
    const VECT vRC         = GetPixelColor( pLightmap, iRow, iRight, iRes );
    const float fLTotal    = vLC.x + vLC.y + vLC.z;
    const float fRTotal    = vRC.x + vRC.y + vRC.z;

    if( iLeft == iRight )
    {
        return true;
    }

    // Compute the pixel if the change innthe left and right is too big
    if( (float)fabs( fLTotal - fRTotal ) > fThreshold )
    {
    }

    if( !SetPixelColor( pLightmap, iRow, iRight / 2, iRes, ( vLC + vRC ) / 2.0f ) ) {
        Log( "if( !SetPixelColor() @ CLightManager::InterpolateLightmap()\n" ); return false; }

    InterpolateLightmap( iRow, iLeft, iRight / 2, iRes, pLightmap );
    InterpolateLightmap( iRow, iLeft, iRight, iRes, pLightmap );

    return true;
}


//-----------------------------------------------------------------------------
// Name: ComputeFastLighting()
// Desc: 
//-----------------------------------------------------------------------------
bool CLightManager::ComputeFastLighting()
{
    TRACE_ENTER_FN( CLightManager::ComputeFastLighting )

    const int iNumPolys  = PM->GetLoadedPolygons();
    const int iNumLights = m_iLoadedLights;
    int iP, iF, iV;


    for( iP = 0; iP < iNumPolys; iP++ )
    {
        const int iNumFaces = PM->P( iP )->GetLoadedFaces();

        for( iF = 0; iF < iNumFaces; iF++ )
        {
            const int iNumVerts = PM->F( iP, iF )->GetLoadedVerts();

            for( iV = 0; iV < iNumVerts; iV++ )
            {
                PM->V( iP, iF, iV )->SetColor( VECT( -1.0f, -1.0f, -1.0f ) );
            }
        }
    }

    for( iP = 0; iP < iNumPolys; iP++ )
    {
        const int iNumFaces = PM->P( iP )->GetLoadedFaces();

        for( iF = 0; iF < iNumFaces; iF++ )
        {
            const int iNumVerts = PM->F( iP, iF )->GetLoadedVerts();

            for( iV = 0; iV < iNumVerts; iV++ )
            {
                CVertex* pV = PM->V( iP, iF, iV );

                if( pV->GetColor() != VECT( -1.0f, -1.0f, -1.0f ) )
                    break;

                const VECT vVPos = pV->GetVertPos();
                VECT vSum( 0.0f, 0.0f, 0.0f );
                
                for( int iL = 0; iL < iNumLights; iL++ )
                {
                    
                    CLight* pL              = m_pLights[iL];
                    const VECT  vLPos       = pL->GetPos();
                    const VECT  vC          = pL->GetColor();
                    const float fBrightness = pL->GetBrightness();
                    VECT vTemp              = vVPos - vLPos;
                    const float fDist       = sqrtf( vTemp.x * vTemp.x +
                                                     vTemp.y * vTemp.y +
                                                     vTemp.z * vTemp.z );

                    vSum += ( fBrightness / ( fDist * fDist ) ) * vC;
                }

                if( vSum.x > 255.0f ) vSum.x = 255.0f;
                if( vSum.y > 255.0f ) vSum.y = 255.0f;
                if( vSum.z > 255.0f ) vSum.z = 255.0f;

                pV->SetColor( vSum );

            }
        }
    }

    RET( true )
}

//-----------------------------------------------------------------------------
// Name: Invalidate()
// Desc: 
//-----------------------------------------------------------------------------
bool CLightManager::Invalidate( LIGHTMAP* hTexture )
{
    TRACE_ENTER_FN( CLightManager::Invalidate )

    RET( m_cLightmapManager.Invalidate( hTexture ) )
}


//-----------------------------------------------------------------------------
// Name: Save()
// Desc: 
//-----------------------------------------------------------------------------
bool CLightManager::Save( const char* strFile, ofstream& out )
{
    TRACE_ENTER_FN( CLightManager::Save )

    if( !m_cLightmapManager.Save( strFile, out ) ) LOG_ERR_RET( false )

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Open()
// Desc: 
//-----------------------------------------------------------------------------
bool CLightManager::Open( const char* strFile, ifstream& in )
{
    TRACE_ENTER_FN( CLightManager::Open )

    if( !m_cLightmapManager.Open( strFile, in ) ) LOG_ERR_RET( false )

    RET( true )
}



//-----------------------------------------------------------------------------
// Name: GetRequiredArrayLength()
// Desc: 
//-----------------------------------------------------------------------------
int CLightManager::GetRequiredArrayLength( const int iRes )
{
    TRACE_ENTER_FN( CLightManager::GetRequiredArrayLength() )

    int iSum = 0;

    // The number of pixels describing this lightmap
    for( int a = 0; a < iRes; a++ ) iSum += a + 1;

    // 5 sides ( up, down, left, right, forward )
    iSum *= 5;

    // 3 components ( from, at, up )
    iSum *= 3;

    RET( iSum )
}


//-----------------------------------------------------------------------------
// Name: GetPositionArray()
// Desc: 
//-----------------------------------------------------------------------------
bool CLightManager::GetPositionArray( CFace *pF,
                                      const int iRes,
                                      VECT* vPositions,
                                      const int iLen )
{
    TRACE_ENTER_FN( CLightManager::GetPositionArray() )

    int iOffset = 0,
        iSide,
        iRow = 0,
        iCol = 0;

    int iSideOffset = 0;

    int iOff2 = 0;

    VECT vFrom1, vAt1, vUp1,
         vFrom2, vAt2, vUp2,
         vFrom,  vAt,  vUp,
         vIncrement, vNormal;

    VECT vOrientations[ 15 ];
    CAMERAVIEW eView;

    // Makes sure that the array length is what it's supposed to be
    if( iLen != GetRequiredArrayLength( iRes ) ) 
    { LOG_VAR( iLen ) LOG_VAR( GetRequiredArrayLength( iRes ) ) LOG_ERR_RET( false ) }

    vNormal = pF->GetFaceNormal() / 10.0f;

    LOG_VAR( iLen )

    for( iSide = 0; iSide < 5; iSide++ )
    {
        // Get the enum value for this side
        eView = GetViewEnumType( iSide );

        // Get the far left position in this row
        if( !GetCameraPosition( 0, 0,        iRes, pF, eView, vFrom1, vAt1, vUp1 ) ) {
            LOG_VAR( iRes ) LOG_VAR( iLen ) LOG_VAR( iOffset ) LOG_VAR( iSide )
            LOG_VAR( iRow ) LOG_VAR( iCol ) LOG_ERR_RET( false ) }

        vOrientations[ iSideOffset + 0 ] = VECT( 0, 0, 0 );
        vOrientations[ iSideOffset + 1 ] = ( vAt1 - vFrom1 ) * 0.33333333f;
        vOrientations[ iSideOffset + 2 ] = vUp1 * 0.3333f;

        iSideOffset += 3;
    }

    for( iRow = 0; iRow < iRes; iRow++ )
    {
        if( iOffset > iLen )
        {
            LOG_VAR( iOffset )
            LOG_VAR( iLen )
            LOG_VAR( iRow )
            LOG_VAR( iSide )
            LOG_ERR_RET( false )
        }

        // Get the far left position in this row
        if( !GetCameraPosition( iRow, 0,        iRes, pF, FORWARDVIEW, vFrom1, vAt1, vUp1 ) ) {
            LOG_VAR( iRes ) LOG_VAR( iLen ) LOG_VAR( iOffset ) LOG_VAR( iSide )
            LOG_VAR( iRow ) LOG_VAR( iCol ) LOG_ERR_RET( false ) }

        // Get the far right position in this row
        if( !GetCameraPosition( iRow, iRes - 1, iRes, pF, FORWARDVIEW, vFrom2, vAt2, vUp2 ) ) {
            LOG_VAR( iRes ) LOG_VAR( iLen ) LOG_VAR( iOffset ) LOG_VAR( iSide )
            LOG_VAR( iRow ) LOG_VAR( iCol ) LOG_ERR_RET( false ) }

        // Determines the space between the camera positions in this row
        vIncrement = ( vFrom2 - vFrom1 ) / ( iRow + 1 );

        vIncrement += vIncrement / ( iRow + 0.5f );
        vFrom = vFrom1;
        vAt   = vAt1;
        vUp   = vUp;

        for( iCol = 0; iCol < iRow + 1; iCol++ )
        {
            for( int a = 0; a < 15; a += 3 )
            {
                vFrom = ( vFrom2 - vIncrement * iCol );// + vNormal;

                vPositions[ iOffset + a + 0 ] = vFrom + vOrientations[ a + 0 ];
                vPositions[ iOffset + a + 1 ] = vFrom + vOrientations[ a + 1 ];
                vPositions[ iOffset + a + 2 ] = vOrientations[ a + 2 ];

                iOffset += 3;
            }
        }   
    }
/*
    vPositions[ 0 ] = ;
    vPositions[ 1 ] = ;
    vPositions[ 2 ] = ;
    vPositions[ 3 ] = ;
    vPositions[ 4 ] = ;
*/
    RET( true )
}


//-----------------------------------------------------------------------------
// Name: GetCameraOrientation()
// Desc: 
//-----------------------------------------------------------------------------
bool CLightManager::GetCameraPosition( const int iU,
                                       const int iV,
                                       const int iRes,
                                       CFace *pF,
                                       CAMERAVIEW eView,
                                       VECT &vFrom,
                                       VECT &vAt,
                                       VECT &vUp )
{
    TRACE_ENTER_FN( CLightManager::DrawLines() )

    VECT vN, vStart, vEnd, vStep,
         vF, vL, vR, vU, vD, vB;

    VECT vP[ 4 ];
    VECT vStepLen[ 2 ];

    // Check if it is a power of 2
    if( iRes & ( iRes - 1 ) ) { LOG_VAR( iRes ) LOG_ERR_RET( false ) }

    // Make sure valid coordinates were picked
    if( iU >= iRes || iV >= iRes  || iU < 0 || iV < 0 ) {
        LOG_VAR( iU ) LOG_VAR( iV ) LOG_VAR( iRes ) LOG_ERR_RET( false ) }

    // Is the face being acted on null
    if( pF == NULL ) LOG_ERR_RET( false )

    // For now must be a triangle face
    if( pF->GetFaceType() != TRIANGLEFACE ) 
    {
        LOG_VAR( pF->GetFaceType() )
        LOG_ERR_RET( false )
    }

    // Get the normal of the face
    vN = pF->GetNormal();

    // Get a list of all verts of the face
    vP[ 0 ] = pF->V( 0 )->GetVertPos();
    vP[ 1 ] = pF->V( 1 )->GetVertPos();
    vP[ 2 ] = pF->V( 2 )->GetVertPos();
    vP[ 3 ] = pF->V( 3 )->GetVertPos();

    VECT temp = ( ( vP[ 1 ] + vP[ 2 ] ) / 2.0f - vP[ 0 ] );

    vP[ 3 ] = ( ( vP[ 1 ] + vP[ 2 ] ) / 2.0f ) - temp;

    // Divides up the lines by the iRes amount
    vStepLen[ 0 ] = ( vP[ 1 ] - vP[ 0 ] ) / iRes;
    vStepLen[ 1 ] = ( vP[ 3 ] - vP[ 2 ] ) / iRes;

    // This line goes across the polygon, and somewhere on this line the camera
    // will be positioned
    vStart = vP[ 0 ] + ( vStepLen[ 0 ] * iU            ) + ( vStepLen[ 0 ] / 2 );
    vEnd   = vP[ 2 ] + ( vStepLen[ 1 ] * ( iRes - iU ) ) - ( vStepLen[ 1 ] / 2 );

    // Divide that line up so we can figure out where to put the camera
    vStep = ( vEnd - vStart ) / iRes;

    // Move down that line until the position for the camera is found
    vFrom.x = vStart.x + vStep.x * iV + ( vStep.x / 2 );
    vFrom.y = vStart.y + vStep.y * iV + ( vStep.y / 2 );
    vFrom.z = vStart.z + vStep.z * iV + ( vStep.z / 2 );

    // Gets all the locations needed to calculate 
    if( !GetCameraDirections( pF, vFrom, vF, vL, vR, vU, vD, vB ) ) LOG_ERR()

    // Depending on which side of the hemicube is being rendered, turn the
    // camera to the appropriate direction
    switch( eView )
    {
        case FORWARDVIEW: { vAt = vF;         vUp = vU;         } break;
        case LEFTVIEW:    { vAt = vL;         vUp = vU;         } break;
        case RIGHTVIEW:   { vAt = vR;         vUp = vU;         } break;
        case UPVIEW:      { vAt = vU + vFrom; vUp = vB - vFrom; } break;
        case DOWNVIEW:    { vAt = vD;         vUp = vF - vFrom; } break;
        default:          { LOG_ERR()                           } break;
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: GetCameraDirections()
// Desc: 
//-----------------------------------------------------------------------------
bool CLightManager::GetCameraDirections( CFace* pF,
                                         const VECT vCenter,
                                         VECT &vForward,
                                         VECT &vLeft,
                                         VECT &vRight,
                                         VECT &vUp,
                                         VECT &vDown,
                                         VECT &vBack )
{
    TRACE_ENTER_FN( CLightManager::GetCameraDirections() )

    if( pF == NULL ) LOG_ERR_RET( false )

    const VECT vNormal = pF->Normalize( pF->GetNormal() );
    VECT v1, v2, v3;


    // Get some starting information
    v1 = pF->V( 0 )->GetVertPos();

    //Front
    vForward = vNormal;

    // Back
    vBack = -vNormal;

    // Left
    vLeft = v1 - vCenter;

    // Right
    vRight = -vLeft;

    // Up
    vUp = pF->GetCrossProduct( vForward, vLeft );

    // Down
    vDown = -vUp;

    vForward = pF->Normalize( vForward );
    vBack    = pF->Normalize( vBack    );
    vLeft    = pF->Normalize( vLeft    );
    vRight   = pF->Normalize( vRight   );
    vDown    = pF->Normalize( vDown    );
    vUp      = pF->Normalize( vUp      );
    
    vForward += vCenter;
    vBack    += vCenter;
    vLeft    += vCenter;
    vRight   += vCenter;
    vDown    += vCenter;

    RET( true )
}

/*
//-----------------------------------------------------------------------------
// Name: GetPositionArray()
// Desc: 
//-----------------------------------------------------------------------------
bool CLightManager::GetPositionArray( CFace *pF,
                                      const int iRes,
                                      VECT* vPositions,
                                      const int iLen )
{
    TRACE_ENTER_FN( CLightManager::GetPositionArray() )

    int iOffset = 0,
        iSide,
        iRows,
        iCol;

    VECT vFrom1, vAt1, vUp1,
         vFrom2, vAt2, vUp2,
         vFrom,  vAt,  vUp,
         vIncrement;

    CAMERAVIEW eView;

    // Makes sure that the array length is what it's supposed to be
    if( iLen != GetRequiredArrayLength( iRes ) ) 
    { LOG_VAR( iLen ) LOG_VAR( GetRequiredArrayLength( iRes ) ) LOG_ERR_RET( false ) }

    // Get each view from this coordinate on the face
    for( iSide = 0; iSide < 5; iSide++ )
    {
        // Get the enum value for this side
        eView = GetViewEnumType( iSide );

        // 
        for( iRows = 0; iRows < iRes; iRows++ )
        {
            // Get the far left position in this row
            if( !GetCameraPosition( iRows, 0,        iRes, pF, eView, vFrom1, vAt1, vUp1 ) ) {
                LOG_VAR( iRes ) LOG_VAR( iLen ) LOG_VAR( iOffset ) LOG_VAR( iSide )
                LOG_VAR( iRows ) LOG_VAR( iCol ) LOG_ERR_RET( false ) }

            // Get the far right position in this row
            if( !GetCameraPosition( iRows, iRes - 1, iRes, pF, eView, vFrom2, vAt2, vUp2 ) ) {
                LOG_VAR( iRes ) LOG_VAR( iLen ) LOG_VAR( iOffset ) LOG_VAR( iSide )
                LOG_VAR( iRows ) LOG_VAR( iCol ) LOG_ERR_RET( false ) }

            // Determines the space between the camera positions in this row
            vIncrement = ( vFrom2 - vFrom1 ) / iRows;

            vFrom = vFrom1;
            vAt   = vAt1;
            vUp   = vUp1;

            // Go from the left to the right, adding increment until it reaches the
            // right side, recording the position data in the array
            for( iCol = 0; iCol < iRows + 1; iCol++ )
            {
                vPositions[ iOffset + 0 ] = vFrom;
                vPositions[ iOffset + 1 ] = vAt;
                vPositions[ iOffset + 2 ] = vUp;

                // Move the position to the next space in this row
                vFrom += vIncrement;
                vAt   += vIncrement;

                iOffset += 3;
            }
        }
    }

    RET( true )
}
*/