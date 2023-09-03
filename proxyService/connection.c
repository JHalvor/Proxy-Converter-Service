/*
 * This is a file that implements the operation on TCP sockets that are used by
 * all of the programs used in this assignment.
 *
 * *** YOU MUST IMPLEMENT THESE FUNCTIONS ***
 *
 * The parameters and return values of the existing functions must not be changed.
 * You can add function, definition etc. as required.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>


#include "connection.h"


/* Takes a hostname and port number 
 * and returns socket if its able to connect to a TCP server socket.
 * Prints error messages and returns -1 in case of failure.
 */
int tcp_connect( char* hostname, int port )
{
    // Checks parameters
    if (hostname == NULL) {
        perror("tcp_connect: hostname is NULL");
        return -1;
    }

    if (port < 0 || port > 65535) {
        perror("tcp_connect: invalid port number");
        return -1;
    }

    // Defining socket and connection descriptors and address
    int sock, con;
    struct sockaddr_in server_addr;

    // Creating a TCP Socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("tcp_connect: ERROR opening socket");
        return -1;
    }

    // Set server adress
    // Initializing sockaddr_in padding bytes to zero before connect call
    memset(&server_addr, 0, sizeof(struct sockaddr_in)); 
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    // Converting hostname to IP address
    con = inet_pton(AF_INET, hostname, &server_addr.sin_addr.s_addr);
    if (con == -1) {
        perror("tcp_connect - inet_pton: unsupported address family");
        tcp_close(sock);
        return -1;
    }
    if (!con) {
        fprintf(stderr, "tcp_connect - inet_pton: bad address fromat %s\n", hostname);
        tcp_close(sock);
        return -1;
    }

    // Connecting to server
    con = connect(sock, 
                 (struct sockaddr*)&server_addr, 
                 sizeof(struct sockaddr_in));
    if (con == -1) {
        perror("tcp_connect - connect: ERROR connecting");
        tcp_close(sock);
        return -1;
    }

    return sock;
}

/* Reads up to n bytes from sock and stores it in buffer.
 * Returns > 0 with number of bytes read.
 * Returns 0 if socket is closed by other side.
 * Returns < 0 and prints error if error occurs
 */
int tcp_read( int sock, char* buffer, int n )
{
    // Reads up to n bytes from sock to buffer
    int num_read = read(sock, buffer, n);

    // Error checks
    if (num_read < 0) {
        // Error message with below zero on return
        perror("tcp_read: Could not read from socket");
        return num_read;
    } else if (num_read == 0) {
        // other side has closed the socket
        fprintf(stderr, "tcp_read: Socket closed by other side\n");
        return num_read;
    } else {
        // Number of bytes read
        return num_read;
    }
}

/* Writes give number of bytes from buffer to sock.
 * Returns number of written bytes
 * Returns -1 in case of error with print of error message
 */
int tcp_write( int sock, char* buffer, int bytes )
{
    // Chekcs if buffer is NULL
    if (buffer == NULL) {
        perror("tcp_write: buffer is NULL");
        return -1;
    }

    // Writing bytes from sock to buffer
    int num_written = write(sock, buffer, bytes);

    // Error checks
    if (num_written < 0) {
        // Error occured
        perror("tcp_write: Could not write to socket");
        return -1;
    } else if (num_written != bytes) {
        // Check that all bytes were written to the socket to ensure data integrity
        fprintf(stderr, "tcp_write: Could not write all bytes to socket\n");
        return -1;
    }

    return num_written;
}

/* Writes up to `bytes` bytes of data from the buffer `buffer`
 * to the socket `sock`. If it is successful, it returns the number of bytes
 * written. If an error occurs, it returns the error code from the `tcp_write`
 * function.
 */
int tcp_write_loop( int sock, char* buffer, int bytes )
{
    // Chekcs if buffer is NULL
    if (buffer == NULL) {
        perror("tcp_write_loop: buffer is NULL");
        return -1;
    }

    int total_written = 0;
    while (total_written < bytes) {
        int remaining = bytes - total_written;
        int num_written = tcp_write(sock, buffer + total_written, remaining);
        // Returns num_written when error occurs
        if (num_written < 0) {
            return num_written;
        }
        total_written += num_written;
    }

    if (total_written != bytes) {
        fprintf(stderr, "tcp_write_loop: Could not write all bytes to socket\n");
        return -1;
    }

    // 'bytes' bytes have been written returns bytes written
    return total_written;
}

void tcp_close( int sock )
{
    close(sock);
}

int tcp_create_and_listen( int port )
{
    if (port < 0 || port > 65535) {
        perror("tcp_create_and_listen: invalid port number");
        return -1;
    }

    struct sockaddr_in server_addr;
    int server_fd, rc;

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("tcp_create_and_listen: Could not create socket");
        return -1;
    }

    // Replacing bytes padding in address with 0's
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    // Set the server address and port
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Binding socket to server address and port
    rc = bind(server_fd, 
            (struct sockaddr*)&server_addr, 
            sizeof(struct sockaddr_in));
    if (rc == -1) {
        perror("tcp_create_and_listen: Could not bind socket");
        tcp_close(server_fd);
        return -1;
    }
        
    // Putting socket in listening state
    rc = listen(server_fd, 5);
    if (rc == -1) {
        perror("tcp_create_and_listen: Could not listen on socket");
        tcp_close(server_fd);
        return -1;
    }

    // Returning socket file descriptor on success
    return server_fd;
}

int tcp_accept( int server_sock )
{
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
 
    // Accepts the first connection in queue in the listening server_sock.
    // On accept client_sock recieves a new client socket from the first connection in queue
    // the Client_addr structure is filled with address info of the client on success
    // And automatically assosiated with client_sock through the accept() successfull call
    int client_sock = accept(server_sock, 
                            (struct sockaddr*)&client_addr, 
                            &client_addr_len);

    // Error check, print to stderr and return -1 on error
    if (client_sock == -1) {
        perror("tcp_accept: Could not accept connection");
        return -1;
    }

    return client_sock;
}

int tcp_wait( fd_set* waiting_set, int wait_end )
{
    // Wait for activity on the sockets (waiting_set) using select()
    int num_sock_activity = select(wait_end + 1, 
                                   waiting_set, 
                                   NULL, 
                                   NULL, 
                                   NULL);

    if (num_sock_activity == -1) {
        perror("tcp_wait: Error in select()");
        return -1;
    }

    // Returns number of sockets that have experienced activity.
    return num_sock_activity;
}

int tcp_wait_timeout( fd_set* waiting_set, int wait_end, int seconds )
{
    struct timeval timeout;
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;

    // Wait (timeout) seconds for activity on the sockets (waiting_set) using select()
    int num_sock_activity = select(wait_end+1, 
                                   waiting_set, 
                                   NULL, 
                                   NULL, 
                                   &timeout);

    if (num_sock_activity < 0) {
        perror("tcp_wait_timeout: Error in select()");
        return -1;
    }

    // Returns number of sockets that have experienced activity.
    return num_sock_activity;
}

