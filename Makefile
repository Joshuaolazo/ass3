all: client server

client: bankingClient.c
	gcc bankingClient.c -g -pthread -o bankingClient

server: bankingServer.c
	gcc bankingServer.c -g -pthread -o bankingServer

clean:
	rm -rf bankingClient bankingServer
