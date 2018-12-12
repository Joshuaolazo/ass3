all: bankingClient bankingServer

bankingClient: bankingClient.c
	gcc bankingClient.c -pthread -o bankingClient

bankingServer: bankingServer.c
	gcc bankingServer.c -pthread -o bankingServer

clean:
	rm -rf bankingClient bankingServer
