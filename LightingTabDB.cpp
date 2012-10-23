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
//#include <FreeImage.h>
#include "PolygonManager.h"
#include "CustomMsg.h"
#include "main.h"
#include "resource.h"
#include "MessageProcs.h"
#include "LightingTabDB.h"
#include "Settings.h"
#include "LightManager.h"
#include "Tracer.h"
//-----------------------------------------------------------------------------

//#pragma comment(lib, "FreeImage.lib")

//-----------------------------------------------------------------------------
// Name: OnLightingTab_WMInitDialog()
// Desc: 
//-----------------------------------------------------------------------------
void OnLightingTab_WMInitDialog( HWND hWnd )
{
    SetEditI( hWnd, IDC_EDIT_TOTAL_BOUNCES,              SE->m_iLighting_LightmapLOD );
    SetEditF( hWnd, IDC_EDIT_LIGHT_INTENSITY,            SE->m_fLighting_LightIntensity );
    SetEditF( hWnd, IDC_EDIT_LIGHT_REFLECTION_INTENSITY, SE->m_fLighting_ReflectionIntensity );
    SetCheck( hWnd, IDC_CHECK_SHOW_HEMICUBE,             SE->m_bLighting_ShowHemicube );
    SetEditF( hWnd, IDC_EDIT_LIGHTMAP_THRESHOLD,         SE->m_fLighting_Threshold );

    
    SendDlgItemMessage( hWnd, IDC_PROGRESS_LIGHTING, PBM_SETRANGE, 0, (LPARAM) MAKELONG( 0, 100 ) );
    SendDlgItemMessage( hWnd, IDC_PROGRESS_LIGHTING, PBM_SETSTEP, 1, 0 );
    
    // The light trackbars
    SendDlgItemMessage( hWnd, IDC_SLIDER_LIGHT_RED,       TBM_SETRANGE, 1, MAKELONG( 0, 255 ) );
    SendDlgItemMessage( hWnd, IDC_SLIDER_LIGHT_GREEN,     TBM_SETRANGE, 1, MAKELONG( 0, 255 ) );
    SendDlgItemMessage( hWnd, IDC_SLIDER_LIGHT_BLUE,      TBM_SETRANGE, 1, MAKELONG( 0, 255 ) );
    SendDlgItemMessage( hWnd, IDC_SLIDER_LIGHT_INTENSITY, TBM_SETRANGE, 1, MAKELONG( 1, 1000 ) );

    SetCheck( hWnd, IDC_RADIO_HIGH_QUALITY_LIGHTING,   SE->m_bLighting_BestQuality   );
    SetCheck( hWnd, IDC_RADIO_NORMAL_QUALITY_LIGHTING, SE->m_bLighting_NormalQuality );

    if( Lookup->LightSelected() ) UpdateSelectedLightData( hWnd );

    ShowWindow( hWnd, true );
    UpdateWindow( hWnd );
}


//-----------------------------------------------------------------------------
// Name: OnLightingTab_WMCommand()
// Desc: 
//-----------------------------------------------------------------------------
void OnLightingTab_WMCommand( HWND hWnd, WPARAM wParam )
{
    switch( LOWORD( wParam ) )
    {
        case IDC_BUTTON_CALCULATE_LIGHTMAPS: OnCalculateLightmaps( hWnd ); break;
        case IDC_BUTTON_APPLY_LIGHT_NAME:    SetLightName( hWnd );         break;
    }
}


//-----------------------------------------------------------------------------
// Name: OnLightingTab_WMDestroy()
// Desc: 
//-----------------------------------------------------------------------------
void OnLightingTab_WMDestroy( HWND hWnd )
{
    UpdateLightingData( hWnd );
    EndDialog( hWnd, 0 );
}



//-----------------------------------------------------------------------------
// Name: UpdateLightingData()
// Desc: 
//-----------------------------------------------------------------------------
void UpdateLightingData( HWND hWnd )
{
    SE->m_iLighting_LightmapLOD         = GetEditI( hWnd, IDC_EDIT_TOTAL_BOUNCES );
    SE->m_fLighting_LightIntensity      = GetEditF( hWnd, IDC_EDIT_LIGHT_INTENSITY );
    SE->m_fLighting_ReflectionIntensity = GetEditF( hWnd, IDC_EDIT_LIGHT_REFLECTION_INTENSITY );
    SE->m_bLighting_ShowHemicube        = GetCheck( hWnd, IDC_CHECK_SHOW_HEMICUBE );
    SE->m_bLighting_BestQuality         = GetCheck( hWnd, IDC_RADIO_HIGH_QUALITY_LIGHTING );
    SE->m_bLighting_NormalQuality       = GetCheck( hWnd, IDC_RADIO_NORMAL_QUALITY_LIGHTING );
    SE->m_fLighting_Threshold           = GetEditF( hWnd, IDC_EDIT_LIGHTMAP_THRESHOLD );
}


