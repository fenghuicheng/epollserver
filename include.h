#ifndef SOCKET_TEST_INCLUDE
#define SOCKET_TEST_INCLUDE

/*headers for socket() bind() listen() accept()*/
#include <sys/socket.h>

/*header for `sockaddr_in`, netinet/in.h */
#include <netinet/in.h>

/*header for `inet_pton()`, apar/inet.h */
#include <arpa/inet.h>

/*header for `close()`, unistd.h */
#include <unistd.h>

/*header for `waitpid()`, etc*/
#include <sys/types.h>
#include <sys/wait.h>

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define QUIT_IF_FAIL(val) \
do\
{\
if (val < 0) {\
		printf("errno = %d: %s\n", errno, strerror(errno));\
		exit(errno);\
	}\
}while(0);

#endif//SOCKET_TEST_INCLUDE 
