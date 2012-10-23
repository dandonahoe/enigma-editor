//----------------------------------------------------------------------------- 
// File: OpenGL.h
//
// Desc: 
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include <fstream>
#include "TextureManager.h"
#include "main.h"
#include "Tracer.h"
#include "glext.h"
#include "LightManager.h"
//-----------------------------------------------------------------------------

using namespace std;
//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
CTextureManager::CTextureManager()
{
    Log( "CTextureManager::CTextureManager()\n" );

    // Invalidate all textures
    ClearAllTextures();

    // Invalidate all engine textures. This is not done in ClearAllTextures() because
    // engine textures need to remain loaded until the application has terminated
    for( unsigned int a = 0; a < MAX_TEXTURES; a++ )
        m_EngineTextures[a].id = NOTEXTURE;

    // Nothing has been loaded yet
    m_LoadedEngineTextures = 0;
    m_LoadedTextures = 0;

    // No texture is an ID that signals that no texture is selected, comparable
    // to using NULL for a pointer
    m_CurrentlySelected.id = NOTEXTURE;
}

CTextureManager::~CTextureManager()
{
    Log( "CTextureManager::~CTextureManager()\n" );
    Release();
}


//-----------------------------------------------------------------------------
// Name: Init()
// Desc:
//-----------------------------------------------------------------------------
bool CTextureManager::Init()
{
    TRACE_ENTER_FN( CTextureManager::Init )

    Log( "CTextureManager::Init()\n" );

    m_strTabTexture = new char[256];

    glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress( "glActiveTextureARB" );

    if( glActiveTextureARB == NULL )
        MessageBox( HWND_DESKTOP, "glActiveTextureARB Failed", "Error", MB_OK );

    TRACE_LEAVE_FN()

    return true;
}


//-----------------------------------------------------------------------------
// Name: Release()
// Desc:
//-----------------------------------------------------------------------------
bool CTextureManager::Release()
{
    TRACE_ENTER_FN( CTextureManager::Release )

    Log( "CTextureManager::Release()\n" );

    // If any loaded textures are present, put them into an array that OpenGL can
    // delete them from
    if( m_LoadedTextures > 0 )
    {
        unsigned int hCustomTextures[256];

        // Copy the IDs
        for( int a = 0; a < m_LoadedTextures; a++ )
            hCustomTextures[a] = m_Textures[a].id;

        // Delete the textures
        glDeleteTextures( m_LoadedTextures, hCustomTextures );
    }

    // If any system textures are present, put them into an array that OpenGL can
    // delete them from
    if( m_LoadedEngineTextures > 0 )
    {
        unsigned int hSystemTextures[256];

        // Copy the IDs
        for( int a = 0; a < m_LoadedEngineTextures; a++ )
            hSystemTextures[a] = m_EngineTextures[a].id;

        // Delete the textures
        glDeleteTextures( m_LoadedEngineTextures, hSystemTextures );
    }

    // Set their values to a cleared state
    ClearAllTextures();

    m_LoadedTextures       = 0;
    m_LoadedEngineTextures = 0;

    Invalidate( m_CurrentlySelected );

    if( m_strTabTexture != NULL )
        delete m_strTabTexture;

    TRACE_LEAVE_FN()

    return true;
}

