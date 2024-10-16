#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <algorithm>
#include <map>
#include <vector>

#include <iostream>
#include <sstream>
#include <thread>
#include <map>

#include <array>
#include <memory>
#include <stdexcept>
#include <string>
#include <cstdio>

#include <fcntl.h>

#define CLIENT_BACKLOG 1 // We only communicate with one client at a time... I think

// Creates a socket with reusable address, and bound to provided port
// Function sets non-blocking if non-block is not set to 0
int set_non_blocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    return fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
}

int open_socket(int port_number) {
    struct sockaddr_in addr;
    int sockfd;
    int on = 1;

    // Create socket and set options
    if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("Failed to open socket.");
        return -1;
    }
    // Allow binding even if port already in use
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) {
        perror("Failed to set SO_REUSEADDR.");
        close(sockfd);
        return -1;
    }
    set_non_blocking(sockfd); // Non-blocking to handle events as they arrive instead of waiting for them

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_number);
    // Bind socket to provided port
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Failed to bind socket.");
        close(sockfd);
        return -1;
    }
    return sockfd;
}

// Note: In this botnet implementation if connection to the client is lost then the program cleansup and shuts down
int main(int argc, char *argv[])
{
    bool finished;
    int listenSock;                        // Socket for connections to server
    int clientSock;                        // Socket of connecting client
    fd_set openSockets;                    // Current open sockets
    fd_set readSockets;                    // Socket list for select()
    fd_set exceptSockets;                  // Exception socket list
    int maxfds;                            // Passed to select() as max fd in set
    struct sockaddr_in client;             // address of incoming client
    socklen_t clientLen;                   // address length
    char buffer[5000];                     // Read buffer is of length 5000, all messages of higher length will be truncated
    std::vector<int> clientSocketsToClear; // List of closed sockets to remove

    clientLen = sizeof(struct sockaddr_in);

    if (argc != 2)
    {
    std::cout << "Usage: server <port>" << std::endl;
    exit(1);
    }


    int server_port;
    try {
        server_port = atoi(argv[1]);
    } catch (std::exception& e) {
        perror("Port must be a number between 0 and 65535");
        exit(1);
    }

    int listen_sock; 
    // Create socket for maintaining all connections
    if ((listen_sock = open_socket(server_port)) < 0) { // 0 indicates NON_BLOCKING is off
        std::cout << "Failed to create client socket" << std::endl;
        exit(1);
    }

    // Start by connecting to client
    if (listen(listen_sock, CLIENT_BACKLOG) < 0) {
        perror("Failed to listen for client on port");
        close(listen_sock);
        exit(1);
    }
    // Exit if accepting 
    std::cout << "Waiting for client to connect" << std::endl;
    if (int client_connection = accept(listen_sock, (struct sockaddr*)&client, &clientLen) < 0) {
        perror("Failed to accept an incoming connection");
        close(listen_sock);
        exit(1);
    }

 


}