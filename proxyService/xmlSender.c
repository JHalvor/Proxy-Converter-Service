/*
 * This is the main program for clients that send records in XML format.
 * They read from files on disk that contain sleeptimes and records in XML format.
 *
 * *** DO NOT CHANGE THIS FILE ***
 */
#include "xmlfile.h"
#include "connection.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void usage( char* cmd )
{
    fprintf( stderr, "Usage: %s <id> <proxyport>\n"
                     "       <id>        - a capital ASCII letter (A, B, C, ...)  giving this node a unique identity\n"
                     "       <proxyip>   - the proxy's IP address in dotted decimal format (e.g. 127.0.0.1)\n"
                     "       <proxyport> - a 16-bit integer in host byte order identifying the proxy's port\n"
                     "\n",
                     cmd );
    exit( -1 );
}

static char getId( char* arg );
static char* makeFilename( char myid );
static int getPort( char* arg );

int main( int argc, char* argv[] )
{
    char* proxyip;
    int   proxyport;
    int sock;
    int ct;
    int err;
    char* filename;
    FILE* file;
    char line[10000];
    char myid;
    unsigned int sleeptime;

    if( argc != 4 ) usage( argv[0] );

    myid      = getId( argv[1] );
    filename  = makeFilename( myid );
    proxyip   = argv[2];
    proxyport = getPort( argv[3] );

    sock = tcp_connect( proxyip, proxyport );
    if( sock < 0 ) exit( -1 );

    /* Send one byte to the proxy to identify this sender as an XML sender. */
    err = tcp_write( sock, "X", 1 );
    if( err < 0 ) exit( -1 );
    fprintf( stderr, "%s:%d %s %c has sent %d bytes to proxy successfully\n", __FILE__, __LINE__, argv[0], myid, err );
    
    /* Send one byte containing the sender's ID to the proxy. */
    err = tcp_write( sock, &myid, 1 );
    if( err < 0 ) exit( -1 );
    fprintf( stderr, "%s:%d %s %c has sent %d bytes to proxy successfully\n", __FILE__, __LINE__, argv[0], myid, err );
    
    file = xml_read_open( filename );
    if( file == NULL ) exit( -1 );
    free( filename ); /* we don't need the name any more */

    /* Loop and read sleeptimes and records from the text file until the end of file. */
    do
    {
        ct = xml_read( file, line, 10000 );
        if( ct > 0 )
        {
            fprintf( stderr, "%s:%d %s %c has read %d bytes from file\n", __FILE__, __LINE__, argv[0], myid, ct );

            err = sscanf( line, "<sleep=\"%u\" />", &sleeptime );
            if( err == 1 )
            {
                fprintf( stderr, "%s:%d read sleep command, sleeping for %d seconds\n", __FILE__, __LINE__, sleeptime );
                sleep( sleeptime );
            }
            else
            {
                err = tcp_write_loop( sock, line, ct );
                if( err < 0 ) exit( -1 );
                fprintf( stderr, "%s:%d %s %c has sent %d bytes to proxy successfully\n", __FILE__, __LINE__, argv[0], myid, err );
            }
        }
    }
    while( ct > 0 );

    xml_close( file );

    tcp_close( sock );

    return 0;
}

static char getId( char* arg )
{
    char id;

    if( strlen( arg ) < 1 )
    {
        fprintf( stderr, "%s:%d The id cannot be shorter than one byte.\n", __FILE__, __LINE__ );
        exit( -1 );
    }

    id = arg[0];

    if( id < 'A' || id > 'Z' )
    {
        fprintf( stderr, "%s:%d The id must be a capital ASCII letter, but it is %c\n", __FILE__, __LINE__, id );
        exit( -1 );
    }

    return id;
}

static char* makeFilename( char myid )
{
    char* filename;
    filename = malloc( 6 );
    if( filename == NULL )
    {
        fprintf( stderr, "%s:%d Could not allocate 6 bytes for the filename\n", __FILE__, __LINE__ );
        exit( -1 );
    }
    sprintf( filename, "%c.xml", myid );
    return filename;
}

static int getPort( char* arg )
{
    int port = atoi( arg );
    if( port < 1024 || port > 65535 )
    {
        fprintf( stderr, "%s:%d The proxy port %d is not in the valid range.\n", __FILE__, __LINE__, port );
        exit( -1 );
    }
    return port;
}

