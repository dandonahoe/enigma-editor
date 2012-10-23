//----------------------------------------------------------------------------- 
// File: Tracer.h
//
// Desc: Origional info below, edited by me
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _TRACER_H_
#define _TRACER_H_


#include <iostream>

class CallStackTracer;
class CallStackTrace;

class CallStackTrace
{
public:
    const char* szFuncName;
    const char* szFileName;
    int iLineNo;
    CallStackTrace* pPrev;
    CallStackTrace* pNext;

    CallStackTrace( const char* n, const char* f, int l );// : szFuncName( n ), szFileName( f ), iLineNo(l) {}

    static void Dump();
    static void Dump( std::ostream& out );
    void Enter();
    void Leave();
    void Unwind() { pNext = 0; Tail = this; }

    static CallStackTrace* Head;
    static CallStackTrace* Tail;
};

class CallStackTracer
{
    CallStackTracer( CallStackTrace& p ) : pTrace( p ) { p.Enter(); }
    ~CallStackTracer() { pTrace.Leave(); }
    CallStackTrace& pTrace;
};



#define TRACE_ENTER_FN(fname) \
    static CallStackTrace s_CallStackTrace(#fname, __FILE__, __LINE__); \
    s_CallStackTrace.Enter();
    
#define TRACE_LEAVE_FN() \
    s_CallStackTrace.Leave();
    
#define TRACE_UNWIND() \
    s_CallStackTrace.Unwind();
    


#else

#define TRACE_ENTER_FN( fname )
#define TRACE_LEAVE_FN()
#define TRACE_UNWIND()

#endif