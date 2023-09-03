/*
 * This is a file that implements the reading of binary records and sleeptimes from disk.
 *
 * *** DO NOT CHANGE THIS FILE ***
 */
#include "binfile.h"
#include "record.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <string.h>
#include <ctype.h>

BinaryFile* bin_read_open( char* filename )
{
    FILE*       file;
    long        err;
    long        size;
    BinaryFile* bf;
    uint8_t*    buffer;

    file = fopen( filename, "r" );
    if( file == NULL )
    {
        fprintf( stderr, "%s:%d Failed to open %s for reading: %s\n", __FILE__, __LINE__, filename, strerror(errno) );
        return NULL;
    }

    err = fseek( file, 0, SEEK_END );
    if( err < 0 )
    {
        fprintf( stderr, "%s:%d Seek to end of binary file %s failed: %s\n", __FILE__, __LINE__, filename, strerror(errno) );
        fclose( file );
        return NULL;
    }

    size = ftell( file );
    if( err < 0 )
    {
        fprintf( stderr, "%s:%d Calling ftell on binary file %s failed: %s\n", __FILE__, __LINE__, filename, strerror(errno) );
        fclose( file );
        return NULL;
    }

    err = fseek( file, 0, SEEK_SET );
    if( err < 0 )
    {
        fprintf( stderr, "%s:%d Seek to start of binary file %s failed: %s\n", __FILE__, __LINE__, filename, strerror(errno) );
        fclose( file );
        return NULL;
    }

    bf = malloc( sizeof(BinaryFile) );
    if( bf == NULL )
    {
        fprintf( stderr, "%s:%d Failed to allocate internal memory for binary file %s\n", __FILE__, __LINE__, filename );
        fclose( file );
        return NULL;
    }

    buffer = malloc( size );
    if( buffer == NULL )
    {
        fprintf( stderr, "%s:%d Failed to allocate buffer for binary file %s\n", __FILE__, __LINE__, filename );
        free( bf );
        fclose( file );
        return NULL;
    }

    err = fread( buffer, 1, size, file );
    if( err < size )
    {
        fprintf( stderr, "%s:%d Failed to read %ld bytes from binary file %s, read %ld\n", __FILE__, __LINE__, size, filename, err );
        free( bf );
        fclose( file );
        return NULL;
    }

    fprintf( stderr, "%s:%d Successfully read %ld bytes from binary file %s\n", __FILE__, __LINE__, size, filename );
    fclose( file );

    bf->buffer       = buffer;
    bf->size         = size;
    bf->offset       = 0;
    bf->expect_sleep = true;

    return bf;
}

int bin_read( BinaryFile* bf, char* buffer, uint32_t bufsz )
{
    if( bf == NULL ) return 0;

    if( bf->offset >= bf->size ) return 0;

    if( bf->expect_sleep )
    {
        if( ( bf->offset + 4 <= bf->size ) && ( bufsz >= 4 ) )
        {
            uint32_t sleeptime;
            memcpy( &sleeptime, &bf->buffer[bf->offset], 4 );
            sleeptime = ntohl( sleeptime );
            memcpy( buffer, &sleeptime, 4 );
            bf->offset += 4;
            bf->expect_sleep = false;
            return 4;
        }
        else
        {
            fprintf( stderr, "%s:%d end of file reached or buffer to small.\n", __FILE__, __LINE__ );
            return 0;
        }
    }

    uint32_t sz = 0;
    uint8_t  flags;

    flags = bf->buffer[bf->offset];
    sz += 1;
    if( flags & FLAG_SRC )      sz += 1;
    if( flags & FLAG_DST )      sz += 1;
    if( flags & FLAG_USERNAME )
    {
        if( bf->offset + sz + 4 < bf->size )
        {
            uint32_t len = 0;
            memcpy( &len, &bf->buffer[bf->offset+sz], 4 );
            len = ntohl( len );
            sz += 4;
            sz += len;
        }
    }
    if( flags & FLAG_ID )       sz += 4;
    if( flags & FLAG_GROUP )    sz += 4;
    if( flags & FLAG_SEMESTER ) sz += 1;
    if( flags & FLAG_GRADE )    sz += 1;
    if( flags & FLAG_COURSES )  sz += 2;

    if( sz > bufsz )
    {
        fprintf( stderr, "%s:%d Buffer too small (%d bytes) to read binary record (%d bytes)n", __FILE__, __LINE__, bufsz, sz );
        return 0;
    }

    memcpy( buffer, &bf->buffer[bf->offset], sz );
    bf->offset += sz;

    bf->expect_sleep = true;

    return sz;
}

void bin_close( BinaryFile* bf )
{
    if( bf == NULL ) return;
    free( bf->buffer );
    free( bf );
}

