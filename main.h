#pragma once

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <iostream>

#include "client.hpp"
#include "Server.h"
#include "filehandler.hpp"

//client constants
#define SERVER_TCP_PORT 7000                    // Default port
#define BUFLEN                  200                     // Buffer length
#define CHAT_LOG_FILE_PATH      "./chat_log.txt"        // Buffer length

//server constants
#define MAX_CONNECT_REQUEST     5                       //max number of connection request allowed
