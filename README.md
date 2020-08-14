# TCP-Chat-Client-Server
This is a console application that uses TCP to either create a chatroom and/or connect to an existing chatroom.

# Build
Navigate to the directory with the source code for the application

Run `make clean`

Run `make`

# General Server Usage
`./chat s [port]`

To run the application as a TCP Server, navigate to the application folder and enter the above command. If no port is specified, the default port is 7000. 

Example:

`./chat s 8000`
    
`./chat s`

# General Client Usage
`./chat c [hostname/ip] [host port] [save]`

To run the application as a TCP Client, navigate to the application folder and enter the above command. Hostname/IP must be provided, the rest of the options are optional. If no optional arguments are provided, the defaults will be used (port = 7000, save = false)

Example:

`./chat c 192.168.0.10 8000 save`

`./chat c 127.0.0.1`  (port 7000 and not saving chat to file)

Once connected, the server hostname/ip will be displayed on the screen. You may begin typing into the standard input and send data by pressing “Enter”. 
