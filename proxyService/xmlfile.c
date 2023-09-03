/*
 * This is a file that implements the reading of XML records and sleeptimes from disk.
 *
 * *** DO NOT CHANGE THIS FILE ***
 */
#include "xmlfile.h"

#include <sys/errno.h>
#include <string.h>
#include <ctype.h>

FILE* xml_read_open( char* filename )
{
    FILE* file = fopen( filename, "r" );
    if( file == NULL )
    {
        fprintf( stderr, "%s:%d Failed to open %s for reading: %s\n", __FILE__, __LINE__, filename, strerror(errno) );
    }
    return file;
}

static int xml_readline( FILE* file, char* buffer, int bufsz )
{
    char* ret;
    ret = fgets( buffer, bufsz, file );
    if( ret == NULL )
    {
        if( !feof(file) )
        {
            fprintf( stderr, "%s:%d An error occurred while reading from XML file: %s\n", __FILE__, __LINE__, strerror(errno) );
            return -1;
        }
        return 0;
    }
    return strlen( buffer );
}

int xml_read( FILE* file, char* buffer, int bufsz )
{
    char* debug = buffer;
    char* fx;
    int sum = 0;
    int ct;
    do
    {
        ct = xml_readline( file, buffer, bufsz );
        if( ct < 0 ) return ct;
        if( ct == 0 )
        {
            break;
        }

        fx      = buffer;
        while( *fx != 0 && isspace(*fx) ) fx++;
        if( *fx == 0 ) continue;

        fx      = buffer;
        sum    += ct;
        buffer += ct;
        bufsz  -= ct;

        while( *fx != 0 && isspace(*fx) ) fx++;
        if( strncmp( fx, "</record>", 9 ) == 0 )
        {
            break;
        }

        if( strncmp( fx, "<sleep", 6 ) == 0 )
        {
            break;
        }
    }
    while( ct > 0 );
    
    fprintf( stderr, "%s:%d Read the following text: >>>>%s<<<<<\n", __FILE__, __LINE__, debug );
    return sum;
}

void xml_close( FILE* file )
{
    fclose( file );
}

