#pragma once

#include <sys/select.h>

/* The function takes a hostname in dotted decimal format and a port number.
 * It returns a socket if it is able to connect to a TCP server socket.
 * It prints error messages to stderr and returns -1 in case of failure.
 */
int tcp_connect( char* hostname, int port );

/* The function reads up to n bytes from the socket sock and stores
 * them in memory at buffer.
 * If it returns a positive value, this is the number of bytes that were
 * read.
 * If it returns 0, it means that the socket has been closed by the other
 * side.
 * If it returns a value lower than 0, an error has occur. The function
 * prints an error message and returns the error code.
 */
int tcp_read( int sock, char* buffer, int n );

/* Writes the give number of bytes from the buffer buffer to the socket sock.
 * Prints an error message to stderr in case of failure.
 * Return the number of written bytes in case of success and -1 in case of
 * error.
 */
int tcp_write( int sock, char* buffer, int bytes );

/* Write to the given TCP socket sock in a loop until an error occurs or
 * bytes many bytes have been written.
 */
int tcp_write_loop( int sock, char* buffer, int bytes );

/* Closes the given socket. Fails silently and would also close all other
 * kinds of file handle.
 */
void tcp_close( int sock );

/* Creates a TCP server socket and puts it into the listening state.
 * Does not wait for any connections.
 * It returns a socket if it is able to create a TCP server socket.
 * It prints error messages to stderr and returns -1 in case of failure.
 */
int tcp_create_and_listen( int port );

/* Accepts a connection to the server socket. The connection gets a
 * new socket, which is return in case of success.
 * In case of failure, the function prints and error message to
 * stderr and return -1.
 */
int tcp_accept( int server_sock );

/* This function waits until there is any form of read activity on one
 * of the sockets that have been added to the waiting_set, but only for
 * socket whose integer value is below wait_end.
 * The function wait endlessly if no activity occurs.
 * Returns -1 in case of error and >=0 in case of success. In case of
 * success, zero or more sockets may have experienced activity.
 */
int tcp_wait( fd_set* waiting_set, int wait_end );

/* Similar to the function tcp_wait, but waits at most timeout seconds
 * before returning.
 */
int tcp_wait_timeout( fd_set* waiting_set, int wait_end, int timeout );

