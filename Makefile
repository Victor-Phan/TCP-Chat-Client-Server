CC=g++ -Wall 

chat: client.o filehandler.o main.o Server.o
	$(CC) -o chat client.o filehandler.o main.o Server.o $(CLIB)

clean:
	rm -f *.o core.* chat
client.o:
	$(CC) -c client.cpp
filehandler.o: 
	$(CC) -c filehandler.cpp
main.o:
	$(CC) -c main.cpp
Server.o:
	$(CC) -c Server.cpp