all: bankingClient bankingServer

bankingClient: bankingClient.c
	gcc bankingClient.c -g -pthread -o bankingClient

bankingServer: bankingServer.c
	gcc bankingServer.c -g -pthread -o bankingServer

clean:
	rm -rf bankingClient bankingServer
