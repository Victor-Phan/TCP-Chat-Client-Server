/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: 	server.cpp - Contains all logic relating to a server in the chat application
--
--
-- PROGRAM: 		Chat Application
--
-- FUNCTIONS:
--                  Server(int portNum): port{portNum}{};
--                  ~Server() = default;
--                  void startServer();
--                  bool createSocket();
--                  bool bindSocket();
--                  void handleConnections();
--                  bool addToConnectedList();
--                  void printConnectedList();
--                  void echoString(std::string &recvString, int index);
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
--                  The Server class utilizes the select call to be notified when there is client activity.
--------------------------------------------------------------------------------------------------------------------*/
#include "Server.h"

/*-----------------------------------------------------------------------------------------------------------------
-- Function:	startServer()
--
-- DATE:		April 5, 2020
--
-- REVISIONS:
--
-- DESIGNER: 	Victor Phan
--
-- PROGRAMMER: 	Victor Phan
--
-- INTERFACE:	void startServer()
--
-- RETURNS:     void
--
-- NOTES:
--              Prepares the resources necessary for a server to run.
-------------------------------------------------------------------------------------------------------------------*/
void Server::startServer()
{
    run_server = true;

    if (!createSocket() || !bindSocket())
    {
        return;
    }

    listen(listenSocket, MAX_CONNECT_REQUEST);
    maxFileDescriptor = listenSocket;
    for (int i = 0; i < FD_SETSIZE; i++)
    {
        client[i].fd = -1;
    }
    FD_ZERO(&allSet);
    FD_SET(listenSocket, &allSet);

    handleConnections();
}

/*-----------------------------------------------------------------------------------------------------------------
-- Function:	createSocket()
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
-- RETURNS:     bool - true if the process was successful
--
-- NOTES:
--              Creates a socket for the server to listen on. Sets additional parameters for the socket.
-------------------------------------------------------------------------------------------------------------------*/
bool Server::createSocket()
{
    int arg, result;
    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == -1)
    {
        std::cerr << "failed to create socket" << std::endl;
        return false;
    }
    result = setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &arg, sizeof(arg));
    if (result == -1)
    {
        std::cerr << "Error setting socket with SO_REUSEADDR" << std::endl;
        return false;
    }
    return true;
}