//-----------------------------------------------------------------------------
// Name: AddTextureFromFile()
// Desc:
//-----------------------------------------------------------------------------
HTEXTURE CTextureManager::AddTextureFromFile( char *strFileName, bool bEngineTexture )
{
    TRACE_ENTER_FN( CTextureManager::AddTextureFromFile )

    HTEXTURE hTexture;

    // Invalidate the texture
    Invalidate( hTexture );

    AUX_RGBImageRec *pBitmap = NULL;
    
    // Make sure a valid file name was sent
    if( strFileName[0] == '\0' || strFileName[0] == '$' ) { TRACE_LEAVE_FN(); return hTexture; }
    
    // If the texture is already loaded, just return the handle to the texture
    // that shares the file name
    if( IsAlreadyLoaded( strFileName, hTexture ) )
    { TRACE_LEAVE_FN() return hTexture; }

    // Load the bitmap into memory
    pBitmap = auxDIBImageLoad( strFileName );                
    
    // If this failed, usually due to memory limits, return the invalid handle
    if( pBitmap == NULL )
    {
        Log( "if( pBitmap == NULL ) @ CTextureManager::AddTextureFromFile()\n" );
        Log( "strFileName: " ); Log( strFileName ); Log( "\n" );
        TRACE_LEAVE_FN()
        return hTexture;
    }

    // If this is a custom texture, load it into the custom texture array
    if( !bEngineTexture )
    {
        strcpy( m_Textures[m_LoadedTextures].path, strFileName );

        glGenTextures( 1, &m_Textures[m_LoadedTextures].id );
        glBindTexture( GL_TEXTURE_2D, m_Textures[m_LoadedTextures].id );
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pBitmap->sizeX, pBitmap->sizeY, GL_RGB, GL_UNSIGNED_BYTE, pBitmap->data);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        hTexture.id = m_LoadedTextures;
        m_LoadedTextures++;
    }
    else // An Engine Texture
    {
        strcpy( m_EngineTextures[m_LoadedEngineTextures].path, strFileName );

        glGenTextures( 1, &m_EngineTextures[m_LoadedEngineTextures].id );
        glBindTexture( GL_TEXTURE_2D, m_EngineTextures[m_LoadedEngineTextures].id );
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pBitmap->sizeX, pBitmap->sizeY, GL_RGB, GL_UNSIGNED_BYTE, pBitmap->data);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        hTexture.id = m_LoadedEngineTextures;
        m_LoadedEngineTextures++;
    }

    // Give the handle its own full path and file name. This is useful only when
    // loading saved files
    strcpy( hTexture.path, strFileName );

    // Release the bitmap
    if( pBitmap )
    {
        if( pBitmap->data )
            free( pBitmap->data);

        free( pBitmap );
    }

    TRACE_LEAVE_FN()

    return hTexture;
}


//-----------------------------------------------------------------------------
// Name: LoadTextureSet()
// Desc:
//-----------------------------------------------------------------------------
bool CTextureManager::LoadTextureSet( char* FileName, bool bEngineTexture )
{
    TRACE_ENTER_FN( CTextureManager::LoadTextureSet )

//    LOG( "CTextureManager::LoadTextureSet()" )

    ifstream fp;
    char Text[256];

    if( !bEngineTexture )
        fp.open( FileName );
    else
        fp.open( FileName );

    while( !fp.eof() )
    {
        fp.getline( Text, 255, '\n' );

        if( !Text ){ TRACE_LEAVE_FN() return true; }

        if( !bEngineTexture )
            AddTextureFromFile( Text, bEngineTexture );
        else
            AddTextureFromFile( Text, bEngineTexture );
    }

    fp.close();

    SelectDefault();

    TRACE_LEAVE_FN()

    return true;
}


//-----------------------------------------------------------------------------
// Name: ClearAllTextures()
// Desc:
//-----------------------------------------------------------------------------
bool CTextureManager::ClearAllTextures()
{
    TRACE_ENTER_FN( CTextureManager::ClearAllTextures )

    m_LoadedTextures = 0;

    for( int a = 0; a < MAX_TEXTURES; a++ )
        Invalidate( m_Textures[a] );

    TRACE_LEAVE_FN()

    return true;
}


//-----------------------------------------------------------------------------
// Name: SelectDefault()
// Desc:
//-----------------------------------------------------------------------------
bool CTextureManager::SelectDefault()
{
    TRACE_ENTER_FN( CTextureManager::SelectDefault )

    HTEXTURE hTexture;
    Invalidate( hTexture );

    PrepareTexture1( hTexture, true );

    TRACE_LEAVE_FN()

    return true;
}


//-----------------------------------------------------------------------------
// Name: IsAlreadyLoaded()
// Desc: Given the file name, if the file has already been loaded, it will set
//       the hTexture parameter to the corresponding texture, then returns true
//       Otherwise returns false leaving the hTexture unchanged
//-----------------------------------------------------------------------------
bool CTextureManager::IsAlreadyLoaded( char* strFileName, HTEXTURE &hTexture )
{
    TRACE_ENTER_FN( CTextureManager::IsAlreadyLoaded )

    for( int a = 0; a < m_LoadedTextures; a++ )
    {
        if( 0 == strcmp( m_Textures[a].path, strFileName ) )
        {
            strcpy( hTexture.path, strFileName );
            hTexture.id = a;
            TRACE_LEAVE_FN()
            return true;
        }
    }

    TRACE_LEAVE_FN()
    return false;
}


