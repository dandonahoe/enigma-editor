//----------------------------------------------------------------------------- 
// File: TextureManager.h
//
// Desc: A singleton texture manager that manages all textures for the world
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _TEXTUREMANAGER_H_
#define _TEXTUREMANAGER_H_

#define MAX_TEXTURES 64

#define CUSTOM_FILE "custom.tex"
#define SYSTEM_FILE "system.tex"

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include <stdio.h>
#include "glext.h"
#include "Singleton.h"
#include "Buffer.h"
#include "LightmapManager.h"

typedef struct _TEXTURE
{
    unsigned int id;
    char path[256];
} HTEXTURE, *LPTEXTURE;


#define NOTEXTURE 99999


class CTextureManager : public CSingleton<CTextureManager>
{
public:
    CTextureManager();
    virtual ~CTextureManager();
    bool  Release();

    void SetSelectedTexture( const HTEXTURE hTexture ) { m_CurrentlySelected = hTexture; } 
    HTEXTURE AddTextureFromFile( char* strFileName, bool bEngineTexture = false );
    void SetNameFromTextureTab( const char* str) { strcpy( m_strTabTexture, str ); }
    HTEXTURE GetSelectedTexture() const { return m_CurrentlySelected; }
    bool LoadTextureSet( char* FileName, bool bEngineTexture = false );
    bool IsAlreadyLoaded( char* strFileName, HTEXTURE &hTexture );
    char* GetNameFromTextureTab() const { return m_strTabTexture; }
    bool Save( const char* File, FILE* fileDescriptor );
    bool Open( const char* File, FILE* fileDescriptor );
    void Invalidate( HTEXTURE &hTexture );
    bool Invalidate( LIGHTMAP* hTexture );
    bool ClearAllTextures();
    bool SelectDefault();
    bool Init();
    
    bool PrepareTexture1( const HTEXTURE hHandle, bool bEngineTexture = false );
    bool PrepareTexture2( const HTEXTURE hHandle );
    bool PrepareLightmap( const HTEXTURE hHandle );

    void DisableTexture1();
    void DisableTexture2();
    void DisableLightmap();
    void EnableTexture1();
    void EnableTexture2();
    void EnableLightmap();

    PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;

private:
    HTEXTURE m_EngineTextures[MAX_TEXTURES];
    HTEXTURE m_CurrentlySelected;
    HTEXTURE m_Textures[MAX_TEXTURES];
    int m_LoadedEngineTextures;
    char* m_strTabTexture;
    int m_LoadedTextures;
    char* m_FileName;
};

#define TextureManager CTextureManager::GetSingletonPtr()
#define TM CTextureManager::GetSingletonPtr()

#endif
