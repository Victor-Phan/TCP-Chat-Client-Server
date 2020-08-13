#pragma once
#include "main.h"

class Server
{
    struct connected_clients
    {
        std::string ipAddress;
        int fd;
    };

public:
    Server(int portNum) : port{portNum} {};
    ~Server() = default;

    void startServer();

private:
    bool createSocket();
    bool bindSocket();
    void handleConnections();
    bool addToConnectedList();
    void printConnectedList();
    void echoString(std::string &recvString, int index);

private:
    bool run_server;

    int listenSocket;
    int port;
    int maxFileDescriptor;
    int maxClientIndex;
    struct connected_clients client[FD_SETSIZE];

    char recvBuffer[200];

    fd_set readingSet, allSet;
    struct sockaddr_in clientAddr;
};