/*-----------------------------------------------------------------------------------------------------------------
-- Function:	bindSocket()
--
-- DATE:		April 5, 2020
--
-- REVISIONS:
--
-- DESIGNER: 	Victor Phan
--
-- PROGRAMMER: 	Victor Phan
--
-- INTERFACE:	bool bindSocket()
--
-- RETURNS:     bool - true if the process was successful
--
-- NOTES:
--              Binds the created socket to the stack on a specified port.
-------------------------------------------------------------------------------------------------------------------*/
bool Server::bindSocket()
{
    struct sockaddr_in server
    {
    };
    int result;

    bzero((char *)&server, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    result = bind(listenSocket, (struct sockaddr *)&server, sizeof(server));
    if (result == -1)
    {
        std::cerr << "Error binding server socket" << std::endl;
        return false;
    }

    return true;
}

/*-----------------------------------------------------------------------------------------------------------------
-- Function:	handleConnections()
--
-- DATE:		April 5, 2020
--
-- REVISIONS:
--
-- DESIGNER: 	Victor Phan
--
-- PROGRAMMER: 	Victor Phan
--
-- INTERFACE:	void handleConnections()
--
-- RETURNS:     void
--
-- NOTES:
--              Main function that handles all the connection logic on the Server.
--              Uses select() to notify for events. If the listen socket is alerted, then will accept the incoming
--              request. If all other sockets are alerted, then will read from the socket and echo the data
--              to other connected clients.
-------------------------------------------------------------------------------------------------------------------*/
void Server::handleConnections()
{
    int numFDReady = -1;
    maxClientIndex = 0;

    while (run_server)
    {
        readingSet = allSet;
        numFDReady = select(maxFileDescriptor + 1, &readingSet, NULL, NULL, NULL);

        if (FD_ISSET(listenSocket, &readingSet))
        {
            if (!addToConnectedList())
            {
                return;
            }
            printConnectedList();
            if (--numFDReady <= 0)
            {
                continue;
            }
        }

        for (int i = 0; i <= maxClientIndex; i++)
        {
            char *bufferPointer = recvBuffer;
            memset(recvBuffer, '\0', sizeof(recvBuffer));
            int socketFileDescriptor = client[i].fd;
            int bytes_to_read = BUFLEN;
            int bytes_read;

            if (socketFileDescriptor < 0)
            { // no data
                continue;
            }
            if (FD_ISSET(socketFileDescriptor, &readingSet))
            {
                bool firstRead = true;
                while ((bytes_read = read(socketFileDescriptor, bufferPointer, bytes_to_read)) > 0)
                {
                    firstRead = false;
                    bufferPointer += bytes_read;
                    bytes_to_read -= bytes_read;
                }
                std::string recv_string{recvBuffer};
                echoString(recv_string, i);

                //Handle disconnection
                if (firstRead)
                {
                    std::cout << "connection closed: " << client[i].ipAddress << std::endl;
                    FD_CLR(socketFileDescriptor, &allSet);
                    close(socketFileDescriptor);
                    client[i].fd = -1;
                }

                if (--numFDReady <= 0)
                {
                    break;
                }
            }
        }
    }
}

/*-----------------------------------------------------------------------------------------------------------------
-- Function:	addToConnectedList()
--
-- DATE:		April 5, 2020
--
-- REVISIONS:
--
-- DESIGNER: 	Victor Phan
--
-- PROGRAMMER: 	Victor Phan
--
-- INTERFACE:	bool addToConnectedList()
--
-- RETURNS:     bool - true if process is successful
--
-- NOTES:
--              Adds a client to the Connected client list and store the client socket.
-------------------------------------------------------------------------------------------------------------------*/
bool Server::addToConnectedList()
{
    int newSocket, clientIndex;
    socklen_t clientLength = sizeof(clientAddr);
    newSocket = accept(listenSocket, (struct sockaddr *)&clientAddr, &clientLength);
    if (newSocket == -1)
    {
        std::cerr << ("Error accepting new client") << std::endl;
        return false;
    }

    // inet_ntop(AF_INET, &clientAddr, client[maxClientIndex].ipAddress, INET_ADDRSTRLEN);

    for (clientIndex = 0; clientIndex < FD_SETSIZE; clientIndex++)
    {
        if (client[clientIndex].fd < 0)
        {
            client[clientIndex].fd = newSocket;
            client[clientIndex].ipAddress = inet_ntoa(clientAddr.sin_addr);
            break;
        }
    }

    if (clientIndex == FD_SETSIZE)
    {
        std::cerr << ("Too many clients connected") << std::endl;
        return false;
    }

    FD_SET(newSocket, &allSet);
    if (newSocket > maxFileDescriptor)
    {
        maxFileDescriptor = newSocket;
    }

    if (clientIndex > maxClientIndex)
    {
        maxClientIndex = clientIndex;
    }

    return true;
}

/*-----------------------------------------------------------------------------------------------------------------
-- Function:	printConnectedList()
--
-- DATE:		April 5, 2020
--
-- REVISIONS:
--
-- DESIGNER: 	Victor Phan
--
-- PROGRAMMER: 	Victor Phan
--
-- INTERFACE:	void printConnectedList()
--
-- RETURNS:     void
--
-- NOTES:
--              Prints all conected clients.
-------------------------------------------------------------------------------------------------------------------*/
void Server::printConnectedList()
{
    std::cout << "Connected client list" << std::endl;
    for (int i = 0; i < maxClientIndex + 1; i++)
    {
        if (client[i].fd >= 0)
            std::cout << client[i].ipAddress << std::endl;
    }
}

/*-----------------------------------------------------------------------------------------------------------------
-- Function:	echoString(std::string &recvString, int index)
--
-- DATE:		April 5, 2020
--
-- REVISIONS:
--
-- DESIGNER: 	Victor Phan
--
-- PROGRAMMER: 	Victor Phan
--
-- INTERFACE:	void echoString(std::string &recvString, int index)
--                  recvString  - string that will be used to send to all clients
--                  index       - index of the client who sent this string out
--
-- RETURNS:     void
--
-- NOTES:
--              Loops through all connected client list and sends the receiveed message to each client that is
--              not the sender.
-------------------------------------------------------------------------------------------------------------------*/
void Server::echoString(std::string &recvString, int index)
{
    for (int i = 0; i < maxClientIndex + 1; i++)
    {
        if (index == i)
        {
            continue;
        }
        // if (client[i].fd >= 0)
        // {
        recvString = "[" + client[index].ipAddress + "] " + recvString;
        write(client[i].fd, recvString.c_str(), recvString.length());
        // }
    }
}
