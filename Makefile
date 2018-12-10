all: client server

client: client.c
	gcc client.c -g -pthread -o client

server: server.c
	gcc server.c -g -pthread -o server

clean:
	rm -rf client server