//-----------------------------------------------------------------------------
// Name: Save()
// Desc:
//-----------------------------------------------------------------------------
bool CTextureManager::Save( const char* File, FILE* fileDescriptor )
{
    Log( "CTextureManager::Save(): SHOULD NOT SEE THIS\n" );
    return true;
}


//-----------------------------------------------------------------------------
// Name: Open()
// Desc:
//-----------------------------------------------------------------------------
bool CTextureManager::Open( const char* File, FILE* fileDescriptor )
{
    Log( "CTextureManager::Open(): SHOULD NOT SEE THIS\n" );
    return true;
}


//-----------------------------------------------------------------------------
// Name: Invalidate()
// Desc:
//-----------------------------------------------------------------------------
void CTextureManager::Invalidate( HTEXTURE &hTexture )
{
    TRACE_ENTER_FN( CTextureManager::Invalidate )

    hTexture.id = NOTEXTURE;
    strcpy( hTexture.path, "$" );

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: Invalidate()
// Desc:
//-----------------------------------------------------------------------------
bool CTextureManager::Invalidate( LIGHTMAP* hTexture )
{
    TRACE_ENTER_FN( CTextureManager::Invalidate )

    RET( LM->Invalidate( hTexture ) )
}

//-----------------------------------------------------------------------------
// Name: PrepareTexture1()
// Desc:
//-----------------------------------------------------------------------------
bool CTextureManager::PrepareTexture1( const HTEXTURE hHandle, bool bEngineTexture )
{
    TRACE_ENTER_FN( CTextureManager::PrepareTexture1 )

    if( !bEngineTexture )
    {
        try
        {
            if( m_Textures[hHandle.id].id == NOTEXTURE )
            {
                Log( "if( m_Textures[hHandle].id == NOTEXTURE ) @ CTextureManager::PrepareTexture1()" );
                Log( "ID: " ); Log( (int)hHandle.id ); Log( "\n" );
                TRACE_LEAVE_FN()
                return false;
            }
        }
        catch( ... )
        {
            LOG_VAR( (int) hHandle.id )
            LOG_VAR( m_LoadedEngineTextures )
            LOG_VAR( m_LoadedTextures )
            try{ LOG_VAR( (int)m_Textures[hHandle.id].id ) } catch( ... ) { LOG_ERR_RET( false ) }
            LOG_ERR_RET( false )
        }

        glActiveTextureARB( GL_TEXTURE0_ARB );
        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, m_Textures[hHandle.id].id );

    }
    else // IS an engine texture
    {
        Log( "HERE\n" );
        try
        {
            if( m_EngineTextures[hHandle.id].id == NOTEXTURE )
            {
                Log( "if( m_EngineTextures[ID].id == NOTEXTURE ) @ CTextureManager::PrepareTexture()\n" );
                Log( "hHandle: " ); Log( (int)hHandle.id ); Log( "\n" );
                TRACE_LEAVE_FN()
                return false;
            }
        }
        catch( ... )
        {
            LOG_VAR( (int) hHandle.id )
            LOG_VAR( m_LoadedEngineTextures )
            LOG_VAR( m_LoadedTextures )
            try{ LOG_VAR( (int)m_Textures[hHandle.id].id ) } catch( ... ) { LOG_ERR_RET( false ) }
            LOG_ERR_RET( false )
        }

        glBindTexture( GL_TEXTURE_2D, m_EngineTextures[hHandle.id].id );
    }


    RET( true )
}

//-----------------------------------------------------------------------------
// Name: PrepareTexture2()
// Desc:
//-----------------------------------------------------------------------------
bool CTextureManager::PrepareTexture2( const HTEXTURE hHandle )
{
    TRACE_ENTER_FN( CTextureManager::PrepareTexture2 )

    static HTEXTURE LastID = hHandle;
/*
    if( LastID.id == hHandle.id ) { TRACE_LEAVE_FN() return true; }

    // This means no texture wanted
    if( hHandle.id == NOTEXTURE ) { TRACE_LEAVE_FN() return true; }

    if( hHandle.id > NOTEXTURE )
    {
        Log( "if( hHandle > NOTEXTURE ) @ CTextureManager::PrepareTexture2()\n" );
        Log( "hHandle: " ); Log( (int)hHandle.id ); Log( "\n" );
        TRACE_LEAVE_FN()
        return false;
    }
*/
 //   if( m_Textures[hHandle.id].id == NOTEXTURE )
 //   {
  //      Log( "if( m_Textures[hHandle].id == NOTEXTURE ) @ CTextureManager::PrepareTexture2()" );
  //      Log( "ID: " ); Log( (int)hHandle.id ); Log( "\n" );
//        TRACE_LEAVE_FN()
  //      return false;
   // }

    glActiveTextureARB( GL_TEXTURE1_ARB );
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, LM->GetHemicube()->GetTextureID( hHandle.id ) );//m_Textures[hHandle.id].id );
    
    LastID = hHandle;

    TRACE_LEAVE_FN()

    return TRUE;
}


