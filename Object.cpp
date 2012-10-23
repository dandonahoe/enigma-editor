//----------------------------------------------------------------------------- 
// File: Object.cpp
//
// Desc: Base class for all engine objects
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#include "main.h"
#include "Object.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
CObject::CObject()
{
}

CObject::~CObject()
{
}


bool CObject::Init()
{
    Log( "HERE\n\n\n\n\n\n\n\n\n" );
    return true;
}

bool CObject::Update( float fElapsed )
{
    return true;
}
bool CObject::Render()
{
    return true;
}
bool CObject::Release()
{
    return true;
}