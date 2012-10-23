//----------------------------------------------------------------------------- 
// File: Singleton.h
//
// Desc: 
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _SINGLETON_H_
#define _SINGLETON_H_


#include <cassert>
#include <stdlib.h>

template <typename T> class CSingleton
{
    static T* m_Singleton;

    public:
        CSingleton()
        {
            assert( !m_Singleton );
            int offset = (int)(T*)1 - (int)( CSingleton <T>* )(T*)1;
            m_Singleton = (T*)((int)this + offset);
        }
        ~CSingleton()
        {
            assert( m_Singleton );
            m_Singleton = NULL;
        }

        static T& GetSingleton()
        {
            assert( m_Singleton );
            return ( *m_Singleton );
        }

        static T* GetSingletonPtr()
        {
            return m_Singleton;
        }
};

template <typename T> T* CSingleton <T>::m_Singleton = 0;

#endif