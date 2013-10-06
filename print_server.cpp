#include "include.h"

/*header for `epoll_create()`, etc */
#include <sys/epoll.h>

int listenfd = 0;
int epollfd = 0;

/*socket, bind and listen*/
void socket_setup()
{
	typedef struct sockaddr SA;
	int value = 0;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	QUIT_IF_FAIL(listenfd);

	struct sockaddr_in homeaddr;
	bzero((void*)&homeaddr, sizeof(homeaddr));
	homeaddr.sin_family = AF_INET;
	homeaddr.sin_port = htons(2000);
	inet_pton(AF_INET, "127.0.0.1", &homeaddr.sin_addr);

	value = bind(listenfd, (SA*)&homeaddr, sizeof(homeaddr));
	QUIT_IF_FAIL(value);

	value = listen(listenfd, 1024);
	QUIT_IF_FAIL(value);
}

/*wait for working process's termination.*/
void sig_chld(int signo)
{
	pid_t pid = 0;
	int stat = 0;
	while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
	{
		QUIT_IF_FAIL(pid);
		printf("child procss %d exit\n", pid);
	}
	
	return;
}

/*serve the client connected by socket `conn_fd`*/
void work(int conn_fd)
{
	if (conn_fd == 0)
	{
		return;
	}
	else
	{
		printf("conn_fd = %d\n", conn_fd);
	}
	char buffer[100];
	int count = 0;
	while((count = read(conn_fd, &buffer, sizeof(buffer))) > 0)
	{
		printf("packsize = %d;\n", count);
		buffer[count] = '\0';
		printf("%s\n", buffer);
	}
	exit(0);
}

/*initialize the epoll family.*/
void init()
{
	epollfd = epoll_create(1);
	QUIT_IF_FAIL(epollfd);

	struct epoll_event event;
	bzero(&event, sizeof(event));
	event.events = EPOLLIN;
	event.data.fd = listenfd;
	int value = epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &event);
	QUIT_IF_FAIL(value);

	struct sigaction act, oldact; 
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_handler = &sig_chld;
	sigaction(SIGCHLD, &act, &oldact);
}

/*close employed file descriptors.*/
void clean()
{
	if (0 != listenfd)
		close(listenfd);
	listenfd = 0;

	if (0 != epollfd)
		close(epollfd);
	epollfd = 0;
}

int main()
{
	pid_t pid = 0;
	socket_setup();
	init();
	struct epoll_event event;

	while(1)
	{
		struct sockaddr_in clientsock;
		socklen_t clientlen = 0;

		bzero(&event, sizeof(event));

		int value = epoll_wait(epollfd, &event, 1, -1);
		printf("one shoot for epoll_wait\n");
		if (value < 0 && errno == EINTR)
			continue;
		else if(value < 0)
			QUIT_IF_FAIL(value);

		int connectfd = accept(listenfd, NULL, NULL);
		QUIT_IF_FAIL(connectfd);
		printf("connet with %d\n", connectfd);
		
		if ((pid = fork()) == 0)
		{
			close(listenfd);
			work(connectfd);
		}

		close(connectfd);
	}
	clean();
	return 0;
}
