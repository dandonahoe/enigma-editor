//----------------------------------------------------------------------------- 
// File: Object.h
//
// Desc: 
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------

#ifndef _OBJECT_H_
#define _OBJECT_H_


#include "dinput.h"
#include "camera.h"

class CObject  
{
public:
    CObject();
    virtual ~CObject();

    virtual bool Init();
    virtual bool Update( float fElapsed );
    virtual bool Render();
    virtual bool Release();
};


#endif