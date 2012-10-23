//----------------------------------------------------------------------------- 
// File: Util.cpp
//
// Desc: 
//
// Copyright (c) 2002 Dan
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
#include "main.h"
#include "Util.h"
#include "Tracer.h"
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: GenerateScreenShot()
// Desc: 
//-----------------------------------------------------------------------------
bool GenerateScreenShot( const char* strFileName,
                         const int iWidth, const int iHeight,
                         const bool bFlip,
                         const unsigned char* pData )
{
    TRACE_ENTER_FN( GenerateScreenShot )

    FILE *tga;
    unsigned char *pBuffer   = new unsigned char[ iWidth * iHeight * 3 ];
    unsigned char *pFlipBuff = new unsigned char[ iWidth * iHeight * 3 ];
    unsigned char ucTemp;
    TARGAHEADER header;

    memcpy( pBuffer, pData, sizeof( unsigned char ) * iWidth * iHeight * 3 );

    // Supply the header information for the .tga file
    header.id_len         = 0;
    header.map_type       = 0;
    header.img_type       = 2;
    header.map_first      = 0;
    header.map_len        = 0;
    header.map_entry_size = 0;
    header.x              = 0;
    header.y              = 0;
    header.width          = iWidth;
    header.height         = iHeight;
    header.bpp            = 24;
    header.misc           = 0x20;

    // Create the .tga file
    tga = fopen( strFileName, "wb" );

    // Write the header information
    WriteTargaHeader( header, tga );

    // Swap the blue and red values
    for( int i = 0; i < iWidth * iHeight * 3; i += 3 )
    {
        ucTemp = pBuffer[ i ];
        pBuffer[ i ] = pBuffer[ i + 2 ];
        pBuffer[ i + 2 ] = ucTemp;
    }

    if( bFlip )
    {
        for( int a = 0; a < iHeight; a++ )
            for( int b = 0; b < iWidth * 3; b++ )
                pFlipBuff[iWidth * iHeight * 3 - ( iWidth * ( a + 1 ) * 3 ) + b] = pBuffer[ a * iWidth * 3 + b ];

        fwrite( pFlipBuff, sizeof( unsigned char ), iWidth * iHeight * 3, tga );
    }
    else
    {
        fwrite( pBuffer, sizeof( unsigned char ), iWidth * iHeight * 3, tga );
    }

    // Copy the image data to the file
    

    // Close the file
    fclose( tga );

    // Delete the buffer
    if( pBuffer != NULL   ) delete[] pBuffer;
    if( pFlipBuff != NULL ) delete[] pFlipBuff;

    TRACE_LEAVE_FN()

    return true;
}


//-----------------------------------------------------------------------------
// Name: WriteTargaHeader()
// Desc: 
//-----------------------------------------------------------------------------
void WriteTargaHeader( TARGAHEADER h, FILE *tga ) 
{
    TRACE_ENTER_FN( WriteTargaHeader )

    fputc( h.id_len,          tga );
    fputc( h.map_type,        tga );
    fputc( h.img_type,        tga );
    fputc( h.map_first % 256, tga ); 
    fputc( h.map_first / 256, tga );
    fputc( h.map_len % 256,   tga );
    fputc( h.map_len / 256,   tga );
    fputc( h.map_entry_size,  tga );
    fputc( h.x % 256,         tga );
    fputc( h.x / 256,         tga );
    fputc( h.y % 256,         tga );
    fputc( h.y / 256,         tga );
    fputc( h.width % 256,     tga );
    fputc( h.width / 256,     tga );
    fputc( h.height % 256,    tga );
    fputc( h.height / 256,    tga );
    fputc( h.bpp,             tga );
    fputc( h.misc,            tga );

    TRACE_LEAVE_FN()
}

