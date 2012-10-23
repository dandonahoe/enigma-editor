//----------------------------------------------------------------------------- 
// File: Buffer.h
//
// Desc:
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------
#ifndef _BUFFER_H_
#define _BUFFER_H_


class CBuffer  
{
public:
    CBuffer();
    CBuffer( const char* data, size_t size );
    virtual ~CBuffer();

    void operator = ( const CBuffer &buf )
    {
        delete[] m_data;
        m_size = buf.m_size;
        m_data = new char[m_size];
        memcpy( m_data, buf.m_data, m_size );
    }

    void operator = ( const char* buf )
    {
        delete[] m_data;
        m_size = strlen( buf );
        m_data = new char[m_size];
        memcpy( m_data, buf, m_size );
    }

    bool operator == ( const CBuffer &buf )
    {
        if( m_size != buf.m_size ) return false;

        for( unsigned int a = 0; a < m_size; a++ )
            if( m_data[a] != buf.m_data[a] )
                return false;

            return true;
    }

    bool operator == ( const char *buf )
    {
        if( m_size != strlen( buf ) ) return false;

        for( unsigned int a = 0; a < strlen( buf ); a++ )
            if( m_data[a] != buf[a] )
                return false;

            return true;
    }

    bool operator != ( const CBuffer &buf )
    {
        if( m_size != buf.m_size ) return true;

        for( unsigned int a = 0; a < m_size; a++ )
            if( m_data[a] != buf.m_data[a] )
                return true;

            return false;
    }

    char* Data() const { return m_data; }

private:
    size_t m_size;
    char*  m_data;
};


#endif