//-----------------------------------------------------------------------------
// Name: SetLightingBarPos()
// Desc: 
//-----------------------------------------------------------------------------
void SetLightingBarPos( HWND hWnd, const int iPos )
{
    SendDlgItemMessage( hWnd, IDC_PROGRESS_LIGHTING, PBM_SETPOS, iPos, 0 );
}


//-----------------------------------------------------------------------------
// Name: DisplayHemiCube()
// Desc: Displays the hemicube on the lighting window
//-----------------------------------------------------------------------------
bool DisplayHemiCube( HWND hWnd )
{
    TRACE_ENTER_FN( DisplayHemiCube )

    LM->GetHemicube()->GenerateScreens();

    // Forward
    if( !DisplayImage( hWnd, "0.tga", 80,      315,      64, 64 ) ) {
        Log( "if( !DisplayImage( 0 ) @ DisplayHemiCube()\n" );
        TRACE_LEAVE_FN() return false; }

    // Left
    if( !DisplayImage( hWnd, "1.tga", 80 - 64, 315,      64, 64 ) ) {
        Log( "if( !DisplayImage( 1 ) @ DisplayHemiCube()\n" );
        TRACE_LEAVE_FN() return false; }

    // Right
    if( !DisplayImage( hWnd, "2.tga", 80 + 64, 315,      64, 64 ) ) {
        Log( "if( !DisplayImage( 2 ) @ DisplayHemiCube()\n" );
        TRACE_LEAVE_FN() return false; }

    // Up
    if( !DisplayImage( hWnd, "3.tga", 80,      315 - 64, 64, 64 ) ) {
        Log( "if( !DisplayImage( 3 ) @ DisplayHemiCube()\n" );
        TRACE_LEAVE_FN() return false; }

    // Down
    if( !DisplayImage( hWnd, "4.tga", 80,      315 + 64, 64, 64 ) ) {
        Log( "if( !DisplayImage( 4 ) @ DisplayHemiCube()\n" );
        TRACE_LEAVE_FN() return false; }

    TRACE_LEAVE_FN()

    return true;
}


//-----------------------------------------------------------------------------
// Name: DisplayImage()
// Desc: Displays a TGA image on a window
//-----------------------------------------------------------------------------
bool DisplayImage( HWND hWnd,        const char* strFileName,
                   const int iX,     const int iY,
                   const int iWidth, const int iHeight )
{
    TRACE_ENTER_FN( DisplayImage )
/*
//    FIBITMAP* targa = NULL;
    HDC hDC = NULL;
    HDC memDC = NULL;
    HBITMAP  hBitmap = NULL;

    // Convert the tga to a bitmap so windows can load it
    targa = FreeImage_LoadTARGA( strFileName );

    if( targa == NULL ) {
        Log( "if( targa == NULL ) @ DisplayImage()\n" );
        TRACE_LEAVE_FN() return false; }

    if( !FreeImage_SaveBMP( targa, "converted.bmp" ) ) {
        Log( "if( !FreeImage_SaveBMP() @ DisplayImage()\n" );
        TRACE_LEAVE_FN() return false; }

    // Get the bitmap's handle
    hBitmap = (HBITMAP)LoadImage( NULL, "converted.bmp", IMAGE_BITMAP, iWidth, iHeight, LR_LOADFROMFILE | LR_CREATEDIBSECTION );

    // If the handle is null, repost the error
    if( hBitmap == NULL )
    {
        static bool bFirstError = true;
        if( bFirstError ) MessageBox( hWnd, "if( hBitmap == NULL ) @ DisplayImage()", strFileName, MB_OK );
        bFirstError = true;
        Log( "if( hBitmap == NULL ) @ DisplayImage()\n" );
        TRACE_LEAVE_FN() return false;
    }

    hDC   = GetDC( hWnd );
    memDC = CreateCompatibleDC( hDC );

    SelectObject( memDC, hBitmap );

    // Draw it
    if( !BitBlt( hDC, iX, iY, iWidth, iHeight, memDC, 0, 0, SRCCOPY ) )
    {
        static bool bFirstError = true;
        if( bFirstError ) MessageBox( hWnd, "if( !BitBlt() )", "DisplayImage()", MB_OK );
        bFirstError = true;
        Log( "if( !BitBlt() ) @ DisplayImage()\n" );
        TRACE_LEAVE_FN() return false; }

    ReleaseDC( hWnd, hDC );
    DeleteDC( memDC );
    DeleteObject( hBitmap );
*/
    TRACE_LEAVE_FN()

    return true;
}


