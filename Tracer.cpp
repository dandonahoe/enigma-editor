//----------------------------------------------------------------------------- 
// File: Tracer.cpp
//
// Desc: 
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------



///////////////////////////////////////////////////////////////////////////////
// CallStackTrace.cpp
// Author: Bryan "daerid" Ross
// Date: Friday, October 18, 2002
//
// This file has no copyright. Use it however you want. Just don't hurt 
// anybody ;)
//
// Conversely, this file has no warranties or guarantees. The author does
// not claim any responsibility for the consequences of use.
// 
// ** USE AT YOUR OWN RIST **
///////////////////////////////////////////////////////////////////////////////


#include "Tracer.h"
#include "main.h"


CallStackTrace* CallStackTrace::Head = 0;
CallStackTrace* CallStackTrace::Tail = 0;

using namespace std;

CallStackTrace::CallStackTrace( const char* n, const char* f, int l )
{
    szFuncName = n;
    szFileName = f;
    iLineNo = l;
}


void CallStackTrace::Dump()
{
    Log( "***Begin Call Stack Dump:*** \n" );

    for( CallStackTrace* r = Head; r != 0; r = r->pNext )
    {
        Log( r->szFuncName ); Log( "()[" ); Log( r->szFileName ); Log( ":" ); Log( r->iLineNo ); Log( "]" );

        if( r->pNext ) Log( "->" );

        Log( "\n" );
    }

    Log( "*** End Call Stack Dump*** \n" );
}

void CallStackTrace::Dump( std::ostream& out )
{
    out << "***Begin Call Stack Dump:*** \n";

    for( CallStackTrace* r = Head; r != 0; r = r->pNext )
    {
        out << r->szFuncName << "()[" << r->szFileName << ":" << r->iLineNo << "]";

        if( r->pNext) out << "->";

        out << "\n";
    }

    out << "*** End Call Stack Dump*** " << endl;    
}

void CallStackTrace::Enter()
{
    if( Head == 0 ) Head = this;

    if( Tail != 0 ) Tail->pNext = this;

    pPrev = Tail;

    Tail = this;
}


void CallStackTrace::Leave()
{
    if( Tail->pPrev ) Tail->pPrev->pNext = 0;

    Tail = Tail->pPrev;

    pPrev = pNext = 0;
}