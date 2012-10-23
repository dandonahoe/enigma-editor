//----------------------------------------------------------------------------- 
// File: PolygonManagerRender.cpp
//
// Desc: 
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#include <windows.h>
#include "PolygonManager.h"
#include "Tracer.h"
#include "Settings.h"
#include "LightManager.h"
#include "enum.h"
#include "map.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene in the specified window
//-----------------------------------------------------------------------------
bool CPolygonManager::Render( CWindow* pWindow )
{
    TRACE_ENTER_FN( CPolygonManager::Render )

    LU->ResetNaming();

    if( pWindow == NULL ) { LOG_ERR_RET( false ) }

    switch( pWindow->GetViewType() )
    {
    case VIEW_3D: RET( Render3D() ) break;
    case VIEW_XZ: RET( RenderXZ() ) break;
    case VIEW_XY: RET( RenderXY() ) break;
    case VIEW_YZ: RET( RenderYZ() ) break;
    default: LOG_ERR_RET( false )   break;   
    }    
}


//-----------------------------------------------------------------------------
// Name: Render3D()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::Render3D() { TRACE_ENTER_FN( CPolygonManager::Render3D ) RET( Render3DScene() ) }


//-----------------------------------------------------------------------------
// Name: Render3DScene()
// Desc: Renders the 3D view
//-----------------------------------------------------------------------------
bool CPolygonManager::Render3DScene()
{
    TRACE_ENTER_FN( CPolygonManager::Render3DScene )

    if( IM->KeyPressed( DIK_K ) )
    {
        HTEXTURE hTex;

        m_Brush.Render();

        for( int iP = 0; iP < GetLoadedPolygons(); iP++ )
        {
            CPolygon* pP = P( iP );

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

                glBindTexture( GL_TEXTURE_2D, LM->GetTextureGL( 0 ) );

                glDrawArrays( GL_TRIANGLES, pBegin[ i ], pEnd[ i ] );
            }
        }

        RenderSelected();
        LM->Render();
    }
    else
    {
        m_Brush.Render();
        Render3DPolygons();
        Render3DPolygonNormals();
        RenderSelected();
        LM->Render();
    }

    RET( true )
}

