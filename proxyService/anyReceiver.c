/*
 * This is the main program for clients that can receive records in XML format and in binary format.
 * They write this information to disk.
 *
 * *** DO NOT CHANGE THIS FILE ***
 */
#include "connection.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void usage( char* cmd )
{
    fprintf( stderr, "Usage: %s <id> <type> <proxyport> <waittime>\n"
                     "       <id>        - a capital ASCII letter (A, B, C, ...)  giving this node a unique identity\n"
                     "       <type>      - the capital letter X or B, reporting as an XML or Binary receiver\n"
                     "       <proxyip>   - the proxy's IP address in dotted decimal format (e.g. 127.0.0.1)\n"
                     "       <proxyport> - a 16-bit integer in host byte order identifying the proxy's port\n"
                     "       <waittime>  - wait for new data for waittime seconds and terminate if nothing arrives (at least 1 sec)\n"
                     "\n",
                     cmd );
    exit( -1 );
}

static char getId( char* arg );
static char getType( char* arg );
static int  getPort( char* arg );

int main( int argc, char* argv[] )
{
    char* proxyip;
    int   proxyport;
    int sock;
    int ct;
    int err;
    char filename[6];
    FILE* file;
    char line[10000];
    char myid;
    char mytype;
    int sleeptime;

    if( argc != 6 )
    {
        usage( argv[0] );
    }

    myid      = getId( argv[1] );
    mytype    = getType( argv[2] );
    proxyip   = argv[3];
    proxyport = getPort( argv[4] );

    if( mytype == 'X' )
        sprintf( filename, "%c.xml", myid );
    else
        sprintf( filename, "%c.bin", myid );

    sleeptime = atoi(argv[5]);
    if( sleeptime < 1 )
    {
        fprintf( stderr, "%s:%d The sleeptime is too small.\n", __FILE__, __LINE__ );
        usage( argv[0] );
        exit( -1 );
    }

    sock = tcp_connect( proxyip, proxyport );
    if( sock < 0 ) exit( -1 );

    err = tcp_write( sock, &mytype, 1 );
    if( err < 0 ) exit( -1 );
    
    err = tcp_write( sock, &myid, 1 );
    if( err < 0 ) exit( -1 );
    
    file = fopen( filename, "w" );
    if( file == NULL ) exit( -1 );

    do
    {
        fprintf( stderr, "%s:%d Waiting for up to %u seconds\n", __FILE__, __LINE__, sleeptime );

        fd_set fdset;
        FD_ZERO( &fdset );
        FD_SET( sock, &fdset );
        err = tcp_wait_timeout( &fdset, sock+1, sleeptime );

        if( FD_ISSET( sock, &fdset ) )
        {
            ct = tcp_read( sock, line, 10000 );
            if( ct > 0 )
            {
                fprintf( stderr, "%s:%d %s %c has read %d bytes from socket\n", __FILE__, __LINE__, argv[0], myid, ct );
                err = fwrite( line, 1, ct, file );
                if( err < ct )
                {
                    fprintf( stderr, "%s:%d Failed to write %d bytes to the file %s\n", __FILE__, __LINE__, ct, filename );
                    exit( -1 );
                }
                else
                {
                    fprintf( stderr, "%s:%d Write %d bytes to the file %s\n", __FILE__, __LINE__, ct, filename );
                }

                fflush( file );
            }
        }
    }
    while( err > 0 );

    fclose( file );

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

static char getType( char* arg )
{
    char type;

    if( strlen( arg ) < 1 )
    {
        fprintf( stderr, "%s:%d The id cannot be shorter than one byte.\n", __FILE__, __LINE__ );
        exit( -1 );
    }

    type = arg[0];

    if( type != 'B' && type != 'X' )
    {
        fprintf( stderr, "%s:%d The type must be either B or X, but it is %c\n", __FILE__, __LINE__, type );
        exit( -1 );
    }

    return type;
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

