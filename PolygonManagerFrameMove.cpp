//----------------------------------------------------------------------------- 
// File: PoFrameMove.cpp
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
#include "Face.h"
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Updates the polygons based on new input
//-----------------------------------------------------------------------------
void CPolygonManager::FrameMove( const float fElapsedTime )
{
    TRACE_ENTER_FN( CPolygonManager::FrameMove )
    
    static bool bXPressed = false;

    if( LU->PolySelected() && LU->FaceSelected() )
    {
        CFace* pF = F( LU->GetPoly(), LU->GetFace() );

        if( pF != NULL )
        {
            HTEXTURE hTex;
            hTex.id = 0;

            if( IM->KeyPressed( DIK_1 ) )
            {
                hTex.id = 0;
                pF->SetTexture2( hTex );
            }

            if( IM->KeyPressed( DIK_2 ) )
            {
                hTex.id = 1;
                pF->SetTexture2( hTex );
            }

            if( IM->KeyPressed( DIK_3 ) )
            {
                hTex.id = 2;
                pF->SetTexture2( hTex );
            }

            if( IM->KeyPressed( DIK_4 ) )
            {
                hTex.id = 3;
                pF->SetTexture2( hTex );
            }

            if( IM->KeyPressed( DIK_5 ) )
            {
                hTex.id = 4;
                pF->SetTexture2( hTex );
            }
        }
    }

    FrameMoveShiftPressed(   fElapsedTime );
    FrameMoveCtrlPressed(    fElapsedTime );
    FrameMoveCapitalPressed( fElapsedTime );
    FrameMoveQPressed(       fElapsedTime );
    FrameMoveLPressed(       fElapsedTime );
    FrameMoveFPressed(       fElapsedTime );
    FrameMoveGeometry(       fElapsedTime );

    //static bool bXPressed = false;

    if( IM->KeyPressed( DIK_X ) && LU->LightSelected() && !bXPressed )
    { 
        LM->RemoveLight(); 
        bXPressed = true; 
    }
    else if( !IM->KeyPressed( DIK_X ) ) 
        bXPressed = false;

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: FrameMoveQPressed()
// Desc:
//-----------------------------------------------------------------------------
void CPolygonManager::FrameMoveQPressed( const float fElapsedTime )
{
    TRACE_ENTER_FN( CPolygonManager::FrameMoveQPressed )

    static bool bQPressed = false;

    if( IM->KeyPressed( DIK_Q ) && LU->FaceSelected() && !bQPressed )
    { 
        DeleteSelectedFace(); 
        bQPressed = true; 
    }
    else if( !IM->KeyPressed( DIK_Q ) ) 
        bQPressed = false;

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: FrameMoveQPressed()
// Desc:
//-----------------------------------------------------------------------------
void CPolygonManager::FrameMoveLPressed( const float fElapsedTime )
{
    TRACE_ENTER_FN( CPolygonManager::FrameMoveLPressed )

    static bool bLPressed = false;

    if( IM->KeyPressed( DIK_L ) &&
        LU->FaceSelected() &&
        LU->PolySelected() &&
        !bLPressed )
    { 
        const int iP = LU->GetPoly();
        const int iF = LU->GetFace();

        LM->AddLight( iP, iF, VECT( 0, 1, 1 ) );
        bLPressed = true; 
    }
    else if( !IM->KeyPressed( DIK_L ) ) 
        bLPressed = false;

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: FrameMoveQPressed()
// Desc:
//-----------------------------------------------------------------------------
void CPolygonManager::FrameMoveFPressed( const float fElapsedTime )
{
    TRACE_ENTER_FN( CPolygonManager::FrameMoveFPressed )

    static bool bFPressed = false;

    if( IM->KeyPressed( DIK_F ) && LU->FaceSelected() && !bFPressed )
    {
        NextFace();
        bFPressed = true;
    }
    else if( !IM->KeyPressed( DIK_F ) )
        bFPressed = false;

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: FrameMoveShiftPressed()
// Desc: When shift is held down and a vertex is selected, holding the mouse
//       buttons and dragging the mouse will move the vertex in a direction.
//-----------------------------------------------------------------------------
void CPolygonManager::FrameMoveShiftPressed( const float fElapsedTime )
{
    TRACE_ENTER_FN( CPolygonManager::FrameMoveShiftPressed )

    static bool bLShiftPressed = false;

        // If left shift is held down, then it moves a single selected vertex
    if( IM->KeyPressed( DIK_LSHIFT ) )
    {
        if( LU->VertSelected() )
        {
            bLShiftPressed = true;
             if( IM->GetMouseButtonLeft() && IM->GetMouseButtonRight() )
            {
                if( TranslateVertZ( (float)IM->GetMouseY() / 100 ) )
                    IM->SetFreeze( true );
            }
            else if( IM->GetMouseButtonLeft() )
            {
                if( TranslateVertX( (float)IM->GetMouseX() / 100 ) )
                    IM->SetFreeze( true );
            }
            else if( IM->GetMouseButtonRight() )
            {
                if( TranslateVertY( -(float)IM->GetMouseY() / 100 ) )
                    IM->SetFreeze( true );
            }
        }
    }
    else
    {
        if( bLShiftPressed ) PM->ResetVirtualCoords();
        bLShiftPressed = false;
    }

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: FrameMoveCtrlPressed()
// Desc: When Ctrl is held down and the mouse is dragged, the entire polygon
//       is moved in a direction.
//-----------------------------------------------------------------------------
void CPolygonManager::FrameMoveCtrlPressed( const float fElapsedTime )
{
    TRACE_ENTER_FN( CPolygonManager::FrameMoveCtrlPressed )

    static bool bLControlPressed = false;

    if( !LU->PolySelected() && !LU->BrushSelected() && !LU->LightSelected() )
    { TRACE_LEAVE_FN() return; }

    // If left control is held down, then it moves a single selected polygon
    if( IM->KeyPressed( DIK_LCONTROL ) )
    {
        if( LU->PolySelected() || LU->BrushSelected() )
        {
            if( !bLControlPressed && LU->VertSelected() )
                PM->ResetVirtualCoords();

            bLControlPressed = true;

            if( IM->GetMouseButtonLeft() && IM->GetMouseButtonRight() )
            {
                if( TranslatePolyZ( (float)IM->GetMouseY() / 100 ) )
                    IM->SetFreeze( true );
            }
            else if( IM->GetMouseButtonLeft() )
            {
                if( TranslatePolyX( (float)IM->GetMouseX() / 100 ) )
                    IM->SetFreeze( true );
            }
            else if( IM->GetMouseButtonRight() )
            {
                if( TranslatePolyY( -(float)IM->GetMouseY() / 100 ) )
                    IM->SetFreeze( true );
            }
        }

        if( LU->LightSelected() )
        {
            if( !bLControlPressed )
                LM->ResetVirtualCoords();

            bLControlPressed = true;

            if( IM->GetMouseButtonLeft() && IM->GetMouseButtonRight() )
            {
                if( LM->TranslateLightZ( (float)IM->GetMouseY() / 100 ) )
                    IM->SetFreeze( true );
            }
            else if( IM->GetMouseButtonLeft() )
            {
                if( LM->TranslateLightX( (float)IM->GetMouseX() / 100 ) )
                    IM->SetFreeze( true );
            }
            else if( IM->GetMouseButtonRight() )
            {
                if( LM->TranslateLightY( -(float)IM->GetMouseY() / 100 ) )
                    IM->SetFreeze( true );
            }
        }
    }
    else
    {
        if( bLControlPressed && LU->VertSelected() )
            PM->ResetVirtualCoords();

        bLControlPressed = false;
    }

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: FrameMoveCapitalPressed()
// Desc:
//-----------------------------------------------------------------------------
void CPolygonManager::FrameMoveCapitalPressed( const float fElapsedTime )
{
    TRACE_ENTER_FN( CPolygonManager::FrameMoveCapitalPressed )

    static bool bLCapitalPressed = false;

    if( !LU->PolySelected() && !LU->BrushSelected() )
    { TRACE_LEAVE_FN() return; }

    if( !LU->FaceSelected() ) { TRACE_LEAVE_FN() return; }

    if( IM->KeyPressed( DIK_CAPITAL ) )
    {
        if( LU->FaceSelected() )
        {
            if( !bLCapitalPressed && LU->VertSelected() )
                PM->ResetVirtualCoords();

            bLCapitalPressed = true;
            if( IM->GetMouseButtonLeft() && IM->GetMouseButtonRight() )
            {
                if( TranslateFaceZ( (float)IM->GetMouseY() / 100 ) )
                    IM->SetFreeze( true );
            }
            else if( IM->GetMouseButtonLeft() )
            {
                if( TranslateFaceX( (float)IM->GetMouseX() / 100 ) )
                    IM->SetFreeze( true );
            }
            else if( IM->GetMouseButtonRight() )
            {
                if( TranslateFaceY( -(float)IM->GetMouseY() / 100 ) )
                    IM->SetFreeze( true );
            }
        }
    }
    else
    {
        if( bLCapitalPressed )
            PM->ResetVirtualCoords();

        bLCapitalPressed = false;
    }

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: FrameMoveGeometry()
// Desc:
//-----------------------------------------------------------------------------
void CPolygonManager::FrameMoveGeometry( const float fElapsedTime )
{
    TRACE_ENTER_FN( CPolygonManager::FrameMoveGeometry )

    CPolygon* pP;
    CFace*    pF;
    CVertex*  pV;

    if( !Settings->m_bEditor_UpdatePolys &&
        !Settings->m_bEditor_UpdateFaces &&
        !Settings->m_bEditor_UpdateVerts )
    {
        TRACE_LEAVE_FN()
        return;
    }

    // Every Polygon
    for( int iP = 0; iP < GetLoadedPolygons(); iP++ )
    {
        if( Settings->m_bEditor_UpdatePolys )
        {
            pP = P( iP );

            if( pP == NULL ) {
                Log( "if( pP == NULL ) @ CPolygonManager::FrameMoveGeometry()\n" );
                TRACE_LEAVE_FN() return; }

            pP->FrameMove( fElapsedTime );
        }

        // Every face
        for( int iF = 0; iF < P( iP )->GetLoadedFaces(); iF++ )
        {
            if( Settings->m_bEditor_UpdateFaces )
            {
                pF = F( iP, iF );

                if( pF == NULL ) {
                    Log( "if( pF == NULL ) @ CPolygonManager::FrameMoveGeometry()\n" );
                    TRACE_LEAVE_FN() return; }

                pF->FrameMove( fElapsedTime );
            }

            // Every Vert
            for( int iV = 0; iV < F( iP, iF )->GetLoadedVerts(); iV++ )
            {
                if( Settings->m_bEditor_UpdateVerts )
                {
                    pV = V( iP, iF, iV );

                    if( pV == NULL ) {
                        Log( "if( pV == NULL ) @ CPolygonManager::FrameMoveGeometry()\n" );
                        TRACE_LEAVE_FN() return; }

                    pV->FrameMove( fElapsedTime );
                }
            }
        }
    }

    TRACE_LEAVE_FN()
}