//-----------------------------------------------------------------------------
// Name: PrepareLightmap()
// Desc:
//-----------------------------------------------------------------------------
bool CTextureManager::PrepareLightmap( const HTEXTURE hHandle)
{
    TRACE_ENTER_FN( CTextureManager::PrepareLightmap )

    static HTEXTURE LastID = hHandle;
/*
    if( LastID.id == hHandle.id ) { TRACE_LEAVE_FN() return true; }

    // This means no texture wanted
    if( hHandle.id == NOTEXTURE ) { TRACE_LEAVE_FN() return true; }

    if( hHandle.id > NOTEXTURE )
    {
        Log( "if( hHandle > NOTEXTURE ) @ CTextureManager::PrepareLightmap()\n" );
        Log( "hHandle: " ); Log( (int)hHandle.id ); Log( "\n" );
        TRACE_LEAVE_FN()
        return false;
    }
*/
    if( m_Textures[hHandle.id].id == NOTEXTURE )
    {
        Log( "if( m_Textures[hHandle].id == NOTEXTURE ) @ CTextureManager::PrepareLightmap()" );
        Log( "ID: " ); Log( (int)hHandle.id ); Log( "\n" );
        TRACE_LEAVE_FN()
        return false;
    }

    glActiveTextureARB( GL_TEXTURE2_ARB );
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, m_Textures[hHandle.id].id );

    LastID = hHandle;

    TRACE_LEAVE_FN()

    return TRUE;
}


//-----------------------------------------------------------------------------
// Name: DisableTexture1()
// Desc:
//-----------------------------------------------------------------------------
void CTextureManager::DisableTexture1()
{
    TRACE_ENTER_FN( CTextureManager::DisableTexture1 )

    glActiveTextureARB( GL_TEXTURE0_ARB );
    glDisable( GL_TEXTURE_2D );

    TRACE_LEAVE_FN()
}

//-----------------------------------------------------------------------------
// Name: DisableTexture2()
// Desc:
//-----------------------------------------------------------------------------
void CTextureManager::DisableTexture2()
{
    TRACE_ENTER_FN( CTextureManager::DisableTexture2 )

    glActiveTextureARB( GL_TEXTURE1_ARB );
    glDisable( GL_TEXTURE_2D );

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: DisableLightmap()
// Desc:
//-----------------------------------------------------------------------------
void CTextureManager::DisableLightmap()
{
    TRACE_ENTER_FN( CTextureManager::DisableLightmap )

    glActiveTextureARB( GL_TEXTURE2_ARB );
    glDisable( GL_TEXTURE_2D );

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: EnableTexture1()
// Desc:
//-----------------------------------------------------------------------------
void CTextureManager::EnableTexture1()
{
    TRACE_ENTER_FN( CTextureManager::EnableTexture1 )

    glActiveTextureARB( GL_TEXTURE0_ARB );
    glEnable( GL_TEXTURE_2D );

    TRACE_LEAVE_FN()
}

//-----------------------------------------------------------------------------
// Name: EnableTexture2()
// Desc:
//-----------------------------------------------------------------------------
void CTextureManager::EnableTexture2()
{
    TRACE_ENTER_FN( CTextureManager::EnableTexture2 )

    glActiveTextureARB( GL_TEXTURE1_ARB );
    glEnable( GL_TEXTURE_2D );

    TRACE_LEAVE_FN()
}


//-----------------------------------------------------------------------------
// Name: EnableLightmap()
// Desc:
//-----------------------------------------------------------------------------
void CTextureManager::EnableLightmap()
{
    TRACE_ENTER_FN( CTextureManager::EnableLightmap )

    glActiveTextureARB( GL_TEXTURE2_ARB );
    glEnable( GL_TEXTURE_2D );

    TRACE_LEAVE_FN()
}