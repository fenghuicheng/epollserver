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
	socklen_t length = 0;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	QUIT_IF_FAIL(listenfd);

	struct sockaddr_in homeaddr;
	bzero((void*)&homeaddr, sizeof(homeaddr));
	homeaddr.sin_family = AF_INET;
	homeaddr.sin_port = htons(2000);
	inet_pton(AF_INET, "127.0.0.1", &homeaddr.sin_addr);

	value = 1;
	length = sizeof(value);
	value = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (void*)&value, length);
	QUIT_IF_FAIL(listenfd);

	value = bind(listenfd, (SA*)&homeaddr, sizeof(homeaddr));
	QUIT_IF_FAIL(value);

	value = listen(listenfd, SOMAXCONN);
	QUIT_IF_FAIL(value);
}

/*wait for working process's termination.*/
void sig_chld(int signo)
{
	pid_t pid = 0;
	int stat = 0;
	
	while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child procss %d exit\n", pid);
		
	return;
}

/*serve the client connected by socket `conn_fd`*/
void work(int conn_fd)
{
	if (conn_fd == 0)
		return;
		
	printf("conn_fd = %d\n", conn_fd);
	
	char buffer[BUFFSIZE] = {0};
	int count = 0;
again:
	while((count = read(conn_fd, &buffer, sizeof(buffer))) > 0)
	{
		printf("packsize = %d;\n", count);
		buffer[count] = '\0';
		printf("%s\n", buffer);
		write(conn_fd, &buffer, count);
	}
	if (count < 0 && errno == SIGINT)
		goto again;

	int value = epoll_ctl(epollfd, EPOLL_CTL_DEL, conn_fd, NULL);
	QUIT_IF_FAIL(value);
	close(conn_fd);
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
	
	/* process the SIGCHLD signal for `fork` model. */
	/*
	struct sigaction act, oldact; 
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_handler = &sig_chld;
	sigaction(SIGCHLD, &act, &oldact);
	*/
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
	struct epoll_event event[MAXEPOLLFD];

	while(1)
	{
		struct sockaddr_in clientsock;
		socklen_t clientlen = 0;

		int value = epoll_wait(epollfd, (epoll_event*)&event, ARRAYCOUNT(event), -1);
#ifndef SA_RESTART
		if (value < 0 && errno == EINTR)
			continue;
		else if(value < 0)
#endif
			QUIT_IF_FAIL(value);

		for (int i = 0; i < value; i++)
		{
			if (event[i].data.fd == listenfd)
			{
				int connectfd = accept(listenfd, NULL, NULL);
				if (connectfd < 0)
				{
#ifndef SA_RESTART
					if (errno != EINTR)
#endif
						close(connectfd);

					perror("print_server connect");
					continue;
				}
				printf("connet with %d\n", connectfd);
				
				struct epoll_event newevent;
				bzero(&newevent, sizeof(newevent));
				newevent.events = EPOLLIN;
				newevent.data.fd = connectfd;

				value = epoll_ctl(epollfd, EPOLL_CTL_ADD, connectfd, &newevent);
				QUIT_IF_FAIL(value);
			} 
			else
			{
				work(event[i].data.fd);
			}
		}
	}

	clean();
	return 0;
}
