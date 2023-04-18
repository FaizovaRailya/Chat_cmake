#pragma once
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <stdlib.h>
#include <cstring>
#if defined(__linux__)
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#elif (defined(_WIN32) || defined(_WIN64))
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#endif

#define MESSAGE_LENGTH 1024 
#define PORT 7777 

class Socket_server
{
private:
    struct sockaddr_in tcpserveraddress, tcpclient;
    char message[MESSAGE_LENGTH];
#if defined(__linux__)
    int socket_id, accept_socket, bind_status, connection_status;
    socklen_t length;
#elif (defined(_WIN32) || defined(_WIN64))
    SOCKET socket_id = INVALID_SOCKET;
    SOCKET accept_socket = INVALID_SOCKET;
#endif

public:
    Socket_server();
    ~Socket_server();

    bool listen_socket();
    char* receive_data();
    int send_data(char* msg);
};

class Socket_client
{
private:
    struct sockaddr_in tcpserveraddress, tcpclient;
    char message[MESSAGE_LENGTH];
#if defined(__linux__)
    int socket_id, connection, bind_status, connection_status;
#elif (defined(_WIN32) || defined(_WIN64))
    SOCKET socket_id = INVALID_SOCKET;
#endif
    
public:
    Socket_client();
    ~Socket_client();

    char* receive_data();
    int send_data(char* msg);
};