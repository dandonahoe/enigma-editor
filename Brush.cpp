//----------------------------------------------------------------------------- 
// File: Brush.cpp
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
#include <D3DX8math.h>
#include "PolygonManager.h"
#include "Brush.h"
#include "Polygon.h"
#include "main.h"
#include "Settings.h"
#include "Util.h"
#include "Tracer.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
CBrush::CBrush()
{
    CPolygon::CPolygon();

    TRACE_ENTER_FN( CBrush::CBrush )

    Log( "CBrush::CBrush()\n" );
    
    m_eBrushType = NOTHINGBRUSH;

    m_pQuadricObj = gluNewQuadric();
    gluQuadricTexture( m_pQuadricObj, false );
    gluQuadricDrawStyle( m_pQuadricObj, GLU_FILL );

    m_vPos = VECT( 0, 0, 0 );

    TRACE_LEAVE_FN()
}
CBrush::~CBrush()
{
    TRACE_ENTER_FN( CBrush::~CBrush )

    Log( "CBrush::~CBrush()\n" );

    Release();

    if( m_pQuadricObj != NULL )
        gluDeleteQuadric( m_pQuadricObj );

    m_vPos = VECT( 0, 0, 0 );

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: Forge()
// Desc: Dynamically alcolates memory for a new CPolygon, then converts all the
//       information about the current CBrush and transfers it to the new
//       CPolygon. CPolygonManager is responsable for deleting all CPolygons.
//-----------------------------------------------------------------------------
CPolygon* CBrush::Forge()
{
    TRACE_ENTER_FN( CBrush::Forge )

    // Alcolate space for a new polygon
    CPolygon* pPoly = new CPolygon();

    // If not allocated, return null
    if( pPoly == NULL ) LOG_ERR_RET( NULL )

    // Allocate space inside the new polygon to
    // hold the data for its faces
    if( !pPoly->AllocateFaceSpace( GetLoadedFaces() ) ) LOG_ERR_RET( NULL )
    
    VECT v1, v2, v3, v4;

    // Loads the currently selected texture from the texture list box into memory
    // if it isn't already there
    TM->SetSelectedTexture( TM->AddTextureFromFile( TM->GetNameFromTextureTab() ) );

    // Cycle through all the faces of the brush. Takes the attributes of the brush and
    // makes them into the polygon format
    for( int iF = 0; iF < GetLoadedFaces(); iF++ )
    {
        // Make sure the next face exists
        if( F( iF ) == NULL ) { LOG_VAR( iF ) LOG_ERR_RET( NULL ) }

        switch( F( iF )->GetFaceType() )
        {
            case TRIANGLEFACE:
            {
                v1 = V( iF, 0 )->GetVertPos();
                v2 = V( iF, 1 )->GetVertPos();
                v3 = V( iF, 2 )->GetVertPos();

                pPoly->NewTriangleFace( v1, v2, v3 );

                pPoly->F( iF )->SetTexture1( TM->GetSelectedTexture() );

                pPoly->V( iF , 0 )->SetTexU( 0 );
                pPoly->V( iF , 0 )->SetTexV( 0 );
                pPoly->V( iF , 1 )->SetTexU( 1 );
                pPoly->V( iF , 1 )->SetTexV( 0 );
                pPoly->V( iF , 2 )->SetTexU( 1 );
                pPoly->V( iF , 2 )->SetTexV( 1 );
            }
            break;

            case QUADFACE:
            {
                v1 = V( iF, 0 )->GetVertPos();
                v2 = V( iF, 1 )->GetVertPos();
                v3 = V( iF, 2 )->GetVertPos();
                v4 = V( iF, 3 )->GetVertPos();

                pPoly->F( iF )->SetTexture1( TM->GetSelectedTexture() );
                pPoly->V( iF, 0 )->SetTexU( 0 );
                pPoly->V( iF, 0 )->SetTexV( 0 );
                
                pPoly->V( iF, 1 )->SetTexU( 1 );
                pPoly->V( iF, 1 )->SetTexV( 0 );

                pPoly->V( iF, 2 )->SetTexU( 1 );
                pPoly->V( iF, 2 )->SetTexV( 1 );

                pPoly->V( iF, 3 )->SetTexU( 0 );
                pPoly->V( iF, 3 )->SetTexV( 1 );
            }
            break;

            default: LOG_ERR() break;
        }
    }

    PM->AlignPolygonTextures();

    switch( m_eBrushType )
    {
        case SHEETBRUSH:    pPoly->SetName( "Sheet",    true ); break;
        case CUBEBRUSH:     pPoly->SetName( "Cube",     true ); break;
        case STAIRSBRUSH:   pPoly->SetName( "Stairs",   true ); break;
        case SPIRALBRUSH:   pPoly->SetName( "Spiral",   true ); break;
        case CYLINDERBRUSH: pPoly->SetName( "Cylinder", true ); break;
        default:            pPoly->SetName( "Unknown",  true ); break;
    }

    if( !pPoly->Rebuild() ) LOG_ERR()

    RET( pPoly )
}


//-----------------------------------------------------------------------------
// Name: SetType()
// Desc: 
//-----------------------------------------------------------------------------
bool CBrush::SetType( const BRUSHTYPE type, CFace* pF )
{
    TRACE_ENTER_FN( CBrush::SetType )

    m_eBrushType = type;

    Release();

    switch( m_eBrushType )
    {
    case NOTHINGBRUSH:                        break;
    case SHEETBRUSH:    NewSheet();           break;
    case CUBEBRUSH:     NewCube();            break;
    case STAIRSBRUSH:   NewStairs();          break;
    case SPIRALBRUSH:   NewSpiralStairs();    break;
    case CYLINDERBRUSH: NewCylinder();        break;
    case CIRCLEBRUSH:   NewCircle();          break;
    case CUSTOMBRUSH:   NewCustomBrush( pF ); break; 

    default: LOG_ERR_RET( false ) break;
    }

    if( !BrushRebuild() ) LOG_ERR_RET( false )

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: 
//-----------------------------------------------------------------------------
void CBrush::Render()
{
    TRACE_ENTER_FN( CBrush::Render )

    if( IM->KeyPressed( DIK_K ) )
    {
        glDisable( GL_TEXTURE_2D );

        float* pVerts  = GetVertexArray();
        float* pColors = GetColorArray();

        glEnableClientState( GL_VERTEX_ARRAY );
        glVertexPointer( 3, GL_FLOAT, 0, pVerts );

        glEnableClientState( GL_COLOR_ARRAY );
        glColorPointer( 3, GL_FLOAT, 0, pColors );

        for( int iF = 0; iF < GetLoadedFaces(); iF++ )
        {
            glDrawArrays( GL_LINE_STRIP, iF * 3, 3 );
            //glDrawArrays( GL_LINE_STRIP, 0, GetLoadedFaces() * 3 );
        }

        glEnable( GL_TEXTURE_2D );
    }
    else
    {
        VECT v;
        VECT c;
        CFace* pF;

        glDisable( GL_TEXTURE_2D );

        for( int iF = 0; iF < GetLoadedFaces(); iF++ )
        {
            pF = F( iF );

            if( pF == NULL ) { LOG_VAR( iF ) LOG_ERR() TRACE_LEAVE_FN() return; }

            switch( pF->GetFaceType() )
            {
                case NOTHINGFACE: break;

                case TRIANGLEFACE:
                {
                    LU->StartNaming();
                    LU->Include( NA, iF, NA, NA, false, true, false );

                    // Render the lines
                    glBegin( GL_LINE_LOOP );

                    for( int b = 0; b < 3; b++ )
                    {
                        v = pF->V( b )->GetVertPos();
                        glColor3f( 0.5f, 1.0f, 0.5f );
                        glVertex3f( v.x, v.y, v.z );
                    }

                    glEnd();
                    LU->EndNaming();
                }
                break;

                case QUADFACE:
                {
                    LU->StartNaming();

                    LU->Include( NA, iF, NA, NA, false, true, false );

                    // Render the lines
                    glBegin( GL_LINE_LOOP );
                    for( int b = 0; b < 4; b++ )
                    {
                        v = pF->V( b )->GetVertPos();
                        //c = F( iF )->m_Verts[b].m_vColor;

                        glColor3f( 0.5f, 1.0f, 0.5f );
                        glVertex3f( v.x, v.y, v.z );
                    }
                    glEnd();

                    LU->EndNaming();
                }
                break;
            }
        }

        glEnable( GL_TEXTURE_2D );

    }

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: NewCube()
// Desc: 
//-----------------------------------------------------------------------------
bool CBrush::NewCube()
{
    TRACE_ENTER_FN( CBrush::NewCube )

    if( !AllocateFaceSpace( 12 ) ) LOG_ERR_RET( false )

    float fDepth  = SE->m_fBrush_Cube_Depth;
    float fWidth  = SE->m_fBrush_Cube_Width;
    float fHeight = SE->m_fBrush_Cube_Height;

    // Back
    VECT v1( -fWidth,  fHeight, -fDepth );
    VECT v2(  fWidth,  fHeight, -fDepth );
    VECT v3(  fWidth, -fHeight, -fDepth );
    VECT v4( -fWidth, -fHeight, -fDepth );
    NewTriangleFace( v1 + m_vPos, v2 + m_vPos, v3 + m_vPos );
    NewTriangleFace( v1 + m_vPos, v3 + m_vPos, v4 + m_vPos );

    // Front
    v1.z = v2.z = v3.z = v4.z = fDepth;
    NewTriangleFace( v3 + m_vPos, v2 + m_vPos, v1 + m_vPos );
    NewTriangleFace( v4 + m_vPos, v3 + m_vPos, v1 + m_vPos );

    // Left
    SetVector( v1, -fWidth,  fHeight, -fDepth );
    SetVector( v2, -fWidth, -fHeight, -fDepth );
    SetVector( v3, -fWidth, -fHeight,  fDepth );
    SetVector( v4, -fWidth,  fHeight,  fDepth );
    NewTriangleFace( v1 + m_vPos, v2 + m_vPos, v3 + m_vPos );
    NewTriangleFace( v1 + m_vPos, v3 + m_vPos, v4 + m_vPos );

    // Right
    v1.x = v2.x = v3.x = v4.x = fWidth;
    NewTriangleFace( v3 + m_vPos, v2 + m_vPos, v1 + m_vPos );
    NewTriangleFace( v4 + m_vPos, v3 + m_vPos, v1 + m_vPos );

    // Top
    SetVector( v1, -fWidth,  fHeight, -fDepth );
    SetVector( v2,  fWidth,  fHeight, -fDepth );
    SetVector( v3,  fWidth,  fHeight,  fDepth );
    SetVector( v4, -fWidth,  fHeight,  fDepth );
    NewTriangleFace( v3 + m_vPos, v2 + m_vPos, v1 + m_vPos );
    NewTriangleFace( v4 + m_vPos, v3 + m_vPos, v1 + m_vPos );

    // Bottom
    v1.y = v2.y = v3.y = v4.y = -fHeight;
    NewTriangleFace( v1 + m_vPos, v2 + m_vPos, v3 + m_vPos );
    NewTriangleFace( v1 + m_vPos, v3 + m_vPos, v4 + m_vPos );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: NewSheet()
// Desc: 
//-----------------------------------------------------------------------------
bool CBrush::NewSheet()
{
    TRACE_ENTER_FN( CBrush::NewSheet )

    if( !AllocateFaceSpace( 2 ) ) LOG_ERR_RET( false )

    float fU = SE->m_fBrush_Sheet_USize;
    float fV = SE->m_fBrush_Sheet_VSize;

    switch( SE->m_iBrush_Sheet_Axis )
    {
        case 1: // X - Axis
            NewTriangleFace( VECT( -fU,  fV, 0 ) + m_vPos,
                             VECT(  fU,  fV, 0 ) + m_vPos,
                             VECT(  fU, -fV, 0 ) + m_vPos );
            NewTriangleFace( VECT( -fU,  fV, 0 ) + m_vPos,
                             VECT(  fU, -fV, 0 ) + m_vPos,
                             VECT( -fU, -fV, 0 ) + m_vPos );
        break;

        case 2: // Y - Axis
            NewTriangleFace( VECT( 0,  fU,  fV ) + m_vPos,
                             VECT( 0,  fU, -fV ) + m_vPos,
                             VECT( 0, -fU, -fV ) + m_vPos );
            NewTriangleFace( VECT( 0,  fU,  fV ) + m_vPos,
                             VECT( 0, -fU, -fV ) + m_vPos,
                             VECT( 0, -fU,  fV ) + m_vPos );

        break;

        case 3: // Z - Axis
            NewTriangleFace( VECT( -fU,  0,  fV ) + m_vPos,
                             VECT( -fU,  0, -fV ) + m_vPos,
                             VECT(  fU,  0, -fV ) + m_vPos );
            NewTriangleFace( VECT( -fU,  0,  fV ) + m_vPos,
                             VECT(  fU,  0, -fV ) + m_vPos,
                             VECT(  fU,  0,  fV ) + m_vPos );
        break;

        default:
        {
            Log( "default: @ CBrush::NewSheet()\n" );

            // Default to the x-axis
            SE->m_iBrush_Sheet_Axis = 1;
            VECT v1( -1,  1, 0 );
            VECT v2(  1,  1, 0 );
            VECT v3(  1, -1, 0 );
            VECT v4( -1, -1, 0 );

            NewTriangleFace( v1 + m_vPos, v2 + m_vPos, v3 + m_vPos );
            NewTriangleFace( v1 + m_vPos, v3 + m_vPos, v4 + m_vPos );
        }
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: NewStairs()
// Desc: 
//-----------------------------------------------------------------------------
bool CBrush::NewStairs()
{
    TRACE_ENTER_FN( CBrush::NewStairs )

    int   iSteps  = SE->m_iBrush_Stairs_NumSteps;
    float fHeight = SE->m_fBrush_Stairs_Height;
    float fWidth  = SE->m_fBrush_Stairs_Width;
    float fDepth  = SE->m_fBrush_Stairs_Depth;

    if( !AllocateFaceSpace( ( ( 4 * iSteps ) + 2 ) * 2 ) ) LOG_ERR_RET( false )

    for( int iA = 0; iA < iSteps; iA++ )
    {
        // Back
        NewTriangleFace( VECT( fDepth * iA,         fHeight * ( iA + 1 ), -fWidth ) + m_vPos,
                         VECT( fDepth * ( iA + 1 ), fHeight * ( iA + 1 ), -fWidth ) + m_vPos,
                         VECT( fDepth * ( iA + 1 ), 0,                    -fWidth ) + m_vPos );
        NewTriangleFace( VECT( fDepth * iA,         fHeight * ( iA + 1 ), -fWidth ) + m_vPos,
                         VECT( fDepth * ( iA + 1 ), 0,                    -fWidth ) + m_vPos,
                         VECT( fDepth * iA,         0,                    -fWidth ) + m_vPos );


        // Front
        NewTriangleFace( VECT( fDepth * iA,         fHeight * ( iA + 1 ), 0 ) + m_vPos,
                         VECT( fDepth * ( iA + 1 ), fHeight * ( iA + 1 ), 0 ) + m_vPos,
                         VECT( fDepth * ( iA + 1 ), 0,                    0 ) + m_vPos );
        NewTriangleFace( VECT( fDepth * iA,         fHeight * ( iA + 1 ), 0 ) + m_vPos,
                         VECT( fDepth * ( iA + 1 ), 0,                    0 ) + m_vPos,
                         VECT( fDepth * iA,         0,                    0 ) + m_vPos );

        // Left
        NewTriangleFace( VECT( fDepth * iA, iA * fHeight,           0       ) + m_vPos,
                         VECT( fDepth * iA, iA * fHeight + fHeight, 0       ) + m_vPos,
                         VECT( fDepth * iA, iA * fHeight + fHeight, -fWidth ) + m_vPos );
        NewTriangleFace( VECT( fDepth * iA, iA * fHeight,           0       ) + m_vPos,
                         VECT( fDepth * iA, iA * fHeight + fHeight, -fWidth ) + m_vPos,
                         VECT( fDepth * iA, iA * fHeight,           -fWidth ) + m_vPos );

        // Top
        NewTriangleFace( VECT( fDepth * iA,          ( iA + 1 ) * fHeight,  0      ) + m_vPos,
                         VECT( fDepth * iA + fDepth, ( iA + 1 ) * fHeight,  0      ) + m_vPos,
                         VECT( fDepth * iA + fDepth, ( iA + 1 ) * fHeight, -fWidth ) + m_vPos );
        NewTriangleFace( VECT( fDepth * iA,          ( iA + 1 ) * fHeight,  0      ) + m_vPos,
                         VECT( fDepth * iA + fDepth, ( iA + 1 ) * fHeight, -fWidth ) + m_vPos,
                         VECT( fDepth * iA,          ( iA + 1 ) * fHeight, -fWidth ) + m_vPos );
    }

    // Right
    NewTriangleFace( VECT( fDepth * iA, 0,            0       ) + m_vPos,
                     VECT( fDepth * iA, iA * fHeight, 0       ) + m_vPos,
                     VECT( fDepth * iA, iA * fHeight, -fWidth ) + m_vPos );
    NewTriangleFace( VECT( fDepth * iA, 0,            0       ) + m_vPos,
                     VECT( fDepth * iA, iA * fHeight, -fWidth ) + m_vPos,
                     VECT( fDepth * iA, 0,            -fWidth ) + m_vPos );

    // Bottom
    NewTriangleFace( VECT( 0,           0, 0       ) + m_vPos,
                     VECT( fDepth * iA, 0, 0       ) + m_vPos,
                     VECT( fDepth * iA, 0, -fWidth ) + m_vPos );
    NewTriangleFace( VECT( 0,           0, 0       ) + m_vPos,
                     VECT( fDepth * iA, 0, -fWidth ) + m_vPos,
                     VECT( 0,           0, -fWidth ) + m_vPos );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: NewSpiralStairs()
// Desc: 
//-----------------------------------------------------------------------------
bool CBrush::NewSpiralStairs()
{
    TRACE_ENTER_FN( CBrush::NewSpiralStairs )

    // Make things easy by making the variable names shorter
    int   iCount   = 0;
    int   iSteps   = SE->m_iBrush_Spiral_NumSteps;
    float fHeight  = SE->m_fBrush_Spiral_Height;
    float fWidth   = SE->m_fBrush_Spiral_Width;
    float fInner   = SE->m_fBrush_Spiral_InnerRadius;
    float fDegrees = SE->m_fBrush_Spiral_Degrees;
    float pi = 3.141592f;
    float fStep;

    if( !AllocateFaceSpace( ( iSteps * 5 + 1 ) * 2 ) ) LOG_ERR_RET( false )

    // The number of degrees that each step is rotated
    fStep = ( ( fDegrees / 360.0f ) * 2 * pi ) / (float)iSteps;

    // Create each step
    for( float fA = 0; iCount < iSteps; fA += fStep, iCount++ )
    {
        // Left             X                                     Y                           Z
        NewTriangleFace( VECT( fInner * sinf( fA         ), fHeight *   iCount,       fInner * cosf( fA         ) * 1 ) + m_vPos,
                         VECT( fInner * sinf( fA         ), fHeight * ( iCount + 1 ), fInner * cosf( fA         ) * 1 ) + m_vPos,
                         VECT( fWidth * sinf( fA         ), fHeight * ( iCount + 1 ), fWidth * cosf( fA         ) * 1 ) + m_vPos );
        NewTriangleFace( VECT( fInner * sinf( fA         ), fHeight *   iCount,       fInner * cosf( fA         ) * 1 ) + m_vPos,
                         VECT( fWidth * sinf( fA         ), fHeight * ( iCount + 1 ), fWidth * cosf( fA         ) * 1 ) + m_vPos,
                         VECT( fWidth * sinf( fA         ), fHeight *   iCount,       fWidth * cosf( fA         ) * 1 ) + m_vPos );

        // Top
        NewTriangleFace( VECT( fInner * sinf( fA + fStep ), fHeight * ( iCount + 1 ), fInner * cosf( fA + fStep ) * 1 ) + m_vPos,
                         VECT( fInner * sinf( fA         ), fHeight * ( iCount + 1 ), fInner * cosf( fA         ) * 1 ) + m_vPos,
                         VECT( fWidth * sinf( fA         ), fHeight * ( iCount + 1 ), fWidth * cosf( fA         ) * 1 ) + m_vPos );
        NewTriangleFace( VECT( fInner * sinf( fA + fStep ), fHeight * ( iCount + 1 ), fInner * cosf( fA + fStep ) * 1 ) + m_vPos,
                         VECT( fWidth * sinf( fA         ), fHeight * ( iCount + 1 ), fWidth * cosf( fA         ) * 1 ) + m_vPos,
                         VECT( fWidth * sinf( fA + fStep ), fHeight * ( iCount + 1 ), fWidth * cosf( fA + fStep ) * 1 ) + m_vPos );

        // Bottom
        NewTriangleFace( VECT( fInner * sinf( fA + fStep ), 0,                        fInner * cosf( fA + fStep ) * 1 ) + m_vPos,
                         VECT( fInner * sinf( fA         ), 0,                        fInner * cosf( fA         ) * 1 ) + m_vPos,
                         VECT( fWidth * sinf( fA         ), 0,                        fWidth * cosf( fA         ) * 1 ) + m_vPos );
        NewTriangleFace( VECT( fInner * sinf( fA + fStep ), 0,                        fInner * cosf( fA + fStep ) * 1 ) + m_vPos,
                         VECT( fWidth * sinf( fA         ), 0,                        fWidth * cosf( fA         ) * 1 ) + m_vPos,
                         VECT( fWidth * sinf( fA + fStep ), 0,                        fWidth * cosf( fA + fStep ) * 1 ) + m_vPos );
        // Back
        NewTriangleFace( VECT( fInner * sinf( fA + fStep ), fHeight * ( iCount + 1 ), fInner * cosf( fA + fStep ) * 1 ) + m_vPos,
                         VECT( fInner * sinf( fA         ), fHeight * ( iCount + 1 ), fInner * cosf( fA         ) * 1 ) + m_vPos,
                         VECT( fInner * sinf( fA         ), 0,                        fInner * cosf( fA         ) * 1 ) + m_vPos );
        NewTriangleFace( VECT( fInner * sinf( fA + fStep ), fHeight * ( iCount + 1 ), fInner * cosf( fA + fStep ) * 1 ) + m_vPos,
                         VECT( fInner * sinf( fA         ), 0,                        fInner * cosf( fA         ) * 1 ) + m_vPos,
                         VECT( fInner * sinf( fA + fStep ), 0,                        fInner * cosf( fA + fStep ) * 1 ) + m_vPos );
        // Front
        NewTriangleFace( VECT( fWidth * sinf( fA + fStep ), fHeight * ( iCount + 1 ), fWidth * cosf( fA + fStep ) * 1 ) + m_vPos,
                         VECT( fWidth * sinf( fA         ), fHeight * ( iCount + 1 ), fWidth * cosf( fA         ) * 1 ) + m_vPos,
                         VECT( fWidth * sinf( fA         ), 0,                        fWidth * cosf( fA         ) * 1 ) + m_vPos );

        NewTriangleFace( VECT( fWidth * sinf( fA + fStep ), fHeight * ( iCount + 1 ), fWidth * cosf( fA + fStep ) * 1 ) + m_vPos,
                         VECT( fWidth * sinf( fA         ), 0,                        fWidth * cosf( fA         ) * 1 ) + m_vPos,
                         VECT( fWidth * sinf( fA + fStep ), 0,                        fWidth * cosf( fA + fStep ) * 1 ) + m_vPos );
    }

    // Right
    NewTriangleFace( VECT( fInner * sinf( fA ), 0,                fInner * cosf( fA ) * 1 ) + m_vPos,
                     VECT( fInner * sinf( fA ), fHeight * iCount, fInner * cosf( fA ) * 1 ) + m_vPos,
                     VECT( fWidth * sinf( fA ), fHeight * iCount, fWidth * cosf( fA ) * 1 ) + m_vPos );

    NewTriangleFace( VECT( fInner * sinf( fA ), 0,                fInner * cosf( fA ) * 1 ) + m_vPos,
                     VECT( fWidth * sinf( fA ), fHeight * iCount, fWidth * cosf( fA ) * 1 ) + m_vPos,
                     VECT( fWidth * sinf( fA ), 0,                fWidth * cosf( fA ) * 1 ) + m_vPos );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: Save()
// Desc: 
//-----------------------------------------------------------------------------
bool CBrush::Save( const char* File, ofstream out )
{
    TRACE_ENTER_FN( CBrush::Save )

    Log( "CBrush::Save()\n" );

    out << (int)m_eBrushType   << '\n';

    TRACE_LEAVE_FN()

    return true;
}


//-----------------------------------------------------------------------------
// Name: Open()
// Desc: 
//-----------------------------------------------------------------------------
bool CBrush::Open( const char* File, ifstream in )
{
    TRACE_ENTER_FN( CBrush::Open )

    Log( "CBrush::Open()\n" );

    int iBrushType;

    in >> iBrushType;
    m_eBrushType = GetBrushEnumType( iBrushType );

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: RenderSelected()
// Desc: 
//-----------------------------------------------------------------------------
bool CBrush::RenderSelected()
{
    TRACE_ENTER_FN( CBrush::RenderSelected )

    if( F( LU->GetFace() ) == NULL ) { LOG_VAR( LU->GetFace() ) LOG_ERR_RET( false ) }

    for( int iVert = 0; iVert < F( LU->GetFace() )->GetLoadedVerts(); iVert++ )
    {
        LU->StartNaming();

        LU->Include( LU->GetPoly(), LU->GetFace(), iVert, NA, true, true, false );

        CVertex* pVert = V( LU->GetFace(), iVert );

        if( pVert == NULL ) { LOG_VAR( LU->GetFace() ) LOG_VAR( iVert ) LOG_ERR_RET( false ) }

        if( LU->GetVert() == iVert )
            glColor3f( 1.0f, 0.0f, 0.0f );
        else
            glColor3f( 1.0f, 1.0f, 1.0f );

        glPushMatrix();
        glTranslatef( pVert->GetVertPos().x, pVert->GetVertPos().y, pVert->GetVertPos().z );
        gluSphere( m_pQuadricObj, 0.1f, 16, 16 );
        glPopMatrix();

        LU->EndNaming();
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: NewCylinder()
// Desc: 
//-----------------------------------------------------------------------------
bool CBrush::NewCylinder()
{
    TRACE_ENTER_FN( CBrush::NewCylinder )

    int   iCount    = 0;
    int   fHeight   = SE->m_fBrush_Cylinder_Height;
    float fRadius   = SE->m_fBrush_Cylinder_Radius;
    int   iNumSides = SE->m_iBrush_Cylinder_NumSides;
    float fPi       = 3.1415926535897932384626433832795f;
    float fStep;

    if( !AllocateFaceSpace( iNumSides * 2 ) ) LOG_ERR_RET( false )

    // The number of degrees that each step is rotated
    fStep = ( 2 * fPi ) / (float)iNumSides;

    // Create each step
    for( float fA = 0; iCount < iNumSides; fA += fStep, iCount++ )
    {
        // Front
        NewTriangleFace( VECT( fRadius * sinf( fA         ), 0,       fRadius * cosf( fA         ) * 1 ) + m_vPos,
                         VECT( fRadius * sinf( fA         ), fHeight, fRadius * cosf( fA         ) * 1 ) + m_vPos,
                         VECT( fRadius * sinf( fA + fStep ), fHeight, fRadius * cosf( fA + fStep ) * 1 ) + m_vPos );

        NewTriangleFace( VECT( fRadius * sinf( fA + fStep ), 0,       fRadius * cosf( fA + fStep ) * 1 ) + m_vPos,
                         VECT( fRadius * sinf( fA         ), 0,       fRadius * cosf( fA         ) * 1 ) + m_vPos,
                         VECT( fRadius * sinf( fA + fStep ), fHeight, fRadius * cosf( fA + fStep ) * 1 ) + m_vPos );
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: NewCircle()
// Desc: 
//-----------------------------------------------------------------------------
bool CBrush::NewCircle()
{
    TRACE_ENTER_FN( CBrush::NewCircle )

    int   iCount    = 0;
    float fRadius   = SE->m_fBrush_Circle_Radius;
    float fDegrees  = SE->m_fBrush_Circle_Degrees;
    int   iNumSides = SE->m_iBrush_Circle_NumSides;
    float fPi       = 3.1415926535897932384626433832795f;
    float fStep;

    if( !AllocateFaceSpace( iNumSides ) ) LOG_ERR_RET( false )

    // The number of degrees that each step is rotated
    fStep = ( ( fDegrees / 360.0f ) * 2 * fPi ) / (float)iNumSides;

    // Create each step
    for( float fA = 0; iCount < iNumSides; fA += fStep, iCount++ )
    {
        // Front
        NewTriangleFace( VECT( fRadius * sinf( fA + fStep ), 0, fRadius * cosf( fA + fStep ) * 1 ) + m_vPos,
                         VECT( fRadius * sinf( fA         ), 0, fRadius * cosf( fA         ) * 1 ) + m_vPos,
                         VECT( 0, 0, 0 ) + m_vPos );
    }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: NewCustomBrush()
// Desc: 
//-----------------------------------------------------------------------------
bool CBrush::NewCustomBrush( CFace* pF )
{
    TRACE_ENTER_FN( CBrush::NewCustomBrush )

    VECT vN;
    int iV;

    if( pF == NULL ) { LOG_ERR_RET( false ) }

    const int iNumVerts = pF->GetLoadedVerts();

    if( !AllocateFaceSpace( ( iNumVerts ) * 2 + ( iNumVerts - 2 ) * 2 ) ) LOG_ERR_RET( false )

    vN = pF->GetFaceNormal() * SE->m_fGeometry_ExpandAmount;
    
    for( iV = 0; iV < iNumVerts - 1; iV++ )
    {
        NewTriangleFace( pF->V( iV + 0 )->GetVertPos(),
                         pF->V( iV + 1 )->GetVertPos(),
                         pF->V( iV + 1 )->GetVertPos() + vN );
        NewTriangleFace( pF->V( iV + 0 )->GetVertPos(),
                         pF->V( iV + 1 )->GetVertPos() + vN,
                         pF->V( iV + 0 )->GetVertPos() + vN );
    }

    NewTriangleFace( pF->V( iV + 0 )->GetVertPos(),
                     pF->V( 0      )->GetVertPos(),
                     pF->V( 0      )->GetVertPos() + vN );

    NewTriangleFace( pF->V( iV + 0 )->GetVertPos(),
                     pF->V( 0      )->GetVertPos() + vN,
                     pF->V( iV + 0 )->GetVertPos() + vN );

    for( iV = 1; iV < iNumVerts - 1; iV++ )
    {
        NewTriangleFace( pF->V( 0      )->GetVertPos() + vN,
                         pF->V( iV + 0 )->GetVertPos() + vN,
                         pF->V( iV + 1 )->GetVertPos() + vN );
    }

    RET( true )
}

//-----------------------------------------------------------------------------
// Name: BrushRebuild()
// Desc: 
//-----------------------------------------------------------------------------
bool CBrush::BrushRebuild()
{
    TRACE_ENTER_FN( CBrush::BrushRebuild )

    float* pVerts = new float[ GetLoadedFaces() * 3 * 3 ];
    float* pColor = new float[ GetLoadedFaces() * 3 * 3 ];
    int iIndex = 0;

    for( int iF = 0; iF < GetLoadedFaces(); iF++ )
    {
        CFace* pF = F( iF );

        if( pF == NULL ) { LOG_VAR( iF ) LOG_ERR_RET( false ) }


        for( int b = 0; b < 3; b++ )
        {
            VECT v = pF->V( b )->GetVertPos();

            pColor[ iIndex + 0 ] = 0.5f;
            pColor[ iIndex + 1 ] = 1.0f;
            pColor[ iIndex + 2 ] = 0.5f;

            pVerts[ iIndex + 0 ] = v.x;
            pVerts[ iIndex + 1 ] = v.y;
            pVerts[ iIndex + 2 ] = v.z;

            iIndex += 3;
        }
    }

    SetVertexArray( pVerts );
    SetColorArray(  pColor );

    RET( true )
}
