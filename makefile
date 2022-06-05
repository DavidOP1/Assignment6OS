all: server client 

server:
	gcc server.c -pthread -o server
	
client:
	clang client.cpp -pthread -o client 

clean:
	rm -rf server
	rm -rf client