DWORD GetCpuSpeed()
{
    TRACE_ENTER_FN( GetCpuSpeed )

    DWORD dwStartTicks = 0;
    DWORD dwEndTicks   = 0;
    DWORD dwTotalTicks = 0;
    DWORD dwCpuSpeed   = 0;

    //    check for high-resolution timer
    LARGE_INTEGER qwFrequency = { 0 };

    if( QueryPerformanceFrequency( &qwFrequency ) && qwFrequency.QuadPart > 0 )
    {    
        LARGE_INTEGER qwStart = { 0 };
        LARGE_INTEGER qwStop  = { 0 };
        //    1. step - get start ticks
        QueryPerformanceCounter( &qwStart );

        for(;;)
        {
            QueryPerformanceCounter( &qwStop );
            
            if( ( qwStop.QuadPart - qwStart.QuadPart ) * 1000 / qwFrequency.QuadPart > 1 )        
            {
                __asm
                {
                    xor    eax, eax
                    xor    ebx, ebx
                    xor    ecx, ecx
                    xor    edx, edx
                    _emit  0x0f
                    //    CPUID
                    _emit  0xa2
                    _emit  0x0f
                    //    RDTSC
                    _emit  0x31
                    mov    [dwStartTicks], eax
                }

                break;
            }
        }

        qwStart.QuadPart = qwStop.QuadPart;
        //    2. step - get end ticks after 1000 ms
        for(;;)
        {
            QueryPerformanceCounter( &qwStop );

            if( ( qwStop.QuadPart - qwStart.QuadPart ) / qwFrequency.QuadPart >= 1 )
            {
                __asm
                {
                    xor    eax, eax
                    xor    ebx, ebx
                    xor    ecx, ecx
                    xor    edx, edx
                    _emit  0x0f
                    // CPUID
                    _emit  0xa2
                    _emit  0x0f
                    // RDTSC
                    _emit  0x31
                    mov    [dwEndTicks], eax
                }

                break;
            }
        }
    }
    else
    {
        DWORD dwStart = 0;
        DWORD dwStop  = 0;
        //    1. step - get start ticks
        dwStart = GetTickCount();

        for(;;)    
        {
            dwStop = GetTickCount();

            if( ( dwStop - dwStart ) > 1 )
                //    ROLLOVER PAST 1
            {
                __asm
                {
                    xor    eax, eax
                    xor    ebx, ebx
                    xor    ecx, ecx
                    xor    edx, edx
                    _emit  0x0f
                    //    CPUID
                    _emit  0xa2
                    _emit  0x0f
                    //    RDTSC
                    _emit  0x31
                    mov    [dwStartTicks], eax
                }     
                
                break;
            }
        }

        dwStart = dwStop;

        //    2. step - get end ticks after 1000 ms
        for(;;)    
        {        
            dwStop = GetTickCount();

            if( ( dwStop - dwStart ) >= 1000 )
            {            
                __asm
                {
                    xor    eax, eax
                    xor    ebx, ebx
                    xor    ecx, ecx
                    xor    edx, edx
                    _emit  0x0f
                    // CPUID
                    _emit  0xa2
                    _emit  0x0f
                    // RDTSC
                    _emit  0x31
                    mov    [dwEndTicks], eax
                }
                
                break;
            }
        }
    }

    //    compute result
    dwTotalTicks = dwEndTicks - dwStartTicks;
    
    dwCpuSpeed = dwTotalTicks / 1000000;

    //    MHz

    TRACE_LEAVE_FN()

    return dwCpuSpeed;
}


