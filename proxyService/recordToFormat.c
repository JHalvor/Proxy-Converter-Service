/*
 * This is a file that implements writing of internal Records
 * into buffers either in XML or in binary format.
 *
 * *** DO NOT CHANGE THIS FILE ***
 */
#include "recordToFormat.h"

#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <ctype.h>
#include <arpa/inet.h>

#define BUFSZ 10000

char* recordToXML( Record* r, int* bufSize )
{
    char buffer[BUFSZ];
    int  sz = 0;

    snprintf( buffer, BUFSZ, "<record>\n");
    sz = strlen( buffer );
    if( r->has_source )
    {
        snprintf( buffer+sz, BUFSZ-sz, "  <source=\"%c\" />\n", r->source );
        sz = strlen( buffer );
    }
    if( r->has_dest )
    {
        snprintf( buffer+sz, BUFSZ-sz, "  <dest=\"%c\" />\n", r->dest );
        sz = strlen( buffer );
    }
    if( r->has_username )
    {
        snprintf( buffer+sz, BUFSZ-sz, "  <username=\"%s\" />\n", r->username );
        sz = strlen( buffer );
    }
    if( r->has_id )
    {
        snprintf( buffer+sz, BUFSZ-sz, "  <id=\"%u\" />\n", r->id );
        sz = strlen( buffer );
    }

    if( r->has_group )
    {
        snprintf( buffer+sz, BUFSZ-sz, "  <group=\"%d\" />\n", r->group );
        sz = strlen( buffer );
    }

    if( r->has_semester )
    {
        snprintf( buffer+sz, BUFSZ-sz, "  <semester=\"%u\" />\n", r->semester );
        sz = strlen( buffer );
    }

    if( r->has_grade )
    {
        switch( r->grade )
        {
        case Grade_Bachelor :
            snprintf( buffer+sz, BUFSZ-sz, "  <grade=\"Bachelor\" />\n" ); break;
        case Grade_Master :
            snprintf( buffer+sz, BUFSZ-sz, "  <grade=\"Master\" />\n" ); break;
        case Grade_PhD :
            snprintf( buffer+sz, BUFSZ-sz, "  <grade=\"PhD\" />\n" ); break;
        case Grade_None :
        default :
            snprintf( buffer+sz, BUFSZ-sz, "  <grade=\"None\" />\n" ); break;
        }
        sz = strlen( buffer );
    }

    if( r->has_courses )
    {
        snprintf( buffer+sz, BUFSZ-sz, "  <courses>\n" );
        sz = strlen( buffer );
        if( r->courses & Course_IN1000 ) { snprintf( buffer+sz, BUFSZ-sz, "    <course=\"IN1000\" />\n" ); sz = strlen( buffer ); }
        if( r->courses & Course_IN1010 ) { snprintf( buffer+sz, BUFSZ-sz, "    <course=\"IN1010\" />\n" ); sz = strlen( buffer ); }
        if( r->courses & Course_IN1020 ) { snprintf( buffer+sz, BUFSZ-sz, "    <course=\"IN1020\" />\n" ); sz = strlen( buffer ); }
        if( r->courses & Course_IN1030 ) { snprintf( buffer+sz, BUFSZ-sz, "    <course=\"IN1030\" />\n" ); sz = strlen( buffer ); }
        if( r->courses & Course_IN1050 ) { snprintf( buffer+sz, BUFSZ-sz, "    <course=\"IN1050\" />\n" ); sz = strlen( buffer ); }
        if( r->courses & Course_IN1060 ) { snprintf( buffer+sz, BUFSZ-sz, "    <course=\"IN1060\" />\n" ); sz = strlen( buffer ); }
        if( r->courses & Course_IN1080 ) { snprintf( buffer+sz, BUFSZ-sz, "    <course=\"IN1080\" />\n" ); sz = strlen( buffer ); }
        if( r->courses & Course_IN1140 ) { snprintf( buffer+sz, BUFSZ-sz, "    <course=\"IN1140\" />\n" ); sz = strlen( buffer ); }
        if( r->courses & Course_IN1150 ) { snprintf( buffer+sz, BUFSZ-sz, "    <course=\"IN1150\" />\n" ); sz = strlen( buffer ); }
        if( r->courses & Course_IN1900 ) { snprintf( buffer+sz, BUFSZ-sz, "    <course=\"IN1900\" />\n" ); sz = strlen( buffer ); }
        if( r->courses & Course_IN1910 ) { snprintf( buffer+sz, BUFSZ-sz, "    <course=\"IN1910\" />\n" ); sz = strlen( buffer ); }
        snprintf( buffer+sz, BUFSZ-sz, "  </courses>\n" );
        sz = strlen( buffer );
    }
    snprintf( buffer+sz, BUFSZ-sz, "</record>\n\n");
    
    char* retval = strdup( buffer );
    if( retval == NULL )
    {
        fprintf( stderr, "%s:%d Failed to allocated memory to duplicate a C string\n", __FILE__, __LINE__ );
        return NULL;
    }
    
    *bufSize = strlen( retval );

    return retval;
}

