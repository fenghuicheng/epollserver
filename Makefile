.PHONY : all
CXXFLAGS:=${CXXFLAGS} -g3
all : server client
server: print_server.cpp include.h
	gcc -o $@ ${CXXFLAGS} $<
client: print_client.cpp include.h
	gcc -o $@ ${CXXFLAGS} $<