//-----------------------------------------------------------------------------
// Name: RotatePointAboutLine()
// Desc: 
// Auth: http://astronomy.swin.edu.au/~pbourke/geometry/rotate/
//-----------------------------------------------------------------------------
VECT RotPtByLn( VECT p, float theta, VECT p1, VECT p2 )
{
    TRACE_ENTER_FN( RotPtByLn )

    VECT u, q1, q2;
    float d;

    /* Step 1 */
    q1.x = p.x - p1.x;
    q1.y = p.y - p1.y;
    q1.z = p.z - p1.z;

    u.x = p2.x - p1.x;
    u.y = p2.y - p1.y;
    u.z = p2.z - p1.z;

    u /= sqrtf( u.x*u.x + u.y*u.y + u.z*u.z );

    d = sqrtf(u.y*u.y + u.z*u.z);

    /* Step 2 */
    float temp = d;
    if( temp < 0 )
        temp = -temp;

//if( abs( d ) > 0.000001)

    if( temp > 0.000001)
    {
        q2.x = q1.x;
        q2.y = q1.y * u.z / d - q1.z * u.y / d;
        q2.z = q1.y * u.y / d + q1.z * u.z / d;
    }
    else
        q2 = q1;

    /* Step 3 */
    q1.x = q2.x * d - q2.z * u.x;
    q1.y = q2.y;
    q1.z = q2.x * u.x + q2.z * d;

    /* Step 4 */
    q2.x = q1.x * cosf(theta) - q1.y * sinf(theta);
    q2.y = q1.x * sinf(theta) + q1.y * cosf(theta);
    q2.z = q1.z;

    /* Inverse of step 3 */
    q1.x =   q2.x * d + q2.z * u.x;
    q1.y =   q2.y;
    q1.z = - q2.x * u.x + q2.z * d;

    /* Inverse of step 2 */

    temp = d;
    if( temp < 0 )
        temp = -temp;

    if( temp > 0.000001 )
    {
        q2.x =   q1.x;
        q2.y =   q1.y * u.z / d + q1.z * u.y / d;
        q2.z = - q1.y * u.y / d + q1.z * u.z / d;
    }
    else
        q2 = q1;


    /* Inverse of step 1 */
    q1.x = q2.x + p1.x;
    q1.y = q2.y + p1.y;
    q1.z = q2.z + p1.z;

    TRACE_LEAVE_FN()

    return q1;
}
/*
typedef struct tImageTGA
{
    int iChannels;
    int iSizeX;
    int iSizeY;
    unsigned char *pData;
} TARGADATA, *PTARGADATA;
*/
//-----------------------------------------------------------------------------
// Name: LoadTGA()
// Desc: Returns targa data
// Auth: DigiBen digiben@gametutorials.com
//-----------------------------------------------------------------------------
TARGADATA* LoadTGA( const char *strFileName )
{
    TRACE_ENTER_FN( LoadTGA )

    TARGADATA *pImageData = NULL;
    WORD width = 0, height = 0;
    byte length = 0;
    byte imageType = 0;
    byte bits = 0;
    FILE *pFile = NULL;
    int channels = 0;
    int stride = 0;
    int i = 0;

    if( ( pFile = fopen( strFileName, "rb" ) ) == NULL ) {
        MessageBox( HWND_DESKTOP, "Unable to load TGA File!", "Error", MB_OK );
        TRACE_LEAVE_FN() return NULL; }

    pImageData = (TARGADATA*)malloc( sizeof( tImageTGA ) );

    fread( &length, sizeof( byte ), 1, pFile );
    fseek( pFile, 1, SEEK_CUR ); 
    fread( &imageType, sizeof( byte ), 1, pFile );
    fseek( pFile, 9, SEEK_CUR ); 
    fread( &width,  sizeof( WORD ), 1, pFile );
    fread( &height, sizeof( WORD ), 1, pFile );
    fread( &bits,   sizeof( byte ), 1, pFile );

    fseek( pFile, length + 1, SEEK_CUR ); 

    if( imageType != TGA_RLE )
    {
        if( bits == 24 || bits == 32 )
        {
            channels = bits / 8;
            stride = channels * width;
            pImageData->pData = new unsigned char[stride * height];
            for( int y = 0; y < height; y++ )
            {
                unsigned char *pLine = &( pImageData->pData[stride * y] );
                fread( pLine, stride, 1, pFile );
                for( i = 0; i < stride; i += channels )
                {
                    int temp     = pLine[i];
                    pLine[i]     = pLine[i + 2];
                    pLine[i + 2] = temp;
                }
            }
        }
        else if( bits == 16 )
        {
            unsigned short pixels = 0;
            int r = 0, g = 0, b = 0;
            channels = 3;
            stride = channels * width;
            pImageData->pData = new unsigned char[stride * height];

            for( int i = 0; i < width * height; i++ )
            {
                fread( &pixels, sizeof( unsigned short ), 1, pFile );
                b = ( pixels & 0x1f ) << 3;
                g = ( ( pixels >> 5 ) & 0x1f ) << 3;
                r = ( ( pixels >> 10 ) & 0x1f ) << 3;
                
                pImageData->pData[i * 3 + 0] = r;
                pImageData->pData[i * 3 + 1] = g;
                pImageData->pData[i * 3 + 2] = b;
            }
        }    
        else
        {
            Log( "HERE!!!" );
            Log( "BITS: " ); Log( bits ); Log( "\n" );
            TRACE_LEAVE_FN() return NULL; }
    }
    else
    {
        byte rleID = 0;
        int colorsRead = 0;
        channels = bits / 8;
        stride = channels * width;

        pImageData->pData = new unsigned char[stride * height];
        byte *pColors = new byte [channels];

        while( i < width*height )
        {
            fread( &rleID, sizeof( byte ), 1, pFile );
            
            if( rleID < 128 )
            {
                rleID++;

                while( rleID )
                {
                    fread( pColors, sizeof( byte ) * channels, 1, pFile );

                    pImageData->pData[colorsRead + 0] = pColors[2];
                    pImageData->pData[colorsRead + 1] = pColors[1];
                    pImageData->pData[colorsRead + 2] = pColors[0];

                    if( bits == 32 )
                        pImageData->pData[colorsRead + 3] = pColors[3];

                    i++;
                    rleID--;
                    colorsRead += channels;
                }
            }
            else
            {
                rleID -= 127;

                fread( pColors, sizeof( byte ) * channels, 1, pFile );

                while( rleID )
                {
                    pImageData->pData[colorsRead + 0] = pColors[2];
                    pImageData->pData[colorsRead + 1] = pColors[1];
                    pImageData->pData[colorsRead + 2] = pColors[0];

                    if( bits == 32 )
                        pImageData->pData[colorsRead + 3] = pColors[3];

                    i++;
                    rleID--;
                    colorsRead += channels;
                } 
            }
        }
    }

    fclose( pFile );

    pImageData->iChannels = channels;
    pImageData->iSizeX    = width;
    pImageData->iSizeY    = height;

    TRACE_LEAVE_FN()

    return pImageData;
}


