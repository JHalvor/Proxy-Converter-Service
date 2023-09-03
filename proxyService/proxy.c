/*
 * This is the main program for the proxy, which receives connections for sending and receiving clients
 * both in binary and XML format. Many clients can be connected at the same time. The proxy implements
 * an event loop.
 *
 * *** YOU MUST IMPLEMENT THESE FUNCTIONS ***
 *
 * The parameters and return values of the existing functions must not be changed.
 * You can add function, definition etc. as required.
 */
#include "xmlfile.h"
#include "connection.h"
#include "record.h"
#include "recordToFormat.h"
#include "recordFromFormat.h"

#include <arpa/inet.h>
#include <sys/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define BUFSIZE 9999        // BUFSIZE for reading to buffer with unknown size
#define MAX_CLIENTS 10      // Maximum expected amount of sender and receiver clients to be connected

/* This struct should contain the information that you want
 * keep for one connected client.
 */
struct Client
{
    int socket;
    struct Record *rec;
    char type;
    char ID;
};

typedef struct Client Client;

void usage( char* cmd )
{
    fprintf( stderr, "Usage: %s <port>\n"
                     "       This is the proxy server. It takes as imput the port where it accepts connections\n"
                     "       from \"xmlSender\", \"binSender\" and \"anyReceiver\" applications.\n"
                     "       <port> - a 16-bit integer in host byte order identifying the proxy server's port\n"
                     "\n",
                     cmd );
    exit( -1 );
}

/*
 * This function is called when a new connection is noticed on the server
 * socket.
 * The proxy accepts a new connection and creates the relevant data structures.
 *
 * *** The parameters and return values of this functions can be changed. ***
 * 
 */
void handleNewClient( int client_sock, fd_set *fds, Client *clients[], int *num_clients_ptr ) {
    // Allocating memory for new clients
    Client *new_client = malloc(sizeof(struct Client));

    // Initializing new client variables
    new_client->socket = client_sock;
    new_client->rec = NULL;
    new_client->type = 0;
    new_client->ID = 0;
    
    // Adding new client to clients array
    clients[*num_clients_ptr] = new_client;
    (*num_clients_ptr)++;
    FD_SET(client_sock, fds);
}
/*
 * This function is called when a connection is broken by one of the connecting
 * clients. Data structures are clean up and resources that are no longer needed
 * are released.
 *
 * *** The parameters and return values of this functions can be changed. ***
 */
void removeClient( fd_set *fds, Client *client, Client *clients[], int *num_clients_ptr )
{
    fprintf(stderr, "Removing client nr: %d\n", client->socket);
    // Remove client from fd set
    FD_CLR(client->socket, fds);

    int i, j;

    // find the index of the client in the array
    for (i = 0; i < *num_clients_ptr; i++) {
        if (client->socket == clients[i]->socket) {
            break;
        }
    }

    // shift all the elements after the client by one index to the left
    for (j = i; j < *num_clients_ptr - 1; j++) {
        clients[j] = clients[j + 1];
    }

    // Close client socket
    tcp_close(client->socket);

    // Free client memory
    if (client->rec) {
        if (client->rec->has_username) {
            clearRecord(client->rec);
        }
        deleteRecord(client->rec);
        client->rec = NULL;
    }
    free(client);

    // decrease the number of clients in the array
    (*num_clients_ptr)--;

    fprintf(stderr, "Client socket numbers left: ");
    for (i = 0; i < *num_clients_ptr; i++) {
        fprintf(stderr, "%d ", clients[i]->socket);
    }
    fprintf(stderr, "\n");
}

/*
 * This function is called when the proxy received enough data from a sending
 * client to create a Record. The 'dest' field of the Record determines the
 * client to which the proxy should send this Record.
 *
 * If no such client is connected to the proxy, the Record is discarded without
 * error. Resources are released as appropriate.
 *
 * If such a client is connected, this functions find the correct socket for
 * sending to that client, and determines if the Record must be converted to
 * XML format or to binary format for sendig to that client.
 *
 * It does then send the converted messages.
 * Finally, this function deletes the Record before returning.
 *
 * *** The parameters and return values of this functions can be changed. ***
 */
void forwardMessage( Client *client, int bufsize, Client *clients[], int *num_clients_ptr )
{
    fprintf(stderr, "Forwarding message!\n");
    int wc, destSocket, destID, destType;
    bool socketFound = false;
    char *buf;

    // Finding destination socket from msg->dest in clients[i]->ID
    for (int i = 0; i < *num_clients_ptr; i++) {
        if (clients[i]->ID == client->rec->dest) {
            destSocket = clients[i]->socket;
            socketFound = true;
            destID = clients[i]->ID;
            destType = clients[i]->type;
            break;
        }
    }

    if (socketFound) {
        if (destType == 'X') {
            fprintf(stderr, "Bytes written %d\n", bufsize);
            // Converting record stuct to XML
            buf = recordToXML(client->rec, &bufsize);
        } 
        else if (destType == 'B') {
            fprintf(stderr, "Bytes written %d\n", bufsize);
            // Converting record struct to Binary
            buf = recordToBinary(client->rec, &bufsize);
        }

        if (buf) {
            // Writing/sending to destination (anyReceiver)
            wc = tcp_write(destSocket, buf, bufsize);

            // Checking if writing/sending failed or not
            if (wc == -1) {
                fprintf(stderr, "Write Failed\n");
            } else {
                fprintf(stderr, "Write successfull with %d bytes written\n", wc);
                fprintf(stderr, "To client with ID %c and Type: %c\n", destID, destType);
            }
        } else {
            fprintf(stderr, "Buffer is empty\n");
        }
    }

    // Free Record
    if (client->rec) {
        clearRecord(client->rec);
        deleteRecord(client->rec);
        client->rec = NULL;
    }
    
    // Free buffer mem alloc
    free(buf);
}