/*
bool CEngine::RenderEnigmaLevel()
{
    TRACE_ENTER_FN( CEngine::RenderEnigmaLevel )

    static bool bFirst = true;



    if( bFirst )
    {
        bFirst = false;

        // Vertex array
        glEnableClientState( GL_VERTEX_ARRAY );
        glVertexPointer( 3, GL_FLOAT, 0, Map->GetVerts() );

        // Vertec color array
        glEnableClientState( GL_COLOR_ARRAY  );
        glColorPointer( 3, GL_FLOAT, 0, Map->GetColors() );

        // Texture coords for texture
        TM->glClientActiveTextureARB( GL_TEXTURE0_ARB );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glTexCoordPointer( 2, GL_FLOAT, 0, Map->GetTexCoords() );

        // Texture coords for lightmaps
        TM->glClientActiveTextureARB( GL_TEXTURE1_ARB );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glTexCoordPointer( 2, GL_FLOAT, 0, Map->GetLightmapCoords() );
    }

    float* pVerts = Map->GetVerts();

    for( int iF = 0; iF < Map->GetNumFaces(); iF++ )
    {
        LPFACE pF = Map->GetFace( iF );

        if( pF->m_bRender )
        {
            TM->PrepareTexture1( pF->m_phTexture );
            TM->PrepareLightmap( pF->m_piLightmap );
            glDrawArrays( pF->m_eFaceType, pF->m_iBeginIndex, pF->m_iEndIndex );
        }
    }

    if( !m_cPlayer.Render() ) { LOG_ERR() }
    if( !m_pHud->Render() )   { LOG_ERR() }

    GL->EndScene();

    TRACE_LEAVE_FN() return true;
}
*/
//-----------------------------------------------------------------------------
// Name: RenderFace()
// Desc: Determines which kind of face it is
//-----------------------------------------------------------------------------
bool CPolygonManager::Render3DFace( int iP, int iF )
{
    TRACE_ENTER_FN( CPolygonManager::Render3DFace )

    CFace* pF = F( iP, iF );

    if( pF == NULL ) { LOG_VAR( iP ) LOG_VAR( iF ) LOG_ERR_RET( false ) }

    // See what kind of face it is
    if( pF->GetFaceType() == QUADFACE )
        Render3DFaceQuad( iP, iF );
    else
        Render3DFaceTriangle( iP, iF );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Render3DFaceQuad()
// Desc: Renders a quad face. Gets the face's texture handle and tells the
//       texture manager to prepare it. Then obtains the information about
//       each vertex and sends it through the OpenGL pipeline.
//-----------------------------------------------------------------------------
bool CPolygonManager::Render3DFaceQuad( int iP, int iF )
{
    TRACE_ENTER_FN( CPolygonManager::Render3DFaceQuad )

    VECT vP, vC;
    VECT vCenter = F( iP, iF )->GetCenter();
    VECT vNormal = F( iP, iF )->GetNormal();
    float fU, fV;
    CVertex* pV;
    CFace* pF = F( iP, iF );

    if( pF == NULL ) { Log( "if( pF == NULL ) @ CPolygonManager::Render3DFaceQuad()\n" ); TRACE_LEAVE_FN() }

    bool bTex1 = pF->UsingTexture1();
    bool bTex2 = pF->UsingTexture2();
    bool bMap  = pF->UsingLightmap();

    if( bTex1 ) TextureManager->PrepareTexture1( pF->GetTexture1() );
    if( bTex2 ) pF->PrepareLightmap();

    //TextureManager->PrepareTexture2( pF->GetTexture2() );
//    if( bMap  ) TextureManager->PrepareLightmap( pF->GetLightmap() );

    glBegin( GL_QUADS );

    glNormal3f( vNormal.x,  vNormal.y, vNormal.z );

    for( int iV = 0; iV < 4; iV++ )
    {
        pV = V( iP, iF, iV );
        
        if( pV == NULL ) LOG_ERR_RET( false )

        // Position
        vP = pV->GetVertPos();

        // Color
        vC = pV->GetColor();

        // Texture Coordinates
        fU = pV->GetTexU();
        fV = pV->GetTexV();

        if( bTex1 )
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, fU, fV );

        if( bTex2 )
        {
            float fU = pF->GetCoordU( iV );
            float fV = pF->GetCoordV( iV );

            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, fU, fV );
        }

        if( bMap )
            glMultiTexCoord2fARB( GL_TEXTURE2_ARB, fU, fV );

        glColor3f(  vC.x, vC.y, vC.z );
        glVertex3f( vP.x, vP.y, vP.z );
    }
    
    glEnd();

    if( bTex1 ) TextureManager->DisableTexture1();
    if( bTex2 ) pF->DisableLightmap();
    if( bMap )  TextureManager->DisableLightmap();

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: RenderSelected()
// Desc: Renders the white circles at the verticies of the selected face
//-----------------------------------------------------------------------------
bool CPolygonManager::RenderSelected()
{
    TRACE_ENTER_FN( CPolygonManager::RenderSelected )

    glDisable( GL_TEXTURE_2D );

    // If the brush is what is selected
    if( Lookup->BrushSelected() )
    {
        m_Brush.RenderSelected();
        glEnable( GL_TEXTURE_2D );
        RET( true )
    }

    // If the brush is what is selected
    if( Lookup->LightSelected() )
    {
        LM->RenderSelected();
        glEnable( GL_TEXTURE_2D );
        RET( true )
    }

    // If nothing is selected, just leave
    if( !Lookup->FaceSelected() && !Lookup->PolySelected() ) RET( true )

    int iP = Lookup->GetPoly();
    int iF = Lookup->GetFace();

    for( int iV = 0; iV < F( iP, iF )->GetLoadedVerts(); iV++ )
    {
        Lookup->StartNaming();

        Lookup->Include( Lookup->GetPoly(), Lookup->GetFace(), iV, NA, true, false, false );

        CVertex* pVert = V( Lookup->GetPoly(), Lookup->GetFace(), iV );

        if( pVert == NULL ) { glEnable( GL_TEXTURE_2D ); LOG_ERR_RET( false )}

        if( Lookup->GetVert() == iV )
            glColor3f( 1.0f, 0.0f, 0.0f );
        else
            glColor3f( 1.0f, 1.0f, 1.0f );

        glPushMatrix();

        glTranslatef( pVert->GetVertPos().x, pVert->GetVertPos().y, pVert->GetVertPos().z );

        gluSphere( m_pQuadricObj, 0.1f, 16, 16 );

        glPopMatrix();

        Lookup->EndNaming();
    }
/*
    const int iLen = LM->GetRequiredArrayLength( 8 );
   // LOG_VAR( iLen )

    VECT* vPositions = new VECT[ iLen ];

    if( !LM->GetPositionArray( F( iP, iF ), 8, vPositions, iLen ) )
    {
        delete[] vPositions;
        glEnable( GL_TEXTURE_2D );
    }

    int iOff = iLen;
    // Show UP components
    if( IM->KeyPressed( DIK_1 ) )
    {
        iOff = 0;
    }
    else if( IM->KeyPressed( DIK_2 ) )
    {
        iOff = 3;
    }
    else if( IM->KeyPressed( DIK_3 ) )
    {
        iOff = 6;
    }
    else if( IM->KeyPressed( DIK_4 ) )
    {
        iOff = 9;
    }
    else if( IM->KeyPressed( DIK_5 ) )
    {
        iOff = 12;
    }
int y = 0;
    //for( int a = 0; a < iLen; a +=3 )
    for( int a = iOff; a < iLen; a += 15 )
    {
        if( a == 0 )
        {
            glBegin( GL_LINES );

            glColor3f( 1, 1, 1 );

            glVertex3f( vPositions[ a + 0 ].x, vPositions[ a + 0 ].y, vPositions[ a + 0 ].z );
            glVertex3f( vPositions[ a + 1 ].x, vPositions[ a + 1 ].y, vPositions[ a + 1 ].z );

            glColor3f( 0, 1, 1 );

            VECT v = vPositions[ a + 0 ] + vPositions[ a + 2 ];

            glVertex3f( vPositions[ a + 0 ].x, vPositions[ a + 0 ].y, vPositions[ a + 0 ].z );
            glVertex3f( v.x, v.y, v.z );

            glEnd();
        }
        else
        {
        glBegin( GL_LINES );

        glColor3f( 1, 1, 1 );

        glVertex3f( vPositions[ a + 0 ].x, vPositions[ a + 0 ].y, vPositions[ a + 0 ].z );
        glVertex3f( vPositions[ a + 1 ].x, vPositions[ a + 1 ].y, vPositions[ a + 1 ].z );

        glColor3f( 0, 1, 0 );

        VECT v = vPositions[ a + 0 ] + vPositions[ a + 2 ];

        glVertex3f( vPositions[ a + 0 ].x, vPositions[ a + 0 ].y, vPositions[ a + 0 ].z );
        glVertex3f( v.x, v.y, v.z );


        glEnd();
        }

        y++;
       // if( y == 5 )
       //     break;
    }

    delete[] vPositions;
*/
    glEnable( GL_TEXTURE_2D );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: RenderXZ()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::RenderXZ()
{
    TRACE_ENTER_FN( CPolygonManager::RenderXZ )

    glDisable( GL_TEXTURE_2D );

    Render2DScene();

    glEnable( GL_TEXTURE_2D );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: RenderXY()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::RenderXY()
{
    TRACE_ENTER_FN( CPolygonManager::RenderXY )

    glDisable( GL_TEXTURE_2D );

    Render2DScene();

    glEnable( GL_TEXTURE_2D );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: RenderYZ()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::RenderYZ()
{
    TRACE_ENTER_FN( CPolygonManager::RenderYZ )

    glDisable( GL_TEXTURE_2D );

    Render2DScene();

    glEnable( GL_TEXTURE_2D );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Render2DScene()
// Desc: Renders the 2D view
//-----------------------------------------------------------------------------
bool CPolygonManager::Render2DScene()
{
    TRACE_ENTER_FN( CPolygonManager::Render2DScene() )

    m_Brush.Render();

    for( int iP = 0; iP < GetLoadedPolygons(); iP++ )
    {
        for( int iF = 0; iF < P( iP )->GetLoadedFaces(); iF++ )
        {
            Lookup->StartNaming();

            Lookup->Include( iP, iF, NA, NA, false, false, false );

            Render2DFace( iP, iF );

            Lookup->EndNaming();
        }
    }

    // Draws the extra things that accompany a selected polygon
    //RenderSelected( name );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Render2DFace()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::Render2DFace( int iP, int iF )
{
    TRACE_ENTER_FN( CPolygonManager::Render2DFace() )

    if( F( iP, iF )->GetFaceType() == QUADFACE )
        RET( Render2DFaceQuad( iP, iF ) )
    else
        RET( Render2DFaceTriangle( iP, iF ) )
}


//-----------------------------------------------------------------------------
// Name: Render2DFaceQuad()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::Render2DFaceQuad( int iP, int iF )
{
    TRACE_ENTER_FN( CPolygonManager::Render2DFaceQuad() )

    VECT vP, vC;

    glBegin( GL_LINE_LOOP );
    
    for( int iV = 0; iV < 4; iV++ )
    {
        vP = V( iP, iF, iV )->GetVertPos();
        vC = V( iP, iF, iV )->GetColor();

        glColor3f(  vC.x, vC.y, vC.z );
        glVertex3f( vP.x, vP.y, vP.z );
    }
    
    glEnd();

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Render2DFaceTriangle()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::Render2DFaceTriangle( int iP, int iF )
{
    TRACE_ENTER_FN( CPolygonManager::Render2DFaceTriangle() )

    VECT vP, vC;

    glBegin( GL_LINE_LOOP );

    for( int iV = 0; iV < 3; iV++ )
    {
        vP = m_pPolygons[ iP ]->F( iF )->V( iV )->GetVertPos();
        vC = m_pPolygons[ iP ]->F( iF )->V( iV )->GetColor();

        glColor3f(  vC.x, vC.y, vC.z );
        glVertex3f( vP.x, vP.y, vP.z );
    }

    glEnd();

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Render3DFaceTriangle()
// Desc: Same, same
//-----------------------------------------------------------------------------
bool CPolygonManager::Render3DFaceTriangle( int iP, int iF )
{
    TRACE_ENTER_FN( CPolygonManager::Render3DFaceTriangle )

    HTEXTURE hTexture;
    VECT vP, vC;

    TM->Invalidate( hTexture );

    TM->PrepareTexture1( m_pPolygons[ iP ]->F( iF )->GetTexture1() );

    glBegin( GL_TRIANGLES );

    for( int iV = 0; iV < 3; iV++ )
    {
        // Position
        vP = m_pPolygons[ iP ]->F( iF )->V( iV )->GetVertPos();

        // Color
        vC = m_pPolygons[ iP ]->F( iF )->V( iV )->GetColor();

        glTexCoord2f( m_pPolygons[ iP ]->F( iF )->V( iV )->GetTexU(),
                      m_pPolygons[ iP ]->F( iF )->V( iV )->GetTexV() );

        glColor3f(  vC.x, vC.y, vC.z );
        glVertex3f( vP.x, vP.y, vP.z );
    }
    
    glEnd();

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Render3DPolygons()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::Render3DPolygons()
{
    TRACE_ENTER_FN( CPolygonManager::Render3DPolygons )

    CPolygon* pP;

    for( int iP = 0; iP < GetLoadedPolygons(); iP++ )
    {
        //pP = P( iP );
        pP = m_pPolygons[ iP ];

        if( pP == NULL ) LOG_ERR_RET( false )

        const int iLoadedFaces = pP->GetLoadedFaces();

        for( int iF = 0; iF < iLoadedFaces; iF++ )
        {
            LU->StartNaming();

            LU->Include( iP, iF, NA, NA, false, false, false );

            Render3DFace( iP, iF );

            LU->EndNaming();
        }
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Render3DPolygonNormals()
// Desc: 
//-----------------------------------------------------------------------------
bool CPolygonManager::Render3DPolygonNormals()
{
    TRACE_ENTER_FN( CPolygonManager::Render3DPolygonNormals )

    glDisable( GL_TEXTURE_2D );

    VECT vNormal, vCenter;
    CFace* pF;
    CPolygon* pP;
    VECT vFrom, vAt, vUp;

    glBegin( GL_LINES );

    for( int iP = 0; iP < GetLoadedPolygons(); iP++ )
    {
        pP = P( iP );

        if( pP == NULL ) LOG_ERR_RET( false )

        const int iLoadedFaces = pP->GetLoadedFaces();

        for( int iF = 0; iF < iLoadedFaces; iF++ )
        {
            pF = F( iP, iF );

            if( pF == NULL ) LOG_ERR_RET( false )

            vCenter = pF->GetCenter();
            vNormal = pF->GetNormal();
        }
    }

    glEnd();

    glEnable( GL_TEXTURE_2D );

    RET( true )
}