//-----------------------------------------------------------------------------
// Name: GetViewType()
// Desc: 
//-----------------------------------------------------------------------------
CAMERAVIEW GetViewEnumType( const int iType )
{
    switch( iType )
    {
        case 0: return FORWARDVIEW;
        case 1: return LEFTVIEW;
        case 2: return RIGHTVIEW;
        case 3: return UPVIEW;
        case 4: return DOWNVIEW;
        default: Log( "default: @ GetViewEnumType()\n" );
    }

    return NOTHINGVIEW;
}


//-----------------------------------------------------------------------------
// Name: GetViewEnumID()
// Desc: 
//-----------------------------------------------------------------------------
int GetViewEnumID( const CAMERAVIEW eView )
{
    switch( eView )
    {
        case FORWARDVIEW: return 0;
        case LEFTVIEW:    return 1;
        case RIGHTVIEW:   return 2;
        case UPVIEW:      return 3;
        case DOWNVIEW:    return 4;
        default: Log( "default @ GetViewEnumID()\n" );
    }

    return 5;
}


//-----------------------------------------------------------------------------
// Name: NormalizeBitmap()
// Desc: 
//-----------------------------------------------------------------------------
bool NormalizeBitmap( double* pData, const iLen )
{
    TRACE_ENTER_FN( NormalizeBitmap )

    if( pData == NULL ) {
        Log( "if( pData == NULL ) @ NormalizeBitmap()\n" );
        TRACE_LEAVE_FN() return false; }

    if( iLen <= 0 ) {
        Log( "if( iLen <= 0 ) @ NormalizeBitmap()\n" );
        TRACE_LEAVE_FN() return false; }

    unsigned long int iTotal = 0;

    for( int a = 0; a < iLen; a++ )
        iTotal += pData[a];

    for( a = 0; a < iLen; a++ )
        pData[a] /= iTotal;

    TRACE_LEAVE_FN()

    return true;
}


