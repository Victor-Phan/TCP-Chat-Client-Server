/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: 	main.cpp - Main source file of the application
--
--
-- PROGRAM: 		Chat Application
--
-- FUNCTIONS:
--                  int main
--
-- DATE: 			April 5, 2020
--
-- REVISIONS:
--
-- DESIGNER: 		Victor Phan
--
-- DESIGNER: 		Victor Phan
--
-- NOTES:
--                  FileHandler class pertains logic such as writing to a file of any type, reading from
-                   any file, and checking if a file exists.
--------------------------------------------------------------------------------------------------------------------*/
#include "main.h"

/*-----------------------------------------------------------------------------------------------------------------
-- Function:	    main()
--
-- DATE:		    April 5, 2020
--
-- REVISIONS:
--
-- DESIGNER: 		Victor Phan
--
-- DESIGNER: 		Victor Phan
--
-- INTERFACE:	    int main(int argc, char **argv)
--                      int argc        - number of arguments provided to the start application
--                      char **argv     - the arguments provided
--
-- RETURNS:         void
--
-- NOTES:
--              Main driver of the application. Parses the arguments given to start the appropriate mode.
-------------------------------------------------------------------------------------------------------------------*/
int main(int argc, char **argv) {
  Client client;
  std::string hostName;

  // Get command line arguments..
  switch (argc) {
      case 2:
          if(*argv[1] == 's'){
              //start server
              Server server{SERVER_TCP_PORT};
              server.startServer();
          } else {
              fprintf(stderr, "Usage: %s host [port] [savetofile]\n", argv[0]);
              exit(1);
          }
      break;
      case 3:
          if(*argv[1] == 's'){
              //start server
              Server server{atoi(argv[2])};
              server.startServer();
          } else if (*argv[1] == 'c'){
              //start client
              hostName = std::string(argv[2]);
              client.setHost(hostName);
              client.setPort(SERVER_TCP_PORT);
              client.startClient();
          }
        break;
      case 4:
          if (*argv[1] == 'c') {
              hostName = std::string(argv[2]);
              client.setHost(hostName);
              client.setPort(atoi(argv[3])); // User specified port
              client.startClient();
          } else {
              fprintf(stderr, "Usage: %s host [port] [savetofile]\n", argv[0]);
              exit(1);
          }
        break;
      case 5:
          if(*argv[1] == 'c'){
              hostName = std::string(argv[2]);
              client.setHost(hostName);
              client.setPort(atoi(argv[3])); // User specified port
              client.toggleMessageLogFile((strcmp(argv[4], "save") == 0));
              client.startClient();
          } else {
              fprintf(stderr, "Usage: %s host [port] [savetofile]\n", argv[0]);
              exit(1);
          }
        break;
      default:
        fprintf(stderr, "Usage: %s host [port] [savetofile]\n", argv[0]);
        exit(1);
  }


  return (0);
}