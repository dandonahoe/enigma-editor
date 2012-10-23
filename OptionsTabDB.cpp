//----------------------------------------------------------------------------- 
// File: BrushTabDB.cpp
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#include <windows.h>
#include <commctrl.h>
#include "PolygonManager.h"
#include "CustomMsg.h"
#include "main.h"
#include "resource.h"
#include "MessageProcs.h"
#include "OptionsTabDB.h"
#include "Settings.h"
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Name: OnBrushTab_WMInitDialog()
// Desc: 
//-----------------------------------------------------------------------------
void OnOptionsTab_WMInitDialog( HWND hWnd )
{
    SetCheck( hWnd, IDC_CHECK_UPDATE_POLYS,    Settings->m_bEditor_UpdatePolys      );
    SetCheck( hWnd, IDC_CHECK_UPDATE_FACES,    Settings->m_bEditor_UpdateFaces      );
    SetCheck( hWnd, IDC_CHECK_UPDATE_VERTS,    Settings->m_bEditor_UpdateVerts      );
    SetCheck( hWnd, IDC_CHECK_RENDER_NORMALS,  Settings->m_bEditor_RenderNormals    );
    SetCheck( hWnd, IDC_CHECK_RENDER_BRUSH,    Settings->m_bEditor_RenderBrush      );
    SetCheck( hWnd, IDC_CHECK_RENDER_SELECTED, Settings->m_bEditor_RenderSelected   );
    SetCheck( hWnd, IDC_CHECK_RENDER_POLYGONS, Settings->m_bEditor_RenderPolygons   );
    SetCheck( hWnd, IDC_CHECK_USE_TEXTURING,   Settings->m_bEditor_UseTexturing     );
    SetCheck( hWnd, IDC_CHECK_USE_LIGHTING,    Settings->m_bEditor_UseLighting      );
    SetCheck( hWnd, IDC_CHECK_ACTIVE_XZ,       Settings->m_bEditor_LeftViewActive   );
    SetCheck( hWnd, IDC_CHECK_ACTIVE_XY,       Settings->m_bEditor_CenterViewActive );
    SetCheck( hWnd, IDC_CHECK_ACTIVE_YZ,       Settings->m_bEditor_RightViewActive  );
    SetCheck( hWnd, IDC_CHECK_ACTIVE_3D,       Settings->m_bEditor_3DViewActive     );
    SetCheck( hWnd, IDC_CHECK_RENDER_LIGHTS,   Settings->m_bEditor_RenderLights     ); 

    ShowWindow( hWnd, true );
    UpdateWindow( hWnd );
}


//-----------------------------------------------------------------------------
// Name: OnBrushTab_WMCommand()
// Desc: 
//-----------------------------------------------------------------------------
void OnOptionsTab_WMCommand( HWND hWnd, WPARAM wParam )
{
    switch( LOWORD( wParam ) )
    {
    case IDC_BUTTON_APPLYEDITOROPTIONS:
        UpdateOptionsData( hWnd );
        break;
    }
}


//-----------------------------------------------------------------------------
// Name: OnBrushTab_WMDestroy()
// Desc: 
//-----------------------------------------------------------------------------
void OnOptionsTab_WMDestroy( HWND hWnd )
{
    EndDialog( hWnd, 0 );
}



//-----------------------------------------------------------------------------
// Name: UpdateOptionsData()
// Desc: 
//-----------------------------------------------------------------------------
void UpdateOptionsData( HWND hWnd )
{
    Settings->m_bEditor_UpdatePolys      = GetCheck( hWnd, IDC_CHECK_UPDATE_POLYS    );
    Settings->m_bEditor_UpdateFaces      = GetCheck( hWnd, IDC_CHECK_UPDATE_FACES    );
    Settings->m_bEditor_UpdateVerts      = GetCheck( hWnd, IDC_CHECK_UPDATE_VERTS    );
    Settings->m_bEditor_RenderNormals    = GetCheck( hWnd, IDC_CHECK_RENDER_NORMALS  );
    Settings->m_bEditor_RenderBrush      = GetCheck( hWnd, IDC_CHECK_RENDER_BRUSH    );
    Settings->m_bEditor_RenderSelected   = GetCheck( hWnd, IDC_CHECK_RENDER_SELECTED );
    Settings->m_bEditor_RenderPolygons   = GetCheck( hWnd, IDC_CHECK_RENDER_POLYGONS );
    Settings->m_bEditor_UseTexturing     = GetCheck( hWnd, IDC_CHECK_USE_TEXTURING   );
    Settings->m_bEditor_UseLighting      = GetCheck( hWnd, IDC_CHECK_USE_LIGHTING    );
    Settings->m_bEditor_LeftViewActive   = GetCheck( hWnd, IDC_CHECK_ACTIVE_XZ       );
    Settings->m_bEditor_CenterViewActive = GetCheck( hWnd, IDC_CHECK_ACTIVE_XY       );
    Settings->m_bEditor_RightViewActive  = GetCheck( hWnd, IDC_CHECK_ACTIVE_YZ       );
    Settings->m_bEditor_3DViewActive     = GetCheck( hWnd, IDC_CHECK_ACTIVE_3D       );
    Settings->m_bEditor_RenderLights     = GetCheck( hWnd, IDC_CHECK_RENDER_LIGHTS   );
}
