//----------------------------------------------------------------------------- 
// File: Hemicube.cpp
//
// Desc: Used in calculating reflections onto a certain point in radiosity
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include "PolygonManager.h"
#include "Hemicube.h"
#include "Tracer.h"
#include "main.h"
#include "Engine.h"
#include "PolygonLookup.h"
#include "Util.h"
#include "dxutil.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
CHemicube::CHemicube() : m_iTexSize( 128 )
{
    TRACE_ENTER_FN( CHemicube::CHemicube )

    for( int a = 0; a < 5; a++ )
        m_pTexBuffer[a] = NULL;

    m_pForwardMap = NULL;
    m_pLeftMap    = NULL;
    m_pBottomMap  = NULL;
    m_pRightMap   = NULL;
    m_pUpMap      = NULL;

    TRACE_LEAVE_FN()
}

CHemicube::~CHemicube()
{
    TRACE_ENTER_FN( CHemicube::~CHemicube )

    Release();

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: Init()
// Desc:
//-----------------------------------------------------------------------------
bool CHemicube::Init()
{
    TRACE_ENTER_FN( CHemicube::Init )
    
    const int iLen = m_iTexSize * m_iTexSize * 3;

    for( int iTexID = 0; iTexID < 5; iTexID++ )
    {
        if( !CreateTexture( m_iTexSize, 3, GL_RGB, iTexID ) ) {
            Log( "if( !CreateTexture() @ CHemicube::Init()\n" );
            TRACE_LEAVE_FN() return false; }

        m_pTexBuffer[ iTexID ] = new unsigned char[m_iTexSize * m_iTexSize * 3];

        if( m_pTexBuffer == NULL ) {
            Log( "if( m_pTexBuffer == NULL ) @ CHemicube::Init()\n" );
            TRACE_LEAVE_FN() return false; }
    }

    m_pForwardMap = new float[ iLen ];
    m_pLeftMap    = new float[ iLen ];
    m_pBottomMap  = new float[ iLen ];
    m_pRightMap   = new float[ iLen ];
    m_pUpMap      = new float[ iLen ];

    GetDataTGA( m_pForwardMap, "forwardmap.tga" );
    GetDataTGA( m_pLeftMap,    "leftmap.tga"    );
    GetDataTGA( m_pBottomMap,  "downmap.tga"    );
    GetDataTGA( m_pRightMap,   "rightmap.tga"   );
    GetDataTGA( m_pUpMap,      "upmap.tga"      );

    TRACE_LEAVE_FN()

    return true;
}


//-----------------------------------------------------------------------------
// Name: Release()
// Desc:
//-----------------------------------------------------------------------------
void CHemicube::Release()
{
    TRACE_ENTER_FN( CHemicube::Release )

    glDeleteTextures( 5, m_uiTextures );

    for( int a = 0; a < 5; a++ )
    {
        SAFE_DELETE( m_pTexBuffer[ a ] )
    }

    SAFE_DELETE( m_pForwardMap )
    SAFE_DELETE( m_pBottomMap  )
    SAFE_DELETE( m_pLeftMap    )
    SAFE_DELETE( m_pRightMap   )
    SAFE_DELETE( m_pUpMap      )

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: GetDataTGA()
// Desc:
//-----------------------------------------------------------------------------
bool CHemicube::GetDataTGA( float* pData, const char* strFileName )
{
    TRACE_ENTER_FN( CHemicube::GetDataTGA )

    PTARGADATA pMultiTGA = LoadTGA( strFileName );

    if( pMultiTGA == NULL ) {
        Log( "if( pMultiTGA == NULL ) @ CHemicube::GetDataTGA()\n" );
        TRACE_LEAVE_FN() return false; }

    int iLen = pMultiTGA->iSizeX * pMultiTGA->iSizeY * pMultiTGA->iChannels;

    for( int a = 0; a < iLen; a++ )
        pData[ a ] = (float)pMultiTGA->pData[ a ];

    SAFE_DELETE( pMultiTGA->pData )

    TRACE_LEAVE_FN()

    return true;
}


//-----------------------------------------------------------------------------
// Name: CreateTexture()
// Desc:
//-----------------------------------------------------------------------------
bool CHemicube::CreateTexture( const int iSize, const int iChannels,
                               const int iType, const int iTexID )                                        
{
    TRACE_ENTER_FN( CHemicube::CreateTexture )

    unsigned char *pTexture = NULL;

    pTexture = new unsigned char[ iSize * iSize * iChannels ];

    if( pTexture == NULL ) {
        Log( "if( pTexture == NULL ) @ CHemicube::CreateTexture()\n" );
        TRACE_LEAVE_FN() return false; }

    memset( pTexture, 255, iSize * iSize * iChannels * sizeof( unsigned char ) );

    glGenTextures( 1, &m_uiTextures[ iTexID ] );    
    glBindTexture( GL_TEXTURE_2D, m_uiTextures[ iTexID ] );
    
    glTexImage2D( GL_TEXTURE_2D, 0, iChannels, iSize, iSize,
                  0, iType, GL_UNSIGNED_BYTE, pTexture );    
    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    if( pTexture != NULL ) delete[] pTexture;
    
    TRACE_LEAVE_FN()

    return true;
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc:
//-----------------------------------------------------------------------------
bool CHemicube::Render( const VECT vFrom, const VECT vAt, const VECT vUp,
                        const CAMERAVIEW eView, const bool bInPhase1 )
{
    TRACE_ENTER_FN( CHemicube::Render )

    int iImage = 0;
    CFace* pF = NULL;

    glClearColor( 0.0f, 0.0f, 0.0f, 1 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();
    glViewport( 0, 0, m_iTexSize, m_iTexSize );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 90.0f, 1, 0.1f , 250.0f );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    
    // Set the camera
    gluLookAt( vFrom.x, vFrom.y, vFrom.z,
               vAt.x,   vAt.y,   vAt.z,
               vUp.x,   vUp.y,   vUp.z );

    // Disable naming (for selecting)
    Lookup->NamingEnabled( false );

    // Read function description for phase explanation
    if( bInPhase1 )
    {
        if( !RenderPhase1() ) {
            Log( "if( !RenderPhase1() ) @ CHemicube::Render()\n" );
            LU->NamingEnabled( true );
            TRACE_LEAVE_FN() return false; }
    }
    else
    {
        if( !RenderPhase2() ) {
            Log( "if( !RenderPhase2() ) @ CHemicube::Render()\n" );
            LU->NamingEnabled( true );
            TRACE_LEAVE_FN() return false; }
    }

    Lookup->NamingEnabled( true );

    // Select the texture
    iImage = GetViewEnumID( eView );

    float m_fAppTime     = DXUtil_Timer( TIMER_GETAPPTIME );

    glBindTexture( GL_TEXTURE_2D, m_uiTextures[ iImage ] );                

    glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 0, 0, m_iTexSize, m_iTexSize, 0 );
   // glCopyTexSubImage2D( GL_TEXTURE_2D , 0 , 0 , 0 , 0 , 0 , 128 , 128 ); 

    glGetTexImage( GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, m_pTexBuffer[ iImage ] ); 

    m_fAppTime = DXUtil_Timer( TIMER_GETAPPTIME ) - m_fAppTime;
    LOG_VAR( m_fAppTime )
    RET( true )
}
/*
glCopyTexSubImage2D  ( GLenum target , GLint level , GLint xoffset , GLint yoffset , GLint x , GLint y , GLsizei width , GLsizei height ); 

  glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 0, 0, m_iTexSize, m_iTexSize, 0 );
*/

//-----------------------------------------------------------------------------
// Name: GetColor()
// Desc: Returns a vector containing the final r-g-b light recieved on the patch
//-----------------------------------------------------------------------------
VECT CHemicube::GetColor( const bool bInPhase1 )
{
    TRACE_ENTER_FN( CHemicube::GetColor )

    VECT vFinalColor( 0, 0, 0 );
    const int iLen = m_iTexSize * m_iTexSize * 3;
    unsigned char* pData = new unsigned char[ iLen ];
    double dR = 0, dG = 0, dB = 0;

    if( pData == NULL ) LOG_ERR_RET( VECT( 0, 0, 0 ) )

    for( int iView = 0; iView < 5; iView++ )
    {
        memcpy( pData, m_pTexBuffer[ iView ], iLen * sizeof( unsigned char ) );

        if( !ApplyViewCorrection( pData, GetViewEnumType( iView ) ) )
            LOG_ERR_RET( VECT( 0, 0, 0 ) )

        for( int iB = 0; iB < iLen; iB += 3 )
        {
            vFinalColor.x += pData[ iB + 0 ];
            vFinalColor.y += pData[ iB + 1 ];
            vFinalColor.z += pData[ iB + 2 ];
        }
    }

    // The reason for 'iLen * 3' is because the hemicube only fills three
    // full textures
    vFinalColor /= (float)iLen * 3.0f;

    // Increase the intensity of the color by the specified amount
    if( bInPhase1 )
        vFinalColor *= Settings->m_fLighting_LightIntensity;
    else 
        vFinalColor *= Settings->m_fLighting_ReflectionIntensity;

    if( pData != NULL ) delete[] pData;

    if( vFinalColor.x > 255.0f ) vFinalColor.x = 255.0f;
    if( vFinalColor.y > 255.0f ) vFinalColor.y = 255.0f;
    if( vFinalColor.z > 255.0f ) vFinalColor.z = 255.0f;

    TRACE_LEAVE_FN()

    return vFinalColor;
}


//-----------------------------------------------------------------------------
// Name: ApplyViewCorrection()
// Desc: 
//-----------------------------------------------------------------------------
bool CHemicube::ApplyViewCorrection( unsigned char* pData, const CAMERAVIEW eView )
{
    TRACE_ENTER_FN( CHemicube::ApplyViewCorrection )

    float* pMulti = NULL;
    int iLen = m_iTexSize * m_iTexSize * 3;

    if( pData == NULL ) LOG_ERR_RET( false )

    switch( eView )
    {
    case FORWARDVIEW: pMulti = m_pForwardMap; break;
    case LEFTVIEW:    pMulti = m_pLeftMap;    break;
    case RIGHTVIEW:   pMulti = m_pRightMap;   break;
    case UPVIEW:      pMulti = m_pUpMap;      break;
    case DOWNVIEW:    pMulti = m_pBottomMap;  break;
    default: Log( "default: @ CHemicube::ApplyViewCorrection()\n" );
             TRACE_LEAVE_FN() return false;
    }

    if( pMulti == NULL ) LOG_ERR_RET( false )

    // Apply the corrention factor
    for( int iA = 0; iA < iLen; iA++ )
        pData[iA] = (unsigned char)((float)pData[ iA ] * ( pMulti[ iA ] / 255.0 ) );

    TRACE_LEAVE_FN()

    return true;
}


//-----------------------------------------------------------------------------
// Name: GenerateScreens()
// Desc: 
//-----------------------------------------------------------------------------
bool CHemicube::GenerateScreens()
{
    TRACE_ENTER_FN( CHemicube::GenerateScreens )

    static int bFirstTime = 0;

    if( bFirstTime == 4 ) RET( true )

    bFirstTime++;

    for( int iView = 0; iView < 5; iView++ )
    {
        switch( iView )
        {
            case 0:
            {
                if( !GenerateScreenShot( "0.tga", 128, 128, true, m_pTexBuffer[ iView ] ) )
                    LOG_ERR_RET( false )
            }
            break;

            case 1:
            {
                if( !GenerateScreenShot( "1.tga", 128, 128, true, m_pTexBuffer[ iView ] ) )
                    LOG_ERR_RET( false )
            }
            break;

            case 2:
            {
                if( !GenerateScreenShot( "2.tga", 128, 128, true, m_pTexBuffer[ iView ] ) )
                    LOG_ERR_RET( false )
            }
            break;

            case 3:
            {
                if( !GenerateScreenShot( "3.tga", 128, 128, true, m_pTexBuffer[ iView ] ) )
                    LOG_ERR_RET( false )
            }
            break;

            case 4:
            {
                if( !GenerateScreenShot( "4.tga", 128, 128, true, m_pTexBuffer[ iView ] ) )
                    LOG_ERR_RET( false )
            }
            break;

            default:
            {
                Log( "default: @ CHemicube::GenerateScreens()\n" );
                TRACE_LEAVE_FN() return false;
            }
        }
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: GenerateScreens()
// Desc: 
//-----------------------------------------------------------------------------
unsigned int CHemicube::GetTextureID( const int iIndex )
{
    TRACE_ENTER_FN( CHemicube::GetTextureID )

    try
    {
        unsigned int iID = m_uiTextures[iIndex];

        TRACE_LEAVE_FN()

        return iID;
    }
    catch( ... )
    {
        LOG_ERR()
    }

    RET( 0 )
}


//-----------------------------------------------------------------------------
// Name: RenderPhase1()
// Desc: In phase one, the only lit objects in the scene are lights. The
//       polygons are all drawn, but are set to pure black so they can be seen.
//-----------------------------------------------------------------------------
bool CHemicube::RenderPhase1()
{
    TRACE_ENTER_FN( CHemicube::RenderPhase1 )
/*
    CFace* pF = NULL;

    const int iPolys = PM->GetLoadedPolygons();

    for( int iP = 0; iP < iPolys; iP++ )
    {
        const int iFaces = PM->P( iP )->GetLoadedFaces();

        for( int iF = 0; iF < iFaces; iF++ )
        {
            pF = PM->F( iP, iF );

            if( pF == NULL ) LOG_ERR_RET( false )

            VECT v0 = pF->V( 0 )->GetColor();
            VECT v1 = pF->V( 1 )->GetColor();
            VECT v2 = pF->V( 2 )->GetColor();
            VECT v3 = pF->V( 3 )->GetColor();

            //pF->V( 0 )->SetColor( VECT( 0, 0, 0 ) );
          //  pF->V( 1 )->SetColor( VECT( 0, 0, 0 ) );
           // pF->V( 2 )->SetColor( VECT( 0, 0, 0 ) );
           // pF->V( 3 )->SetColor( VECT( 0, 0, 0 ) );

            PM->Render3DFace( iP, iF );

            pF->V( 0 )->SetColor( v0 );
            pF->V( 1 )->SetColor( v1 );
            pF->V( 2 )->SetColor( v2 );
            pF->V( 3 )->SetColor( v3 );
        }
    }

    // Now render the lights
    LM->Render();
*/
HTEXTURE hTex;

        //m_Brush.Render();

        for( int iP = 0; iP < PM->GetLoadedPolygons(); iP++ )
        {
            CPolygon* pP = PM->P( iP );

            if( pP == NULL ) { LOG_ERR_RET( false ) }

            // Get all the crap needed to render this polygon
            unsigned int* pTex     = pP->GetUniqueTextureArray();
            float* pVerts          = pP->GetVertexArray();
            float* pColors         = pP->GetColorArray();
            float* pTexCoords      = pP->GetTexCoordArray();
            float* pLightmapCoords = pP->GetLightmapCoordArray();
            int*   pTexSwitches    = pP->GetTexSwitchArray();
            int*   pBegin          = pP->GetVertexBeginArray();
            int*   pEnd            = pP->GetVertexEndArray();
            int iNumTexSwitches    = pP->GetNumTexSwitches();


            glEnableClientState( GL_VERTEX_ARRAY          );
            glVertexPointer( 3,   GL_FLOAT, 0, pVerts     );

            glEnableClientState( GL_COLOR_ARRAY           );
            glColorPointer( 3,    GL_FLOAT, 0, pColors    );

            TM->glActiveTextureARB( GL_TEXTURE0_ARB );
            glEnableClientState( GL_TEXTURE_COORD_ARRAY   );
            glTexCoordPointer( 2, GL_FLOAT, 0, pTexCoords );

            TM->glActiveTextureARB( GL_TEXTURE1_ARB );
            glEnableClientState( GL_TEXTURE_COORD_ARRAY );
            glTexCoordPointer( 2, GL_FLOAT, 0, pLightmapCoords );

            // Draw the polygon.
            for( int i = 0; i < iNumTexSwitches; i++ )
            {
                hTex.id = pTex[ i ];

                TM->PrepareTexture1( hTex );

              //  glBindTexture( GL_TEXTURE_2D, LM->GetTextureGL( 0 ) );

                glDrawArrays( GL_TRIANGLES, pBegin[ i ], pEnd[ i ] );
            }
        }

    TRACE_LEAVE_FN()

    return true;
}


//-----------------------------------------------------------------------------
// Name: RenderPhase2()
// Desc: 
//-----------------------------------------------------------------------------
bool CHemicube::RenderPhase2()
{
    TRACE_ENTER_FN( CHemicube::RenderPhase2 )

    CFace* pF = NULL;

    const int iPolys = PM->GetLoadedPolygons();
    
    for( int iP = 0; iP < iPolys; iP++ )
    {
        const int iFaces = PM->P( iP )->GetLoadedFaces();

        for( int iF = 0; iF < iFaces; iF++ )
        {
            pF = PM->F( iP, iF );

            if( pF == NULL ) LOG_ERR_RET( false )

          //  const int iTempTex = pF->GetLightmapID();

            //pF->SetLightmapID( NOTEXTURE );

            PM->Render3DFace( iP, iF );

//            pF->SetLightmapID( iTempTex );
        }
    }

    RET( true )
}