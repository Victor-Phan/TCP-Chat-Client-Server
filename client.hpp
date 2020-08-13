#pragma once

#include "main.h"
#include "filehandler.hpp"

class Client {
protected:
    int sd = 0;
    int port;
    std::string host;
    bool saveToFile = false;
    struct hostent *hp;
    struct sockaddr_in server;
    fd_set readfds;

    void startChat();

    void startSendMessage();

    void startReceiveMessage();

    bool createSocket();

    bool setServerInformation();

    bool connectServer();


public:
    Client() = default;

    Client(int port, std::string host) : port(port), host(host) {}

    virtual ~Client() {
        if (sd != 0) {
            close(sd);
        }
    }

    void startClient();

    void setPort(int port) { this->port = port; }

    void setHost(std::string host) { this->host = host; }

    int getSocket() { return sd; }

    void toggleMessageLogFile(bool save) { saveToFile = save; }
};