void fprintRecordAsXML( FILE* f, Record* r )
{
    int dummy;
    char* text = recordToXML( r, &dummy );
    if( text != NULL )
    {
        fprintf( f, "%s", text );
        free( text );
    }
}

void printRecordAsXML( Record* r )
{
    fprintRecordAsXML( stdout, r );
}

char* recordToBinary( Record* r, int* bufSize )
{
    uint8_t flags;
    flags = 0;
    if( r->has_source )   flags |= FLAG_SRC;
    if( r->has_dest )     flags |= FLAG_DST;
    if( r->has_username ) flags |= FLAG_USERNAME;
    if( r->has_id )       flags |= FLAG_ID;
    if( r->has_group )    flags |= FLAG_GROUP;
    if( r->has_semester ) flags |= FLAG_SEMESTER;
    if( r->has_grade )    flags |= FLAG_GRADE;
    if( r->has_courses )  flags |= FLAG_COURSES;

    int sz = 1 + 8*4;
    if( r->has_username ) sz += strlen(r->username);

    char* buffer = malloc( sz );
    if( buffer == NULL )
    {
        fprintf( stderr, "%s:%d Failed to allocated heap memory\n", __FILE__, __LINE__ );
        return NULL;
    }

    buffer[0] = '\0';
    sz = 0;

    buffer[sz] = flags;
    sz += 1;

    if( r->has_source )
    {
        buffer[sz] = r->source;
        sz += 1;
    }

    if( r->has_dest )
    {
        buffer[sz] = r->dest;
        sz += 1;
    }

    if( r->has_username )
    {
        size_t slen = strlen( r->username );
        uint32_t len = htonl( slen );
        memcpy( &buffer[sz], &len, 4 );
        sz += 4;
        memcpy( &buffer[sz], r->username, slen );
        sz += slen;
    }

    if( r->has_id )
    {
        uint32_t val = htonl( r->id );
        memcpy( &buffer[sz], &val, 4 );
        sz += 4;
    }

    if( r->has_group )
    {
        uint32_t val = htonl( r->group );
        memcpy( &buffer[sz], &val, 4 );
        sz += 4;
    }

    if( r->has_semester )
    {
        memcpy( &buffer[sz], &r->semester, 1 );
        sz += 1;
    }

    if( r->has_grade )
    {
        memcpy( &buffer[sz], &r->grade, 1 );
        sz += 1;
    }

    if( r->has_courses )
    {
        uint16_t val = htons( r->courses );
        memcpy( &buffer[sz], &val, 2 );
        sz += 2;
    }
    
    *bufSize = sz;

    char* retval = malloc( sz );
    if( retval == NULL )
    {
        fprintf( stderr, "%s:%d Failed to allocated heap memory\n", __FILE__, __LINE__ );
        free( buffer );
        return NULL;
    }
    memcpy( retval, buffer, sz );
    free( buffer );

    return retval;
}

void fprintRecordAsBinary( FILE* f, Record* r )
{
    int bufSize = 0;
    char* buffer = recordToBinary( r, &bufSize );
    if( buffer != NULL )
    {
        fwrite( buffer, 1, bufSize, f );
        free( buffer );
    }
}

