.PHONY : all
all : server client
server: echo_server.cpp include.h
	gcc -o $@ -g3 echo_server.cpp
client: echo_client.cpp include.h
	gcc -o $@ -g3 echo_client.cpp

