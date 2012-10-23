//----------------------------------------------------------------------------- 
// File: PolygonLookup.cpp
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Include
#include <windows.h>
#include <gl/glu.h>
#include <stdio.h>
#include "PolygonLookup.h"
#include "Tracer.h"
#include "main.h"
#include "PolygonManager.h"
#include "LightManager.h"
#include "Engine.h"
#include "CustomMsg.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
CPolygonLookup::CPolygonLookup()
{
    TRACE_ENTER_FN( CPolygonLookup::CPolygonLookup )

    Log( "CPolygonLookup::CPolygonLookup()\n" );

    m_iCurrentID = 0;
    m_iFaceCount = 0;
    m_bFinishedNaming = true;
    m_PolyLookup = NULL;
    m_bNamingEnabled = true;

    TRACE_LEAVE_FN()
}

CPolygonLookup::~CPolygonLookup()
{
    TRACE_ENTER_FN( CPolygonLookup::~CPolygonLookup )

    Log( "CPolygonLookup::~CPolygonLookup()\n" );

    if( m_PolyLookup != NULL )
    {
        delete[] m_PolyLookup;
        m_PolyLookup = NULL;
    }

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: ResetLookupTable()
// Desc: Sets the initial lookup table values
//-----------------------------------------------------------------------------
void CPolygonLookup::ResetLookupTable()
{
    TRACE_ENTER_FN( CPolygonLookup::ResetLookupTable )

    m_iFaceCount = 0;

    // If it exists (if any polygons exist then it does), delete it and set it
    // to null
    if( m_PolyLookup != NULL )
    {
        delete[] m_PolyLookup;
        m_PolyLookup = NULL;
    }

    int iLoadedPolygons = PolygonManager->GetLoadedPolygons();

    // Find out the total number of faces in the scene
    for( int a = 0; a < iLoadedPolygons; a++ )
        m_iFaceCount += PolygonManager->P( a )->GetLoadedFaces();

    m_iFaceCount += PolygonManager->Brush()->GetLoadedFaces() + LM->GetLoadedLights() + 1;

    // Because there are four windows...
    m_iFaceCount *= 4;

    if( GetTotalFaces() > 0 )
        m_PolyLookup = new POLYLOOKUP[GetTotalFaces()];
    else
    {
        Log( "if( GetTotalFaces() > 0 ) @ CPolygonLookup::ResetLookupTable()\n" );
        TRACE_LEAVE_FN() return;
    }

    // Make sure it was created
    if( m_PolyLookup == NULL ) {
        Log( "if( m_PolyLookup == NULL ) @ CPolygonLookup::ResetLookupTable()\n" );
        TRACE_LEAVE_FN() return; }

    ClearLookupTable();

    // Nothing selected
    m_SelectedPolygon.iFace  = -2;
    m_SelectedPolygon.iPoly  = -2;
    m_SelectedPolygon.iVert  = -2;
    m_SelectedPolygon.iLight = -2;
    m_SelectedPolygon.bBrsh  = false;
    m_SelectedPolygon.bVert  = false;
    m_SelectedPolygon.bLight = false;

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: ClearLookupTable()
// Desc: Sets the initial values for the lookup table
//-----------------------------------------------------------------------------
void CPolygonLookup::ClearLookupTable()
{
    TRACE_ENTER_FN( CPolygonLookup::ClearLookupTable )

    try
    {
        // Set all elements to unselected
        for( int a = 0; a < GetTotalFaces(); a++ )
        {
            m_PolyLookup[a].iFace  = NA;
            m_PolyLookup[a].iPoly  = NA;
            m_PolyLookup[a].iVert  = NA;
            m_PolyLookup[a].iLight = NA;
            m_PolyLookup[a].bBrsh  = false;
            m_PolyLookup[a].bVert  = false;
            m_PolyLookup[a].bLight = false;
        }
    }
    catch( ... ) {
        Log( "catch( ... ) @ CPolygonLookup::ClearLookupTable()\n" );
        TRACE_LEAVE_FN() return; }

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: AnythingSelected()
// Desc: If anything in any view is selected, 'true' is returned
//-----------------------------------------------------------------------------
bool CPolygonLookup::AnythingSelected()
{
    TRACE_ENTER_FN( CPolygonLookup::AnythingSelected )

    PolygonManager->TreeSelected( true );

    if( FaceSelected()  || PolySelected() ||
        BrushSelected() || VertSelected() ) {
        TRACE_LEAVE_FN() return true; }

    PolygonManager->TreeSelected( false );

    TRACE_LEAVE_FN()

    return false;
}


//-----------------------------------------------------------------------------
// Name: AnythingSelected()
// Desc: If anything in any view is selected, 'true' is returned
//-----------------------------------------------------------------------------
bool CPolygonLookup::PolySelected()  { return ( m_SelectedPolygon.iPoly >= 0 ); }
bool CPolygonLookup::FaceSelected()  { return ( m_SelectedPolygon.iFace >= 0 ); }
bool CPolygonLookup::BrushSelected() { return m_SelectedPolygon.bBrsh;          }
bool CPolygonLookup::VertSelected()  { return m_SelectedPolygon.bVert;          }
bool CPolygonLookup::LightSelected() { return m_SelectedPolygon.bLight;         }


//-----------------------------------------------------------------------------
// Name: SetSelectedPolygon()
// Desc: Given the ID of the object selected, the exact polygon and face can be
//       determined and stored in the m_SelectedPolygon structure. A message is
//       then posted notifying the tree that a new selection has been made.
//       NOTE: This system is slightly buggy, but works 99% of the time (FIX IT)
//-----------------------------------------------------------------------------
void CPolygonLookup::SetSelectedPolygon( const int ID )
{
    TRACE_ENTER_FN( CPolygonLookup::SetSelectedPolygon )

    // Make sure that the ID doesn't exceed the total number of faces
    if( ID > GetTotalFaces() ) {
        Log( "if( TotalFaces < ID ) @ CPolygonLookup::SetSelectedPolygon\n" );
        TRACE_LEAVE_FN() return; }

    // Be sure that the table actually exists
    if( m_PolyLookup == NULL ) {
        Log( "if( m_PolyLookup == NULL ) @ CPolygonLookup::SetSelectedPolygon()\n" );
        TRACE_LEAVE_FN() return; }

    // Copy the element to the selection structure
    m_SelectedPolygon.iPoly  = m_PolyLookup[ID].iPoly;
    m_SelectedPolygon.iFace  = m_PolyLookup[ID].iFace;
    m_SelectedPolygon.iVert  = m_PolyLookup[ID].iVert;
    m_SelectedPolygon.iLight = m_PolyLookup[ID].iLight;
    m_SelectedPolygon.bBrsh  = m_PolyLookup[ID].bBrsh;
    m_SelectedPolygon.bVert  = m_PolyLookup[ID].bVert;
    m_SelectedPolygon.bLight = m_PolyLookup[ID].bLight;

    // Print the info about the selection to the log file
    LogCurrentSelection();

    // Update the polygon tree if necessary
    if( PolySelected() || FaceSelected() ) PM->NewTreeSelection();

    if( LightSelected() ) PostMessage( EN->GetTabWnd(), CMSG_UPDATELIGHTDATA, 0, 0 );

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: ResetNaming()
// Desc: 
//-----------------------------------------------------------------------------
void CPolygonLookup::ResetNaming()
{
    TRACE_ENTER_FN( CPolygonLookup::ResetNaming )

    if( !m_bNamingEnabled ) { TRACE_LEAVE_FN() return; }

    // Start with a fresh table
    ClearLookupTable();

    // Start naming over
    glInitNames();
    glPushName( 0 );

    m_iCurrentID = 1;
    m_bFinishedNaming = true;

    TRACE_LEAVE_FN()
}



//-----------------------------------------------------------------------------
// Name: StartNaming()
// Desc: Begins naming for the following calls
//-----------------------------------------------------------------------------
void CPolygonLookup::StartNaming()
{
    TRACE_ENTER_FN( CPolygonLookup::StartNaming )

    if( !m_bNamingEnabled ) {
        TRACE_LEAVE_FN() return; }

    // Make sure that it has finished naming before starting a new name
    if( !m_bFinishedNaming ) {
        Log( "if( !m_bFinishedNaming ) @ CPolygonLookup::StartNaming()\n" );
        TRACE_LEAVE_FN() return; }

    // Load the name for the followinf polygons
    glLoadName( m_iCurrentID );

    m_bFinishedNaming = false;

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: EndNaming()
// Desc: Ends naming for the preceeding calls
//-----------------------------------------------------------------------------
void CPolygonLookup::EndNaming()
{
    TRACE_ENTER_FN( CPolygonLookup::EndNaming )

    if( !m_bNamingEnabled ) {
        TRACE_LEAVE_FN() return; }

    // Makes sure that it is still currently naming
    if( m_bFinishedNaming ) {
        Log( "if( !m_bFinishedNaming ) @ CPolygonLookup::EndNaming()\n" );
        TRACE_LEAVE_FN() return; }

    // Finished loading names for this section
    glEnd();
    m_bFinishedNaming = true;

    // Increment the name
    m_iCurrentID++;

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: Include()
// Desc: 
//-----------------------------------------------------------------------------
void CPolygonLookup::Include( const int  iPoly,
                              const int  iFace,
                              const int  iVert,
                              const int  iLight,
                              const bool bVert,
                              const bool bBrsh,
                              const bool bLight)
{
    TRACE_ENTER_FN( CPolygonLookup::Include )

    if( !m_bNamingEnabled ) {
        TRACE_LEAVE_FN() return; }

    // Make sure that it wont overflow the lookup array
    // (IT SHOULDNT EVER DO THAT)
    if( GetCurrentIndex() > GetTotalFaces() ) {
        Log( "if( m_iCurrentID > TotalFaces() ) @ CPolygonLookup::Include()\n" );
        Log( "GetTotalFaces(): " ); Log( GetTotalFaces() ); Log( "\n" );
        Log( "GetCurrentIndex(): " ); Log( GetCurrentIndex() ); Log( "\n" );
        TRACE_LEAVE_FN() return; }

    if( GetCurrentIndex() < 0 ) {
        Log( "if( GetCurrentIndex() < 0 ) @ CPolygonLookup::Include()\n" );
        Log( "GetCurrentIndex(): " ); Log( GetCurrentIndex() ); Log( "\n" );
        TRACE_LEAVE_FN() return; }

    m_PolyLookup[GetCurrentIndex()].iPoly  = iPoly;
    m_PolyLookup[GetCurrentIndex()].iFace  = iFace;
    m_PolyLookup[GetCurrentIndex()].iVert  = iVert;
    m_PolyLookup[GetCurrentIndex()].iLight = iLight;
    m_PolyLookup[GetCurrentIndex()].bVert  = bVert;
    m_PolyLookup[GetCurrentIndex()].bBrsh  = bBrsh;
    m_PolyLookup[GetCurrentIndex()].bLight = bLight;

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: LogCurrentSelection()
// Desc: 
//-----------------------------------------------------------------------------
void CPolygonLookup::LogCurrentSelection()
{
    /*
    Log( "\n***CURRENT SELECTION***\n" );
    Log( "Poly:  " ); Log( m_SelectedPolygon.iPoly ); Log( "\n" );
    Log( "Face:  " ); Log( m_SelectedPolygon.iFace ); Log( "\n" );
    Log( "Vert:  " ); Log( m_SelectedPolygon.iVert ); Log( "\n" );
    Log( "bBrsh: " ); Log( m_SelectedPolygon.bBrsh ); Log( "\n" );
    Log( "bVert: " ); Log( m_SelectedPolygon.bVert ); Log( "\n" );
    Log( "\n" );
    */
}