//----------------------------------------------------------------------------- 
// File: PolygonLookup.h
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _POLYGONLOOKUP_H_
#define _POLYGONLOOKUP_H_

#include <gl/glu.h>
#include "Singleton.h"


#define NA -1


class CPolygonLookup : public CSingleton<CPolygonLookup>
{
private:
    typedef struct _POLYLOOKUP
    {
        int  iPoly, iVert, iFace, iLight;
        bool bBrsh, bVert, bLight;
    } POLYLOOKUP;

    POLYLOOKUP* m_PolyLookup;
    POLYLOOKUP  m_SelectedPolygon;

    int m_iCurrentID,
        m_bFinishedNaming,
        m_iFaceCount;

    bool m_bNamingEnabled;

    void ClearLookupTable();
    int GetCurrentIndex() const { return m_iCurrentID; }
    

public:
    CPolygonLookup();
    virtual ~CPolygonLookup();

    void LogCurrentSelection();

    int GetVert()  const { return m_SelectedPolygon.iVert;       }
    int GetFace()  const { return m_SelectedPolygon.iFace;       }
    int GetPoly()  const { return m_SelectedPolygon.iPoly;       }
    int GetLight() const { return m_SelectedPolygon.iLight;      }
    void SetVert(  const int i ) { m_SelectedPolygon.iVert  = i; }
    void SetFace(  const int i ) { m_SelectedPolygon.iFace  = i; }
    void SetPoly(  const int i ) { m_SelectedPolygon.iPoly  = i; }
    void SetLight( const int i ) { m_SelectedPolygon.iLight = i; }

    int GetTotalFaces() const { return m_iFaceCount; }

    void SetSelectedPolygon( const int ID );
    void ResetLookupTable();

    bool AnythingSelected();
    bool BrushSelected();
    bool VertSelected();
    bool PolySelected();
    bool FaceSelected();
    bool LightSelected();

    void ResetNaming();
    void StartNaming();
    void EndNaming();
    void Include( const int  iPoly,
                  const int  iFace,
                  const int  iVert,
                  const int  iLight,
                  const bool bVert,
                  const bool bBrsh,
                  const bool bLight );

    void NamingEnabled( const bool b ) { m_bNamingEnabled = b; }
};

#define Lookup CPolygonLookup::GetSingletonPtr()
#define LU CPolygonLookup::GetSingletonPtr()

#endif