//-----------------------------------------------------------------------------
// Name: UpdateWindowUpdateStats()
// Desc: Puts all the data in the lighting window when doing radiosity
//       computations
//-----------------------------------------------------------------------------
void UpdateWindowStats( HWND hWnd,
                        const int iCurrentPolygon,
                        const int iCurrentFace,
                        const int iTotalPolygons,
                        const int iTotalFaces,
                        const int iCoordU,
                        const int iCoordV,
                        const int iRes,
                        const int iCurrentStep,
                        const int iTotalSteps,
                        const VECT vColor )
{
    TRACE_ENTER_FN( UpdateWindowStats )

    const int iPercent = (float)iCurrentStep / (float)iTotalSteps * 100.0f;

    SetEditI( hWnd, IDC_EDIT_CURRENT_POLYGON,       iCurrentPolygon );
    SetEditI( hWnd, IDC_EDIT_CURRENT_FACE,          iCurrentFace    );
    SetEditI( hWnd, IDC_EDIT_TOTAL_POLYGONS,        iTotalPolygons  );
    SetEditI( hWnd, IDC_EDIT_TOTAL_FACES,           iTotalFaces     );
    SetEditI( hWnd, IDC_EDIT_CURRENT_U,             iCoordU         );
    SetEditI( hWnd, IDC_EDIT_CURRENT_V,             iCoordV         );
    SetEditI( hWnd, IDC_EDIT_LIGHTMAP_RES,          iRes            );
    SetEditI( hWnd, IDC_EDIT_CURRENT_PATCH,         iCurrentStep    );
    SetEditI( hWnd, IDC_EDIT_CURRENT_TOTAL_PATCHES, iTotalSteps     );
    SetEditI( hWnd, IDC_EDIT_LIGHT_PERCENT,         iPercent        );
    SetEditF( hWnd, IDC_EDIT_LIGHTING_R,            vColor.x        );
    SetEditF( hWnd, IDC_EDIT_LIGHTING_G,            vColor.y        );
    SetEditF( hWnd, IDC_EDIT_LIGHTING_B,            vColor.z        );

    SendDlgItemMessage( hWnd, IDC_PROGRESS_LIGHTING, PBM_SETPOS, iPercent, 0 );

    UpdateWindow( hWnd );

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: UpdateSelectedLightData()
// Desc: Updates the lighting box in the lighting tab to reflct the selected
//       light
//-----------------------------------------------------------------------------
void UpdateSelectedLightData( HWND hWnd )
{
    TRACE_ENTER_FN( UpdateSelectedLightData )

    char str[256];

    LM->GetSelectedLightName( str );
    VECT vL           = LM->GetSelectedLightColor();
    float fBrightness = LM->GetSelectedLightBrightness();

    SetEditS( hWnd, IDC_EDIT_LIGHT_NAME, str );

    SetTrackPos( hWnd, IDC_SLIDER_LIGHT_RED  ,      (int)( vL.x * 255        ) );
    SetTrackPos( hWnd, IDC_SLIDER_LIGHT_GREEN,      (int)( vL.y * 255        ) );
    SetTrackPos( hWnd, IDC_SLIDER_LIGHT_BLUE ,      (int)( vL.z * 255        ) );
    SetTrackPos( hWnd, IDC_SLIDER_LIGHT_INTENSITY , (int)( fBrightness * 100 ) );

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: SetLightName()
// Desc: Changes the current light's name to what appears in the name box
//       on the lighting tab
//-----------------------------------------------------------------------------
void SetLightName( HWND hWnd )
{
    TRACE_ENTER_FN( SetLightName )

    char str[256];
    GetEditS( hWnd, IDC_EDIT_LIGHT_NAME, str, 256 );
    LM->SetSelectedLightName( str );

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: UpdateLightColor()
// Desc: Changes the light's color when sliding a color trackbar
//-----------------------------------------------------------------------------
void UpdateLightColor( HWND hWnd )
{
    TRACE_ENTER_FN( UpdateLightColor )

    const float iR = GetTrackPos( hWnd, IDC_SLIDER_LIGHT_RED        );
    const float iG = GetTrackPos( hWnd, IDC_SLIDER_LIGHT_GREEN      );
    const float iB = GetTrackPos( hWnd, IDC_SLIDER_LIGHT_BLUE       );
    const float iI = GetTrackPos( hWnd, IDC_SLIDER_LIGHT_INTENSITY  );

    LM->SetSelectedLightColor( VECT( iR, iG, iB ) / 255.0f, iI / 10.0f );

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: OnCalculateLightmaps()
// Desc: Invokes the light manager to compute the lighting for the scene
//-----------------------------------------------------------------------------
void OnCalculateLightmaps( HWND hWnd )
{
    TRACE_ENTER_FN( OnCalculateLightmaps )

    UpdateLightingData( hWnd );

    RADIOSITYMODE eMode = NOTHINGLIGHTING;

    if( SE->m_bLighting_BestQuality )
        eMode = BESTLIGHTING;
    else if( SE->m_bLighting_NormalQuality )
        eMode = FASTLIGHTING;

    LM->CalculateLighting( eMode );

    TRACE_LEAVE_FN()
}