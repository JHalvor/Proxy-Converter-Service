/*
 * This is the main program for clients that send records in binary format.
 * They read from files on disk that contain sleeptimes and records in binary format.
 *
 * *** DO NOT CHANGE THIS FILE ***
 */
#include "binfile.h"
#include "connection.h"

#include <stdio.h>
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
    BinaryFile* file;
    char line[10000];
    char myid;

    if( argc != 4 ) usage( argv[0] );

    myid      = getId( argv[1] );
    filename  = makeFilename( myid );
    proxyip   = argv[2];
    proxyport = getPort( argv[3] );

    sock = tcp_connect( proxyip, proxyport );
    if( sock < 0 ) exit( -1 );

    /* Send one byte to the proxy to identify this sender as a binary sender. */
    err = tcp_write( sock, "B", 1 );
    if( err < 0 ) exit( -1 );
    fprintf( stderr, "%s:%d %s %c has sent %d bytes to proxy successfully\n", __FILE__, __LINE__, argv[0], myid, err );
    
    /* Send one byte containing the sender's ID to the proxy. */
    err = tcp_write( sock, &myid, 1 );
    if( err < 0 ) exit( -1 );
    fprintf( stderr, "%s:%d %s %c has sent %d bytes to proxy successfully\n", __FILE__, __LINE__, argv[0], myid, err );
    
    file = bin_read_open( filename );
    if( file == NULL ) exit( -1 );
    free( filename ); /* we don't need the name any more */

    /* Loop and read sleeptimes and records from the binary file until the end of file. */
    do
    {
        uint32_t sleeptime;
        ct = bin_read( file, (char*)(&sleeptime), 4 );
        if( ct > 0 )
        {
            fprintf( stderr, "%s:%d read sleep command, sleeping for %d seconds\n", __FILE__, __LINE__, sleeptime );
            sleep( sleeptime );
        }

        ct = bin_read( file, line, 10000 );
        if( ct > 0 )
        {
            fprintf( stderr, "%s:%d %s %c has read %d bytes from file\n", __FILE__, __LINE__, argv[0], myid, ct );

            err = tcp_write_loop( sock, line, ct );
            if( err < 0 ) exit( -1 );
            fprintf( stderr, "%s:%d %s %c has sent %d bytes to proxy successfully\n", __FILE__, __LINE__, argv[0], myid, err );
        }
    }
    while( ct > 0 );

    bin_close( file );

    tcp_close( sock );

    return 0;
}

/*
 * Extract the sender's ID from the argument.
 */
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

/*
 * Make a filename based on the sender's ID.
 */
static char* makeFilename( char myid )
{
    char* filename;
    filename = malloc( 6 );
    if( filename == NULL )
    {
        fprintf( stderr, "%s:%d Could not allocate 6 bytes for the filename\n", __FILE__, __LINE__ );
        exit( -1 );
    }
    sprintf( filename, "%c.bin", myid );
    return filename;
}

/*
 * Get the proxy's port from the argument.
 */
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