//-----------------------------------------------------------------------------
// Name: GetBrushEnumType()
// Desc: 
//-----------------------------------------------------------------------------
BRUSHTYPE GetBrushEnumType( const int i )
{
    switch( i )
    {
    case 0: return NOTHINGBRUSH;
    case 1: return SHEETBRUSH;
    case 2: return CUBEBRUSH;
    case 3: return STAIRSBRUSH;
    case 4: return SPIRALBRUSH;
    case 5: return CYLINDERBRUSH;
    case 6: return CIRCLEBRUSH;
    default:
        Log( "default: @ GetBrushEnumType()\n" );
        Log( "i = " ); Log( i ); Log( '\n' );
        break;
    }

    return NOTHINGBRUSH;
}


//-----------------------------------------------------------------------------
// Name: GetBrushEnumID()
// Desc: 
//-----------------------------------------------------------------------------
int GetBrushEnumID( const int eType )
{
    switch( eType )
    {
        case NOTHINGBRUSH:  return 0;
        case SHEETBRUSH:    return 1;
        case CUBEBRUSH:     return 2;
        case STAIRSBRUSH:   return 3;
        case SPIRALBRUSH:   return 4;
        case CYLINDERBRUSH: return 5;
        case CIRCLEBRUSH:   return 6;
        default: Log( "default: @ GetBrushEnumID()\n" ); break;
    }
    return 0;
}


//-----------------------------------------------------------------------------
// Name: GetFaceEnumType()
// Desc: 
//-----------------------------------------------------------------------------
FACETYPE GetFaceEnumType( const int i )
{
    switch( i )
    {
    case 0: return NOTHINGFACE;
    case 1: return TRIANGLEFACE;
    case 2: return QUADFACE;
    default:
        Log( "default: @ GetFaceEnumType()\n" );
        Log( "i = " ); Log( i ); Log( '\n' );
        break;
    }

    return NOTHINGFACE;
}


