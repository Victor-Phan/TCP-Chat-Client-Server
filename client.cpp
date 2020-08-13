/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: 	client.cpp - Contains all logic relating to a client in the chat application
--
--
-- PROGRAM: 		Chat Application
--
-- FUNCTIONS:
--                  bool createSocket()
--                  bool setServerInformation()
--                  bool connectServer()
--                  void startSendMessage()
--                  void startClient()
--                  void startChat()
--
-- DATE: 			April 5, 2020
--
-- REVISIONS:
--
-- DESIGNER: 		Victor Phan
--
-- PROGRAMMER: 		Victor Phan
--
-- NOTES:
--                  FileHandler class pertains logic such as writing to a file of any type, reading from
-                   any file, and checking if a file exists.
--------------------------------------------------------------------------------------------------------------------*/
#include "client.hpp"

/*-----------------------------------------------------------------------------------------------------------------
-- Function:	createSocket
--
-- DATE:		April 5, 2020
--
-- REVISIONS:
--
-- DESIGNER: 	Victor Phan
--
-- PROGRAMMER: 	Victor Phan
--
-- INTERFACE:	bool createSocket()
--
-- RETURNS:     true if successful
--
-- NOTES:
--              Creates a TCP socket and assigns it
-------------------------------------------------------------------------------------------------------------------*/
bool Client::createSocket() {
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cerr << "Cannot create socket" << std::endl;
        return false;
    }
    return true;
}

/*-----------------------------------------------------------------------------------------------------------------
-- Function:	setServerInformation
--
-- DATE:		April 5, 2020
--
-- REVISIONS:
--
-- DESIGNER: 	Victor Phan
--
-- PROGRAMMER: 	Victor Phan
--
-- INTERFACE:	bool setServerInformation()
--
-- RETURNS:     true if successful
--
-- NOTES:
--              Gets the server's address information and assigns it.
--              Uses the port and hostname within the object to get the server's address information.
-------------------------------------------------------------------------------------------------------------------*/
bool Client::setServerInformation() {
    bzero((char *) &server, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if ((hp = gethostbyname(host.c_str())) == NULL) {
        std::cerr << "Unknown Server address" << std::endl;
        return false;
    }
    bcopy(hp->h_addr, (char *) &server.sin_addr, hp->h_length);
    return true;
}

/*-----------------------------------------------------------------------------------------------------------------
-- Function:	connectServer
--
-- DATE:		April 5, 2020
--
-- REVISIONS:
--
-- DESIGNER: 	Victor Phan
--
-- PROGRAMMER: 	Victor Phan
--
-- INTERFACE:	bool connectServer()
--
-- RETURNS:     true if successful
--
-- NOTES:
--              Connects to the server specified in setServerInformation.
-------------------------------------------------------------------------------------------------------------------*/
bool Client::connectServer() {
    if (connect(sd, (struct sockaddr *) &server, sizeof(server)) == -1) {
        std::cerr << "Can't connect to Server" << std::endl;
        return false;
    }
    return true;
}

/*-----------------------------------------------------------------------------------------------------------------
-- Function:	startSendMessage
--
-- DATE:		April 5, 2020
--
-- REVISIONS:
--
-- DESIGNER: 	Victor Phan
--
-- PROGRAMMER: 	Victor Phan
--
-- INTERFACE:	void startSendMessage()
--
-- RETURNS:     void
--
-- NOTES:
--              Reads from STDIN and writes the input to the TCP socket
-------------------------------------------------------------------------------------------------------------------*/
void Client::startSendMessage() {
    char sbuf[BUFLEN];
    memset(sbuf, 0, BUFLEN);
    read(0, sbuf, BUFLEN);
    //fgets(sbuf, BUFLEN, stdin);
    // Transmit data through the socket
    send(getSocket(), sbuf, BUFLEN, 0);
    fflush(stdout);
    if (saveToFile) {
        FileHandler::saveDataToFile(CHAT_LOG_FILE_PATH, sbuf, strlen(sbuf));
    }
}

/*-----------------------------------------------------------------------------------------------------------------
-- Function:	startReceiveMessage
--
-- DATE:		April 5, 2020
--
-- REVISIONS:
--
-- DESIGNER: 	Victor Phan
--
-- PROGRAMMER: 	Victor Phan
--
-- INTERFACE:	void startReceiveMessage()
--
-- RETURNS:     void
--
-- NOTES:
--              Reads from the TCP socket and prints the output. Also appends the output
--              to a file if the option was specified.
-------------------------------------------------------------------------------------------------------------------*/
void Client::startReceiveMessage() {
    char *bp, rbuf[BUFLEN];
    int bytes_to_read = BUFLEN;
    int n = 0;
    int numBytes = 0;
    bp = rbuf;
    memset(rbuf, 0, BUFLEN);
    while ((n = recv(getSocket(), bp, bytes_to_read, 0)) > 0 && bytes_to_read <= 0) {
        numBytes += n;
        bp += n;
        bytes_to_read -= n;
    }
    if (saveToFile) {
        FileHandler::saveDataToFile(CHAT_LOG_FILE_PATH, rbuf, strlen(rbuf));
    }
    //Messsage should have a /n already
    fflush(stdout);
    printf("%s", rbuf);
    fflush(stdout);
}

/*-----------------------------------------------------------------------------------------------------------------
-- Function:	startClient
--
-- DATE:		April 5, 2020
--
-- REVISIONS:
--
-- DESIGNER: 	Victor Phan
--
-- PROGRAMMER: 	Victor Phan
--
-- INTERFACE:	void startClient()
--
-- RETURNS:     void
--
-- NOTES:
--              Hostname and port must be provided before this function is called.
--              Creates a socket, and connects to the TCP server.
--              The client will then be able to send and receive messages.
-------------------------------------------------------------------------------------------------------------------*/
void Client::startClient() {
    // Create the socket
    if (!createSocket()) {
        exit(1);
    }
    // Save Server information
    if (!setServerInformation()) {
        exit(1);
    }
    // Connecting to the Server
    if (!connectServer()) {
        exit(1);
    }
    startChat();
}

/*-----------------------------------------------------------------------------------------------------------------
-- Function:	startChat
--
-- DATE:		April 6, 2020
--
-- REVISIONS:
--
-- DESIGNER: 	Victor Phan
--
-- PROGRAMMER: 	Victor Phan
--
-- INTERFACE:	void startChat()
--
-- RETURNS:     void
--
-- NOTES:
--              Uses select to handle reading from STDIN and reading from the socket.
--
-------------------------------------------------------------------------------------------------------------------*/
void Client::startChat() {
    while (true) {
        FD_ZERO(&readfds);
        FD_SET(sd, &readfds);
        //0 is STDIN
        FD_SET(0, &readfds);
        if(select(sd + 1, &readfds, NULL,NULL,NULL) < 0){
            fprintf(stdout, "select() error\n");
            exit(1);
        }
        if(FD_ISSET(sd, &readfds) != 0) {
            startReceiveMessage();
        }
        if(FD_ISSET(0, &readfds) != 0) {
            startSendMessage();
        }
    }
}