/*
 * This function is called whenever activity is noticed on a connected socket,
 * and that socket is associated with a client. This can be sending client
 * or a receiving client.
 *
 * The calling function finds the Client structure for the socket where acticity
 * has occurred and calls this function.
 *
 * If this function receives data that completes a record, it creates an internal
 * Record data structure on the heap and calls forwardMessage() with this Record.
 *
 * If this function notices that a client has disconnected, it calls removeClient()
 * to release the resources associated with it.
 *
 * *** The parameters and return values of this functions can be changed. ***
 */
void handleClient( fd_set *fds, Client *client, Client *clients[], int *num_clients_ptr )
{
    fprintf(stderr, "Reading from connected client:\n");
    char buf[BUFSIZE];
    memset( buf, 0, BUFSIZE );
    int rc;
    
    // Read data from client
    rc = tcp_read(client->socket, buf, BUFSIZE);
    if (rc < 0) {
        perror("read() failed");
        exit(1);
    }

    // Client closed connection
    if (rc == 0) {
        removeClient( fds, client, clients, num_clients_ptr );
        return;
    }

    // Applying type and ID to client
    // Handling if the sender sends ID and type after each other
    if (rc == 1) {
        if (client->type == 0) {
            client->type = buf[0];
            fprintf(stderr, "Client Type: %c\n", buf[0]);
        }
        else if (client->ID == 0) {
            client->ID = buf[0];
            fprintf(stderr, "Client ID: %c\n", buf[0]);
        }
    }

    // Handling if the sender sends ID and type in the same message
    if (rc == 2) {
        if (client->type == 0) {
            client->type = buf[0];
            fprintf(stderr, "Client Type: %c\n", buf[0]);
        }
        if (client->ID == 0) {
            client->ID = buf[1];
            fprintf(stderr, "Client ID: %c\n", buf[1]);
        }
    }

    if (client->type != 0 && rc > 2) {
        if (client->type == 'X') {
            // Check if buffer contains a complete record for XML
            if (strncmp(buf, "<record>", 8) == 0 && 
                strstr(buf, "</record>") != NULL) {
                fprintf(stderr, "Bytes read %d\n", rc);

                // Converting XML to Record stuct
                client->rec = XMLtoRecord(buf, rc, &rc);
            }
        }
        else if (client->type == 'B') {
            fprintf(stderr, "Bytes read %d\n", rc);
            
            // Converting Binary message to Record stuct
            client->rec = BinaryToRecord(buf, rc, &rc);
        }
        // Fordarding message
        forwardMessage(client, rc, clients, num_clients_ptr);
    }
}


int main( int argc, char* argv[] )
{
    int port;
    int server_sock;
    fd_set master_fds;              // Master fd set
    fd_set read_fds;                // Temporary fd set for select()
    int fdmax;                      // Maximum fd number
    Client *clients[MAX_CLIENTS];
    int num_clients = 0;

    if( argc != 2 )
    {
        usage( argv[0] );
    }

    port = atoi( argv[1] );

    server_sock = tcp_create_and_listen( port );
    if( server_sock < 0 ) exit( -1 );

    // Clear the fd sets
    FD_ZERO(&master_fds);
    FD_ZERO(&read_fds);
    
    // Add the server socket to the fd set
    FD_SET(server_sock, &master_fds);
    fdmax = server_sock;
    
    /*
     * The following part is the event loop of the proxy. It waits for new connections,
     * new data arriving on existing connection, and events that indicate that a client
     * has disconnected.
     *
     * This function uses handleNewClient() when activity is seen on the server socket
     * and handleClient() when activity is seen on the socket of an existing connection.
     *
     * The loops ends when no clients are connected any more.
     */
    do
    {
        // Copy the master fd set to the temporary fd set for select()
        read_fds = master_fds;

        // Gets num of sockets/fd's that is ready 
        int readyFdCount = tcp_wait_timeout(&read_fds, fdmax, 10);
        if (readyFdCount == -1) { // Error
            return 0;
        }

        if (readyFdCount > 0) {
            // Iterates through to find the sockets with activity
            for (int fd = 0; fd <= fdmax; fd++) {
                // Found socket/fd with activity
                if (FD_ISSET(fd, &read_fds)) {
                    fprintf(stderr, "Found activity on socket %d\n", fd);
                    // Server socket activity: incoming connection
                    if (fd == server_sock) {
                        fprintf(stderr, "Setting up connection with new client\n");
                        int client_sock = tcp_accept(server_sock);
                        if (client_sock < 0) {
                            perror("tcp_accept() failed");
                            exit(-1);
                        }

                        // Add client socket to fd set
                        handleNewClient( client_sock, &master_fds, clients, &num_clients );
                        if (client_sock > fdmax) {
                            fdmax = client_sock;
                        }
                    }

                    // Client socket activity: incoming data
                    else {
                        // Find client
                        struct Client *client = NULL;
                        for (int i = 0; i <= num_clients; i++) {
                            if (clients[i]->socket == fd) {
                                client = clients[i];
                                break;
                            }
                        }

                        if (client == NULL) {
                            fprintf(stderr, "Error: client not found\n");
                            exit(1);
                        }

                        handleClient( &master_fds, client, clients, &num_clients );
                    }
                }
            }
        }
        
        fprintf(stderr, "\nClient connections left: %d\n", num_clients);
    }
    while( num_clients > 0 );

    FD_ZERO(&master_fds);
    FD_ZERO(&read_fds);
    tcp_close( server_sock );

    return 0;
}