//-----------------------------------------------------------------------------
// Name: GetFaceEnumID()
// Desc: 
//-----------------------------------------------------------------------------
int GetFaceEnumID( const FACETYPE eType )
{
    switch( eType )
    {
        case NOTHINGFACE:  return 0;
        case TRIANGLEFACE: return 1;
        case QUADFACE:     return 2;
        default: Log( "default: @ GetFaceEnumID()\n" ); break;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: GetClassEnumType()
// Desc: 
//-----------------------------------------------------------------------------
CLASSTYPE GetClassEnumType( const int i )
{
    switch( i )
    {
    case 0: return NOTHINGCLASS;
    case 1: return POLYGONMANAGER;
    case 2: return POLYGONCLASS;
    case 3: return FACECLASS;
    case 4: return VERTEXCLASS;
    case 5: return LIGHTCLASS;
    default:
        Log( "default: @ GetClassEnumType()\n" );
        Log( "i = " ); Log( i ); Log( "\n" );
        break;
    }

    return NOTHINGCLASS;
}


//-----------------------------------------------------------------------------
// Name: GetClassEnumID()
// Desc: 
//-----------------------------------------------------------------------------
int GetClassEnumID( const CLASSTYPE eType )
{
    switch( eType )
    {
    case NOTHINGCLASS:   return 0;
    case POLYGONMANAGER: return 1;
    case POLYGONCLASS:   return 2;
    case FACECLASS:      return 3;
    case VERTEXCLASS:    return 4;
    case LIGHTCLASS:     return 5;
    default: Log( "default: @ GetClassEnumID()\n" ); break;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: GetClassEnumID()
// Desc: 
//-----------------------------------------------------------------------------
bool RemoveArrayIndex( float** pfArray, const int iLength, const int iRemoveIndex )
{
    TRACE_ENTER_FN( DeleteArrayIndex() )

    int iA;
    int iIndex = 0;
    float** pTemp = NULL;
    bool bRetError = false;

    if( pfArray == NULL )        LOG_ERR_RET( false )
    if( iLength <= 0 )           { LOG_VAR( iLength )                         LOG_ERR_RET( false ) }
    if( iRemoveIndex <= 0 )      { LOG_VAR( iRemoveIndex )                    LOG_ERR_RET( false ) }
    if( iRemoveIndex > iLength ) { LOG_VAR( iRemoveIndex ) LOG_VAR( iLength ) LOG_ERR_RET( false ) }

    try
    {
        // Runs through the array and makes sure that nothing is null to
        // begin with. Every null index is outputted to the log file
        // and then an error is returned
        for( iA = 0; iA < iLength; iA++ )
        {
            if( pfArray[ iA ] == NULL )
            {
                bRetError = true;
                LOG_VAR( iA )
            }
        }

        if( bRetError ) LOG_ERR_RET( false )

        pfArray[ iRemoveIndex ] = NULL;

        pTemp = new float*[ iLength ];

        if( pTemp == NULL ) LOG_ERR_RET( false )

        for( iA = 0; iA < iLength; iA++ )
            pTemp[ iA ] = NULL;

        for( iA = 0; iA < iLength; iA++ )
        {
            if( pfArray[ iA ] != NULL )
            {
                pTemp[ iIndex ] = pfArray[ iA ];
                iIndex++;
            }
        }

        delete[] pfArray;
        pfArray = NULL;

        if( iLength == 1 ) RET( true )

        pfArray = new float*[ iLength - 1 ];

        for( iA = 0; iA < iLength - 1; iA++ )
            pfArray[ iA ] = pTemp[ iA ];

        delete[] pTemp;
    }
    catch( ... ) { LOG_ERR_RET( false ) }

    RET( true )
}


//-----------------------------------------------------------------------------
// Name: GetClassEnumID()
// Desc: 
//-----------------------------------------------------------------------------
bool RemoveArrayIndex( float*** pfArray, const int iLength, const int iRemoveIndex )
{
    TRACE_ENTER_FN( DeleteArrayIndex() )

    int iA;
    int iIndex = 0;
    float*** pTemp = NULL;
    bool bRetError = false;
    int iPlacement = 0;

//RET( true )

    if( pfArray == NULL )        LOG_ERR_RET( false )
    if( iLength <= 0 )           { LOG_VAR( iLength )                         LOG_ERR_RET( false ) }
    if( iRemoveIndex < 0 )      { LOG_VAR( iRemoveIndex )                    LOG_ERR_RET( false ) }
    if( iRemoveIndex > iLength ) { LOG_VAR( iRemoveIndex ) LOG_VAR( iLength ) LOG_ERR_RET( false ) }

    iPlacement++;

    try
    {
        // Runs through the array and makes sure that nothing is null to
        // begin with. Every null index is outputted to the log file
        // and then an error is returned
        for( iA = 0; iA < iLength; iA++ )
        {
            if( pfArray[ iA ] == NULL )
            {
                bRetError = true;
                LOG_VAR( iA )
            }
        }
iPlacement++;
        if( bRetError ) LOG_ERR_RET( false )
iPlacement++;
        pfArray[ iRemoveIndex ] = NULL;
iPlacement++;
        pTemp = new float**[ iLength ];
iPlacement++;
        if( pTemp == NULL ) LOG_ERR_RET( false )
iPlacement++;
        for( iA = 0; iA < iLength; iA++ )
            pTemp[ iA ] = NULL;
iPlacement++;
        for( iA = 0; iA < iLength; iA++ )
        {
            if( pfArray[ iA ] != NULL )
            {
                pTemp[ iIndex ] = pfArray[ iA ];
                iIndex++;
            }
        }
iPlacement++;
        delete[] pfArray;
        pfArray = NULL;
iPlacement++;
        if( iLength == 1 ) RET( true )
iPlacement++;
        pfArray = new float**[ iLength - 1 ];
iPlacement++;
        for( iA = 0; iA < iLength - 1; iA++ )
            pfArray[ iA ] = pTemp[ iA ];
iPlacement++;
        delete[] pTemp;
iPlacement++;
    }
    catch( ... ) { LOG_VAR( iPlacement ) LOG_ERR_RET( false ) }

    RET( true )
}