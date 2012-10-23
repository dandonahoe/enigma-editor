//----------------------------------------------------------------------------- 
// File: SkyBox.h
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _SKYBOX_H_
#define _SKYBOX_H_


#include "Camera.h"
#include "Object.h"
#include "TextureManager.h"


class CSkyBox : public CObject
{
public:
    CSkyBox();
    virtual ~CSkyBox();
    virtual bool Init();
    virtual bool Update( float fElapsed, CDInput &Input, CCamera* camera );
    virtual bool Render();
    virtual bool Release();

private:
    CCamera         m_Camera;
};


#endif
