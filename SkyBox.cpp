//----------------------------------------------------------------------------- 
// File: SkyBox.cpp
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#include "SkyBox.h"
#include "main.h"
#include "OpenGL.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
CSkyBox::CSkyBox()
{
    Log( "CSkyBox::CSkyBox()\n" );

    m_Camera.SetFromVec( 0, 0, 0 );
}
CSkyBox::~CSkyBox()
{
    Log( "CSkyBox::~CSkyBox()\n" );
}


//-----------------------------------------------------------------------------
// Name: Init()
// Desc: 
//-----------------------------------------------------------------------------
bool CSkyBox::Init()
{
    Log( "CSkyBox::Init()\n" );
    //TextureManager->LoadTextureSet( false, "custom.tex" );

    return true;
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the skybox
//-----------------------------------------------------------------------------
bool CSkyBox::Render()
{

    m_Camera.Render();

    glColor3f( 255, 255, 255 );

    float side = 3.0f;

//    TextureManager->PrepareTexture( 0 ); // Back
    glBegin( GL_QUADS );
    glTexCoord2f( 0,    1.0f ); glVertex3f( -side,  side, -side ); 
    glTexCoord2f( 1.0f, 1.0f ); glVertex3f(  side+.01f,  side, -side );
    glTexCoord2f( 1.0f, 0    ); glVertex3f(  side+.01f, -side, -side );
    glTexCoord2f( 0,    0    ); glVertex3f( -side, -side, -side );
    glEnd();


//    TextureManager->PrepareTexture( 1 ); // Bottom
    glBegin( GL_QUADS );
    glTexCoord2f( 1.0f, 0    ); glVertex3f( -side, -side, -side );
    glTexCoord2f( 0,    0    ); glVertex3f(  side, -side, -side );
    glTexCoord2f( 0,    1.0f ); glVertex3f(  side, -side,  side );
    glTexCoord2f( 1.0f, 1.0f ); glVertex3f( -side, -side,  side );
    glEnd();


//    TextureManager->PrepareTexture( 2 ); // Front
    glBegin( GL_QUADS );
    glTexCoord2f( 1.0f, 1.0f ); glVertex3f( -side,  side,  side );
    glTexCoord2f( 0,    1.0f ); glVertex3f(  side,  side,  side );
    glTexCoord2f( 0,    0    ); glVertex3f(  side, -side,  side );
    glTexCoord2f( 1.0f, 0    ); glVertex3f( -side, -side,  side );
    glEnd();


//    TextureManager->PrepareTexture( 5 ); // Left
    glBegin( GL_QUADS );
    glTexCoord2f( 0,    1.0f ); glVertex3f( -side,  side,  side );
    glTexCoord2f( 1.0f, 1.0f ); glVertex3f( -side,  side, -side );
    glTexCoord2f( 1.0f, 0    ); glVertex3f( -side, -side, -side );
    glTexCoord2f( 0,    0    ); glVertex3f( -side, -side,  side );
    glEnd();


//    TextureManager->PrepareTexture( 4 ); // Right
    glBegin( GL_QUADS );
    glTexCoord2f( 1.0f, 1.0f ); glVertex3f(  side,  side,  side );
    glTexCoord2f( 0,    1.0f ); glVertex3f(  side,  side, -side-.01f );
    glTexCoord2f( 0,    0    ); glVertex3f(  side, -side, -side-.01f );
    glTexCoord2f( 1.0f, 0    ); glVertex3f(  side, -side,  side );
    glEnd();


//    TextureManager->PrepareTexture( 5 ); // Top
    glBegin( GL_QUADS );
    glTexCoord2f( 1.0f, 1.0f ); glVertex3f( -side,  side, -side );
    glTexCoord2f( 0,    1.0f ); glVertex3f(  side,  side, -side );
    glTexCoord2f( 0,    0    ); glVertex3f(  side,  side,  side );
    glTexCoord2f( 1.0f, 0    ); glVertex3f( -side,  side,  side );
    glEnd();

    // This makes the sky a backdrop
    glClear( GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();

    return true;
}


//-----------------------------------------------------------------------------
// Name: Update()
// Desc: Updates the skybox
//-----------------------------------------------------------------------------
bool CSkyBox::Update( float fElapsed, CDInput &Input, CCamera* camera )
{
    m_Camera.SetToVec( camera->GetToVec() - camera->GetFromVec() );

    return true;
}


//-----------------------------------------------------------------------------
// Name: Update()
// Desc: Updates the skybox
//-----------------------------------------------------------------------------
bool CSkyBox::Release()
{
    return true;
}